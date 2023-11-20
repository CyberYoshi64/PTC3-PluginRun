#include "menu.h"

#define STRUCT menuStruct->_template
#define ALPHA menuStruct->alpha
MenuStructPointers menuTemplate__Ptr = {menuTemplate__Init, menuTemplate__Exit, menuTemplate__Act, menuTemplate__Render, menuTemplate__AnimIn, menuTemplate__AnimOut};

void menuTemplate__Init() {
    STRUCT.textbuf = C2D_TextBufNew(4096);
}

void menuTemplate__Exit() {
    C2D_TextBufDelete(STRUCT.textbuf);

}

int menuTemplate__Act() {

    // Only for use here, as this template is used as fallback
    Dialog* d = dialogNewTemp(DIALOG_ENABLE_BUTTON1|DIALOG_TITLE);
    dialogTitle(d, "Menu flow error");
    dialogMessage(d, "Something tried to jump to a menu that doesn't exist (yet)...\n\nReturning to the main menu.");
    dialogPrepare(d);
    dialogShow(d);
    return menuNext(MENUID_MAIN);
    //

    return MENUREACT_CONTINUE;
}

void menuTemplate__Render(gfxScreen_t screen) {
    if (screen == GFX_TOP) {

    } else {

    }
}

bool menuTemplate__AnimIn() {
    ALPHA += .125f;

    return ALPHA >= 1.f;
}

bool menuTemplate__AnimOut() {
    ALPHA -= .125f;

    return ALPHA <= 0.f;
}