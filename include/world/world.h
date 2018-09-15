#pragma once

#include "scene/scene.h"
#include "draw/map.h"

//#include "scene/game.h"

namespace ace { namespace world {
    struct WorldObject {
        WorldObject(scene::GameScene &scene) : scene(scene) {}
        virtual ~WorldObject() = default;

        virtual void draw() = 0;
        virtual bool update(double dt) = 0;

        scene::GameScene &scene;
    };

    struct DebrisGroup;

    struct World {
        World(scene::GameScene &scene, uint8_t *buf);

        void update(double dt);
        void draw();

        bool build_block(int x, int y, int z, glm::u8vec3 color, bool force = false);
        bool destroy_block(int x, int y, int z, net::ACTION type = net::ACTION::DESTROY);
        // Damage a block, subtracting `damage` from its health. If health <= 0 after, optionally destroy. -> success
        bool damage_block(int x, int y, int z, int damage, bool allow_destroy);

        uint32_t get_color(int x, int y, int z, bool wrapped = false) {
            return this->map.get_color(x, y, z, wrapped);
        }

        bool is_solid(int x, int y, int z, bool wrapped = false) const {
            return this->map.is_solid(x, y, z, wrapped);
        }

        bool get_block(int x, int y, int z, uint32_t *color, bool wrapped = false) {
            return this->map.get_block(x, y, z, color, wrapped);
        }

        int get_z(int x, int y, int start = 0, bool wrapped = false) {
            return this->map.get_z(x, y, start, wrapped);
        }

        Face hitscan(const glm::dvec3 &p, const glm::dvec3 &d, glm::ivec3 *h) const {
            return this->map.hitscan(p, d, h);
        }

        void add_listener(MapListener &listener) { return this->map.add_listener(listener); }
        void remove_listener(MapListener &listener) { return this->map.remove_listener(listener); }

        bool clipworld(int x, int y, int z) const;
        bool clipworld(float x, float y, float z) const {
            return this->clipworld(int(std::floor(x)), int(std::floor(y)), int(std::floor(z)));
        }

        bool clipbox(int x, int y, int z) const;
        bool clipbox(float x, float y, float z) const {
            return this->clipbox(int(std::floor(x)), int(std::floor(y)), int(std::floor(z)));
        }

        template<typename TObj, typename... TArgs, typename = std::enable_if_t<std::is_base_of<world::WorldObject, TObj>::value>>
        TObj *create_object(TArgs&&... args) {
            // TODO: TRANSITION WorldObject TO TAKE World AS PARAMETER, NOT SCENE.
            // TODO: EVENTUALLY, MAKE WorldObject AND World INDEPENDENT OF GameScene!
            auto obj = std::make_unique<TObj>(this->scene, std::forward<TArgs>(args)...);
            auto *ptr = obj.get();
            // avoid creating objects in the middle of the frame
            // (especially when objects create new objects inside update)
            this->queued_objects.emplace_back(std::move(obj));
            return ptr;
        }

        void create_debris(glm::vec3 position, glm::u8vec3 color, float velocity_mod, int number) const;
    private:
        // Destroy block AND check for floating structures, adding any removed floating blocks to the `destroyed` vector. -> success
        bool destroy_block(int x, int y, int z, std::vector<VXLBlock> &destroyed);

        void add_node(std::vector<glm::ivec3> &v, const int x, const int y, const int z) const {
            if (this->map.is_solid(x, y, z))
                v.emplace_back(x, y, z);
        }

        void add_neighboring_nodes(std::vector<glm::ivec3> &v, const int x, const int y, const int z) const {
            this->add_node(v, x, y, z - 1);
            this->add_node(v, x, y - 1, z);
            this->add_node(v, x, y + 1, z);
            this->add_node(v, x - 1, y, z);
            this->add_node(v, x + 1, y, z);
            this->add_node(v, x, y, z + 1);
        }

        void check_floating(int x, int y, int z, std::vector<VXLBlock> &floating, bool destroy = true);

        void update_damaged_blocks();
        void update_objects(double dt);

        scene::GameScene &scene;

        std::vector<std::unique_ptr<world::WorldObject>> objects;
        std::vector<std::unique_ptr<world::WorldObject>> queued_objects;

        // Floating block detection
        std::vector<glm::ivec3> nodes;
        std::unordered_set<glm::ivec3> marked;

        std::vector<std::pair<double, glm::ivec3>> damaged_blocks; // {

        AceMap map;
        draw::MapRenderer map_renderer;
        DebrisGroup &debris;
    };
}}