#include "draw/sprite.h"

#include "SDL_image.h"
#include "glm/gtx/matrix_transform_2d.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "util/except.h"

namespace ace { namespace draw {
    inline std::pair<SDL_Surface *, bool> load_image(const std::string& file_name) {

        SDL_RWops *rwop = SDL_RWFromFile(file_name.c_str(), "rb");
        bool is_bmp = IMG_isBMP(rwop);
        SDL_Surface *data = IMG_Load_RW(rwop, 1);
        if (!data) THROW_ERROR("Couldn't load texture {0}!", file_name);
        return {data, is_bmp};
    }

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

        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        auto format = data->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data->pixels);

        GLfloat vertices[] = {
            0.f, 0.f,
            0.f,   h,
            w,   0.f,
            w,     h,
        };

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0); // vert

        glBindBuffer(GL_ARRAY_BUFFER, models);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SpriteVert), reinterpret_cast<void*>(offsetof(SpriteVert, tint)));
        glEnableVertexAttribArray(1); // tint
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SpriteVert), reinterpret_cast<void*>(offsetof(SpriteVert, region)));
        glEnableVertexAttribArray(2); // tint
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVert), reinterpret_cast<void*>(offsetof(SpriteVert, model) + sizeof(glm::mat3::col_type) * 0));
        glEnableVertexAttribArray(3); // model col 1                                                  offsetof(SpriteVert, model[0]) should work REEEEEE
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVert), reinterpret_cast<void*>(offsetof(SpriteVert, model) + sizeof(glm::mat3::col_type) * 1));
        glEnableVertexAttribArray(4); // model col 2
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVert), reinterpret_cast<void*>(offsetof(SpriteVert, model) + sizeof(glm::mat3::col_type) * 2));
        glEnableVertexAttribArray(5); // model col 3

        glVertexAttribDivisor(0, 0);
        for(int i = 1; i <= 5; i++) {
            glVertexAttribDivisor(i, 1);
        }

        SDL_FreeSurface(data);
    }

    void SpriteGroup::draw(glm::vec4 tint, glm::mat3 model, glm::vec4 region) {
        this->verts.emplace_back(tint, model, region);
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
        this->verts.emplace_back(tint, model, region);
    }

    void SpriteGroup::draw(ShaderProgram &s) {
        if (this->verts.empty()) return;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        s.uniform("sprite_tex", 0);

        
        glBindBuffer(GL_ARRAY_BUFFER, models);
        glInvalidateBufferData(GL_ARRAY_BUFFER);
        glBufferData(GL_ARRAY_BUFFER, this->verts.size() * sizeof(SpriteVert), this->verts.data(), GL_STREAM_DRAW);

        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, this->verts.size());
        this->verts.clear();
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

    void SpriteManager::draw(ShaderProgram &s) {
        std::vector<SpriteGroup *> groups;
        groups.reserve(this->sprites.size());

        for (auto &kv : this->sprites) {
            groups.push_back(&kv.second);
        }

        std::sort(groups.begin(), groups.end(), [](const SpriteGroup *lhs, const SpriteGroup *rhs) { return lhs->order < rhs->order; });
        for(auto &x : groups) {
            x->draw(s);
        }
    }
}}
