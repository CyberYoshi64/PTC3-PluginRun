#pragma once

#include <3ds.h>
#include <citro2d.h>
#include "menuButton.h"

typedef struct {
    C2D_TextBuf textbuf;
    struct {
        C2D_Text    text;
        float       px;
        float       py;
        float       sx;
        float       sy;
    } header;
    u32 buttonLabelFG;
    u32 buttonLabelBG;
    
    Button      exitBtn;
    C2D_Text    exitText;
    
    C2D_Text    launchHeading;
    
    Button      launchJPNBtn;
    C2D_Text    launchJPNText;
    
    Button      launchUSABtn;
    C2D_Text    launchUSAText;
    
    Button      launchEURBtn;
    C2D_Text    launchEURText;
    
    float       alpha;
} MenuPlay__Struct;

void    menuPlay__Init(void);
void    menuPlay__Exit(void);
int     menuPlay__Act(void);
void    menuPlay__Render(gfxScreen_t screen);
bool    menuPlay__AnimIn(void);
bool    menuPlay__AnimOut(void);
