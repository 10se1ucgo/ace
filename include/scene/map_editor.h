#pragma once
#ifdef USE_PYBIND11
#include "draw/font.h"
#include "scene/scene.h"
#include "camera.h"
#include "draw/kv6.h"
#include "draw/map.h"
#include "draw/debug.h"

#include "glm/gtx/component_wise.hpp"

#include "bindings.h"
#include <pybind11/embed.h>
#undef snprintf // pyerrors.h why!!!!!!!

namespace ace { namespace scene {
    std::unordered_map<glm::ivec3, glm::u8vec3> kv6_to_blocks(const draw::KV6Data &data);

    struct BlockSelection {
        BlockSelection(EditableMap &map) : map(map) {
        }
        virtual ~BlockSelection() = default;

        virtual const std::vector<glm::ivec3> &get_blocks() = 0;

        virtual void build(glm::u8vec3 color) {
            for (const auto &pos : this->get_blocks())
                this->map.set_block(pos.x, pos.y, pos.z, true, glm::u8vec4(color, 0x7F));
        }

        virtual void build(glm::u8vec3 color, Face f, int depth = 1) {
            depth = glm::max(1, depth);
            for (auto pos : this->get_blocks()) {
                for (int i = 0; i < depth; i++) {
                    pos = draw::next_block(pos.x, pos.y, pos.z, f);
                    this->map.set_block(pos.x, pos.y, pos.z, true, glm::u8vec4(color, 0x7F));
                }
            }
        }

        virtual void destroy() {
            for (const auto &pos : this->get_blocks())
                this->map.set_solid(pos.x, pos.y, pos.z, false);
        }

        virtual void destroy(Face f) {
            for (auto pos : this->get_blocks()) {
                pos = draw::next_block(pos.x, pos.y, pos.z, f);
                this->map.set_solid(pos.x, pos.y, pos.z, false);
            }
        }

        virtual void paint(glm::u8vec3 color) {
            for (const auto &pos : this->get_blocks())
                this->map.set_color(pos.x, pos.y, pos.z, glm::u8vec4(color, 0x7F));
        }

        virtual void paint(glm::u8vec3 color, Face f) {
            for (auto pos : this->get_blocks()) {
                pos = draw::next_block(pos.x, pos.y, pos.z, f);
                this->map.set_color(pos.x, pos.y, pos.z, glm::u8vec4(color, 0x7F));
            }
        }

        EditableMap &map;
    };

    struct LineSelection final : BlockSelection {
        using BlockSelection::BlockSelection;
        LineSelection(EditableMap &map, glm::ivec3 start, glm::ivec3 end): BlockSelection(map) {
            this->set(start, end);
        }

        void set(glm::ivec3 start, glm::ivec3 end) {
            this->start = start;
            this->end = end;
            this->blocks.clear();
        }

        const std::vector<glm::ivec3> &get_blocks() override {
            if(this->blocks.empty()) {
                block_line(this->start, this->end, this->blocks);
            }

            return this->blocks;
        }

    private:
        glm::ivec3 start{0}, end{0};
        std::vector<glm::ivec3> blocks;
    };

    struct BoxSelection : BlockSelection {
        using BlockSelection::BlockSelection;
        BoxSelection(EditableMap &map, glm::ivec3 low, glm::ivec3 high): BlockSelection(map) {
            this->set(low, high);
        }

        void set(glm::ivec3 low, glm::ivec3 high) {
            this->low = low;
            this->high = high;
            this->blocks.clear();
        }

        const std::vector<glm::ivec3> &get_blocks() override {
            // TODO: clean way to lazily generate these
            if (this->blocks.empty()) {
                this->blocks.reserve(glm::compMul(this->high - this->low));
                for (int x = this->low.x; x <= this->high.x; x++) {
                    for (int y = this->low.y; y <= this->high.y; y++) {
                        for (int z = this->low.z; z <= this->high.z; z++) {
                            this->blocks.emplace_back(x, y, z);
                        }
                    }
                }
            }

            return this->blocks;
        }

    private:
        glm::ivec3 low{0}, high{0};
        std::vector<glm::ivec3> blocks;
    };

    struct FaceSelection : BlockSelection {
        using BlockSelection::BlockSelection;

        void set(glm::ivec3 pos, Face f);

        const std::vector<glm::ivec3> &get_blocks() override {
            return this->blocks;
        }
    private:
        void add_node(glm::ivec3 pos, Face f) {
            auto next = draw::next_block(pos.x, pos.y, pos.z, f);
            if (this->map.is_solid(pos.x, pos.y, pos.z) && !this->map.is_solid(next.x, next.y, next.z))
                this->nodes.emplace_back(pos);
        }

        std::vector<glm::ivec3> nodes, blocks;
        std::unordered_set<glm::ivec3> marked;
    };

    class MapEditor final : public Scene {
    public:
        MapEditor(GameClient &client, const std::string &map_name = "vxl/lastsav.vxl");
        ~MapEditor();

        void start() override;
        void update(double dt) override;
        void draw() override;

        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override;
        void on_mouse_button(int button, bool pressed) override;
        void on_mouse_scroll(int vertical, int horizontal) override;

        void on_text_finished(bool cancelled) override;

        Face hitcast(glm::ivec3 *hit) const;

        void save_to(const std::string &file_name) {
            FILE *f = fopen(file_name.c_str(), "wb");
            std::vector<uint8_t> v(this->map.write());
            fwrite(v.data(), 1, v.size(), f);
            fclose(f);
        }

        template<typename... TArgs>
        void script_hook_run(const char *hook_name, TArgs &&...args) {
            try {
                this->script_manager.attr(hook_name)(std::forward<TArgs>(args)...);
            } catch (pybind11::error_already_set &e) {
                fmt::print(stderr, "Error in Python script hook!\n{}", e.what());
            }
        }

        Camera cam;

        EditableMap map;
        draw::MapRenderer map_renderer;

        draw::DebugDraw debug;
        draw::FontManager &fonts;
        gl::experimental::ubo<Uniforms3D> uniforms;

        glm::mat4 projection_2d;

        pybind11::scoped_interpreter interpreter;
        pybind11::object script_manager;
    };
}}
#endif