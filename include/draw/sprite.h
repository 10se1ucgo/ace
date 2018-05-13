#pragma once
#include <string>

#include "SDL.h"
#include "SDL_image.h"
#include "glm/glm.hpp"

#include "gl/shader.h"
#include "gl/gl_util.h"
#include "draw/draw.h"
#include "util/except.h"


namespace ace { namespace draw {
    inline std::pair<SDL_Surface *, bool> load_image(const std::string& file_name) {
        SDL_RWops *rwop = SDL_RWFromFile(file_name.c_str(), "rb");
        bool is_bmp = IMG_isBMP(rwop);
        SDL_Surface *data = IMG_Load_RW(rwop, 1);
        if (!data) THROW_ERROR("Couldn't load texture {0}! {1}", file_name, SDL_GetError());
        return { data, is_bmp };
    }

    struct SpriteGroup {
        SpriteGroup(const std::string &file_name, int order=0);
        // i REALLY hate intializer lists lol
        SpriteGroup(const std::string &file_name, const std::pair<SDL_Surface *, bool> params, int order = 0) : SpriteGroup(file_name, params.first, params.second, order) {}
        SpriteGroup(const std::string &file_name, SDL_Surface *data, bool color_key = false, int order = 0);

        void draw(glm::vec4 tint, glm::mat3 model, glm::vec4 region = { 0, 0, 1, 1 });
        void draw(glm::vec4 tint, glm::vec2 position, float rotation, glm::vec2 scale = { 1.0, 1.0 }, Align align = Align::TOP_LEFT, glm::vec4 region = { 0, 0, 1, 1 });
        void draw(gl::ShaderProgram &s);

        void set_antialias(bool antialias);

        int order{};
        int w() const { return this->tex.w(); }
        int h() const { return this->tex.h(); }
        gl::experimental::texture2d tex;
    private:
#pragma pack(push, 1)
        struct SpriteVert {
            glm::vec4 tint;
            glm::vec4 region;
            glm::mat3 model;

            SpriteVert(glm::vec4 tint, glm::mat3 model, glm::vec4 region = { 0, 0, 1, 1 }) : tint(tint), region(region), model(model) { }
        };
#pragma pack(pop)

        gl::experimental::vao vao;
        gl::experimental::vbo<glm::vec2> vbo;
        gl::experimental::vbo<SpriteVert> models{ GL_STREAM_DRAW };
    };

    inline glm::vec2 get_aligned_position(glm::vec2 position, const glm::vec2 &scale, const glm::vec2 &size, Align alignment) {
        switch (alignment) {
        case Align::TOP_LEFT:
            break;
        case Align::BOTTOM_LEFT:
            position.y -= size.y * scale.y;
            break;
        case Align::TOP_RIGHT:
            position.x -= size.x * scale.x;
            break;
        case Align::BOTTOM_RIGHT:
            position -= size * scale;
            break;
        case Align::CENTER:
            position -= size / 2.f * scale;
            break;
        case Align::BOTTOM_CENTER:
            position.y -= size.y * scale.y;
        case Align::TOP_CENTER:
            position.x -= size.x / 2.f * scale.x;
            break;
        default:
            break;
        }
        return position;
    }

    struct Sprite {
        Sprite(SpriteGroup *group = nullptr) : group(group) { }

        void draw() const {
            this->group->draw(this->tint, this->position, this->rotation, this->scale, this->alignment, this->region);
        }

        void flush(gl::ShaderProgram& s) {
            this->group->draw(s);
        }

        glm::vec2 get_position() const {
            return get_aligned_position(this->position, this->scale, { this->group->w(), this->group->h() }, this->alignment);
        }

        glm::vec2 get_position(Align align) const {
            auto p = this->get_position();
            auto x = get_aligned_position(p, this->scale, { this->group->w(), this->group->h() }, align);
            return p - x + p;
        }

        void set_region(glm::vec2 bl, glm::vec2 tr) {
            this->region = { bl.x / this->group->w(), bl.y / this->group->h(), tr.x / this->group->w(), tr.y / this->group->h() };
        }

        float w() const { return group->w() * scale.x; }
        float h() const { return group->h() * scale.y; }
        glm::vec2 size() const { return { this->w(), this->h() }; }

        SpriteGroup *group;

        glm::vec2 position{ 0.0f }, scale{ 1.0f };
        glm::vec4 tint{ 1.0f }, region{ 0.0f, 0.0f, 1.0f, 1.0f };
        float rotation{ 0.0f };

        Align alignment{ Align::TOP_LEFT };
    };

    struct SpriteManager {
        SpriteGroup *get(const std::string &name, SDL_Surface *data = nullptr);

        void draw(gl::ShaderProgram &s);
    private:
        std::unordered_map<std::string, SpriteGroup> sprites;
    };
}}
