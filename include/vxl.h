#pragma once
#include <bitset>
#include <cstdint>
#include <vector>
#include <unordered_set>
#include <random>

#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"


//struct Pos3 {
//    int x, y, z;
//};

constexpr size_t MAP_X = 512;
constexpr size_t MAP_Y = 512;
constexpr size_t MAP_Z = 64;
constexpr uint32_t DEFAULT_COLOR = 0xFF674028;

constexpr size_t get_pos(const int x, const int y, const int z) {
    return x + (y * MAP_Y) + (z * MAP_X * MAP_Y);
}

constexpr bool is_valid_pos(const int x, const int y, const int z) {
    return x >= 0 && x < MAP_X && y >= 0 && y < MAP_Y && z >= 0 && z < MAP_Z;
}

constexpr bool is_valid_pos(const int pos) {
    return pos >= get_pos(0, 0, 0) && pos <= get_pos(MAP_X - 1, MAP_Y - 1, MAP_Z - 1);
}

constexpr uint32_t pack_color(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return uint32_t(a) << 24 | uint32_t(r) << 16 | uint32_t(g) << 8 | uint32_t(b) << 0;
}

constexpr void unpack_color(uint32_t color, uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b) {
    *a = color >> 24 & 0xFF; // lighting byte; I USE IT FOR BLOCK DAMAGE
    *r = color >> 16 & 0xFF;
    *g = color >>  8 & 0xFF;
    *b = color >>  0 & 0xFF;
}

struct VXLBlock {
    glm::ivec3 position;
    uint32_t color;
    uint8_t vis = 255;
};

enum class Face : int {
    INVALID = -1,
    LEFT,
    RIGHT,
    BACK,
    FRONT,
    TOP,
    BOTTOM
};

class AceMap {
public:
    AceMap(uint8_t *buf = nullptr);
    virtual ~AceMap() = default;

    void read(uint8_t *buf);
    std::vector<uint8_t> write();
    size_t write(std::vector<uint8_t> &v, int *sx, int *sy, int columns = -1);

    bool is_surface(int x, int y, int z);
    bool get_solid(int x, int y, int z, bool wrapped = false) const;
    uint32_t get_color(int x, int y, int z, bool wrapped = false);
    int get_z(int x, int y, int start = 0) const;
    void get_random_point(int *x, int *y, int *z, int x1, int y1, int x2, int y2);

    std::vector<glm::ivec3> block_line(const glm::ivec3 start, const glm::ivec3 end) const {
        return this->block_line(start.x, start.y, start.z, end.x, end.y, end.z);
    }
    std::vector<glm::ivec3> block_line(int x1, int y1, int z1, int x2, int y2, int z2) const;

    virtual bool set_point(int x, int y, int z, bool solid, uint32_t color = 0);
    bool set_point(size_t pos, bool solid, uint32_t color);
    bool check_node(int x, int y, int z, bool destroy, std::vector<VXLBlock> &destroyed);

    bool cast_ray(const glm::vec3 &position, const glm::vec3 &direction, long *x, long *y, long *z, float length = 32, bool isdirection=true) const;
    Face hitscan(const glm::dvec3 &p, const glm::dvec3 &d, glm::ivec3 *h) const;

    bool clipworld(long x, long y, long z) const;
    bool clipbox(long x, long y, long z) const;

    void add_neighbors(std::vector<glm::ivec3> &v, const int x, const int y, const int z) const {
        this->add_node(v, x, y, z - 1);
        this->add_node(v, x, y - 1, z);
        this->add_node(v, x, y + 1, z);
        this->add_node(v, x - 1, y, z);
        this->add_node(v, x + 1, y, z);
        this->add_node(v, x, y, z + 1);
    }

    uint8_t get_vis(const int x, const int y, const int z, bool wrapped=false) const {
        if (!this->get_solid(x, y, z, wrapped)) return 0;

        uint8_t vis = 0;
        if (!this->get_solid(x - 1, y, z, wrapped)) vis |= 1 << int(Face::LEFT);
        if (!this->get_solid(x + 1, y, z, wrapped)) vis |= 1 << int(Face::RIGHT);
        if (!this->get_solid(x, y - 1, z, wrapped)) vis |= 1 << int(Face::BACK);
        if (!this->get_solid(x, y + 1, z, wrapped)) vis |= 1 << int(Face::FRONT);
        if (!this->get_solid(x, y, z - 1, wrapped)) vis |= 1 << int(Face::TOP);
        if (!this->get_solid(x, y, z + 1, wrapped)) vis |= 1 << int(Face::BOTTOM);
        return vis;
    }

    int sunblock(int x, int y, int z) const {
        int dec = 18;
        int i = 127;

        while (dec && z) {
            if (this->get_solid(x, --y, --z, true))
                i -= dec;
            dec -= 2;
        }
        return i;
    }
protected:
    std::bitset<MAP_X * MAP_Y * MAP_Z> geometry;
    uint32_t colors[MAP_X * MAP_Y * MAP_Z];

    std::vector<glm::ivec3> nodes;
    std::unordered_set<glm::ivec3> marked;

    std::default_random_engine eng;

    void add_node(std::vector<glm::ivec3> &v, const int x, const int y, const int z) const {
        if (this->get_solid(x, y, z))
            v.emplace_back(x, y, z);
    }
};


// int check_node(int x, int y, int z, AceMap * map, int destroy);