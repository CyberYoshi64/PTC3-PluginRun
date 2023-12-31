#pragma once

#include <3ds.h>
#include <citro2d.h>
#include "main.h"
#include "objects/button.h"
#include "objects/dialog.h"

typedef struct {
    C2D_TextBuf textbuf;
    struct {
        C2D_Text    text;
        float       py;
        float       sy;
    } header;
    u32 buttonLabelFG;
    u32 buttonLabelBG;
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

void    menuMain__ButtonBackground(float x, float y, float w, float h, bool disabled);
void    menuMain__Init(void);
void    menuMain__Exit(void);
int     menuMain__Act(void);
void    menuMain__Render(gfxScreen_t screen);
bool    menuMain__AnimIn(void);
bool    menuMain__AnimOut(void);