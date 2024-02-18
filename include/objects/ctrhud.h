#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "sheets/sheet_hud.h"

enum NetworkState {
    NTWKSTATE_ENABLED_INIT = 0,
    NTWKSTATE_ENABLED_SWITCHING,
    NTWKSTATE_INTERNET,
    NTWKSTATE_LOCAL,
    NTWKSTATE_LOCAL1,
    NTWKSTATE_STREETPASS,
    NTWKSTATE_LOCAL2,
    NTWKSTATE_DISABLED,
    NTWKSTATE_LENGTH
};

typedef struct HUD_CTR_global_s {
    C2D_SpriteSheet graphics;
    C2D_TextBuf     textBuf;
    u32             iconColor[NTWKSTATE_LENGTH];
    u32             textColor[NTWKSTATE_LENGTH];
    u32             maskColor[NTWKSTATE_LENGTH];
    u32             frameColor[NTWKSTATE_LENGTH];
    char            names[NTWKSTATE_LENGTH][32];
} HUD_CTR_global;

typedef struct HUD_CTR_s {
    HUD_CTR_global  *global;
    u8              networkState;
    u8              strength;
    u8              timer;
    u8              type;
    u8              flags;
    C2D_Text        text;
    float           x;
    float           y;
    float           width;
    float           height;
} HUD_CTR;

HUD_CTR_global  *hudCtrInit(void);
void            hudCtrFree(HUD_CTR_global* self);
HUD_CTR         *hudCtrIconInit(HUD_CTR_global* global);
void            hudCtrIconFree(HUD_CTR* self);
void            hudCtrSetPosition(HUD_CTR* self, float x, float y);
void            hudCtrSetBoxSize(HUD_CTR* self, float width, float height);
void            hudCtrRender(HUD_CTR* self);
void            hudCtrTick(HUD_CTR* self);
void            hudCtrTickFake(HUD_CTR* self, u8 networkState, u8 strength);