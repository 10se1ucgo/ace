#include "game_client.h"
#include "util/except.h"

#include "scene/scene.h"

#include "net.h"

#include <SDL_image.h>

namespace ace {
    void APIENTRY gl_error(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
    {
        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

        fmt::print("---------------------opengl-callback-start------------\n");
        fmt::print("message: {}\n", message);
        fmt::print("type: {}\n", testogl::get_gl_debug_type_name(type));
        fmt::print("id: {}\n", id);
        fmt::print("severity: {}\n", testogl::get_gl_debug_severity_name(severity));
        fmt::print("---------------------opengl-callback-end--------------\n");
    }


    void sdl_error(std::string msg) {
        THROW_ERROR("{}: {}\n", msg, SDL_GetError());
        // fprintf(stderr, "%s: %s\n", msg, SDL_GetError());
    }

    GameClient::GameClient(std::string caption, int w, int h, WINDOW style):
        tasks(*this), window_title(std::move(caption)) {

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
            sdl_error("SDL_Init");
        if (IMG_Init(IMG_INIT_PNG) < 0)
            sdl_error("IMG_Init");

        SDL_GL_LoadLibrary(nullptr);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        
        this->window = SDL_CreateWindow(window_title.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,
            SDL_WINDOW_OPENGL | (style == WINDOW::WINDOWED ? 0 : SDL_WINDOW_FULLSCREEN)
        );
        if(style == WINDOW::FULLSCREEN_BORDERLESS) {
            SDL_SetWindowBordered(this->window, SDL_FALSE);
        }
        if (this->window == nullptr)
            sdl_error("SDL_CreateWindow");

        this->context = SDL_GL_CreateContext(this->window);
        if (this->context == nullptr)
            sdl_error("SDL_GL_CreateContext");

        if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
            THROW_ERROR("gladLoaderGLLoader fail");
        }

        SDL_GL_MakeCurrent(this->window, this->context);



        glEnable(GL_DEBUG_OUTPUT);
//        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_error, nullptr);

        SDL_GL_SetSwapInterval(0);

        fmt::print("OpenGL: {}\n", glGetString(GL_VERSION));
        fmt::print("GLSL: {}\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        fmt::print("Renderer: {}\n", glGetString(GL_RENDERER));
        fmt::print("Vendor: {}\n", glGetString(GL_VENDOR));

        keyboard.keys = SDL_GetKeyboardState(&keyboard.numkeys);
        this->w = w;
        this->h = h;

        enet_initialize();
        this->net = std::make_unique<net::NetworkClient>(*this);

        this->shaders = std::make_unique<ShaderManager>();

        SDL_StopTextInput();
    }

    GameClient::~GameClient() {
        this->net.reset();
        enet_deinitialize();

        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
    }

    void GameClient::run() {
        Uint64 last = SDL_GetPerformanceCounter();
        while (!this->quit) {
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
        tasks.update(dt);
        if(this->net) this->net->update(dt);
        this->sound.update(dt);
        this->scene->update(dt);
        this->draw();
    }

    void GameClient::draw() const {
        this->scene->draw();
        SDL_GL_SwapWindow(this->window);
    }

    void GameClient::update_fps() {
        fps_counter.frames++;
        if (this->time - fps_counter.last_update >= 1) {
            SDL_SetWindowTitle(this->window, fmt::format("{} (FPS: {})", this->window_title, fps_counter.frames).c_str());
            fps_counter.frames = 0;
            fps_counter.last_update = this->time;
        }
    }

    void GameClient::poll_events() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                this->quit = true;
                break;
            case SDL_KEYDOWN:
                if(this->text_input_active()) {
                    switch(event.key.keysym.scancode) {
                    case SDL_SCANCODE_BACKSPACE:
                        if(!this->input_buffer.empty())
                            this->input_buffer.pop_back();
                        break;
                    case SDL_SCANCODE_RETURN:
                        this->scene->on_text_finished();
                    case SDL_SCANCODE_ESCAPE:
                        SDL_StopTextInput();
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
