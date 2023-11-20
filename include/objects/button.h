#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

typedef void (*ButtonRenderCB)(float, float, float, float, bool, bool, bool); // x, y, width, height, selected, inArea, disabled

typedef struct Button_s {
    struct {
        float x, y, w, h;
    } pos;
    bool selected;          // Whether the button is selected
    bool inArea;            // Whether the button is physically being touched
    bool disabled;          // Whether the button is disabled
    bool renderBox;         // Whether to render the default box around the button
    ButtonRenderCB render;  // Render callback used to draw the button content
} Button;

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