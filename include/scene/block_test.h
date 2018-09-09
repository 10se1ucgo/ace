#pragma once
#include "scene/scene.h"
#include "camera.h"
#include "draw/map.h"
#include "draw/debug.h"

namespace ace { namespace scene {
    struct MapEditor;


    // TODO Make a base EditorTool class.
    struct CornerTool {
        explicit CornerTool(MapEditor &scene);
        virtual ~CornerTool() = default;

        virtual void update(double dt);
        virtual void draw() { }

        virtual void on_mouse_button(int button, bool pressed);

        virtual bool is_selecting() const { return is_valid_pos(this->start) && is_valid_pos(this->end); }


        virtual std::string name() const = 0;

        bool build_mode{ true };
    protected:
        virtual void selection_start(glm::ivec3 start) { }
        virtual void selection_changed(glm::ivec3 start, glm::ivec3 end) { }
        virtual void selection_done(glm::ivec3 start, glm::ivec3 end) { }
        virtual void cancel() { }

        MapEditor &scene;
    private:
        glm::ivec3 start, end;
    };


    struct BlockLineTool : CornerTool {
        explicit BlockLineTool(MapEditor &scene);

        void draw() override;

        std::string name() const override { return "Line"; }
    protected:
        void selection_changed(glm::ivec3 start, glm::ivec3 end) override;
        void selection_done(glm::ivec3 start, glm::ivec3 end) override;

        void update_outline(glm::ivec3 start, glm::ivec3 end, bool do_build = false);
        
        draw::VXLBlocks outline;
    };

    struct BlockBoxTool : CornerTool {
        explicit BlockBoxTool(MapEditor &scene);

        void draw() override;

        std::string name() const override { return "Box"; }
    protected:
        void selection_changed(glm::ivec3 start, glm::ivec3 end) override;
        void selection_done(glm::ivec3 start, glm::ivec3 end) override;
        void update_outline(glm::ivec3 start, glm::ivec3 end, bool do_build = false);

        // std::unordered_map<glm::ivec3, uint32_t> blocks;
        // draw::VXLBlocks outline;

        glm::vec3 center, size;
    };


    struct MapEditor final : Scene {
    public:
        MapEditor(GameClient &client, const std::string &map_name = "normandie.vxl");

        void start() override;
        void update(double dt) override;
        void draw() override;

        void on_key(SDL_Scancode scancode, int modifiers, bool pressed) override;
        void on_mouse_button(int button, bool pressed) override;

        CornerTool &get_tool();

        Face hitcast(glm::ivec3 *hit) const;

        Camera cam;

        AceMap map;
        draw::MapRenderer map_renderer;

        draw::DebugDraw debug;

        gl::experimental::ubo<Uniforms3D> uniforms;

        BlockLineTool blt;
        BlockBoxTool bbt;
        bool tool;

        glm::mat4 projection_2d;

        glm::u8vec3 color;

        
    };
}}
