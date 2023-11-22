#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

extern C2D_SpriteSheet commonSheet;

typedef struct WaitIcon_s {
    float px, py;
    float scale;
    float alpha;
    u32 baseColor;
    u32 tintColor[4];
} WaitIcon;

#define WAITICON_STRUCTSIZE   sizeof(WaitIcon)
#define WAITICON_NATIVE_SIZE  24.0f

void        waitIconTick(void);
WaitIcon    *waitIconNew(void);
void        waitIconFree(WaitIcon *self);
void        waitIconInit(WaitIcon *self);
void        waitIconSetPosition(WaitIcon *self, float x, float y);
void        waitIconSetScale(WaitIcon *self, float scale);
void        waitIconSetBaseColor(WaitIcon *self, u32 color);
void        waitIconSetTintColor(WaitIcon *self, s32 corner, u32 color);
void        waitIconSetAlpha(WaitIcon *self, float alpha);
void        waitIconRender(WaitIcon *self);