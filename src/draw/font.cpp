#include "draw/font.h"
#include <algorithm>
#include "fmt/printf.h"

namespace ace { namespace draw {
    namespace {
#pragma pack(push, 1)
        struct Vertex {
            glm::vec4 pos_tex;
            glm::vec3 color;
        };
#pragma pack(pop)
    }


    Font::Font(const std::string &name, int size, FT_Library ft): width(0), height(0), size_(size) {
        FT_Face face;
        FT_New_Face(ft, name.c_str(), 0, &face);
        FT_Set_Pixel_Sizes(face, 0, size);

        constexpr FT_Int32 flags = FT_LOAD_NO_BITMAP;

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

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, pos_tex)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, color)));
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

        unsigned int x = 0;
        for (int c = 0; c < 256; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER | flags)) {
                continue;
            }
            glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

            chars[c].advance = { g->advance.x >> 6, g->advance.y >> 6 };
            chars[c].dim = { g->bitmap.width, g->bitmap.rows };
            chars[c].bearing = { g->bitmap_left, g->bitmap_top };
            chars[c].tx = float(x) / width;

            x += g->bitmap.width + 1;
        }

        FT_Done_Face(face);
    }

    void Font::draw(const glm::mat4 &pv, ShaderProgram &s) {
        if (this->strings.empty()) return;

        std::vector<Vertex> vertices;
        vertices.reserve(6 * this->strings.size() * 25);

        for(const auto &ds : this->strings) {
            glm::vec2 p = ds.pos;
            for(unsigned char c : ds.str) {
                float x2 = p.x + chars[c].bearing.x * ds.scale.x;
                float y2 = -p.y + chars[c].bearing.y * ds.scale.y;
                float w = chars[c].dim.x * ds.scale.x;
                float h = chars[c].dim.y * ds.scale.y;

                p += glm::vec2(chars[c].advance) * ds.scale;

                if (!w || !h) continue;

                vertices.push_back({ {x2,     -y2    , chars[c].tx, 0}, ds.color });
                vertices.push_back({ {x2 + w, -y2    , chars[c].tx + chars[c].dim.x / float(this->width), 0}, ds.color });
                vertices.push_back({ {x2,     -y2 + h, chars[c].tx, chars[c].dim.y / float(this->height)}, ds.color });
                vertices.push_back({ {x2 + w, -y2    , chars[c].tx + chars[c].dim.x / float(this->width), 0}, ds.color });
                vertices.push_back({ {x2,     -y2 + h, chars[c].tx, chars[c].dim.y / float(this->height)}, ds.color });
                vertices.push_back({ {x2 + w, -y2 + h, chars[c].tx + chars[c].dim.x / float(this->width), chars[c].dim.y / float(this->height)}, ds.color });
            }
        }

        s.uniform("mvp", pv);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glInvalidateBufferData(GL_ARRAY_BUFFER);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STREAM_DRAW);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        this->strings.clear();
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

//        switch(alignment) {
//        case Align::MIDDLE:
//            pos.x -= this->measure(str, scale).x / 2.f;
//            break;
//        case Align::RIGHT:
//            pos.x -= this->measure(str, scale).x;
//            break;
//        case Align::LEFT:
//        default:
//            break;
//        }


        this->strings.emplace_back(str, pos, scale, color);
    }

    FontManager::FontManager() {
        FT_Init_FreeType(&ftl);
    }

    FontManager::~FontManager() {
        FT_Done_FreeType(ftl);
    }

    Font *FontManager::get(const std::string &name, int size) {
        try {
            return fonts.at(name).get();
        } catch (std::out_of_range &) {
            auto x = std::make_unique<Font>("font/" + name, size, this->ftl);
            return fonts.insert({ fmt::format("{}{}", name, size), std::move(x) }).first->second.get();
        }
    }

    void FontManager::draw(const glm::mat4& pv, ShaderProgram& s) {
        for(auto &kv : this->fonts) {
            kv.second->draw(pv, s);
        }
    }
}}
