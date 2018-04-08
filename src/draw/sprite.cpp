#include "draw/sprite.h"

#include "SDL_image.h"
#include "glm/gtx/matrix_transform_2d.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "util/except.h"

namespace ace { namespace draw {
    SpriteGroup::SpriteGroup(const std::string& file_name, int order) : SpriteGroup(load_image(file_name), order) {
    }

    SpriteGroup::SpriteGroup(SDL_Surface* data, bool color_key, int order) :
        w(data->w), h(data->h), order(order) {

        if (color_key) {
            // AoS has some BMPs that use a color key for transparency
            // So we use a color key on the existing surface, and then blit it to a new surface with the proper format.
            SDL_SetColorKey(data, SDL_TRUE, SDL_MapRGB(data->format, 0, 0, 0));
            SDL_Surface *rgba = SDL_CreateRGBSurface(0, w, h, 32, 0xFF, 0xFF << 8, 0xFF << 16, 0xFF << 24);
            SDL_BlitSurface(data, nullptr, rgba, nullptr);
            SDL_FreeSurface(data);
            data = rgba;
        }

        glBindTexture(GL_TEXTURE_2D, this->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        auto format = data->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, this->w, this->h, 0, format, GL_UNSIGNED_BYTE, data->pixels);
        SDL_FreeSurface(data);

        this->vbo->emplace_back(0.0f, 0.0f);
        this->vbo->emplace_back(0.0f, h);
        this->vbo->emplace_back(w, 0.0f);
        this->vbo->emplace_back(w, h);
        this->vbo.upload();

        this->vao.attrib_pointer("2f", this->vbo.handle)
                 .attrib_pointer("4f,4f,3x3f", this->models.handle, 1);
    }

    void SpriteGroup::draw(glm::vec4 tint, glm::mat3 model, glm::vec4 region) {
        this->models->emplace_back(tint, model, region);
    }

    void SpriteGroup::draw(glm::vec4 tint, glm::vec2 position, float rotation, glm::vec2 scale, Align align, glm::vec4 region) {
        if (tint.a <= 0.f) return;

        const glm::vec2 aligned_position(get_aligned_position(position, scale, {w, h}, align));
        const glm::vec2 anchor(position - aligned_position);

        glm::mat3 model(1.0f);
        model = translate(model, aligned_position);
        model = translate(model, anchor);
        model = rotate(model, glm::radians(rotation));
        model = translate(model, -anchor);
        model = glm::scale(model, scale);
        this->models->emplace_back(tint, model, region);
    }

    void SpriteGroup::draw(gl::ShaderProgram &s) {
        if (this->models->empty()) return;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        s.uniform("sprite_tex", 0);

        this->models.upload();

        this->vao.draw_instanced(GL_TRIANGLE_STRIP, 4, this->models.draw_count);
    }

    void SpriteGroup::set_antialias(bool antialias) {
        glBindTexture(GL_TEXTURE_2D, this->tex);
        auto filter = antialias ? GL_LINEAR : GL_NEAREST;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    }

    SpriteGroup *SpriteManager::get(const std::string &name, SDL_Surface *data) {
        try {
            return &sprites.at(name);
        } catch (std::out_of_range &) {
            if(data) {
                return &sprites.emplace(name, data).first->second;
            }
            return &sprites.emplace(name, "png/" + name).first->second;
        }
    }

    void SpriteManager::draw(gl::ShaderProgram &s) {
        std::vector<SpriteGroup *> groups;
        groups.reserve(this->sprites.size());

        for (auto &kv : this->sprites) {
            groups.push_back(&kv.second);
        }

        std::sort(groups.begin(), groups.end(), [](const SpriteGroup *lhs, const SpriteGroup *rhs) { return lhs->order < rhs->order; });
        for(auto &x : groups) {
            x->draw(s);
        }
//        for (auto &x : sorted_view(this->sprites.begin(), this->sprites.end(), [](auto *lhs, auto *rhs) { return lhs->second.order < rhs->second.order; })) {
//            x->second.draw(s);
//        }
    }
}}
