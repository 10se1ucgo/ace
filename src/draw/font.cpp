#include "draw/font.h"

#include <algorithm>

#include "fmt/format.h"

namespace ace { namespace draw {
    namespace {
        // convert monochrome 1bpp bitmap to grayscale 8bpp
        void unpack_monochrome_buffer(std::vector<uint8_t> &vec, const FT_GlyphSlot g) {
            vec.resize(g->bitmap.width * g->bitmap.rows);
            for (unsigned int x = 0; x < g->bitmap.rows; x++) {
                for (int y = 0; y < g->bitmap.pitch; y++) {
                    auto byte = g->bitmap.buffer[x * g->bitmap.pitch + y];
                    auto row = x * g->bitmap.width + y * 8;
                    for (auto bi = 0; bi < std::min(8, int(g->bitmap.width) - y * 8); bi++) {
                        auto bit = byte & (1 << (7 - bi));
                        vec[row + bi] = bit ? 255 : 0;
                    }
                }
            }
        }
    }

    Text::Text(Font *font, std::string str, glm::vec3 color, glm::vec2 scale, Align alignment):
        font(font),
        _str(std::move(str)),
        _color(color),
        _scale(scale),
        _alignment(alignment) {
        this->update();
    }

    void Text::update(std::string str, glm::vec3 color, glm::vec2 scale, Align alignment) {
        this->_str = std::move(str);
        this->_color = color;
        this->_scale = scale;
        this->_alignment = alignment;
        this->update();
    }

    void Text::draw() const {
        this->font->draw(*this);
    }

    void Text::draw_shadowed() const {
        this->font->draw_shadowed(*this);
    }

    glm::vec2 Text::get_position() const {
        return this->font->get_aligned_position(this->position, this->_size, this->_alignment);
    }

    glm::vec2 Text::get_position(Align align) const {
        auto p = this->get_position();
        auto x = this->font->get_aligned_position(p, this->_size, align);
        return p - x + p;
    }

    void Text::update() {
        this->vertices.clear();
        this->_size = this->font->measure(this->_str, this->_scale);
        this->font->render(this->_str, this->font->get_aligned_position({ 0, 0 }, this->_size, this->_alignment), this->_color, this->_scale, this->vertices);
    }

    Font::Font(const std::string &name, int size, bool monochrome, FT_Library ft): width(0), height(0), size_(size) {
        FT_Face face;
        FT_New_Face(ft, name.c_str(), 0, &face);
        FT_Set_Pixel_Sizes(face, 0, size);

        const FT_Int32 flags = FT_LOAD_NO_BITMAP | (monochrome ? FT_LOAD_TARGET_MONO : 0);

        FT_GlyphSlot g = face->glyph;
        for (int c = 0; c < 256; c++) {
            if(FT_Load_Char(face, c, FT_LOAD_BITMAP_METRICS_ONLY | flags)) {
                fmt::print(stderr, "Couldn't load char@{}!\n", c);
                continue;
            }
            this->width += g->bitmap.width + 1;
            this->height = std::max(this->height, g->bitmap.rows);
        }

        this->vao.attrib_pointer("2f,2f,3f", this->vbo.handle);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // I noticed that some fonts would have weird spots at the bottom.
        // Probably because the default contents of a texture are undefined and some glyphs don't fill up the entire block?
        // Clear texture if available, otherwise just upload an empty texture.
        if(GLAD_GL_VERSION_4_4 || GLAD_GL_ARB_clear_texture) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->width, this->height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
            uint8_t r = 0;
            glClearTexImage(this->tex, 0, GL_RED, GL_UNSIGNED_BYTE, &r);
        } else {
            std::vector<uint8_t> data(this->width * this->height, 0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->width, this->height, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
        }

        std::vector<uint8_t> bmpbuffer; // if using FT_LOAD_MONOCHROME;
        unsigned int x = 0;
        for (int c = 0; c < 256; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER | flags)) {
                continue;
            }
            auto buffer = g->bitmap.buffer;
            if (monochrome) {
                bmpbuffer.clear();
                unpack_monochrome_buffer(bmpbuffer, g);
                buffer = bmpbuffer.data();
            }
            glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, buffer);

            this->chars[c].advance = { g->advance.x >> 6, g->advance.y >> 6 };
            this->chars[c].dim = { g->bitmap.width, g->bitmap.rows };
            this->chars[c].bearing = { g->bitmap_left, g->bitmap_top };
            auto tx = float(x) / this->width;
            this->chars[c].tx = tx;

            this->chars[c].tl = { tx, 0 };
            this->chars[c].tr = { tx + this->chars[c].dim.x / float(this->width), 0 };
            this->chars[c].bl = { tx, this->chars[c].dim.y / float(this->height) };
            this->chars[c].br = { tx + this->chars[c].dim.x / float(this->width), this->chars[c].dim.y / float(this->height) };

            x += g->bitmap.width + 1;
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        this->_line_height = face->size->metrics.height >> 6;

        FT_Done_Face(face);
    }

    void Font::draw(const glm::mat4 &pv, gl::ShaderProgram &s) {
        if (this->vbo->empty()) return;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->tex);
        
        this->vbo.upload();
        this->vao.draw(GL_TRIANGLES, this->vbo.draw_count, this->vbo.draw_offset);
    }

    glm::vec2 Font::get_aligned_position(glm::vec2 pos, glm::vec2 size, Align alignment) const {
        switch (alignment) {
        case Align::TOP_LEFT:
            pos.y += size.y;
            break;
        case Align::BOTTOM_LEFT:
            break;
        case Align::TOP_RIGHT:
            pos.y += size.y;
            pos.x -= size.x;
            break;
        case Align::BOTTOM_RIGHT:
            pos.x -= size.x;
            break;
        case Align::CENTER:
            pos.y += size.y / 2.f;
            pos.x -= size.x / 2.f;
            break;
        case Align::TOP_CENTER:
            pos.y += size.y;
        case Align::BOTTOM_CENTER:
            pos.x -= size.x / 2.f;
            break;
        default:
            break;
        }
        return pos;
    }

    glm::vec2 Font::measure(const std::string& str, glm::vec2 scale) const {
        // this is bad

        glm::ivec2 size(0, 0);
        int pen = 0, lines = 1;
        for(unsigned char c : str) {
            if(c == '\n') {
                size.x = std::max(pen, size.x);
                pen = 0;
                lines++;
            }
            pen += this->chars[c].advance.x;
            size.y = std::max(this->chars[c].bearing.y, size.y);
        }
        size.x = std::max(pen, size.x);
        if(lines > 1) {
            size.y += (lines - 1) * this->_line_height;
        }
        return glm::vec2(size) * scale;
    }

    void Font::render(const std::string &str, glm::vec2 pos, glm::vec3 color, glm::vec2 scale, std::vector<detail::GlyphVertex> &v) const {
        glm::vec2 opos(pos);
        for (unsigned char c : str) {
            if(c == '\n') {
                // this doesnt work at all for aligned text lol
                // i really should ditch this and use a third party library, its more of a headache than its worth
                opos.y += this->_line_height;
                pos = opos;
                continue;
            }
            this->add_glyph(c, pos, color, scale, v);
        }
    }

    void Font::add_glyph(char c, glm::vec2 &pos, glm::vec3 color, glm::vec2 scale, std::vector<detail::GlyphVertex> &v) const {
        auto &glyph = this->chars[uint8_t(c)];

        float x = pos.x + glyph.bearing.x * scale.x;
        float y = pos.y - glyph.bearing.y * scale.y;
        float w = glyph.dim.x * scale.x;
        float h = glyph.dim.y * scale.y;

        pos += glm::vec2(glyph.advance) * scale;

        if (!w || !h) return;

        v.push_back({ { x,     y }, glyph.tl, color }); // top left
        v.push_back({ { x + w, y }, glyph.tr, color }); // top right
        v.push_back({ { x,     y + h }, glyph.bl, color }); // bottom left
        v.push_back({ { x + w, y }, glyph.tr, color });
        v.push_back({ { x,     y + h }, glyph.bl, color });
        v.push_back({ { x + w, y + h }, glyph.br, color }); // bottom right
    }

    void Font::draw(const std::string &str, glm::vec2 pos, glm::vec3 color, glm::vec2 scale, Align alignment) {
        pos = this->get_aligned_position(pos, this->measure(str, scale), alignment);
        this->render(str, pos, color, scale, this->vbo.data);
    }

    void Font::draw_truncated(float max_length, const std::string &str, glm::vec2 pos, glm::vec3 color, glm::vec2 scale,  Align alignment) {
        pos = this->get_aligned_position(pos, this->measure(str, scale), alignment);
        float max_x_pos = pos.x + max_length;
        for (unsigned char c : str) {
            if (pos.x >= max_x_pos) return;
            this->add_glyph(c, pos, color, scale, this->vbo.data);
        }
        // TODO make this not crap and also draw ellipsis
    }

    void Font::draw(const Text &r) {
        this->vbo->reserve(this->vbo->size() + r.vertices.size());
        for(auto &x : r.vertices) {
            this->vbo->push_back({ x.pos + r.position, x.tex, x.color });
        }
    }

    void Font::draw_shadowed(const Text &r) {
        // TODO dont do this inefficient recalculation of every vertex
        this->draw(r.str(), r.position + glm::vec2(2), glm::vec3(0.5), r.scale(), r.alignment());
        this->draw(r);
    }

    void Font::draw_shadowed(const std::string &str, glm::vec2 pos, glm::vec3 color, glm::vec2 scale, Align alignment) {
        this->draw(str, pos + glm::vec2(2), glm::vec3(0.5), scale, alignment); // B)
        this->draw(str, pos, color, scale, alignment);
    }

    FontManager::FontManager() {
        FT_Init_FreeType(&ftl);
    }

    FontManager::~FontManager() {
        FT_Done_FreeType(ftl);
    }

    Font *FontManager::get(const std::string &name, int size, bool antialias) {
        auto n = fmt::format("{}{}{}", name, size, !antialias);
        try {
            return &fonts.at(n);
        } catch (std::out_of_range &) {
            return &fonts.emplace(std::move(n), Font(get_resource_path("font/" + name), size, !antialias, this->ftl)).first->second;
        }
    }

    void FontManager::draw(const glm::mat4& pv, gl::ShaderProgram& s) {
        s.uniform("mvp", pv);
        for(auto &kv : this->fonts) {
            kv.second.draw(pv, s);
        }
    }
}}
