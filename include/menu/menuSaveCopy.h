#pragma once

#include <3ds.h>
#include <citro2d.h>
#include "main.h"
#include "objects/button.h"
#include "objects/dialog.h"
#include "objects/waitIcon.h"

typedef struct SaveCopyEntry_s {
    char source[256];
    char dest[256];
    int appTaskId;
    bool folder;
    bool done;
    bool succeeded;
} SaveCopyEntry;

typedef struct {
    C2D_TextBuf textbuf;
    struct {
        C2D_Text    text;
        float       py;
        float       sy;
    } header;
    Button      exitBtn;
    C2D_Text    exitText;

    C2D_TextBuf fnamebuf;
    
    ProgressBar current;
    ProgressBar total;
    WaitIcon    waiticon;
    C2D_Text    label1;
    
    u32 buttonLabelFG;
    u32 buttonLabelBG;
    
} MenuSaveCopy__Struct;

void    menuSaveCopy__Init(void);
void    menuSaveCopy__Exit(void);
int     menuSaveCopy__Act(void);
void    menuSaveCopy__Render(gfxScreen_t screen);
bool    menuSaveCopy__AnimIn(void);
bool    menuSaveCopy__AnimOut(void);

bool    saveCopy__Add(const char* src, const char* dest);
void    saveCopy__Clear(void);
void    saveCopy__RetryFailed(void);
