#pragma once

#include <3ds.h>
#include "main.h"

// Menus
#include "menu/menuMain.h"
#include "menu/menuPlay.h"
#include "menu/menuUpdatesTop.h"
#include "menu/menuTemplate.h"

typedef enum MenuID {
    MENUID_NONE = 0,        // Used to exit the application
    MENUID_MAIN,            // Main Menu
    MENUID_PLAY,            // Start CYX
    MENUID_UPDATES_TOP,     // Updates Top
    MENUID_SETTING_TOP,     // Settings Top
    MENUID_SAVEFS_COPY,     // "Menu" to copy between CYX and SB3 savefs
} MenuID;

enum MenuReactValue {
    MENUREACT_CONTINUE = 0, // Menu is done processing input and wants to continue
    MENUREACT_NEXTMENU,     // Menu is done processing input and wants to transition to another one - Only invoke this with menuNext()
};

typedef void (*menuPtrVoidF)();
typedef bool (*menuPtrBoolF)();
typedef int (*menuPtrActF)();
typedef void (*menuPtrRenderF)(gfxScreen_t);

typedef struct MenuStructPointers_s {
    menuPtrVoidF    Init;       // Initialize & allocate data needed for the menu
    menuPtrVoidF    Exit;       // Free and clean up data used by the menu
    menuPtrActF     Act;        // Respond to user input
    menuPtrRenderF  Render;     // Render the menu (called twice for both screens)
    menuPtrBoolF    AnimIn;     // Used for fading in the menu elements (optional)
    menuPtrBoolF    AnimOut;    // Used for fading out the menu elements (optional)
} MenuStructPointers;

typedef struct {
    float       alpha;
    union {
        MenuMain__Struct        main;
        MenuPlay__Struct        play;
        MenuUpdatesTop__Struct  updatesTop;
        MenuTemplate__Struct    _template; // Dummy entry (it's to simplify creating menus)
        u8                  pad[32700];
    };
} CTR_ALIGN(1024) MenuStruct;

#define MENUSTRUCT_SIZE sizeof(MenuStruct)

extern MenuStructPointers menuMain__Ptr;
extern MenuStructPointers menuPlay__Ptr;
extern MenuStructPointers menuUpdatesTop__Ptr;
extern MenuStructPointers menuTemplate__Ptr;

extern MenuStruct* menuStruct;

extern MenuStructPointers*  currMenuPtr;
extern MenuStructPointers*  nextMenuPtr;
extern Dialog*              currDialog;
extern Dialog*              nextDialog;

void dialogShow(Dialog* dlg);
int menuNext(MenuID id);
void menuAskExit(void);
bool spawnUpdateCheckDialog(void);