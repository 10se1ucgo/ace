#pragma once
#include <string>

#include "SDL.h"
#include "glm/glm.hpp"

#include "gl/shader.h"
#include "gl/gl_util.h"
#include "draw/draw.h"


namespace ace { namespace draw {
    struct SpriteVert {
        glm::vec4 tint;
        glm::vec4 region;
        glm::mat3 model;

        SpriteVert(glm::vec4 tint, glm::mat3 model, glm::vec4 region = {0, 0, 1, 1}) : tint(tint), region(region), model(model) { }
    };

    struct SpriteGroup {
        SpriteGroup(const std::string &file_name, int order=0);
        // i REALLY hate intializer lists lol
        SpriteGroup(const std::pair<SDL_Surface *, bool> params, int order = 0) : SpriteGroup(params.first, params.second, order) {}
        SpriteGroup(SDL_Surface *data, bool color_key = false, int order = 0);

        void draw(glm::vec4 tint, glm::mat3 model, glm::vec4 region = { 0, 0, 1, 1 });
        void draw(glm::vec4 tint, glm::vec2 position, float rotation, glm::vec2 scale = { 1.0, 1.0 }, Align align = Align::TOP_LEFT, glm::vec4 region = { 0, 0, 1, 1 });
        void draw(ShaderProgram &s);

        int w{}, h{}, order{};
        gl::vao vao;
        gl::vbo vbo, models;
        gl::texture tex;

        std::vector<SpriteVert> verts;
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

        void flush(ShaderProgram& s) {
            this->group->draw(s);
        }

        glm::vec2 get_position() const {
            return get_aligned_position(this->position, this->scale, { group->w, group->h }, this->alignment);
        }

//        glm::vec2 get_position(Align align) const {
//            return get_aligned_position(this->position, this->scale, { group->w, group->h }, align);
//        }

        glm::vec2 get_position(Align align) const {
            auto p = this->get_position();
            auto x = get_aligned_position(p, this->scale, { group->w, group->h }, align);
            return p - x + p;
        }

        void set_region(glm::vec2 bl, glm::vec2 tr) {
            this->region = { bl.x / this->group->w, bl.y / this->group->h, tr.x / this->group->w, tr.y / this->group->h };
        }

        int w() const { return group->w * scale.x; }
        int h() const { return group->h * scale.y; }

        SpriteGroup *group;

        glm::vec2 position{ 0.0f }, scale{ 1.0f };
        glm::vec4 tint{ 1.0f }, region{ 0.0f, 0.0f, 1.0f, 1.0f };
        float rotation{ 0.0f };

        Align alignment{ Align::TOP_LEFT };
    };

    struct SpriteManager {
        SpriteGroup *get(const std::string &name, SDL_Surface *data = nullptr);

        void draw(ShaderProgram &s);
    private:
        std::unordered_map<std::string, SpriteGroup> sprites;
    };
}}
