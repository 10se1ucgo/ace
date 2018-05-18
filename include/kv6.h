#pragma once
#include <string>

#include "glm/glm.hpp"
#include "glad/glad.h"

#include "common.h"
#include "gl/shader.h"
#include "gl/gl_util.h"

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

struct KV6Mesh {
    KV6Mesh(const std::string &name);

    void draw() const {
        this->vao.draw(GL_TRIANGLES, this->vbo.draw_count);
    }

    ace::gl::experimental::vao vao;
    ace::gl::experimental::vbo<detail::KV6Vertex> vbo;

    long xsiz, ysiz, zsiz, num_voxels;
    float xpiv, ypiv, zpiv;
};


struct KV6 {
    KV6(KV6Mesh *mesh, float scale=0.1f): scale(scale), rotation(0), position(0),
                                          lighting_rotation(0), mesh(mesh) {
    }

    bool sprhitscan(glm::vec3 p0, glm::vec3 v0, glm::vec3 *h);

    glm::mat4 get_model() const {
        return ace::model_matrix(position, rotation, scale);
    }

    glm::mat4 get_local_model() const {
        glm::vec3 rot(rotation);
        rot.y += 180;
        return ace::model_matrix(position, rot, scale);
    }

    void draw(const glm::mat4 &pv, ace::gl::ShaderProgram &s) const {
        const glm::mat4 model(this->get_model());
        s.uniform("model", model);
        s.uniform("normal_matrix", glm::mat3(transpose(inverse(model))));
        s.uniform("local", false);
        this->mesh->draw();
    }

    void draw_local(const glm::mat4 &p, ace::gl::ShaderProgram &s) const {
        s.uniform("model", this->get_local_model());
        s.uniform("normal_matrix", glm::mat3(transpose(inverse(ace::model_matrix(position, lighting_rotation, scale))))); // for lighting and stuff. WHAT AM I DOING
        s.uniform("local", true);
        this->mesh->draw();
    }

    glm::vec3 scale, rotation, position, lighting_rotation;
    KV6Mesh *mesh;
};


struct KV6Manager {
    KV6Mesh *get(const std::string &name) {
        try {
            return &models.at(name);
        } catch (std::out_of_range &) {
//            auto x = std::make_unique<KV6Mesh>("kv6/" + name);
            return &models.emplace(name, "kv6/" + name).first->second;
        }
    }
private:
    std::unordered_map<std::string, KV6Mesh> models;
};


