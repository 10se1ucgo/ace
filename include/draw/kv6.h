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

        void draw() const {
            this->mesh.draw();
        }

        gl::experimental::mesh<detail::KV6Vertex> mesh{ "3f,3f,3f,3f" };
        KV6Data data;
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
            const glm::mat4 model(this->get_model());
            s.uniform("model", model);
            s.uniform("normal_matrix", glm::mat3(transpose(inverse(model))));
            s.uniform("local", false);
            this->mesh->draw();
        }

        void draw_local(ace::gl::ShaderProgram &s, bool use_lighting_rotation = true) const {
            const glm::mat4 m(this->get_local_model());
            const glm::mat3 nm(transpose(inverse(use_lighting_rotation ? ace::model_matrix(position, lighting_rotation, scale) : m)));


            s.uniform("model", m);
            s.uniform("normal_matrix", nm); // for lighting and stuff. WHAT AM I DOING
            s.uniform("local", true);
            this->mesh->draw();
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

