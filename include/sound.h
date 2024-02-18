/**
 * @file sound.h
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

#pragma once

#include <3ds.h>
#include <string.h>
#include <malloc.h>
#include <cwav.h>

enum SoundID {
    SND_BACK = 1,
    SND_SELECT,
    SND_TOUCH_IN,
    SND_TOUCH_OUT,
    SND_TOUCH_OUT_IN,
    SND_ERROR,
    SND_NOTICE,
};

typedef struct Sound_s {
    CWAV* sound;
    bool isLoaded;
    float masterVolume;
    int lChann, rChann;
    int targetVolFrame, targetPitchFrame, targetDPitchFrame;
    int currVolFrame, currPitchFrame, currDPitchFrame, currStopFrame;
    float fromVolAmount, fromPitchAmount, fromDPitchAmount;
    float toVolAmount, toPitchAmount, toDPitchAmount;
} Sound;

typedef struct SoundSlot_s {
    Sound* sound;
    u32 id;
} SoundSlot;

void soundInit(void);
void soundExit(void);
void soundTick(void);
void soundLoadSlot(u32 id, const char* fileName, int simulPlays);
Sound* soundGet(u32 id);
void soundSetAllMasterVolume(float volume);
void soundSetAllTargetVolume(float volume, int frames);
void soundSetAllTargetStop(int frames);

#define SND(id) soundGet(id)

Sound* soundNew(void);
void soundFree(Sound* self);
bool soundLoad(Sound* self, const char* fileName, int simulPlays);
bool soundIsLoaded(Sound* self);
bool soundIsPlaying(Sound* self);
void soundPlay(Sound* self);
void soundEnsurePlaying(Sound* self);
void soundStereoPlay(Sound* self);
void soundStop(Sound* self);
void soundSetMasterVolume(Sound* self, float volume);
void soundSetVolume(Sound* self, float volume);
void soundSetPitch(Sound* self, float amount, bool affect);
void soundSetTargetVolume(Sound* self, float amount, int frames);
void soundSetCreatePitch(Sound* self, float amount, int frames);
void soundSetDecayPitch(Sound* self, float amount, int frames);
void soundSetTargetStop(Sound* self, int frames);
void soundTickUnit(Sound* self);
