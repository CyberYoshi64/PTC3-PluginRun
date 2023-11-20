#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "objects/button.h"
#include "objects/progress.h"
#include "objects/waitIcon.h"

#define __DLG_TITLE_SIZE   64
#define __DLG_MSG_SIZE     2048
#define __DLG_BTN_COUNT    2
#define __DLG_BTN_SIZE     32
#define __DLG_PARSELINE    150
#define __DLG_MISCBUF_SIZE 256
#define __DLG_BUTTON1  "Cancel"
#define __DLG_BUTTON2  "Okay"

enum DlgMode {
    DIALOG_ENABLE_BUTTON1 = BIT(0),
    DIALOG_ENABLE_BUTTON2 = BIT(0)|BIT(1),
    DIALOG_ENABLE_BUTTON__2 = BIT(1),
    DIALOG_TITLE = BIT(2),
    DIALOG_WAIT = BIT(8),
    DIALOG_PROGRESS = BIT(9),
    DIALOG_FADE_TOP = BIT(30),
    DIALOG_ONESHOT = BIT(31)  // Immediately free dialog when closed
};

/**
 * @brief Dialog Wait Mode callback
 * @return Boolean, whether to close the dialog
 * @param[out] u32 Button mask
 * @param[out] float Progress percentage
 */
typedef bool    (*DlgWaitCBF)(u32*, float*);

/**
 * @brief Dialog Button callback
 * @return Boolean, whether to accept the input
 * @param[in] s32  Dialog Result Code
 */
typedef bool    (*DlgButtonCBF)(s32);

typedef struct Dialog_s {
    u32                 mode;           // Mode to be initialized with
    s32                 rc;             // Result code (1 = Okay, 2 = Cancel)
    float               boxScale;
    float               boxAlpha;
    u32                 state;
    u32                 buttonState;    // Not used yet, should gray out inactive buttons (see buttonCallback)
    
    ProgressBar         progress;
    WaitIcon            waiticon;

    DlgWaitCBF          waitCallback;   // Wait callback - close the dialog when the task is done or to set inactive buttons
    DlgButtonCBF        buttonCallback; // Result callback - whether to accept the result code or act on it
    C2D_TextBuf         miscBuf;
    C2D_TextBuf         msgBuf;
    Button              cancelBtn;
    Button              okayBtn;

    char                title[__DLG_TITLE_SIZE];
    char                message[__DLG_MSG_SIZE];
    char                buttonText[__DLG_BTN_COUNT][__DLG_BTN_SIZE];
    C2D_Text            titleT;
    C2D_Text            msgT;
    C2D_Text            buttonT[__DLG_BTN_COUNT];
    u32                 lines[__DLG_PARSELINE];
} Dialog;

extern C2D_SpriteSheet dialogSheet;

Dialog* dialogNew(u32 mode);
Dialog* dialogNewTemp(u32 mode);
void dialogMessage(Dialog* self, const char* message);
void dialogMessageAppend(Dialog* self, const char* message);
void dialogTitle(Dialog* self, const char* title);
void dialogButton(Dialog* self, int index, const char* text);
void dialogSetWaitCallback(Dialog* self, DlgWaitCBF func);
void dialogSetButtonCallback(Dialog* self, DlgButtonCBF func);
void dialogPrepare(Dialog* self);
void dialogFree(Dialog* self);

void dialog__Render(Dialog* self, gfxScreen_t screen);
bool dialog__Tick(Dialog* self);
