#pragma once
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "gl/shader.h"

#include "ft2build.h"
#include FT_FREETYPE_H

#include <unordered_map>
#include <memory>
#include <utility>

#include "draw/draw.h"

namespace ace { namespace draw {
    struct Font {
        Font(const std::string &name, int size, FT_Library ft);
        void draw(const std::string &str, glm::vec2 pos, glm::vec3 color = glm::vec3(1.0f), glm::vec2 scale = glm::vec2(1.0f), Align alignment = Align::BOTTOM_LEFT);
        void draw(const glm::mat4 &pv, ShaderProgram &s);
        glm::vec2 measure(const std::string &str, glm::vec2 scale) const;
        int size() const { return size_; }
    private:
        GLuint vao, vbo, tex;
        unsigned int width, height;
        int size_;
       
        struct FontChar {
            glm::ivec2 advance, dim, bearing;
            float tx;
        } chars[256];

        struct DrawString {
            DrawString(std::string str, glm::vec2 pos, glm::vec2 scale, glm::vec3 color) : str(std::move(str)), pos(pos), scale(scale), color(color) {
            }

            std::string str;
            glm::vec2 pos, scale;
            glm::vec3 color;
        };
        std::vector<DrawString> strings;
    };

    struct FontManager {
        FontManager();
        ~FontManager();

        Font *get(const std::string &name, int size);
        void draw(const glm::mat4 &pv, ShaderProgram &s);
    private:
        FT_Library ftl;
        std::unordered_map<std::string, std::unique_ptr<Font>> fonts;
    };
}}
