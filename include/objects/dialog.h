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
 * @param[out] bm Button mask
 * @param[out] progress Progress percentage
 */
typedef bool    (*DlgWaitCBF)(u32* bm, float* progress);

/**
 * @brief Dialog Button callback
 * @return Boolean, whether to accept the input
 * @param[in] rc Dialog Result Code
 */
typedef bool    (*DlgButtonCBF)(s32 rc);

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

// Dedicated spritesheet for the dialog box
extern C2D_SpriteSheet dialogSheet;

/**
 * @brief Allocate a new dialog box structure
 * 
 * @param mode Dialog Mode (see enum DlgMode)
 * @return New dialog structure; NULL on failure
 */
Dialog* dialogNew(u32 mode);

/**
 * @brief Allocate a temporary dialog box structure
 * 
 * @note
 * This function only allocates, if the menu system has a free spot
 * for a dialog box to queue itself in. If there is no free spot, NULL is returned.
 * 
 * @note
 * Additionally, if a dialog struct was created, you shouldn't `free` it yourself;
 * that is handles automatically.
 * 
 * @param mode Dialog Mode (see enum DlgMode)
 * @return New dialog structure; NULL on failure
 */
Dialog* dialogNewTemp(u32 mode);

/**
 * @brief Sets the message of a dialog box
 * 
 * @note
 * Line breaks get automatically inserted.
 * 
 * @note
 * The dialog box currently doesn't implement a scrolling system,
 * so if the text takes too many lines, it gets cut off.
 * 
 * @param self Dialog box struct
 * @param message Message to display
 */
void dialogMessage(Dialog* self, const char* message);

/**
 * @brief Append to a message of a dialog box
 * 
 * @note
 * Line breaks get automatically inserted.
 * 
 * @note
 * The dialog box currently doesn't implement a scrolling system,
 * so if the text takes too many lines, it gets cut off.
 * 
 * @param self Dialog box struct
 * @param message Message to display
 */
void dialogMessageAppend(Dialog* self, const char* message);

/**
 * @brief Set a title of a dialog box
 * 
 * @note
 * This function is only effective if the dialog mode includes
 * the `DIALOG_TITLE` flag.
 * 
 * @param self Dialog box struct
 * @param title Title to display
 */
void dialogTitle(Dialog* self, const char* title);

/**
 * @brief Override the text of a button of a dialog box
 * 
 * @note
 * This function is only effective if the dialog mode includes
 * either the `DIALOG_ENABLE_BUTTON1` or `DIALOG_ENABLE_BUTTON2` flags.
 * 
 * @note
 * If the dialog mode doesn't include the `DIALOG_ENABLE_BUTTON2` flag,
 * setting the text for index 1 has no effect.
 * 
 * @param self Dialog box struct
 * @param index 0=Reset, 1=Cancel, 2=Okay
 * @param text Text to display
 */
void dialogButton(Dialog* self, int index, const char* text);

/**
 * @brief Set the wait callback of a dialog box (for use with `DIALOG_WAIT`/`DIALOG_PROGRESS`)
 * 
 * @param self Dialog box struct
 * @param func Wait callback function
 */
void dialogSetWaitCallback(Dialog* self, DlgWaitCBF func);

/**
 * @brief Set the button callback of a dialog box
 * 
 * @param self Dialog box struct
 * @param func Button callback function
 */
void dialogSetButtonCallback(Dialog* self, DlgButtonCBF func);

/**
 * @brief Prepares a dialog box for rendering itself
 * 
 * @param self Dialog box struct
 */
void dialogPrepare(Dialog* self);

/**
 * @brief Unallocate a dialog box from memory
 * 
 * @param self Dialog box struct
 */
void dialogFree(Dialog* self);

void dialog__Render(Dialog* self, gfxScreen_t screen);
bool dialog__Tick(Dialog* self);
