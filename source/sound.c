/**
 * @file sound.c
 * @brief CWAV Wrapper (from YAMKC3DS) - demake to C
 * 
 * @copyright
 * Copyright (c) 2020-2021 PabloMK7, Ragnarok, JoanCC
 * 
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising
 * from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it freely,
 * subject to the following restrictions:
 * 
 *  1. The origin of this software must not be misrepresented; you must not claim
 *     that you wrote the original software. If you use this software in a product,
 *     an acknowledgment in the product documentation would be appreciated
 *     but is not required.
 * 
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 * 
 *  3. This notice may not be removed or altered from any source distribution.
 * 
 * @note
 * Changes include:
 * 
 *  - Converted from C++ class to struct
 *  - Added sound slots
 */

#include "sound.h"
#include <3ds.h>

static SoundSlot sounds[64];

void soundInit(void) {
    ndspInit();
    memset(&sounds, 0, sizeof(sounds));
}

void soundExit(void) {
    for (int i = 0; i < sizeof(sounds) / sizeof(SoundSlot); i++) {
        sounds[i].id = 0;
        soundFree(sounds[i].sound);
    }
    ndspExit();
}

void soundTick(void) {
    for (int i = 0; i < sizeof(sounds) / sizeof(SoundSlot); i++)
        soundTickUnit(sounds[i].sound);
}

void soundSetAllMasterVolume(float volume) {
    for (int i = 0; i < sizeof(sounds) / sizeof(SoundSlot); i++)
        soundSetMasterVolume(sounds[i].sound, volume);
}

void soundSetAllTargetVolume(float volume, int frames) {
    for (int i = 0; i < sizeof(sounds) / sizeof(SoundSlot); i++)
        soundSetTargetVolume(sounds[i].sound, volume, frames);
}

void soundSetAllTargetStop(int frames) {
    for (int i = 0; i < sizeof(sounds) / sizeof(SoundSlot); i++)
        soundSetTargetStop(sounds[i].sound, frames);
}

void soundLoadSlot(u32 id, const char* fileName, int simulPlays) {
    if (!id || !fileName || simulPlays < 0) return;
    for (int i = 0; i < sizeof(sounds) / sizeof(SoundSlot); i++) {
        if (sounds[i].id == id) {
            soundFree(sounds[i].sound);
            sounds[i].sound = soundNew();
            soundLoad(sounds[i].sound, fileName, simulPlays);
            return;
        } else if (!sounds[i].id) {
            soundFree(sounds[i].sound);
            sounds[i].id = id;
            sounds[i].sound = soundNew();
            soundLoad(sounds[i].sound, fileName, simulPlays);
            return;
        }
    }
}
Sound* soundGet(u32 id) {
    for (int i = 0; i < sizeof(sounds) / sizeof(SoundSlot); i++) {
        if (sounds[i].id == id)
            return sounds[i].sound;
    }
    return NULL;
}

Sound* soundNew(void) {
    Sound* sound = (Sound*)malloc(sizeof(Sound));
    if (!sound) return NULL;
    memset(sound, 0, sizeof(Sound));
    sound->toVolAmount = -1.f;
    sound->toPitchAmount = -1.f;
    sound->masterVolume = 1.f;
    return sound;
}

void soundFree(Sound* self) {
    if (!self) return;
    if (self->isLoaded) {
        cwavFileFree(self->sound);
        if (self->sound) free(self->sound);
    }
    free(self); self = NULL;
}

bool soundLoad(Sound* self, const char* fileName, int simulPlays) {
    if (self->isLoaded && self->sound) {
        cwavFileFree(self->sound);
        if (self->sound) free(self->sound);
        self->isLoaded = false;
    }
    self->sound = (CWAV*)malloc(sizeof(CWAV));
    cwavFileLoad(self->sound, fileName, simulPlays);
    self->isLoaded = self->sound->loadStatus == CWAV_SUCCESS;
    return self->isLoaded;
}

bool soundIsLoaded(Sound* self) {
    if (!self) return false;
    return self->isLoaded;
}

void soundPlay(Sound* self) {
    if (!self) return;
    if (self->isLoaded) {
        float bak = self->sound->volume;
        self->sound->volume *= self->masterVolume;
        cwavPlayResult res = cwavPlay(self->sound, 0, -1);
        self->sound->volume = bak;
        if (res.playStatus == CWAV_SUCCESS)
        {
            self->lChann = res.monoLeftChannel;
            self->rChann = -1;
        }
    }
}

void soundEnsurePlaying(Sound* self) {
    if (!self) return;
    self->currStopFrame = 0;
    if (!soundIsPlaying(self)) soundPlay(self);
}

void soundStereoPlay(Sound* self) {
    if (!self) return;
    if (self->isLoaded) {
        cwavPlayResult res = cwavPlay(self->sound, 0, 1);
        if (res.playStatus == CWAV_SUCCESS)
        {
            self->lChann = res.monoLeftChannel;
            self->rChann = res.rightChannel;
        }
    }        
}

void soundStop(Sound* self) {
    if (!self) return;
    if(self->isLoaded) {
        cwavStop(self->sound, -1, -1);
        self->lChann = self->rChann = -1;
    }
}

bool soundIsPlaying(Sound* self) {
    if (!self) return false;
    if(self->isLoaded) {
        return cwavIsPlaying(self->sound);
    } else {
        return self->isLoaded;
    }
}

void soundSetPitch(Sound* self, float amount, bool affect) {
    if (!self) return;
    if (!self->isLoaded)
        return;
    if (affect)
        self->sound->pitch = amount;
    else {
        self->fromPitchAmount = amount;
        self->toDPitchAmount = amount;
    }        
    if (soundIsPlaying(self) && affect)
    {
        if (self->rChann != -1)
        {
            ndspChnSetRate(self->rChann, self->sound->sampleRate * amount);
        }
        ndspChnSetRate(self->lChann, self->sound->sampleRate * amount);
    }
}

void soundSetMasterVolume(Sound* self, float volume) {
    if (!self) return;
    self->masterVolume = volume;
    soundSetVolume(self, volume);
}

void soundSetVolume(Sound* self, float volume) {
    if (!self) return;
    if (!self->isLoaded)
        return;
    float mix[12] = {0};
    self->sound->volume = volume;
    if (soundIsPlaying(self))
    {
        if (self->rChann == -1)
        {
            float rightPan = (self->sound->monoPan + 1.f) / 2.f;
            float leftPan = 1.f - rightPan;
            mix[0] = 0.8f * leftPan * volume * self->masterVolume; // Left front
            mix[2] = 0.2f * leftPan * volume * self->masterVolume; // Left back
            mix[1] = 0.8f * rightPan * volume * self->masterVolume; // Right front
            mix[3] = 0.2f * rightPan * volume * self->masterVolume; // Right back
            ndspChnSetMix(self->lChann, mix);
        } else {
            
            mix[0] = 0.8f * volume * self->masterVolume; // Left front
            mix[2] = 0.2f * volume * self->masterVolume; // Left back
            ndspChnSetMix(self->lChann, mix);
            memset(mix, 0, sizeof(mix));
            mix[1] = 0.8f  * volume * self->masterVolume; // Right front
            mix[3] = 0.2f  * volume * self->masterVolume; // Right back
            ndspChnSetMix(self->rChann, mix);
        }
        
    }
        
}

void soundSetTargetVolume(Sound* self, float amount, int frames) {
    if (!self) return;
    if (amount == self->toVolAmount)
        return;
    self->currVolFrame = frames;
    self->targetVolFrame = frames;
    self->fromVolAmount = self->sound->volume;
    self->toVolAmount = amount;
}
void soundSetCreatePitch(Sound* self, float amount, int frames) {
    if (!self) return;
    if (amount == self->toPitchAmount)
        return;
    self->currPitchFrame = frames;
    self->targetPitchFrame = frames;
    self->fromPitchAmount = self->sound->pitch;
    self->toPitchAmount = amount;
}
void soundSetDecayPitch(Sound* self, float amount, int frames) {
    if (!self) return;
    if (amount == self->toDPitchAmount)
        return;
    self->currDPitchFrame = frames;
    self->targetDPitchFrame = frames;
    self->fromDPitchAmount = self->sound->pitch;
    self->toDPitchAmount = amount;
}

void soundSetTargetStop(Sound* self, int frames) {
    if (!self) return;
    if (self->currStopFrame == 0 && soundIsPlaying(self))
        self->currStopFrame = frames;
}

void soundTickUnit(Sound* self) {
    if (!self) return;
    if (self->currVolFrame != 0) {
        float prog = 1.f - (--self->currVolFrame / (float)self->targetVolFrame);
        float newVol = self->fromVolAmount + (self->toVolAmount - self->fromVolAmount) * prog;
        soundSetVolume(self, newVol);
    }
    if (self->currPitchFrame != 0) {
        float prog = 1.f - (--self->currPitchFrame / (float)self->targetPitchFrame);
        float newPitch = self->fromPitchAmount + (self->toPitchAmount - self->fromPitchAmount) * prog;
        soundSetPitch(self, newPitch, true);
    }
    if (self->currDPitchFrame != 0) {
        float prog = 1.f - (--self->currDPitchFrame / (float)self->targetDPitchFrame);
        float newPitch = self->fromDPitchAmount + (self->toDPitchAmount - self->fromDPitchAmount) * prog;
        soundSetPitch(self, newPitch, true);
    }
    if (self->currStopFrame != 0) {
        if (!--self->currStopFrame)
            soundStop(self);
    }
}