#pragma once
#include <string>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "gl/shader.h"
#include <memory>
#include <utility>
#include "common.h"

struct KV6Mesh {
    KV6Mesh(const std::string &name);
    // KV6Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices);
    ~KV6Mesh() {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }


    void draw() const {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, this->vertices);
    }

    GLuint vao, vbo;
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
        return model_matrix(position, rotation, scale);
    }

    glm::mat4 get_local_model() const {
        glm::vec3 rot(local_rotation);
        rot.y += 180;
        return model_matrix(local_position, rot, local_scale);
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
    KV6Mesh *get(const std::string &name);
private:
    std::unordered_map<std::string, std::unique_ptr<KV6Mesh>> models;
};


