#include "sound/sound.h"

#include "SDL_audio.h"

#include "glm/gtc/type_ptr.hpp"

#include "util/except.h"


#include "alure.h"

namespace ace { namespace sound {

#ifdef NDEBUG
#define CHECK_AL_ERROR() 
#else
#define CHECK_AL_ERROR() do { ALenum error = alGetError(); \
        if(error != AL_NO_ERROR && error != AL_OUT_OF_MEMORY) { \
            THROW_ERROR("OPENAL ERROR: {}\n", alGetString(error)); \
        } \
    } while (0)
#endif

    SoundBuffer::SoundBuffer(const std::string& name) {
        CHECK_AL_ERROR();
        if(alureBufferDataFromFile(name.c_str(), this->abo) == AL_FALSE) {
            THROW_ERROR("FAILED TO LOAD SOUND FILE {} WITH ERROR{}\n", name, alureGetErrorString());
        }
    }

    Sound::Sound(SoundBuffer *buf) : position(0), velocity(0), volume(1), pitch(1), local(true) {
        CHECK_AL_ERROR();
        this->set_buf(buf);
    }

    void Sound::set_buf(SoundBuffer *buf) {
        if (buf != nullptr) {
            alSourcei(this->snd, AL_BUFFER, buf->abo);
            CHECK_AL_ERROR();
        }
    }


    void Sound::play(bool loop) {
        this->update();
        alSourcei(this->snd, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
        CHECK_AL_ERROR();
        alSourcePlay(this->snd);
        CHECK_AL_ERROR();
    }

    void Sound::stop() {
        alSourceStop(this->snd);
        CHECK_AL_ERROR();
    }

    bool Sound::stopped() {
        ALint state;
        alGetSourcei(this->snd, AL_SOURCE_STATE, &state);
        CHECK_AL_ERROR();
        return state == AL_STOPPED;
    }

    void Sound::update() {
        alSourcefv(this->snd, AL_POSITION, glm::value_ptr(local ? glm::vec3(0) : this->position));
        CHECK_AL_ERROR();
        alSourcefv(this->snd, AL_VELOCITY, glm::value_ptr(this->velocity));
        CHECK_AL_ERROR();
        alSourcef(this->snd, AL_ROLLOFF_FACTOR, 1.f);
        CHECK_AL_ERROR();
        alSourcei(this->snd, AL_SOURCE_RELATIVE, local ? AL_TRUE : AL_FALSE);
        CHECK_AL_ERROR();
        alSourcef(this->snd, AL_GAIN, volume / 100.f);
        CHECK_AL_ERROR();
        alSourcef(this->snd, AL_PITCH, pitch);
        CHECK_AL_ERROR();
        alSourcef(this->snd, AL_REFERENCE_DISTANCE, 1.f);
        CHECK_AL_ERROR();
        alSourcef(this->snd, AL_MAX_DISTANCE, 128.f);
        CHECK_AL_ERROR();
    }

    SoundManager::SoundManager() {
        alureInitDevice(nullptr, nullptr);
        alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
        this->set_listener({ 0, 0, 0 }, { 0, 0, 1 }, {0, 1, 0});
        this->music = std::make_unique<Sound>(nullptr);
    }

    SoundManager::~SoundManager() {
        alureShutdownDevice();
    }

    Sound *SoundManager::play(const std::string &name, glm::vec3 position, float volume, bool local) {
        if (this->sources.size() > 128) return nullptr;
         
        this->sources.emplace_back(this->get(name));
        Sound *snd = &this->sources.back();
        snd->position = position;
        snd->volume = volume;
        snd->local = local;
        snd->play();
        return snd;
    }

    void SoundManager::play_music(const std::string &name, float volume, bool loop) {
        this->fading_out = false;
        this->music->stop();
        this->music->set_buf(this->get(name));
        this->music->volume = volume;
        this->music->local = false;
        this->music->play(loop);
    }

    void SoundManager::stop_music(bool fadeout) {
        this->fading_out = fadeout;
        if(!fadeout)
            this->music->stop();
    }

    bool SoundManager::music_playing() {
        return !this->music->stopped();
    }

    void SoundManager::update(double dt) {
        if(this->fading_out) {
            this->music->volume -= 25 * dt;
            if(this->music->volume <= 0.0) {
                this->music->stop();
            } else {
                this->music->update();
            }
        }

        for (auto i = sources.begin(); i != sources.end();) {
            if (i->stopped()) {
                i = sources.erase(i);
            } else {
                ++i;
            }
        }
    }

    void SoundManager::set_listener(glm::vec3 position, glm::vec3 forward, glm::vec3 up, glm::vec3 velocity) const {
        float ori[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };

        alListenerfv(AL_POSITION, glm::value_ptr(position));
        CHECK_AL_ERROR();
        alListenerfv(AL_ORIENTATION, ori);
        CHECK_AL_ERROR();
        alListenerfv(AL_VELOCITY, glm::value_ptr(velocity));
        CHECK_AL_ERROR();
    }

    SoundBuffer *SoundManager::get(const std::string& name) {
        try {
            return &buffers.at(name);
        }
        catch (std::out_of_range &) {
            return &buffers.emplace(name, "wav/" + name).first->second;
        }
    }
}}
