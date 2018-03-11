#pragma once
#include <string>

#include "glm/glm.hpp"
#include "glad/glad.h"

#include "common.h"
#include "gl/shader.h"
#include "gl/gl_obj.h"


struct KV6Mesh {
    KV6Mesh(const std::string &name);

    void draw() const {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, this->vertices);
    }

    ace::gl::vao vao;
    ace::gl::vbo vbo;
    GLsizei vertices;

    long xsiz, ysiz, zsiz, num_voxels;
    float xpiv, ypiv, zpiv;
};


struct KV6 {
    KV6(KV6Mesh *mesh, float scale=0.1f): scale(scale), rotation(0), position(0),
                                          local_scale(scale), local_rotation(0), local_position(0), mesh(mesh) {
    }

    bool sprhitscan(glm::vec3 p0, glm::vec3 v0, glm::vec3 *h);

    glm::mat4 get_model() const {
        return ace::model_matrix(position, rotation, scale);
    }

    glm::mat4 get_local_model() const {
        glm::vec3 rot(local_rotation);
        rot.y += 180;
        return ace::model_matrix(local_position, rot, local_scale);
    }

    void draw(const glm::mat4 &pv, ShaderProgram &s) const {
        const glm::mat4 model(this->get_model());
        s.uniform("mvp", pv * model);
        s.uniform("normal_matrix", glm::mat3(transpose(inverse(model))));

        this->mesh->draw();
    }

    void draw_local(const glm::mat4 &p, ShaderProgram &s) const {
        s.uniform("mvp", p * this->get_local_model());
        s.uniform("normal_matrix", glm::mat3(transpose(inverse(this->get_model())))); // for lighting and stuff. WHAT AM I DOING
        this->mesh->draw();
    }

    glm::vec3 scale, rotation, position;
    glm::vec3 local_scale, local_rotation, local_position; // what the hELL am i doing
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


