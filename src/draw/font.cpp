#include "draw/font.h"
#include <algorithm>
#include "fmt/printf.h"

#include FT_BITMAP_H

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

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(detail::GlyphVertex), reinterpret_cast<void *>(offsetof(detail::GlyphVertex, pos_tex)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(detail::GlyphVertex), reinterpret_cast<void *>(offsetof(detail::GlyphVertex, color)));
        glEnableVertexAttribArray(0);


        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &this->tex);
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
//                FT_Bitmap bmp;
//                FT_Bitmap_Init(&bmp);
//                FT_Bitmap_Convert(ft, &g->bitmap, &bmp, 1);
//                buffer = bmp.buffer;
            }
            glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, buffer);

            chars[c].advance = { g->advance.x >> 6, g->advance.y >> 6 };
            chars[c].dim = { g->bitmap.width, g->bitmap.rows };
            chars[c].bearing = { g->bitmap_left, g->bitmap_top };
            chars[c].tx = float(x) / width;

            x += g->bitmap.width + 1;
        }

        FT_Done_Face(face);
    }

    void Font::draw(const glm::mat4 &pv, ShaderProgram &s) {
        if (this->vertices.empty()) return;

        s.uniform("mvp", pv);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glInvalidateBufferData(GL_ARRAY_BUFFER);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(detail::GlyphVertex), this->vertices.data(), GL_STREAM_DRAW);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());

        this->vertices.clear();
    }

    glm::vec2 Font::measure(const std::string& str, glm::vec2 scale) const {
        glm::vec2 p;
        p.y = -std::numeric_limits<float>::infinity();
        for (unsigned char c : str) {
            p.x += chars[c].advance.x * scale.x;
            p.y = std::max(p.y, chars[c].bearing.y * scale.y);
        }
        return p;
    }

    void Font::draw(const std::string &str, glm::vec2 pos, glm::vec3 color, glm::vec2 scale, Align alignment) {
        auto size = this->measure(str, scale);
        switch(alignment) {
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

        for (unsigned char c : str) {
            float x2 = pos.x + chars[c].bearing.x * scale.x;
            float y2 = -pos.y + chars[c].bearing.y * scale.y;
            float w = chars[c].dim.x * scale.x;
            float h = chars[c].dim.y * scale.y;

            pos += glm::vec2(chars[c].advance) * scale;

            if (!w || !h) continue;

            vertices.push_back({ { x2,     -y2    , chars[c].tx, 0 }, color });
            vertices.push_back({ { x2 + w, -y2    , chars[c].tx + chars[c].dim.x / float(this->width), 0 }, color });
            vertices.push_back({ { x2,     -y2 + h, chars[c].tx, chars[c].dim.y / float(this->height) }, color });
            vertices.push_back({ { x2 + w, -y2    , chars[c].tx + chars[c].dim.x / float(this->width), 0 }, color });
            vertices.push_back({ { x2,     -y2 + h, chars[c].tx, chars[c].dim.y / float(this->height) }, color });
            vertices.push_back({ { x2 + w, -y2 + h, chars[c].tx + chars[c].dim.x / float(this->width), chars[c].dim.y / float(this->height) }, color });
        }
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
            return fonts.at(n).get();
        } catch (std::out_of_range &) {
            auto x = std::make_unique<Font>("font/" + name, size, !antialias, this->ftl);
            // yeah this is awful im sorry
            return fonts.insert({ std::move(n), std::move(x) }).first->second.get();
        }
    }

    void FontManager::draw(const glm::mat4& pv, ShaderProgram& s) {
        for(auto &kv : this->fonts) {
            kv.second->draw(pv, s);
        }
    }
}}
