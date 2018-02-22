#pragma once

#include "glad/glad.h"
#include "gl/shader.h"
#include "glm/glm.hpp"
#include "SDL.h"

#include <string>
#include <memory>
#include <glm/gtx/transform.hpp>

#include "draw/draw.h"

namespace ace {
    namespace draw {
#pragma pack(push, 1)
        struct SpriteVert {
            glm::vec4 tint;
            glm::mat3 model;

            SpriteVert(glm::vec4 tint, glm::mat3 model) : tint(tint), model(model) { }
        };
#pragma pack(pop)

        struct SpriteGroup {
            SpriteGroup(const std::string &file_name);
            // i REALLY hate intializer lists lol
            SpriteGroup(const std::pair<SDL_Surface *, bool> params) : SpriteGroup(params.first, params.second) {}
            SpriteGroup(SDL_Surface *data, bool color_key = false);

            void draw(glm::vec4 tint, glm::mat3 mvp);
            void draw(glm::vec4 tint, glm::vec2 position, float rotation, glm::vec2 scale = { 1.0, 1.0 }, Align align = Align::TOP_LEFT);
            void draw(ShaderProgram &s);

            int w{}, h{};
            GLuint tex{}, vao{}, vbo{}, models{};

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
                this->group->draw(this->tint, this->position, this->rotation, this->scale, this->alignment);
            }

            void flush(ShaderProgram& s) {
                this->group->draw(s);
            }

            glm::vec2 get_position() const {
                return get_aligned_position(this->position, this->scale, { group->w, group->h }, this->alignment);
            }
            int w() const { return group->w * scale.x; }
            int h() const { return group->h * scale.y; }

            SpriteGroup *group;

            glm::vec2 position{ 0.0f }, scale{ 1.0f };
            glm::vec4 tint{ 1.0f };
            float rotation{ 0.0f };

            Align alignment{ Align::TOP_LEFT };
        };

        struct SpriteManager {
            SpriteGroup *get(const std::string &name, SDL_Surface *data = nullptr);

            void draw(ShaderProgram& s) {
                for(auto &kv : this->sprites) {
                    kv.second->draw(s);
                }
            }
        private:
            std::unordered_map<std::string, std::unique_ptr<SpriteGroup>> sprites;
        };
    }
}
