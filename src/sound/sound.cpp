#include "sound/sound.h"

#include "SDL_audio.h"

#include "glm/gtc/type_ptr.hpp"

#include "util/except.h"


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
        SDL_AudioSpec spec;
        Uint32 length;
        Uint8 *buffer;
        if(SDL_LoadWAV(name.c_str(), &spec, &buffer, &length) == nullptr) {
            THROW_ERROR("FAILED TO LOAD SOUND FILE {} WITH ERROR {}\n", name, SDL_GetError());
        }

        switch(spec.channels) {
        case 1:
            format = spec.format == AUDIO_U8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
            break;
        case 2:
            format = spec.format == AUDIO_U8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
            break;
        default:
            THROW_ERROR("FAILED TO LOAD SOUND FILE {}: UNRECOGNIZED FORMAT {}\n", name, Uint16(spec.format));
        }


        CHECK_AL_ERROR();
        alBufferData(abo, format, buffer, length, spec.freq);
        CHECK_AL_ERROR();
        SDL_FreeWAV(buffer);
    }

    Sound::Sound(SoundBuffer *buf) : position(0), velocity(0), volume(1), pitch(1), local(true) {
        CHECK_AL_ERROR();
        alSourcei(snd, AL_BUFFER, buf->abo);
        CHECK_AL_ERROR();

        ALint channels;
        alGetBufferi(buf->abo, AL_CHANNELS, &channels);
    }

    void Sound::play(bool loop) {
        this->update();
        alSourcei(snd, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
        CHECK_AL_ERROR();
        alSourcePlay(snd);
        CHECK_AL_ERROR();
    }

    void Sound::stop() {
        alSourceStop(snd);
        CHECK_AL_ERROR();
    }

    bool Sound::stopped() {
        ALint state;
        alGetSourcei(snd, AL_SOURCE_STATE, &state);
        CHECK_AL_ERROR();
        return state == AL_STOPPED;
    }

    void Sound::update() {
        alSourcefv(snd, AL_POSITION, glm::value_ptr(local ? glm::vec3(0, 0, 0) : this->position));
        CHECK_AL_ERROR();
        alSourcefv(snd, AL_VELOCITY, glm::value_ptr(this->velocity));
        CHECK_AL_ERROR();
        alSourcef(snd, AL_ROLLOFF_FACTOR, 1.f);
        CHECK_AL_ERROR();
        alSourcei(snd, AL_SOURCE_RELATIVE, local ? AL_TRUE : AL_FALSE);
        CHECK_AL_ERROR();
        alSourcef(snd, AL_GAIN, volume / 100.f);
        CHECK_AL_ERROR();
        alSourcef(snd, AL_PITCH, pitch);
        CHECK_AL_ERROR();
        alSourcef(snd, AL_REFERENCE_DISTANCE, 1.f);
        CHECK_AL_ERROR();
        alSourcef(snd, AL_MAX_DISTANCE, 128.f);
        CHECK_AL_ERROR();
    }

    SoundManager::SoundManager() {
        device = alcOpenDevice(nullptr);
        context = alcCreateContext(device, nullptr);
        alcMakeContextCurrent(context);
        alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
        this->set_listener({ 0, 0, 0 }, { 0, 0, 1 }, {0, 1, 0});
    }

    SoundManager::~SoundManager() {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);
    }

    Sound *SoundManager::play(const std::string &name, glm::vec3 position, float volume, bool local) {
        if (sources.size() > 128) return nullptr;
         
        sources.emplace_back(std::make_unique<Sound>(this->get(name)));
        Sound *snd = sources.back().get();
        snd->position = position;
        snd->volume = volume;
        snd->local = local;
        snd->play();
        return snd;
    }

    void SoundManager::update(double dt) {
        for (auto i = sources.begin(); i != sources.end();) {
            if ((*i)->stopped()) {
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
