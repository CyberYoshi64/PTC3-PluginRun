#pragma once

#include <3ds.h>
#include "menu/menuMain.h"
#include "hidRead.h"
#include "menuDialog.h"
#include "menuButton.h"

typedef enum MenuID {
    MENUID_NONE = 0,
    MENUID_MAIN,
    MENUID_PLAY,
    MENUID_UPDATES,
    MENUID_SETTING_TOP,
} MenuID;

enum MenuReactValue {
    MENUREACT_CONTINUE = 0,
    MENUREACT_NEXTMENU,
};

typedef void (*menuPtrVoidF)();
typedef bool (*menuPtrBoolF)();
typedef int (*menuPtrActF)();
typedef void (*menuPtrRenderF)(gfxScreen_t);

typedef struct MenuStructPointers_s {
    menuPtrVoidF    Init;
    menuPtrVoidF    Exit;
    menuPtrActF     Act;
    menuPtrRenderF  Render;
    menuPtrBoolF    AnimIn;
    menuPtrBoolF    AnimOut;
} MenuStructPointers;

typedef struct {
    float       alpha;
    union {
        MenuMain__Struct    main;
        u8                  pad[32700];
    };
} CTR_ALIGN(1024) MenuStruct;

#define MENUSTRUCT_SIZE sizeof(MenuStruct)

extern MenuStructPointers menuMain__Ptr;
extern MenuStruct* menuStruct;

extern MenuStructPointers* currMenuPtr;
extern MenuStructPointers* nextMenuPtr;
extern MenuDialog*         currDialog;
extern MenuDialog*         nextDialog;

void menuDialogShow(MenuDialog* dlg);
int menuNext(MenuID id);
void menuAskExit(void);
bool spawnUpdateCheckDialog(void);