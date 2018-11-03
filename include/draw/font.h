#pragma once
#include <unordered_map>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H

#include "draw/draw.h"
#include "gl/shader.h"
#include "gl/gl_util.h"


namespace ace { namespace draw {
    namespace detail {
        // Quad explosion done in geometry shader
#pragma pack(push, 1)
        struct GlyphVertex {
            glm::vec2 pos, size;
            // maybe I can even move glyph tex coords data to the gpu
            // but then id never have room to maybe expand to support unicode
            glm::vec2 tex_tl, tex_br;
            glm::vec3 color;
        };
#pragma pack(pop)
    }

    struct Font;

    // todo expand this to be more useful
    struct Text {
        Text(Font *font, std::string str, glm::vec3 color = glm::vec3(1.0f),
             glm::vec2 scale = glm::vec2(1.0f), Align alignment = Align::BOTTOM_LEFT);
        
        void update(std::string str, glm::vec3 color = glm::vec3(1.0f),
                    glm::vec2 scale = glm::vec2(1.0f), Align alignment = Align::BOTTOM_LEFT);
        
        void draw() const;
        void draw_shadowed() const;

        glm::vec2 get_position() const;
        glm::vec2 get_position(Align align) const;

        const std::string &str() const { return this->_str; }
        glm::vec3 color() const { return this->_color; }
        glm::vec2 scale() const { return this->_scale; }
        Align alignment() const { return this->_alignment; }
        glm::vec2 size() const { return _size; }
        
        void set_str(const std::string &str) {
            if (str == this->_str) return;
            this->_str = str;
            this->update();
        }
        void set_color(glm::vec3 color) { this->_color = color; this->update(); }
        void set_scale(glm::vec2 scale) { this->_scale = scale; this->update(); }
        void set_alignment(Align align) { this->_alignment = align; this->update(); }

        glm::vec2 position;
    private:
        void update();

        Font *font;
        std::string _str;
        glm::vec3 _color;
        glm::vec2 _scale;
        Align _alignment;
        glm::vec2 _size;

        std::vector<detail::GlyphVertex> vertices;

        friend Font;
    };

    struct Font {
        Font(const std::string &name, int size, bool monochrome, FT_Library ft);

        // Returns the bottom left of where the next glyph would have started
        glm::vec2 draw(const std::string &str, glm::vec2 pos, glm::vec3 color = glm::vec3(1.0f), glm::vec2 scale = glm::vec2(1.0f), Align alignment = Align::BOTTOM_LEFT);
        glm::vec2 draw_truncated(float max_length, const std::string &str, glm::vec2 pos, glm::vec3 color = glm::vec3(1.0f), glm::vec2 scale = glm::vec2(1.0f), Align alignment = Align::BOTTOM_LEFT);
        glm::vec2 draw_shadowed(const std::string &str, glm::vec2 pos, glm::vec3 color = glm::vec3(1.0f), glm::vec2 scale = glm::vec2(1.0f), Align alignment = Align::BOTTOM_LEFT);

        void draw(const glm::mat4 &pv, gl::ShaderProgram &s);

        glm::vec2 get_aligned_position(glm::vec2 pos, glm::vec2 size, Align alignment) const;
        glm::vec2 measure(const std::string &str, glm::vec2 scale) const;

        int size() const { return size_; }
    private:
        glm::vec2 render(const std::string &str, glm::vec2 pos, glm::vec3 color, glm::vec2 scale, std::vector<detail::GlyphVertex> &v) const;
        void add_glyph(char c, glm::vec2 &pos, glm::vec3 color, glm::vec2 scale, std::vector<detail::GlyphVertex> &v) const;

        void draw(const Text &r);
        void draw_shadowed(const Text &r);

        gl::experimental::vao vao;
        gl::experimental::streaming_vbo<detail::GlyphVertex> vbo;
        // gl::experimental::mesh<detail::GlyphVertex> mesh{ "2f,2f,3f", GL_STREAM_DRAW };
        gl::texture tex;

        unsigned int width, height;
        int size_;
        int _line_height;
       
        struct FontChar {
            glm::vec2 advance, bearing, dim;
            glm::vec2 top_left, bottom_right;
        } chars[256];

        friend Text;
    };

    struct FontManager {
        FontManager();
        ~FontManager();
        ACE_NO_COPY_MOVE(FontManager)

        Font *get(const std::string &name, int size, bool antialias=true);
        void flush(const glm::mat4 &pv, gl::ShaderProgram &s);
    private:
        FT_Library ftl{nullptr};
        std::unordered_map<std::string, Font> fonts;
    };
}}
