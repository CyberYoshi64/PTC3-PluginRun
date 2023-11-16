#pragma once

#include <3ds.h>
#include <citro2d.h>
#include "menuButton.h"

typedef struct {
    C2D_TextBuf textbuf;
    struct header {
        C2D_Text    text;
        float       px;
        float       py;
        float       sx;
        float       sy;
    } header;
    Button      play;
    C2D_Text    playText;
    Button      updates;
    C2D_Text    updatesText;
    Button      settings;
    C2D_Text    settingsText;
    Button      exitBtn;
    C2D_Text    exitText;
    float       alpha;
} MenuMain__Struct;

void menuMain__Init(void);
void menuMain__Exit(void);
int menuMain__Act(void);
void menuMain__Render(gfxScreen_t screen);
bool menuMain__AnimIn(void);
bool menuMain__AnimOut(void);
