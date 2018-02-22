#pragma once
#include "common.h"

#include "al.h"
#include "alc.h"
#include "glm/vec3.hpp"

#include <string>
#include <unordered_map>
#include <memory>


namespace ace { namespace sound {
    struct SoundBuffer {
        SoundBuffer(const std::string &name);
        ~SoundBuffer();
        ACE_NO_COPY_MOVE(SoundBuffer)

        ALuint abo;
        ALuint format;
    };

    struct Sound {
        Sound(SoundBuffer *snd);
        ~Sound();
        ACE_NO_COPY_MOVE(Sound)

        void play(bool loop=false);
        void stop();
        bool stopped();


        void update();

        glm::vec3 position, velocity;
        float volume, pitch;
        bool local;

        ALuint snd;
    };

    struct SoundManager {
        SoundManager();
        ~SoundManager();

        // fire and forget
        // THE RETURNED REFERENCE ISN'T GUARANTEED TO STAY VALID FOR VERY LONG
        // IDK WHY IM RETURNING IT JUST DONT BE DUMB KTNX
        Sound *play(const std::string &name, glm::vec3 position, float volume = 100.f, bool local = false);
        void update(double dt);

        void set_listener(glm::vec3 position, glm::vec3 forward, glm::vec3 up, glm::vec3 velocity = {0, 0, 0}) const;

        SoundBuffer *get(const std::string &name);
    private:
        std::unordered_map<std::string, std::unique_ptr<SoundBuffer>> buffers;
        std::vector<std::unique_ptr<Sound>> sources;
        ALCdevice *device;
        ALCcontext *context;
    };
}}
