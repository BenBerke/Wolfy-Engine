//
// Created by berke on 5/14/2026.
//

#ifndef TILKY_ENGINE_SOUNDMANAGER_H
#define TILKY_ENGINE_SOUNDMANAGER_H

#include <AL/al.h>
#include <AL/alc.h>

#include <chrono>

struct Vector3;

namespace SoundManager {
    bool InitializeOpenAL();
    void DestroyOpenAL();

    // The multiplier for the frequency. 1.0 is normal. 2.0 is an octave higher and double speed.
    void SetSourcePitch(const std::string& sourceName, float pitch);

    // The volume/amplitude multiplier. 1.0 is unity gain; 0.0 is silent.
    void SetSourceGain(const std::string& sourceName, float gain);

    void SetSourcePosition(const std::string& sourceName, Vector3 pos);

    // Set to AL_TRUE to make the sound repeat automatically when it reaches the end.
    void SetSourceLooping(const std::string& sourceName, bool looping);

    void PlaySoundOnSource(const std::string& sourceName, const std::string& soundName);
}
#endif //TILKY_ENGINE_SOUNDMANAGER_H