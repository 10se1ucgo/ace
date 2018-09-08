#include "scene/block_test.h"
#include "game_client.h"

namespace ace { namespace scene {
    using namespace gl::literals;

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

    BlockTest::BlockTest(GameClient &client, const std::string &map_name) :
        Scene(client),
        cam(*this, { 256, 0, 256 }, { 0, -1, 0 }),
        map(read_file(map_name).get()),
        map_renderer(this->map),
        uniforms(this->client.shaders->create_ubo<Uniforms3D>("SceneUniforms")) {

        this->cam.thirdperson = true;
        this->cam.set_projection(75.0f, this->client.width(), this->client.height(), 0.1f, 128.f);

        glClearColor(1, 1, 1, 1.0f);
        this->uniforms->light_pos = normalize(glm::vec3{ -0.16, 0.8, 0.56 });
        this->uniforms->fog_color = glm::vec3{1, 1, 1};

        this->projection_2d = glm::ortho<float>(0.f, this->client.width(), this->client.height(), 0.0f);
    }

    void BlockTest::start() {
        this->client.set_exclusive_mouse(true);
    }

    void BlockTest::update(double dt) {
        Scene::update(dt);

        auto fnt = this->client.fonts.get("nevis.ttf", 18);

        glm::ivec3 hit;
        this->map.hitscan(draw2vox(this->cam.position), draw2vox(this->cam.forward), &hit);
        glm::u8vec3 color = unpack_argb(this->map.get_color(hit.x, hit.y, hit.z));

        fnt->draw(fmt::format("Block: {}\nColor: {}", hit, color), glm::vec2{ 30 }, draw::colors::red(), { 1, 1 }, draw::Align::TOP_LEFT);


        this->cam.update(dt, this->uniforms);
    }

    void BlockTest::draw() {
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

    void BlockTest::on_key(SDL_Scancode scancode, int modifiers, bool pressed) {
    }

    void BlockTest::on_mouse_button(int button, bool pressed) {
        if (!pressed) return;

        glm::ivec3 hit;
        Face f = this->map.hitscan(draw2vox(this->cam.position), draw2vox(this->cam.forward), &hit);
        if (f == Face::INVALID) return;

        if(button == SDL_BUTTON_LEFT) {
            hit = draw::next_block(hit.x, hit.y, hit.z, f);
        }

        this->map.set_solid(hit.x, hit.y, hit.z, button == SDL_BUTTON_LEFT);
    }

}}
