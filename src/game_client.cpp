#include "game_client.h"

#include "SDL_image.h"
#include "nlohmann/json.hpp"

#include <fstream>

#include "scene/scene.h"

namespace ace {
    void APIENTRY gl_error(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

        fmt::print("=============O P E N G L  D E B U G  C A L L B A C K=============\n");
        fmt::print("source: {}\n", get_gl_debug_source_name(type));
        fmt::print("type: {}\n", get_gl_debug_type_name(type));
        fmt::print("id: {}\n", id);
        fmt::print("severity: {}\n", get_gl_debug_severity_name(severity));
        fmt::print("message: {}\n", message);
        fmt::print("=============O P E N G L  D E B U G  C A L L B A C K=============\n");
    }

    void print_gl_info() {
        fmt::print("OpenGL: {}\n", glGetString(GL_VERSION));
        fmt::print("GLSL: {}\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        fmt::print("Renderer: {}\n", glGetString(GL_RENDERER));
        fmt::print("Vendor: {}\n", glGetString(GL_VENDOR));
        GLint num_ext;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_ext);
        fmt::print("Extensions ({}): ", num_ext);
        for(int i = 0; i < num_ext; i++) {
            fmt::print("{},", glGetStringi(GL_EXTENSIONS, i));
        }
        fmt::print("\n");
    }

#define SDL_ERROR(msg) THROW_ERROR("{}: {}\n", msg, SDL_GetError())

    GameConfig::GameConfig() {  }

    GameConfig::GameConfig(const std::string &file_name) {
        this->read(file_name);
    }

    void GameConfig::read(const std::string &file_name) {
        std::ifstream{ file_name } >> this->json;
    }

    SDL_Scancode GameConfig::get_key(const std::string &key) {
        auto &entry(this->json["controls"].at(key));
        if (entry.is_number()) {
            return SDL_Scancode(entry.get<std::underlying_type_t<SDL_Scancode>>());
        }

        const auto &key_name = entry.get_ref<const std::string &>();
        auto it = this->keybind_to_scancode.find(key_name);
        if(it == this->keybind_to_scancode.end()) {
            return this->keybind_to_scancode[key_name] = SDL_GetScancodeFromName(key_name.c_str());
        }

        return it->second;
    }

    // void GameConfig::read() {
    // }
    //
    // void GameConfig::write() {
    // }

    GameClient::GameClient(std::string caption /*, int w, int h, WINDOW_STYLE style */):
        net(*this),
        sound(*this),
        tasks(),
        window_title(std::move(caption)) {

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
            SDL_ERROR("SDL_Init");
        if (IMG_Init(IMG_INIT_PNG) < 0)
            SDL_ERROR("IMG_Init");

        this->config.read(get_resource_path("config.json"));

        SDL_GL_LoadLibrary(nullptr);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        auto antialias = config.json["graphics"].value("antialias", 4);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, bool(antialias));
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, antialias);

        auto window_mode = config.json["graphics"].value("window_mode", "windowed");
        this->w = config.json["graphics"].value("window_width", 800);
        this->h = config.json["graphics"].value("window_height", 600);

        Uint32 flags = SDL_WINDOW_OPENGL;
        flags |= window_mode == "fullscreen" ? SDL_WINDOW_FULLSCREEN : 0;
        flags |= window_mode == "borderless" ? SDL_WINDOW_BORDERLESS : 0;
        this->window = SDL_CreateWindow(
            this->window_title.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->w, this->h,
            flags
        );
        if (this->window == nullptr)
            SDL_ERROR("SDL_CreateWindow");

        this->context = SDL_GL_CreateContext(this->window);
        if (this->context == nullptr) {
            // lower version and try again
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            this->context = SDL_GL_CreateContext(this->window);
            fmt::print("Couldn't load OpenGL 4.3, falling back to 3.3...\n");
            if (this->context == nullptr) {
                THROW_ERROR("OpenGL 3.3 or above REQUIRED! Please update your drivers and try again.\nSDL_GL_CreateContext: {}", SDL_GetError());
            }
        }

        if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
            THROW_ERROR("gladLoaderGLLoader fail");
        }

        SDL_GL_MakeCurrent(this->window, this->context);
        SDL_GL_SetSwapInterval(this->config.json["graphics"].value("vsync", false));

        if (GLAD_GL_VERSION_4_3 && this->config.json["graphics"].value("debug", false)) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(gl_error, nullptr);
        }

        print_gl_info();

        this->keyboard.keys = SDL_GetKeyboardState(&this->keyboard.numkeys);

        this->shaders = std::make_unique<gl::ShaderManager>();

        SDL_StopTextInput();
    }

    GameClient::~GameClient() {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
    }

    constexpr double FIXED_DELTA_TIMESTEP = 1.0 / 60.0;
    // One slow frame means physics has to run more, which makes next frame even slower, which means physics has to run even more
    // This caps the fixed timestep accumulator to lower the number of fixed update calls when lagging to prevent cascading lag effect 
    constexpr double MAX_FIXED_STEPS = 1.0 / 3.0;

    // TODO: Decide if I want to do this more like Unreal than Unity, just run physics in sync with frame with a cap
    // i.e.: this->scene->fixed_update(std::min(dt, MAX_PHYSICS_TIMESTEP))
    //       this->scene->update(dt);

    void GameClient::run() {
        Uint64 last = SDL_GetPerformanceCounter();
        while (!this->_quit) {
            Uint64 now = SDL_GetPerformanceCounter();
            double dt = (now - last) / double(SDL_GetPerformanceFrequency());
            this->time += dt;
            last = now;

            this->update(dt);
        }
    }


    void GameClient::update(double dt) {
        if (this->new_scene != nullptr) {
            this->tasks.tasks.clear();
            this->scene.reset();
            this->scene = std::move(this->new_scene);
            this->scene->start();
        }

        this->poll_events();
        this->update_fps();
        this->tasks.update(this->time, dt);
        this->net.update(dt);
        this->url.update(dt);
        this->sound.update(dt);

#if USE_PHYSICS_SUBSTEP
        this->fixed_update_accumulator = std::min(MAX_FIXED_STEPS, this->fixed_update_accumulator + dt);
        while (this->fixed_update_accumulator >= FIXED_DELTA_TIMESTEP) {
            // this->fixed_update(FIXED_DELTA_TIMESTEP);
            this->scene->fixed_update(FIXED_DELTA_TIMESTEP);
            // fmt::print("FIXED UPDATE: {}/{}\n", FIXED_DELTA_TIMESTEP, fixed_accum);
            this->fixed_update_accumulator -= FIXED_DELTA_TIMESTEP;

        }
#else
        // in this case fixed_update isnt exactly a good name :P
        // maybe ill call it `physics_update` or something stupid
        this->scene->fixed_update(std::min(dt, FIXED_DELTA_TIMESTEP));
#endif

        this->scene->update(dt);
        this->draw();
    }

    void GameClient::draw() const {
        this->scene->draw();
        SDL_GL_SwapWindow(this->window);
    }

    void GameClient::update_fps() {
        this->fps_counter.frames++;
        if (this->time - this->fps_counter.last_update >= 1) {
            SDL_SetWindowTitle(this->window, fmt::format("{} (FPS: {})", this->window_title, this->fps_counter.frames).c_str());
            this->fps_counter.frames = 0;
            this->fps_counter.last_update = this->time;
        }
    }

    void GameClient::poll_events() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                this->quit();
                break;
            case SDL_KEYDOWN:
                if(this->text_input_active()) {
                    switch(event.key.keysym.scancode) {
                    case SDL_SCANCODE_BACKSPACE:
                        if (!this->input_buffer.empty() && this->input_cursor > 0) {
                            this->input_buffer.erase(this->input_cursor - 1, 1);
                            this->input_cursor = std::min<size_t>(this->input_cursor - 1, this->input_buffer.size());
                        }
                        break;
                    case SDL_SCANCODE_RETURN:
                        SDL_StopTextInput();
                        this->scene->on_text_finished(false);
                        this->input_buffer.clear();
                        this->input_cursor = 0;
                        break;
                    case SDL_SCANCODE_ESCAPE:
                        SDL_StopTextInput();
                        this->scene->on_text_finished(true);
                        this->input_buffer.clear();
                        this->input_cursor = 0;
                        break;
                    case SDL_SCANCODE_LEFT:
                        if(this->input_cursor > 0)
                            this->input_cursor = std::min<size_t>(this->input_cursor - 1, this->input_buffer.size());
                        break;
                    case SDL_SCANCODE_RIGHT:
                        this->input_cursor = std::min<size_t>(this->input_cursor + 1, this->input_buffer.size());
                        break;
                    default:
                        break;
                    }
                    return;
                }
                if(!event.key.repeat) this->scene->on_key(event.key.keysym.scancode, event.key.keysym.mod, true);
                handle_key_press(event);
                break;
            case SDL_KEYUP:
                if (this->text_input_active()) return;
                if(!event.key.repeat) this->scene->on_key(event.key.keysym.scancode, event.key.keysym.mod, false);
                break;
            case SDL_WINDOWEVENT:
                handle_window_event(event);
                break;
            case SDL_MOUSEBUTTONDOWN:
                this->scene->on_mouse_button(event.button.button, true);
                break;
            case SDL_MOUSEBUTTONUP:
                this->scene->on_mouse_button(event.button.button, false);
                break;
            case SDL_MOUSEMOTION:
                this->scene->on_mouse_motion(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
                break;
            case SDL_MOUSEWHEEL: {
                int mult = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1;
                this->scene->on_mouse_scroll(event.wheel.y * mult, event.wheel.x * mult);
            } break;
            case SDL_TEXTINPUT: {
                std::string str(event.text.text);
                if (this->scene->on_text_typing(str)) {
                    this->input_buffer.insert(std::min<size_t>(this->input_cursor, this->input_buffer.size()), str);
                    this->input_cursor += str.length();
                }
            } break;
            default:
                break;
            }
        }
        this->keyboard.mods = SDL_GetModState();
        this->mouse.state = SDL_GetMouseState(&this->mouse.x, &this->mouse.y);
        this->mouse.dstate = SDL_GetRelativeMouseState(&this->mouse.dx, &this->mouse.dy);
    }

    void GameClient::handle_key_press(const SDL_Event &event) {
//        this->quit |= event.key.keysym.sym == SDLK_ESCAPE;
    }

    void GameClient::handle_window_event(const SDL_Event &event) {
        switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
            //        case SDL_WINDOWEVENT_RESIZED:
            auto oldw = this->w, oldh = this->h;
            this->w = event.window.data1;
            this->h = event.window.data2;
            this->scene->on_window_resize(oldw, oldh);
        } break;
        default: break;
        }
    }
}
