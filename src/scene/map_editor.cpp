#ifdef USE_PYBIND11
#include "scene/map_editor.h"
#include "game_client.h"
#include <cctype>
#include <queue>
#include "vxl.h"

using namespace ace::gl::literals;
namespace py = pybind11;

namespace ace { namespace scene {
    namespace {
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
    }

    MapEditor::MapEditor(GameClient &client, const std::string &map_name) :
        Scene(client),
        cam(*this, {256, 0, 256}, {0, -1, 0}),
        map(read_file(map_name).get()),
        map_renderer(this->map),
        fonts(this->client.fonts),
        uniforms(this->client.shaders->create_ubo<Uniforms3D>("SceneUniforms")),
        projection_2d(glm::ortho<float>(0.f, this->client.width(), this->client.height(), 0.0f)), scripts(py::module::import("scripts").attr("ScriptLoader")(this)) {

        this->cam.thirdperson = true;
        this->cam.set_projection(75.0f, this->client.width(), this->client.height(), 0.1f, 128.f);

        glClearColor(1, 1, 1, 1.0f);
        this->uniforms->light_pos = normalize(glm::vec3{-0.16, 0.8, 0.56});
        
        this->uniforms->fog_start = 64.f;
        this->uniforms->fog_end = 128.f;

        // this->color = {random::random(0, 255), random::random(0, 255), random::random(0, 255)};
    }

    MapEditor::~MapEditor() {
        script_hook_run("unload_scripts");
    };

    void MapEditor::start() {
        this->client.set_exclusive_mouse(true);
        script_hook_run("load_scripts", false);
    }

    void MapEditor::update(double dt) {
        Scene::update(dt);

        glm::ivec3 hit;
        this->map.hitscan(draw2vox(this->cam.position), draw2vox(this->cam.forward), &hit);
        glm::u8vec3 color(this->map.get_color(hit.x, hit.y, hit.z));
        
        auto fnt = this->client.fonts.get("Vera.ttf", 18);
        // auto p = fnt->draw(fmt::format("Cursor: {}\n", hit), glm::vec2{ 30, 10 }, draw::colors::red(), { 1, 1 }, draw::Align::TOP_LEFT);
        // p = fnt->draw(fmt::format("Color@Cursor: {}\n", color), p, glm::vec3(color) / 255.f);
        // p = fnt->draw(fmt::format("Tool: {}\n", this->get_tool().name()), p, draw::colors::red());
        // p = fnt->draw(fmt::format("Tool Mode: {}\n", this->get_tool().build_mode ? "Build" : "Delete"), p, draw::colors::red());
        // p = fnt->draw(fmt::format("Tool Color: {}\n", this->color), p, glm::vec3(this->color) / 255.f);

        if(this->client.text_input_active()) {
            auto &bfr(this->client.input_buffer);
            fnt->draw(fmt::format("Enter Command:\n{}_{}", bfr.substr(0, this->client.input_cursor), bfr.substr(this->client.input_cursor)), glm::vec2(30,  500));
        }

        script_hook_run("update", dt);

        this->cam.set_projection(75.0f, this->client.width(), this->client.height(), 0.1f, this->uniforms->fog_end + 16);
        this->cam.thirdperson = !this->client.text_input_active();
        this->cam.update(dt, this->uniforms.data);
    }

    void MapEditor::draw() {
        // this->uniforms->fog_color = glm::vec3(this->color) / 255.f;

        glClearColor(this->uniforms->fog_color.r, this->uniforms->fog_color.g, this->uniforms->fog_color.b, 1.0f);
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
        this->client.shaders->map.uniforms("model"_u = glm::mat4(1.0), "replacement_color"_u = glm::vec3(0.f), "filter_color"_u = glm::vec3(0.f), "alpha"_u = 1.0f);
        this->map_renderer.draw(this->client.shaders->map, this->cam);

        glClear(GL_DEPTH_BUFFER_BIT);

        // this->get_tool().draw();

        script_hook_run("draw");

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
        

        script_hook_run("on_key", scancode, SDL_Keymod(modifiers), pressed);

        if (!pressed) return;

        if (scancode == SDL_SCANCODE_ESCAPE) this->client.quit();
        if (scancode == SDL_SCANCODE_R) script_hook_run("load_scripts", true);
        if (scancode == SDL_SCANCODE_T) {
            this->client.tasks.schedule(0, [this](util::Task &t) { this->client.toggle_text_input(); });
            return;
        }
        if (scancode == SDL_SCANCODE_SLASH) this->client.toggle_text_input();
    }

    void MapEditor::on_mouse_button(int button, bool pressed) {
        // this->get_tool().on_mouse_button(button, pressed);
        script_hook_run("on_mouse_button", button, pressed);
    }

    void MapEditor::on_text_finished(bool cancelled) {
        script_hook_run("on_text_finished", this->client.input_buffer, cancelled);
    }

    Face MapEditor::hitcast(glm::ivec3 *hit) const {
        return this->map.hitscan(draw2vox(this->cam.position), draw2vox(this->cam.forward), hit);
    }

}}

#endif
