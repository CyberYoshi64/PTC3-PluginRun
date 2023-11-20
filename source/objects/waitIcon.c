#include "objects/waitIcon.h"
#include "sheets/sheet_common.h"

extern u32 mainCnt;
float waitIconAngle = 0;

void waitIconTick() {
    waitIconAngle = C3D_AngleFromDegrees((int)(mainCnt * 7.5f) % 3600);
}

WaitIcon *waitIconNew() {
    WaitIcon *w = malloc(WAITICON_STRUCTSIZE);
    if (w) waitIconInit(w);
    return w;
}

void waitIconFree(WaitIcon *self) {
    if (!self) return;
    free(self);
}

void waitIconInit(WaitIcon *self) {
    if (!self) return;
    memset(self, 0, WAITICON_STRUCTSIZE);
    self->scale = WAITICON_NATIVE_SIZE;
    self->baseColor     = 0x60000000U;
    self->tintColor[0]  = C2D_Color32f(.5, 1, 1, 0);
    self->tintColor[1]  = C2D_Color32f(0, .75, 1, 0);
    self->tintColor[2]  = C2D_Color32f(0, .3, .8, 0);
    self->tintColor[3]  = C2D_Color32f(.05, .15, .15, 0);
}

void waitIconSetPosition(WaitIcon *self, float x, float y) {
    if (!self) return;
    self->px = x;
    self->py = y;
}

void waitIconSetScale(WaitIcon *self, float scale) {
    if (!self) return;
    self->scale = WAITICON_NATIVE_SIZE * scale;
}

void waitIconSetBaseColor(WaitIcon* self, u32 c) {
    if (!self) return;
    self->baseColor = c & 0xFFFFFF;
}

void waitIconSetTintColor(WaitIcon* self, s32 corner, u32 c) {
    if (!self) return;
    if (corner >= 0 && corner < 4)
        self->tintColor[corner] = c & 0xFFFFFF;
    else {
        for (u32 i = 0; i < 4; i++)
            self->tintColor[i] = c & 0xFFFFFF;
    }
}

void waitIconSetAlpha(WaitIcon *self, float alpha) {
    if (!self) return;
    self->alpha = alpha;
}

void waitIconRender(WaitIcon *self) {
    if (!self) return;
    u32 a = C2D_FloatToU8(self->alpha)<<24;
    C2D_DrawParams p = {{self->px, self->py, self->scale / 2, self->scale / 2}, {self->scale / 2, self->scale / 2}, 0, waitIconAngle};
    C2D_Image i = C2D_SpriteSheetGetImage(commonSheet, sheet_common_waiticon_bg_idx);
    C2D_ImageTint t;
    C2D_PlainImageTint(&t, (self->baseColor & 0xFFFFFF) | (u8)(self->alpha * (u8)(self->baseColor>>24))<<24, 1);
    C2D_SetTintMode(C2D_TintSolid);
    C2D_DrawImage(i, &p, &t);
    p.pos.w *= -1;
    p.center.x = 0;
    C2D_DrawImage(i, &p, &t);
    p.pos.w *= -1;
    p.center.x = self->scale / 2;
    p.pos.h *= -1;
    p.center.y = 0;
    C2D_DrawImage(i, &p, &t);
    p.pos.w *= -1;
    p.center.x = 0;
    C2D_DrawImage(i, &p, &t);
    p.pos.w = p.pos.h = self->scale;
    p.center.x = p.center.y = self->scale / 2;
    i = C2D_SpriteSheetGetImage(commonSheet, sheet_common_waiticon_idx);
    C2D_SetImageTint(&t, C2D_TopLeft, self->tintColor[0]|a, 1);
    C2D_SetImageTint(&t, C2D_BotLeft, self->tintColor[1]|a, 1);
    C2D_SetImageTint(&t, C2D_BotRight, self->tintColor[2]|a, 1);
    C2D_SetImageTint(&t, C2D_TopRight, self->tintColor[3]|a, 1);
    C2D_DrawImage(i, &p, &t);
}
