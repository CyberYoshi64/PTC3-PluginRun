#include "menu.h"

#define STRUCT menuStruct->template
#define ALPHA menuStruct->alpha
MenuStructPointers menuTemplate__Ptr = {menuTemplate__Init, menuTemplate__Exit, menuTemplate__Act, menuTemplate__Render, menuTemplate__AnimIn, menuTemplate__AnimOut};

void menuTemplate__Init() {

}

void menuTemplate__Exit() {

}

int menuTemplate__Act() {



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