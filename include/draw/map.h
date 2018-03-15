#pragma once
#include <vector>

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "scene/scene.h"
#include "draw/sprite.h"
#include "gl/shader.h"
#include "gl/gl_util.h"
#include "vxl.h"


namespace ace { namespace draw {
    constexpr size_t PILLAR_SIZE = 16;

    constexpr bool valid_build_pos(const int x, const int y, const int z) {
        return z < MAP_Z - 2 && is_valid_pos(x, y, z);
    }

    // i hate initializer lists
    inline glm::vec3 get_centroid(const std::vector<VXLBlock> &blocks) {
        glm::vec3 centroid;
        for (const VXLBlock &b : blocks) {
            centroid += b.position;
        }
        return glm::round(centroid / float(blocks.size()));
    }

    struct VXLBlocks {
        VXLBlocks(const std::vector<VXLBlock> &blocks) : VXLBlocks(blocks, get_centroid(blocks)) { }
        VXLBlocks(const std::vector<VXLBlock> &blocks, const glm::vec3 &center);
        void draw(const glm::mat4 &pv, ShaderProgram &s) const;

        gl::vao vao;
        gl::vbo vbo;
        GLsizei vertices;
        glm::vec3 scale, rotation, position, centroid;
    };

    struct Pillar {
        Pillar(AceMap &map, size_t x, size_t y);

        void update();
        void draw();

        bool dirty;
        AceMap &map;
        size_t x, y, vbo_size, vertices;
        gl::vao vao;
        gl::vbo vbo;
    };

    struct DrawMap : AceMap {
        DrawMap(scene::GameScene &s, const std::string &file_path);
        DrawMap(scene::GameScene &s, uint8_t *buf = nullptr);

        void update(double dt);
        void draw(const glm::vec3 &position, ShaderProgram& shader);

        bool set_point(int x, int y, int z, bool solid, uint32_t color) override;
        bool build_point(int x, int y, int z, const glm::ivec3 &color, bool force=false);
        bool destroy_point(int x, int y, int z, std::vector<VXLBlock> &destroyed);
        bool damage_point(int x, int y, int z, uint8_t damage);

        static glm::ivec3 next_block(int x, int y, int z, Face face) {
            switch(face) {
                case Face::LEFT: return { x - 1, y, z };
                case Face::RIGHT: return { x + 1, y, z };
                case Face::BACK: return { x, y - 1, z };
                case Face::FRONT: return { x, y + 1, z };
                case Face::TOP: return { x, y, z - 1 };
                case Face::BOTTOM: return { x, y, z + 1 };
                default: return {-1, -1, -1};
            }
        }

        static glm::vec3 get_face(int x, int y, int z, Face face) {
            switch (face) {
            case Face::LEFT: return { x - 0.1, y + 0.5, z + 0.5 };
            case Face::RIGHT: return { x + 1.1, y + 0.5, z + 0.5 };
            case Face::BACK: return { x + 0.5, y - 0.1, z + 0.5 };
            case Face::FRONT: return { x + 0.5, y + 1.1, z + 0.5 };
            case Face::TOP: return { x + 0.5, y + 0.5, z - 0.1 };
            case Face::BOTTOM: return { x + 0.5, y + 0.5, z + 1.1 };
            default: return { -1, -1, -1 };
            }
        }

        Pillar &get_pillar(int x, int y, int z = 0);

        draw::SpriteGroup *get_overview();

        scene::GameScene &scene;
        std::vector<Pillar> pillars;
        std::vector<std::pair<double, glm::ivec3>> damage_queue;
    private:
        void gen_pillars();
    };
}}