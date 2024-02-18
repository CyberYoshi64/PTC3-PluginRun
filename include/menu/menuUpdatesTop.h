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
    Button      exitBtn;
    C2D_Text    exitText;
    Button      dmy1Btn;
    Button      dmy2Btn;
    Button      dmy3Btn;
    Button      dmy4Btn;
    Button      dmy5Btn;
    Button      dmy6Btn;
    
    u32 buttonLabelFG;
    u32 buttonLabelBG;
    
} MenuUpdatesTop__Struct;

extern bool updatesDownloadedData;
extern s32  updatesDlAppTask;

void    menuUpdatesTop__Init(void);
void    menuUpdatesTop__Exit(void);
int     menuUpdatesTop__Act(void);
void    menuUpdatesTop__Render(gfxScreen_t screen);
bool    menuUpdatesTop__AnimIn(void);
bool    menuUpdatesTop__AnimOut(void);
