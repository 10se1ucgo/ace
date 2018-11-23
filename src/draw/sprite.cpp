#include "draw/sprite.h"

#include "SDL_image.h"
#include "glm/gtx/matrix_transform_2d.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "util/except.h"

namespace ace { namespace draw {
    SpriteGroup::SpriteGroup(const std::string &file_name, int order) : SpriteGroup(file_name, load_image(file_name), order) {
    }

    SpriteGroup::SpriteGroup(const std::string &file_name, SDL_Surface* data, bool color_key, int order) :
        order(order),
        tex(data->w, data->h) {

        SDL_Surface *converted;
        if (color_key) {
            // AoS has some BMPs that use a color key for transparency
            // So we use a color key on the existing surface, and then blit it to a new surface with the proper format.
            SDL_SetColorKey(data, SDL_TRUE, SDL_MapRGB(data->format, 0, 0, 0));
            converted = SDL_CreateRGBSurfaceWithFormat(0, data->w, data->h, 32, SDL_PIXELFORMAT_RGBA32);
            SDL_BlitSurface(data, nullptr, converted, nullptr);
        } else {
            converted = SDL_ConvertSurfaceFormat(data, SDL_PIXELFORMAT_RGBA32, 0);
        }
        SDL_FreeSurface(data);

        if(converted == nullptr) {
            THROW_ERROR("Couldn't convert texture {0}! {1}", file_name, SDL_GetError());
        }

        this->tex.copy_from_surface(converted);
        SDL_FreeSurface(converted);

        this->init_buffer();
    }

    SpriteGroup::SpriteGroup(const std::string &file_name, gl::experimental::texture2d texture, int order):
        order(order),
        tex(std::move(texture)) {
        this->init_buffer();
    }

    void SpriteGroup::init_buffer() {
        this->vbo->emplace_back(0.0f, 0.0f);
        this->vbo->emplace_back(0.0f, this->h());
        this->vbo->emplace_back(this->w(), 0.0f);
        this->vbo->emplace_back(this->w(), this->h());
        this->vbo.upload();

        this->vao.attrib_pointer("2f", this->vbo.handle)
            .attrib_pointer("4f,4f,3x3f", this->models.handle, 1);
    }

    void SpriteGroup::draw(glm::vec4 tint, glm::mat3 model, glm::vec4 region) {
        this->models->emplace_back(tint, model, region);
    }

    void SpriteGroup::draw(glm::vec4 tint, glm::vec2 position, float rotation, glm::vec2 scale, Align align, glm::vec4 region) {
        if (tint.a <= 0.f) return;

        const glm::vec2 aligned_position(get_aligned_position(position, scale, { this->w(), this->h() }, align));

        glm::mat3 model(1.0f);
        model = translate(model, position);
        model = rotate(model, glm::radians(rotation));
        model = translate(model, aligned_position - position);
        model = glm::scale(model, scale);
        this->models->emplace_back(tint, model, region);
    }

    void SpriteGroup::flush(gl::ShaderProgram &s) {
        if (this->models->empty()) return;

        glActiveTexture(GL_TEXTURE0);
        this->tex.bind();
        s.uniform("sprite_tex", 0);

        this->models.upload();

        this->vao.draw_instanced(GL_TRIANGLE_STRIP, 4, this->models.draw_count);
    }

    void SpriteGroup::set_antialias(bool antialias) {
        this->tex.set_filter_mode(antialias ? GL_LINEAR : GL_NEAREST);
    }

    // YIKES
    // this is a C++17 feature so lemme reimplement it here real quick
    template<typename TMap, typename TKey, typename TValue>
    std::pair<typename TMap::iterator, bool> insert_or_assign(TMap &map, TKey &&key, TValue &&value) {
        auto it = map.find(key);
        if (it == map.end())
            return (map.emplace(std::forward<TKey>(key), std::forward<TValue>(value)));
        it->second = std::forward<TValue>(value);
        return (std::pair<typename TMap::iterator, bool>(it, false));
    }

    SpriteGroup *SpriteManager::get(const std::string &name) {
        try {
            return &this->sprites.at(name);
        } catch (std::out_of_range &) {
            return &this->sprites.emplace(name, get_resource_path("png/" + name)).first->second;
        }
    }

    SpriteGroup *SpriteManager::get(const std::string &name, SDL_Surface *data) {
        return &insert_or_assign(this->sprites, name, SpriteGroup{ name, data }).first->second;
    }

    SpriteGroup *SpriteManager::get(const std::string &name, gl::experimental::texture2d tex) {
        return &insert_or_assign(this->sprites, name, SpriteGroup{ name, std::move(tex) }).first->second;
    }

    void SpriteManager::flush(gl::ShaderProgram &s) {
        std::vector<SpriteGroup *> groups;
        groups.reserve(this->sprites.size());

        for (auto &kv : this->sprites) {
            groups.push_back(&kv.second);
        }

        std::sort(groups.begin(), groups.end(), [](const SpriteGroup *lhs, const SpriteGroup *rhs) { return lhs->order < rhs->order; });
        for(auto &x : groups) {
            x->flush(s);
        }
//        for (auto &x : sorted_view(this->sprites.begin(), this->sprites.end(), [](auto *lhs, auto *rhs) { return lhs->second.order < rhs->second.order; })) {
//            x->second.draw(s);
//        }
    }
}}
