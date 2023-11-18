#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdlib.h>
#include "sheets/sheet_progress.h"

typedef struct ProgressBar_s {
    float   px, py;
    float   width;
    float   scale;
    u32     baseColor;
    u32     backColor;
    u32     tintColor[4];
    bool    useSpark;
    float   sparkAnim;
    float   alpha;
    float   progress;
} ProgressBar;

#define PROGRESSBAR_STRUCTSIZE  sizeof(ProgressBar)
#define PROGRESSBAR_WIDTH       124.f
#define PROGRESSBAR_HEIGHT      12.f

extern C2D_SpriteSheet menuProgressSheet;

ProgressBar*    progressBarNew();
void            progressBarFree(ProgressBar* self);
void            progressBarRender(ProgressBar* self);
void            progressBarSetBaseColor(ProgressBar* self, u32 c);
void            progressBarSetTintColor(ProgressBar* self, s32 corner, u32 c);
void            progressBarSetPosition(ProgressBar* self, float x, float y);
void            progressBarSetWidth(ProgressBar* self, float width);
void            progressBarSetScale(ProgressBar* self, float scale);
void            progressBarSetAlpha(ProgressBar* self, float alpha);
void            progressBarSetProgress(ProgressBar* self, float p);
void            progressBarUseSparkAnimation(ProgressBar* self, bool enabled);