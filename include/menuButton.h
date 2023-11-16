#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

typedef void (*ButtonRenderCB)(float, float, float, float, bool, bool, bool);

typedef struct Button_s {
    struct {
        float x, y, w, h;
    } pos;
    bool selected;
    bool inArea;
    bool disabled;
    bool renderBox;
    ButtonRenderCB render;
} CTR_ALIGN(16) Button;

#define BUTTON_STRUCTSIZE   sizeof(Button)

extern float buttonAlpha;

void buttonSetup(Button* self, float x, float y, float width, float height);
void buttonSetupCB(Button* self, float x, float y, float width, float height, ButtonRenderCB render);
void buttonSetPosition(Button* self, float x, float y);
void buttonSetSize(Button* self, float width, float height);
void buttonSetCallback(Button* self, ButtonRenderCB render);
void buttonSetEnabled(Button* self, bool enabled);
void buttonSetDrawBox(Button* self, bool enabled);
bool buttonTick(Button* self);
void buttonRender(Button* self);