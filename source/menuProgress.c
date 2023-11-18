#include "menuProgress.h"

C2D_SpriteSheet menuProgressSheet;

ProgressBar* progressBarNew() {
    ProgressBar* p = malloc(PROGRESSBAR_STRUCTSIZE);
    if (p) memset(p, 0, PROGRESSBAR_STRUCTSIZE);
    progressBarSetBaseColor(p, -1);
    progressBarSetTintColor(p, -1, 0xFFA000);
    return p;
}
void progressBarFree(ProgressBar* self) {
    if (!self) return;
    free(self);
}
void progressBarRender(ProgressBar* self) {
    if (!self) return;
    if (!self->width) self->width = 160;
    self->progress = C2D_Clamp(self->progress, 0.f, 1.f);

    float innerSize = (self->width - 4) * self->scale;
    float halfSize = (self->width - 6.f) * self->scale;
    u32 a = C2D_FloatToU8(self->alpha)<<24;
    C2D_DrawParams p = {{self->px + halfSize, self->py + 6 * self->scale, 6 * self->scale, 6 * self->scale}, {halfSize, 6 * self->scale}, 0, 0};
    C2D_Image i = C2D_SpriteSheetGetImage(menuProgressSheet, sheet_progress_base_idx);
    C2D_ImageTint t;
    C2D_PlainImageTint(&t, self->baseColor | a, 1);
    C2D_DrawRectSolid(self->px + 2 * self->scale, self->py + 2 * self->scale, 0, innerSize, 8 * self->scale, self->backColor | (C2D_FloatToU8(self->alpha * .8)<<24));
    C2D_DrawRectangle(
        self->px + 2 * self->scale, self->py + 2 * self->scale, 0, innerSize * self->progress, 8 * self->scale,
        self->tintColor[0]|a, self->tintColor[1]|a, self->tintColor[2]|a, self->tintColor[3]|a
    );
    C2D_DrawImage(i, &p, &t);
    p.center.x = 0;
    p.pos.w *= -1;
    C2D_DrawImage(i, &p, &t);
    p.center.x = halfSize;
    p.pos.w *= -1;
    p.center.y = 0;
    p.pos.h *= -1;
    C2D_DrawImage(i, &p, &t);
    p.center.x = 0;
    p.pos.w *= -1;
    C2D_DrawImage(i, &p, &t);
    p.pos.h *= -1;
    p.center.x = (self->width - 12) * self->scale;
    p.center.y = 6 * self->scale;
    p.pos.w = (self->width - 12.f) * self->scale;
    i = C2D_SpriteSheetGetImage(menuProgressSheet, sheet_progress_baseMiddle_idx);
    C2D_DrawImage(i, &p, &t);
    p.center.y = 0;
    p.pos.h *= -1;
    C2D_DrawImage(i, &p, &t);
    if (self->progress > .05f && self->useSpark) {
        
        if (self->sparkAnim < 1.f)
            self->sparkAnim += .0078125f / (1 + ((self->progress >= .97625f)));
        else if (self->progress < 1.f)
            self->sparkAnim = 0;
        float sinSpark = sin(self->sparkAnim * M_PI / 2);
        float sparkSize = (10 - fabsf(.5 - self->sparkAnim) * 4) * self->scale;
        
        C2D_PlainImageTint(&t, 0xFFFFFF|C2D_FloatToU8(self->alpha * (1 - fabsf(.5 - sinSpark) * 2))<<24, 1);
        i = C2D_SpriteSheetGetImage(menuProgressSheet, sheet_progress_spark_idx);
        p.pos.w = p.pos.h = p.center.x = p.center.y = sparkSize;
        p.pos.x = self->px + (2 + innerSize * sinSpark * self->progress) * self->scale;
        p.pos.y = self->py + 6 * self->scale;
        C2D_DrawImage(i, &p, &t);
        p.center.x = 0;
        p.pos.w *= -1;
        C2D_DrawImage(i, &p, &t);
        p.center.x = p.pos.w = sparkSize;
        p.center.y = 0;
        p.pos.h *= -1;
        C2D_DrawImage(i, &p, &t);
        p.center.x = 0;
        p.pos.w *= -1;
        C2D_DrawImage(i, &p, &t);
    } else {
        self->sparkAnim = 0;
    }
}
void progressBarSetBaseColor(ProgressBar* self, u32 c) {
    if (!self) return;
    self->baseColor = c & 0xFFFFFF;
}
void progressBarSetTintColor(ProgressBar* self, s32 corner, u32 c) {
    if (!self) return;
    if (corner >= 0 && corner < 4)
        self->tintColor[corner] = c & 0xFFFFFF;
    else {
        for (u32 i = 0; i < 4; i++)
            self->tintColor[i] = c & 0xFFFFFF;
    }
}
void progressBarSetPosition(ProgressBar* self, float x, float y) {
    if (!self) return;
    self->px = x;
    self->py = y;
}
void progressBarSetWidth(ProgressBar* self, float width) {
    if (!self) return;
    self->width = C2D_Clamp(width, 12.f, 400.f);
}
void progressBarSetScale(ProgressBar* self, float scale) {
    if (!self) return;
    self->scale = scale;
}
void progressBarSetAlpha(ProgressBar* self, float alpha) {
    if (!self) return;
    self->alpha = alpha;
}
void progressBarSetProgress(ProgressBar* self, float p) {
    if (!self) return;
    self->progress = p;
}

void progressBarUseSparkAnimation(ProgressBar* self, bool enabled) {
    if (!self) return;
    self->useSpark = enabled;
}