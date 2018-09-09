#include "scene/block_test.h"
#include "game_client.h"

namespace ace { namespace scene {
    using namespace gl::literals;

    CornerTool::CornerTool(MapEditor &scene):
        scene(scene),
        start(-1),
        end(-1) {
    }

    void CornerTool::update(double dt) {
        if (is_valid_pos(this->start)) {
            glm::ivec3 hit;
            Face f = this->scene.hitcast(&hit);
            if (this->build_mode) hit = draw::next_block(hit.x, hit.y, hit.z, f);
            if (this->end != hit && is_valid_pos(hit)) {
                this->end = hit;
                this->selection_changed(this->start, this->end);
            }
        }
    }

    void CornerTool::on_mouse_button(int button, bool pressed) {
        glm::ivec3 hit;
        Face f = this->scene.hitcast(&hit);
        if (f == Face::INVALID) return;

        if (button == SDL_BUTTON_LEFT) {
            if (this->build_mode) hit = draw::next_block(hit.x, hit.y, hit.z, f);

            if (!is_valid_pos(hit)) return;

            if (pressed) {
                this->start = hit;
                this->selection_start(hit);
            } else {
                this->selection_done(this->start, hit);
                this->start = this->end = glm::ivec3(-1);
            }
        }
    }

    BlockLineTool::BlockLineTool(MapEditor &scene):
        CornerTool(scene),
        outline(std::vector<VXLBlock>{VXLBlock{{0, 0, 0}, 0xFF000000}}) {
    }

    void BlockLineTool::draw() {
        if (!this->is_selecting()) return;

        gl::ShaderProgram &s = this->scene.client.shaders->map;
        s.bind();
        s.uniform("alpha", 0.6f);
        s.uniform("replacement_color", this->build_mode ? glm::vec3(this->scene.color) / 255.f : glm::vec3(0));

        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        this->outline.draw(s);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    void BlockLineTool::selection_changed(glm::ivec3 start, glm::ivec3 end) {
        this->update_outline(start, end);
    }

    void BlockLineTool::selection_done(glm::ivec3 start, glm::ivec3 end) {
        this->update_outline(start, end, true);
    }

    void BlockLineTool::update_outline(glm::ivec3 start, glm::ivec3 end, bool do_build) {
        auto blocks(ace::block_line(start, end));

        if(do_build) {
            for (auto &pos : blocks) {
                this->scene.map.set_block(pos.x, pos.y, pos.z, this->build_mode, pack_bytes(0x7F, scene.color.r, scene.color.g, scene.color.b));
            }

            this->outline.position = { -1, -1, -1 };
            return;
        }

        std::vector<VXLBlock> d_blocks;
        for (auto &pos : blocks) {
            d_blocks.push_back({ pos, 0xFF000000 });
        }
        this->outline.update(d_blocks, end);
        this->outline.position = vox2draw(end);
    }

    BlockBoxTool::BlockBoxTool(MapEditor &scene):
        CornerTool(scene)
        /* outline(std::vector<VXLBlock>{VXLBlock{{0, 0, 0}, 0xFF000000}}) */ {
    }

    void BlockBoxTool::draw() {
        if (!this->is_selecting()) return;

        this->scene.debug.draw_cube(this->center, this->size, draw::colors::red());

        // gl::ShaderProgram &s = this->scene.client.shaders->map;
        // s.bind();
        // s.uniform("alpha", 0.6f);
        // s.uniform("replacement_color", glm::vec3(this->scene.color) / 255.f);
        //
        // glEnable(GL_BLEND);
        // glDepthMask(GL_FALSE);
        // this->outline.draw(s);
        // glDepthMask(GL_TRUE);
        // glDisable(GL_BLEND);
    }

    void BlockBoxTool::selection_changed(glm::ivec3 start, glm::ivec3 end) {
        this->update_outline(start, end, false);
    }

    void BlockBoxTool::selection_done(glm::ivec3 start, glm::ivec3 end) {
        this->update_outline(start, end, true);
    }

    void BlockBoxTool::update_outline(glm::ivec3 start, glm::ivec3 end, bool do_build) {

       

        auto low = glm::min(start, end);
        auto high = glm::max(start, end);

        if (do_build) {

            for (int x = low.x; x <= high.x; x++) {
                for (int y = low.y; y <= high.y; y++) {
                    for (int z = low.z; z <= high.z; z++) {
                        this->scene.map.set_block(x, y, z, this->build_mode, pack_bytes(0x7F, scene.color.r, scene.color.g, scene.color.b));
                    }
                }
            }
            return;
        }

        auto siz = high - low;

        this->center = vox2draw(glm::vec3(low) + (glm::vec3(siz) / 2.f) + 0.5f);
        this->size = vox2draw((glm::vec3(siz) + glm::vec3(1.f)));

        // TODO: this is slow because im bad
        // TODO: Perhaps a box mesh struct that just makes one huge box vs individual cubes
        // blocks.reserve(siz.x * 2 + siz.y * 2 + siz.z * 2);
        //
        //
        // for(int x = low.x; x <= high.x; x++) {
        //     for(int y = low.y; y <= high.y; y++) {
        //         blocks[{ x, y, low.z }] = 0xFF000000;
        //         blocks[{ x, y, high.z }] = 0xFF000000;
        //     }
        //
        //     for (int z = low.z; z <= high.z; z++) {
        //         blocks[{ x, low.y, z }] = 0xFF000000;
        //         blocks[{ x, high.y, z }] = 0xFF000000;
        //     }
        // }
        //
        // for (int y = low.y; y <= high.y; y++) {
        //     for (int z = low.z; z <= high.z; z++) {
        //         blocks[{ low.x, y, z }] = 0xFF000000;
        //         blocks[{ high.x, y, z }] = 0xFF000000;
        //     }
        // }
        //
        //
        //
        // // for(int x = low.x; x <= high.x; x++) {
        // //     for(int y = low.y; y <= high.y; y++) {
        // //         for (int z = low.z; z <= high.z; z++) {
        // //
        // //             if(x == low.x || x == high.x || y == low.y || y == high.y || z == low.z || z == high.z)
        // //                 d_blocks.push_back({ {x, y, z}, 0xFF000000 });
        // //         }
        // //     }
        // // }
        //
        // this->outline.update(blocks, end);
        // this->outline.position = vox2draw(end);
        // this->blocks.clear();
    }

    std::unique_ptr<uint8_t[]> read_file(const std::string &file_path) {
        FILE *f = fopen(file_path.c_str(), "rb");
        if (!f) THROW_ERROR("COULD NOT READ MAP FILE {}\n", file_path);

        fseek(f, 0, SEEK_END);
        long len = ftell(f);
        rewind(f);
        auto buf = std::make_unique<uint8_t[]>(len);
        fread(buf.get(), len, 1, f);
        fclose(f);
        return buf;
    }

    MapEditor::MapEditor(GameClient &client, const std::string &map_name) :
        Scene(client),
        cam(*this, {256, 0, 256}, {0, -1, 0}),
        map(read_file(map_name).get()),
        map_renderer(this->map),
        uniforms(this->client.shaders->create_ubo<Uniforms3D>("SceneUniforms")),
        blt(*this),
        bbt(*this),
        projection_2d(glm::ortho<float>(0.f, this->client.width(), this->client.height(), 0.0f)) {

        this->cam.thirdperson = true;
        this->cam.set_projection(75.0f, this->client.width(), this->client.height(), 0.1f, 128.f);

        glClearColor(1, 1, 1, 1.0f);
        this->uniforms->light_pos = normalize(glm::vec3{-0.16, 0.8, 0.56});
        this->uniforms->fog_color = glm::vec3{1, 1, 1};

        this->color = { random::random(0, 255), random::random(0, 255), random::random(0, 255) };
    }

    void MapEditor::start() {
        this->client.set_exclusive_mouse(true);
    }

    void MapEditor::update(double dt) {
        Scene::update(dt);

        this->get_tool().update(dt);

        glm::ivec3 hit;
        this->map.hitscan(draw2vox(this->cam.position), draw2vox(this->cam.forward), &hit);
        glm::u8vec3 color = unpack_argb(this->map.get_color(hit.x, hit.y, hit.z));
        
        auto fnt = this->client.fonts.get("Vera.ttf", 18);
        auto p = fnt->draw(fmt::format("Cursor: {}\n", hit), glm::vec2{ 30, 10 }, draw::colors::red(), { 1, 1 }, draw::Align::TOP_LEFT);
        p = fnt->draw(fmt::format("Color@Cursor: {}\n", color), p, glm::vec3(color) / 255.f);
        p = fnt->draw(fmt::format("Tool: {}\n", this->get_tool().name()), p, draw::colors::red());
        p = fnt->draw(fmt::format("Tool Mode: {}\n", this->get_tool().build_mode ? "Build" : "Delete"), p, draw::colors::red());
        p = fnt->draw(fmt::format("Tool Color: {}\n", this->color), p, glm::vec3(this->color) / 255.f);

        this->cam.update(dt, this->uniforms);
    }

    void MapEditor::draw() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 3d
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);


        this->uniforms.upload();

        this->client.shaders->map.bind();
        this->client.shaders->map.uniform("model"_u = glm::mat4(1.0), "alpha"_u = 1.0f, "replacement_color"_u = glm::vec3(0.f));
        this->map_renderer.draw(this->client.shaders->map, this->cam);

        glClear(GL_DEPTH_BUFFER_BIT);

        this->get_tool().draw();

        this->client.shaders->line.bind();
        this->debug.flush(this->cam.matrix(), this->client.shaders->line);

        // 2d
        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        this->client.sprites.get("target.png")->draw({ 1, 1, 1, 1 }, this->client.size() / 2.f, 0, { 1, 1 }, draw::Align::CENTER);

        this->client.shaders->sprite.bind();
        this->client.shaders->sprite.uniform("projection", this->projection_2d);
        this->client.sprites.flush(this->client.shaders->sprite);

        this->client.shaders->text.bind();
        this->client.fonts.flush(this->projection_2d, this->client.shaders->text);
    }

    void MapEditor::on_key(SDL_Scancode scancode, int modifiers, bool pressed) {
        if (!pressed) return;

        if (scancode == SDL_SCANCODE_ESCAPE) this->client.quit();
        if (scancode == SDL_SCANCODE_R) this->color = { random::random(0, 255), random::random(0, 255), random::random(0, 255) };
        if (scancode == SDL_SCANCODE_B) this->get_tool().build_mode = !this->get_tool().build_mode;
        if (scancode == SDL_SCANCODE_T) this->tool = !this->tool;
    }

    void MapEditor::on_mouse_button(int button, bool pressed) {
        this->get_tool().on_mouse_button(button, pressed);
    }

    CornerTool &MapEditor::get_tool() {
        // TODO obviously scale this to multiple tools
        if(this->tool) {
            return this->bbt;
        } else {
            return this->blt;
        }
    }

    Face MapEditor::hitcast(glm::ivec3 *hit) const {
        return this->map.hitscan(draw2vox(this->cam.position), draw2vox(this->cam.forward), hit);
    }

}}
