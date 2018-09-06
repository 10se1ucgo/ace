#pragma once
#include <bitset>
#include <cstdint>
#include <vector>
#include <random>
#include <unordered_map>
#include <unordered_set>

#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"



//struct Pos3 {
//    int x, y, z;
//};
namespace ace {
    constexpr int MAP_X = 512;
    constexpr int MAP_Y = 512;
    constexpr int MAP_Z = 64;
    using coord_t = uint32_t;
    constexpr uint32_t DEFAULT_COLOR = 0xFF674028;

    constexpr size_t get_pos(const int x, const int y, const int z) {
        return x + (y * MAP_Y) + (z * MAP_X * MAP_Y);
    }

    constexpr bool is_valid_pos(const int x, const int y, const int z) {
        return x >= 0 && x < MAP_X && y >= 0 && y < MAP_Y && z >= 0 && z < MAP_Z;
    }

    constexpr bool is_valid_pos(const size_t pos) {
        return pos <= get_pos(MAP_X - 1, MAP_Y - 1, MAP_Z - 1);
    }

    inline bool is_valid_pos(glm::ivec3 pos) {
        return is_valid_pos(pos.x, pos.y, pos.z);
    }

    glm::u8vec3 jit_color(glm::u8vec3 color);

    struct VXLBlock {
        glm::ivec3 position;
        uint32_t color;
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

    class AceMap;

    // lets take a page out of Minecraft's book
    struct MapListener {
        virtual ~MapListener() = default;
        // do rule of 5s apply here???
        virtual void on_block_changed(int x, int y, int z, AceMap &map) = 0;
        virtual void all_changed(AceMap &map) = 0;
    };

    class AceMap {
    public:
        AceMap(uint8_t *buf = nullptr);

        void read(uint8_t *buf);
        std::vector<uint8_t> write();
        size_t write(std::vector<uint8_t> &v, int *sx, int *sy, int columns = -1);

        bool set_block(int x, int y, int z, bool solid, uint32_t color = 0, bool wrapped = false);
        bool get_block(int x, int y, int z, uint32_t *color, bool wrapped = false);

        bool set_solid(int x, int y, int z, bool solid, bool wrapped = false);
        bool is_solid(int x, int y, int z, bool wrapped = false) const;

        // FORMAT: 0xAARRGGBB
        void set_color(int x, int y, int z, uint32_t color, bool wrapped = false);
        uint32_t get_color(int x, int y, int z, bool wrapped = false);

        int get_z(int x, int y, int start = 0, bool wrapped = false);

        uint8_t get_vis(int x, int y, int z, bool wrapped = false) const {
            if (!this->is_solid(x, y, z, wrapped)) return 0;

            uint8_t vis = 0;
            if (!this->is_solid(x - 1, y, z, wrapped)) vis |= 1 << int(Face::LEFT);
            if (!this->is_solid(x + 1, y, z, wrapped)) vis |= 1 << int(Face::RIGHT);
            if (!this->is_solid(x, y - 1, z, wrapped)) vis |= 1 << int(Face::BACK);
            if (!this->is_solid(x, y + 1, z, wrapped)) vis |= 1 << int(Face::FRONT);
            if (!this->is_solid(x, y, z - 1, wrapped)) vis |= 1 << int(Face::TOP);
            if (!this->is_solid(x, y, z + 1, wrapped)) vis |= 1 << int(Face::BOTTOM);
            return vis;
        }

        Face hitscan(const glm::dvec3 &p, const glm::dvec3 &d, glm::ivec3 *h) const;

        void add_listener(MapListener &listener) {
            this->listeners.push_back(&listener);
        }
        void remove_listener(MapListener &listener) {
            this->listeners.erase(std::remove(this->listeners.begin(), this->listeners.end(), &listener), this->listeners.end());
        }
    private:
        bool is_surface(const int x, const int y, const int z) const {
            if (!this->is_solid_unchecked(get_pos(x, y, z))) return false;
            if (x     >     0 && !this->is_solid_unchecked(get_pos(x - 1, y, z))) return true;
            if (x + 1 < MAP_X && !this->is_solid_unchecked(get_pos(x + 1, y, z))) return true;
            if (y     >     0 && !this->is_solid_unchecked(get_pos(x, y - 1, z))) return true;
            if (y + 1 < MAP_Y && !this->is_solid_unchecked(get_pos(x, y + 1, z))) return true;
            if (z     >     0 && !this->is_solid_unchecked(get_pos(x, y, z - 1))) return true;
            if (z + 1 < MAP_Z && !this->is_solid_unchecked(get_pos(x, y, z + 1))) return true;
            return false;
        }

        void set_solid_unchecked(size_t pos, bool solid) {
            this->geometry[pos] = solid;
        }

        bool is_solid_unchecked(size_t pos) const {
            return this->geometry[pos];
        }

        void set_color_unchecked(size_t pos, uint32_t color) {
            this->colors[pos] = color;
        }

        // TODO: This one requires x,y,z to calculate dirt color if needed. Maybe a separate wrapper for that.
        uint32_t get_color_unchecked(size_t pos, int x, int y, int z);

        void notify_listeners(int x, int y, int z) {
            for (auto &listener : this->listeners) {
                listener->on_block_changed(x, y, z, *this);
            }
        }

        std::vector<MapListener *> listeners;

        std::bitset<MAP_X * MAP_Y * MAP_Z> geometry; // x,y,z -> is_solid?

        // ALTERNATIVE #1:
        // Just(?) as cache efficient as the std::bitset
        // But better when iterating over every block in a column
        // uint64_t geometry[MAP_X][MAP_Y] // x,y -> bitset of is_solid? from z 0->63

        // ALTERNATIVE #2:
        // Less cache efficient as above solutions (16 mb! vs just 2 mb)
        // But perhaps better for rendering lookups where visflags are needed very often.
        // uint8_t geometry[MAP_X][MAP_Y][MAP_Z] // x,y,z -> bitset of visibility flags (bits 0-5?) + is_solid at bit 6?.

        std::unordered_map<size_t, uint32_t> colors; // x,y,z -> color
    };

    std::vector<glm::ivec3> block_line(int x1, int y1, int z1, int x2, int y2, int z2);
    inline std::vector<glm::ivec3> block_line(const glm::ivec3 start, const glm::ivec3 end) {
        return block_line(start.x, start.y, start.z, end.x, end.y, end.z);
    }
}