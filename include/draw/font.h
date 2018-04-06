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
#pragma pack(push, 1)
        struct GlyphVertex {
            glm::vec4 pos_tex;
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

        void draw();
        void draw_shadowed();

        const std::string &str() const { return this->_str; }
        glm::vec3 color() const { return this->_color; }
        glm::vec2 scale() const { return this->_scale; }
        Align alignment() const { return this->_alignment; }
        glm::vec2 size() const { return _size; }
        
        void set_str(std::string str) { this->_str = std::move(str); this->update(); }
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

        void draw(const std::string &str, glm::vec2 pos, glm::vec3 color = glm::vec3(1.0f), glm::vec2 scale = glm::vec2(1.0f), Align alignment = Align::BOTTOM_LEFT);
        
        void draw_shadowed(const std::string &str, glm::vec2 pos, glm::vec3 color = glm::vec3(1.0f), glm::vec2 scale = glm::vec2(1.0f), Align alignment = Align::BOTTOM_LEFT);

        void draw(const glm::mat4 &pv, gl::ShaderProgram &s);

        glm::vec2 get_aligned_pos(glm::vec2 pos, glm::vec2 size, Align alignment) const;
        glm::vec2 measure(const std::string &str, glm::vec2 scale) const;

        int size() const { return size_; }
    private:
        void render(const std::string &str, glm::vec2 pos, glm::vec3 color, glm::vec2 scale, std::vector<detail::GlyphVertex> &v) const;
        void draw(Text &r);
        void draw_shadowed(Text &r);

        gl::experimental::vao vao;
        gl::experimental::streaming_vbo<detail::GlyphVertex> vbo;
        gl::texture tex;

        unsigned int width, height;
        int size_;
       
        struct FontChar {
            glm::ivec2 advance, dim, bearing;
            float tx;
        } chars[256];

        friend Text;
    };

    struct FontManager {
        FontManager();
        ~FontManager();
        ACE_NO_COPY_MOVE(FontManager)

        Font *get(const std::string &name, int size, bool antialias=true);
        void draw(const glm::mat4 &pv, gl::ShaderProgram &s);
    private:
        FT_Library ftl;
        std::unordered_map<std::string, Font> fonts;
    };
}}
