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


#define SDL_ERROR(msg) THROW_ERROR("{}: {}\n", msg, SDL_GetError())

    GameConfig::GameConfig(std::string file_name) {
        std::ifstream{ file_name } >> this->json;
        
        // i >> j;
    }

    SDL_Scancode GameConfig::get_key(const std::string &key) {
        ; // k.get<std::underlying_type_t<SDL_Scancode>>();
        auto &entry(this->json["controls"].at(key));
        if (entry.is_number()) {
            return SDL_Scancode(entry.get<std::underlying_type_t<SDL_Scancode>>());
        }

        const auto &key_name = entry.get_ref<const std::string &>();
        auto it = this->name_to_scancode.find(key_name);
        if(it == this->name_to_scancode.end()) {
            return this->name_to_scancode[key_name] = SDL_GetScancodeFromName(key_name.c_str());
        }

        return it->second;
    }

    // void GameConfig::read() {
    // }
    //
    // void GameConfig::write() {
    // }

    GameClient::GameClient(std::string caption /*, int w, int h, WINDOW_STYLE style */):
        net(*this), tasks(*this), config("config.json"), window_title(std::move(caption)) {

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
            SDL_ERROR("SDL_Init");
        if (IMG_Init(IMG_INIT_PNG) < 0)
            SDL_ERROR("IMG_Init");

        SDL_GL_LoadLibrary(nullptr);
//        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
//        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
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

        auto &window_mode = config.json["graphics"].at("window_mode").get_ref<const std::string &>();
        this->w = config.json["graphics"].at("window_width");
        this->h = config.json["graphics"].at("window_height");

        this->window = SDL_CreateWindow(
            this->window_title.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->w, this->h,
            SDL_WINDOW_OPENGL | (window_mode == "windowed" ? 0 : SDL_WINDOW_FULLSCREEN)
        );
        if(window_mode == "borderless") {
            SDL_SetWindowBordered(this->window, SDL_FALSE);
        }
        if (this->window == nullptr)
            SDL_ERROR("SDL_CreateWindow");

        this->context = SDL_GL_CreateContext(this->window);
        if (this->context == nullptr)
            SDL_ERROR("SDL_GL_CreateContext");

        if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
            THROW_ERROR("gladLoaderGLLoader fail");
        }

        SDL_GL_MakeCurrent(this->window, this->context);
        SDL_GL_SetSwapInterval(this->config.json["graphics"].value("vsync", false));

        if(GLAD_GL_VERSION_4_3 && this->config.json["graphics"].value("debug", false)) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(gl_error, nullptr);
        }

        fmt::print("OpenGL: {}\n", glGetString(GL_VERSION));
        fmt::print("GLSL: {}\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        fmt::print("Renderer: {}\n", glGetString(GL_RENDERER));
        fmt::print("Vendor: {}\n", glGetString(GL_VENDOR));

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
        this->poll_events();
        this->update_fps();
        this->tasks.update(dt);
        this->net.update(dt);
        this->url.update(dt);
        this->sound.update(dt);
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
                        if(!this->input_buffer.empty())
                            this->input_buffer.pop_back();
                        break;
                    case SDL_SCANCODE_RETURN:
                        this->scene->on_text_finished(false);
                    case SDL_SCANCODE_ESCAPE:
                        SDL_StopTextInput();
                        this->scene->on_text_finished(true);
                        this->input_buffer.clear();
                        break;
                    default:
                        break;
                    }
                    return;
                }
                this->scene->on_key(event.key.keysym.scancode, event.key.keysym.mod, true);
                handle_key_press(event);
                break;
            case SDL_KEYUP:
                this->scene->on_key(event.key.keysym.scancode, event.key.keysym.mod, false);
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
            case SDL_TEXTINPUT:
                if(this->scene->on_text_typing(std::string(event.text.text)))
                    this->input_buffer.append(event.text.text);
                break;
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
