#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "menuButton.h"

#define MENUDLG__TITLE_SIZE   64
#define MENUDLG__MSG_SIZE     2048
#define MENUDLG__BTN_COUNT    2
#define MENUDLG__BTN_SIZE     32
#define MENUDLG__PARSELINE    150
#define MENUDLG__MISCBUF_SIZE 256
#define MENUDLG__BUTTON1  "Cancel"
#define MENUDLG__BUTTON2  "Okay"

enum MenuDlgMode {
    MENUDIALOG_ENABLE_BUTTON1 = BIT(0),
    MENUDIALOG_ENABLE_BUTTON2 = BIT(0)|BIT(1),
    MENUDIALOG_ENABLE_BUTTON__2 = BIT(1),
    MENUDIALOG_TITLE = BIT(2),
    MENUDIALOG_WAIT = BIT(8),
    MENUDIALOG_PROGRESS = BIT(9),
    MENUDIALOG_FADE_TOP = BIT(30),
    MENUDIALOG_ONESHOT = BIT(31)  // Immediately free dialog when closed
};

/**
 * @brief Dialog Wait Mode callback
 * @return Boolean, whether to close the dialog
 * @param[out] u32 Button mask
 * @param[out] float Progress percentage
 */
typedef bool    (*MenuDlgWaitCBF)(u32*, float*);

/**
 * @brief Dialog Button callback
 * @return Boolean, whether to accept the input
 * @param[in] s32  Dialog Result Code
 */
typedef bool    (*MenuDlgButtonCBF)(s32);

typedef struct MenuDialog_s {
    u32                 mode;
    s32                 rc;
    float               boxScale;
    float               boxAlpha;
    u32                 state;
    u32                 buttonState;
    float               progress;
    MenuDlgWaitCBF      waitCallback;
    MenuDlgButtonCBF    buttonCallback;
    C2D_TextBuf         miscBuf;
    C2D_TextBuf         msgBuf;
    Button              cancelBtn;
    Button              okayBtn;

    char                title[MENUDLG__TITLE_SIZE];
    char                message[MENUDLG__MSG_SIZE];
    char                buttonText[MENUDLG__BTN_COUNT][MENUDLG__BTN_SIZE];
    C2D_Text            titleT;
    C2D_Text            msgT;
    C2D_Text            buttonT[MENUDLG__BTN_COUNT];
    u32                 lines[MENUDLG__PARSELINE];
} CTR_ALIGN(256) MenuDialog;

extern C2D_SpriteSheet menuDialogSheet;

MenuDialog* menuDialogNew(u32 mode);
MenuDialog* menuDialogNewTemp(u32 mode);
void menuDialogMessage(MenuDialog* self, const char* message);
void menuDialogMessageAppend(MenuDialog* self, const char* message);
void menuDialogTitle(MenuDialog* self, const char* title);
void menuDialogButton(MenuDialog* self, int index, const char* text);
void menuDialogSetWaitCallback(MenuDialog* self, MenuDlgWaitCBF func);
void menuDialogSetButtonCallback(MenuDialog* self, MenuDlgButtonCBF func);
void menuDialogPrepare(MenuDialog* self);
void menuDialogFree(MenuDialog* self);

void menuDialog__Render(MenuDialog* self, gfxScreen_t screen);
bool menuDialog__Tick(MenuDialog* self);
