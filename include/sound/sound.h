#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include "al.h"
#include "alc.h"
#include "glm/vec3.hpp"

#include "common.h"
#include "gl/gl_util.h"


namespace ace { namespace sound {
    // is this cheating
    using abo = gl::GLObj<alGenBuffers, alDeleteBuffers, ALuint>;
    using aso = gl::GLObj<alGenSources, alDeleteSources, ALuint>;

    struct SoundBuffer {
        SoundBuffer(const std::string &name);

        abo abo;
        ALuint format;
    };

    struct Sound {
        Sound(SoundBuffer *snd);

        void play(bool loop=false);
        void stop();
        bool stopped();


        void update();

        glm::vec3 position, velocity;
        float volume, pitch;
        bool local;

        aso snd;
    };

    struct SoundManager {
        SoundManager();
        ~SoundManager();

        // fire and forget
        // THE RETURNED REFERENCE ISN'T GUARANTEED TO STAY VALID FOR VERY LONG
        // IDK WHY IM RETURNING IT JUST DONT BE DUMB KTNX
        Sound *play(const std::string &name, glm::vec3 position, float volume = 100.f, bool local = false);
        Sound *play_local(const std::string &name, float volume = 100.f) {
            return this->play(name, {}, volume, true);
        }
        void update(double dt);

        void set_listener(glm::vec3 position, glm::vec3 forward, glm::vec3 up, glm::vec3 velocity = {0, 0, 0}) const;

        SoundBuffer *get(const std::string &name);
    private:
        std::unordered_map<std::string, SoundBuffer> buffers;
        std::vector<std::unique_ptr<Sound>> sources;
        ALCdevice *device;
        ALCcontext *context;
    };
}}
