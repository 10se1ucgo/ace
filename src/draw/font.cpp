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
                        vec[row + bi] = bool(bit) * 255;
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

    void Text::draw() {
        this->font->draw(*this);
    }

    void Text::update() {
        this->vertices.clear();
        this->_size = this->font->measure(this->_str, this->_scale);
        this->font->render(this->_str, this->font->get_aligned_pos({ 0, 0 }, this->_size, this->_alignment), this->_color, this->_scale, this->vertices);
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
            width += g->bitmap.width + 1;
            height = std::max(height, g->bitmap.rows);
        }

        this->vao.attrib_pointer("4f,3f", this->vbo.handle);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

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

            chars[c].advance = { g->advance.x >> 6, g->advance.y >> 6 };
            chars[c].dim = { g->bitmap.width, g->bitmap.rows };
            chars[c].bearing = { g->bitmap_left, g->bitmap_top };
            chars[c].tx = float(x) / width;

            x += g->bitmap.width + 1;
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        FT_Done_Face(face);
    }

    void Font::draw(const glm::mat4 &pv, gl::ShaderProgram &s) {
        if (this->vbo->empty()) return;

        s.uniform("mvp", pv);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        
        this->vbo.upload();
        this->vao.draw(GL_TRIANGLES, this->vbo.draw_count, this->vbo.draw_offset);
    }

    glm::vec2 Font::get_aligned_pos(glm::vec2 pos, glm::vec2 size, Align alignment) const {
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
        glm::vec2 p;
        p.y = -std::numeric_limits<float>::infinity();
        for (unsigned char c : str) {
            p.x += chars[c].advance.x;
            p.y = std::max(p.y, float(chars[c].bearing.y));
        }
        return p * scale;
    }

    void Font::render(const std::string &str, glm::vec2 pos, glm::vec3 color, glm::vec2 scale, std::vector<detail::GlyphVertex> &v) const {
        for (unsigned char c : str) {
            float x = pos.x + chars[c].bearing.x * scale.x;
            float y = -pos.y + chars[c].bearing.y * scale.y;
            float w = chars[c].dim.x * scale.x;
            float h = chars[c].dim.y * scale.y;

            pos += glm::vec2(chars[c].advance) * scale;

            if (!w || !h) continue;

            v.push_back({ { x,     -y    , chars[c].tx, 0 }, color });
            v.push_back({ { x + w, -y    , chars[c].tx + chars[c].dim.x / float(this->width), 0 }, color });
            v.push_back({ { x,     -y + h, chars[c].tx, chars[c].dim.y / float(this->height) }, color });
            v.push_back({ { x + w, -y    , chars[c].tx + chars[c].dim.x / float(this->width), 0 }, color });
            v.push_back({ { x,     -y + h, chars[c].tx, chars[c].dim.y / float(this->height) }, color });
            v.push_back({ { x + w, -y + h, chars[c].tx + chars[c].dim.x / float(this->width), chars[c].dim.y / float(this->height) }, color });
        }
    }

    void Font::draw(const std::string &str, glm::vec2 pos, glm::vec3 color, glm::vec2 scale, Align alignment) {
        pos = this->get_aligned_pos(pos, this->measure(str, scale), alignment);
        this->render(str, pos, color, scale, this->vbo.data);
    }

    void Font::draw(Text &r) {
        this->vbo->reserve(this->vbo->size() + r.vertices.size());
        for(auto &x : r.vertices) {
            auto p(x.pos_tex);
            p.x += r.position.x;
            p.y += r.position.y;
            this->vbo->push_back({ p, x.color });
        }
        // this->vbo->insert(this->vbo->end(), std::make_move_iterator(r.vertices.begin()), std::make_move_iterator(r.vertices.end()));
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
            return &fonts.emplace(std::move(n), Font("font/" + name, size, !antialias, this->ftl)).first->second;
        }
    }

    void FontManager::draw(const glm::mat4& pv, gl::ShaderProgram& s) {
        for(auto &kv : this->fonts) {
            kv.second.draw(pv, s);
        }
    }
}}
