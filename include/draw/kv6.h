#pragma once
#include <string>

#include "glm/glm.hpp"
#include "glad/glad.h"

#include "common.h"
#include "gl/shader.h"
#include "gl/gl_util.h"

namespace ace { namespace draw {
    namespace detail {
#pragma pack(push, 1)
        struct KV6Vertex {
            glm::vec3 vertex;
            glm::vec3 color;
            glm::vec3 normal; // face normal
            glm::vec3 kv6norm; // kv6 normal
        };
#pragma pack(pop)
    }

    struct KV6Data {
        explicit KV6Data(const std::string &file);

        glm::i32vec3 size{};
        glm::vec3 pivot{};
        int32_t num_voxels{};
        size_t num_columns;

        struct KV6Entry {
            glm::u8vec3 color;
            uint16_t z_pos;
            uint8_t vis_flags, normal_index;
        };

        std::unique_ptr<KV6Entry[]> blocks;
        std::unique_ptr<uint16_t[]> column_lengths;
    };

    struct KV6Mesh {
        explicit KV6Mesh(const std::string &file);

        void draw(gl::ShaderProgram &s, const glm::mat4 &model, bool local = false) {
            this->draw(s, model, glm::mat3(transpose(inverse(model))), local);
        }

        void draw(gl::ShaderProgram &s, const glm::mat4 &model, const glm::mat3 &normal, bool local = false) {
            s.uniform("model", model);
            s.uniform("normal_matrix", normal);
            s.uniform("local", local);
            this->mesh.draw();
        }

        void draw() const {
            this->mesh.draw();
        }

        gl::experimental::mesh<detail::KV6Vertex> mesh{ "3f,3f,3f,3f" };
        const KV6Data data;
    };


    struct KV6 {
        KV6(KV6Mesh *mesh, float scale = 0.1f) : scale(scale), rotation(0), position(0),
            lighting_rotation(0), mesh(mesh) {
        }

        // Adapted from https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms/18459#18459
        // *h -> set to the (world) position of the hit (unchanged if not hit)
        bool sprhitscan(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 *h) const;

        glm::mat4 get_model() const {
            return model_matrix(position, rotation, scale);
        }

        glm::mat4 get_local_model() const {
            glm::vec3 rot(rotation);
            rot.y += 180;
            return model_matrix(position, rot, scale);
        }

        void draw(ace::gl::ShaderProgram &s) const {
            this->mesh->draw(s, this->get_model());
        }

        void draw_local(ace::gl::ShaderProgram &s, bool use_lighting_rotation = true) const {
            const glm::mat4 m(this->get_local_model());
            if (use_lighting_rotation) {
                this->mesh->draw(s, m, transpose(inverse(ace::model_matrix(position, lighting_rotation, scale))), true);
            } else {
                this->mesh->draw(s, m, true);
            }
        }

        glm::vec3 scale, rotation, position, lighting_rotation;
        KV6Mesh *mesh;
    };


    struct KV6Manager {
        KV6Mesh *get(const std::string &name) {
            try {
                return &this->models.at(name);
            } catch (std::out_of_range &) {
    //            auto x = std::make_unique<KV6Mesh>("kv6/" + name);
                return &this->models.emplace(name, get_resource_path("kv6/" + name)).first->second;
            }
        }
    private:
        std::unordered_map<std::string, KV6Mesh> models;
    };
}}