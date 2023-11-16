#include "menuButton.h"
#include "hidRead.h"

float buttonAlpha = 1.f;

void buttonSetup(Button* self, float x, float y, float width, float height) {
    if (!self) return;
    memset(self, 0, sizeof(Button));
    self->pos.x = x;
    self->pos.y = y;
    self->pos.w = width;
    self->pos.h = height;
    self->renderBox = true;
}

void buttonSetupCB(Button* self, float x, float y, float width, float height, ButtonRenderCB render) {
    if (!self) return;
    memset(self, 0, sizeof(Button));
    self->pos.x = x;
    self->pos.y = y;
    self->pos.w = width;
    self->pos.h = height;
    self->render = render;
    self->renderBox = true;
}

void buttonSetPosition(Button* self, float x, float y) {
    if (!self) return;
    self->pos.x = x;
    self->pos.y = y;}

void buttonSetSize(Button* self, float width, float height) {
    if (!self) return;
    self->pos.w = width;
    self->pos.h = height;
}

void buttonSetCallback(Button* self, ButtonRenderCB render) {
    if (!self) return;
    self->render = render;
}

void buttonSetEnabled(Button* self, bool enabled) {
    if (!self) return;
    self->disabled = !enabled;
}

void buttonSetDrawBox(Button* self, bool enabled) {
    if (!self) return;
    self->renderBox = enabled;
}

bool buttonTick(Button* self) {
    if (!self) return false;

    bool ack = false;
    self->inArea = hidTouchedBox(self->pos.x, self->pos.y, self->pos.w, self->pos.h);
    bool touchedBefore = hidTouchedBoxBefore(self->pos.x, self->pos.y, self->pos.w, self->pos.h);

    if ((HID_TOUCHTIME == 1) && self->inArea)
        self->selected = true;

    if (!HID_TOUCHTIME) {
        if (touchedBefore && self->selected && !self->disabled) ack = true;
        self->selected = false;
    }
    
    return ack;
}

void buttonRender(Button* self) {
    if (!self) return;
    u32 color1 = self->disabled ? C2D_Color32f(1,1,1,buttonAlpha/8) : C2D_Color32f(1,1,1,buttonAlpha/2);
    u32 color2 = self->disabled ? color1 : C2D_Color32f(1,1,1,buttonAlpha/4);

    if (self->render)
        self->render(self->pos.x, self->pos.y, self->pos.w, self->pos.h, self->selected, self->inArea, self->disabled);

    if (self->renderBox) {
        if (self->pos.x > 0) C2D_DrawLine(self->pos.x, self->pos.y, color1, self->pos.x, self->pos.y + self->pos.h, color1, 2, 0);
        if (self->pos.y > 0) C2D_DrawLine(self->pos.x, self->pos.y, color1, self->pos.x + self->pos.w, self->pos.y, color1, 2, 0);
        if ((self->pos.x + self->pos.w) < 319) C2D_DrawLine(self->pos.x + self->pos.w, self->pos.y, color1, self->pos.x + self->pos.w, self->pos.y + self->pos.h, color1, 2, 0);
        if ((self->pos.y + self->pos.h) < 239) C2D_DrawLine(self->pos.x, self->pos.y + self->pos.h, color1, self->pos.x + self->pos.w, self->pos.y + self->pos.h, color1, 2, 0);
        if (self->selected && self->inArea) C2D_DrawRectSolid(self->pos.x, self->pos.y, 0, self->pos.w, self->pos.h, color2);
    }
}