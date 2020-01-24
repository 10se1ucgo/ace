#pragma once
#include <vector>

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "scene/scene.h"
#include "draw/sprite.h"
#include "gl/shader.h"
#include "gl/gl_util.h"
#include "vxl.h"
#include "camera.h"


namespace ace { namespace draw {
    namespace detail {
#pragma pack(push, 1)
        struct VXLVertex {
            glm::vec3 vertex;
            glm::u8vec3 color;
            GLubyte face;
            GLubyte ao{ 0 };
        };
#pragma pack(pop)
    }
    constexpr size_t PILLAR_SIZE = 16;
    constexpr auto NUM_PILLARS = (MAP_X / PILLAR_SIZE) * (MAP_Y / PILLAR_SIZE);

    constexpr bool valid_build_pos(const int x, const int y, const int z) {
        return z < MAP_Z - 2 && is_valid_pos(x, y, z);
    }

    // i hate initializer lists
    inline glm::vec3 get_centroid(const std::vector<VXLBlock> &blocks) {
        glm::vec3 centroid(0.0);
        for (const VXLBlock &b : blocks) {
            centroid += b.position;
        }
        return glm::round(centroid / float(blocks.size()));
    }

    struct VXLBlocks {
        explicit VXLBlocks(glm::u8vec3 color);
        // TODO consolidate these
        // No reason to inconsistently use uint32s or u8vec3s for color
        // Maybe also remove VXLBlock.
        VXLBlocks(const std::vector<VXLBlock> &blocks, const glm::vec3 &center) { this->update(blocks, center); }
        VXLBlocks(const std::unordered_map<glm::ivec3, uint32_t> &blocks, const glm::vec3 &center) { this->update(blocks, center); }
        VXLBlocks(const std::unordered_map<glm::ivec3, glm::u8vec3> &blocks, const glm::vec3 &center) { this->update(blocks, center); }

        void update(const std::vector<VXLBlock> &blocks, const glm::vec3 &center);
        void update(const std::unordered_map<glm::ivec3, uint32_t> &blocks, const glm::vec3 &center);
        void update(const std::unordered_map<glm::ivec3, glm::u8vec3> &blocks, const glm::vec3 &center);

        void draw(gl::ShaderProgram &s) const;

        gl::experimental::mesh<detail::VXLVertex> mesh{ "3f,3Bn,1B,1B" };
        glm::vec3 scale{ 1 }, rotation{ 0 }, position{ 0 }, centroid{ 0 };
    private:
        void _gen_faces(const glm::vec3 &position, const glm::u8vec3 &color, uint8_t visibility);
    };

    struct Pillar {
        Pillar(AceMap &map, size_t x, size_t y);

        void update(bool use_ao = true, bool use_sunblock = true);
        void draw(bool update_with_ao = true, bool use_sunblock = true);

        bool contains(glm::vec3 pos) const {
            return this->x <= pos.x && pos.x <= this->x + PILLAR_SIZE && this->y <= pos.y && pos.y <= this->y + PILLAR_SIZE;
        }

        glm::vec3 center() {
            return glm::vec3(this->x + PILLAR_SIZE / 2, MAP_Z / 2, this->y + PILLAR_SIZE / 2);
        }

        int sunblock(int x, int y, int z) const;

        bool dirty = false;
        AceMap &map;
        size_t x = 0, y = 0;
        gl::experimental::mesh<detail::VXLVertex> mesh{ "3f,3Bn,1B,1B", GL_DYNAMIC_DRAW };
        glm::mat4 model;
    };

    struct MapRenderer : MapListener {
        MapRenderer(AceMap &map, bool use_ao = true, bool use_sunblock = true);

        // TODO: DIRTY!!! how do i get the view frustum normally without being tied to the scene??
        void draw(gl::ShaderProgram &shader, Camera &camera);

        void on_block_changed(int x, int y, int z, AceMap &map) override;
        void all_changed(AceMap &map) override;
    private:
        Pillar &get_pillar(const int x, const int y, const int z) {
            int xp = (x & MAP_X - 1) / PILLAR_SIZE;
            int yp = (y & MAP_Y - 1) / PILLAR_SIZE;
            return this->pillars[xp * (MAP_Y / PILLAR_SIZE) + yp];
        }

        void gen_pillars();

        AceMap &map;
        std::vector<Pillar> pillars;
        std::array<Pillar *, NUM_PILLARS> sorted_pillars;
        bool use_ao, use_sunblock;
    };


    inline glm::ivec3 next_block(int x, int y, int z, Face face) {
        glm::ivec3 pos;
        switch(face) {
            case Face::LEFT: pos = { x - 1, y, z }; break;
            case Face::RIGHT: pos = { x + 1, y, z }; break;
            case Face::BACK: pos = { x, y - 1, z }; break;
            case Face::FRONT: pos = { x, y + 1, z }; break;
            case Face::TOP: pos = { x, y, z - 1 }; break;
            case Face::BOTTOM: pos = { x, y, z + 1 }; break;
            default: return { -1, -1, -1 };
        }
        if(is_valid_pos(pos.x, pos.y, pos.z)) {
            return pos;
        }
        return { -1, -1, -1 };
    }

    inline glm::vec3 get_face(int x, int y, int z, Face face) {
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
}}
