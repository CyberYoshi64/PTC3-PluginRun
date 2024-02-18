#include "menu.h"

#include "yaz0.h"

#define STRUCT menuStruct->updatesTop
#define ALPHA menuStruct->alpha
MenuStructPointers menuUpdatesTop__Ptr = {menuUpdatesTop__Init, menuUpdatesTop__Exit, menuUpdatesTop__Act, menuUpdatesTop__Render, menuUpdatesTop__AnimIn, menuUpdatesTop__AnimOut};

static bool updatesHasUpdates = false;
bool updatesDownloadedData = false;
s32 updatesDlAppTask = -1;

void menuUpdatesTop__ExitBtnRender(float x, float y, float w, float h, bool selected, bool touched, bool disabled) {
    menuMain__ButtonBackground(x, y, w, h, disabled);
    C2D_DrawText(&STRUCT.exitText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2 + 2.5f, 0, .7, .7, STRUCT.buttonLabelBG);
    C2D_DrawText(&STRUCT.exitText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2, 0, .7, .7, STRUCT.buttonLabelFG);
}

void menuUpdatesTop__Init() {
    STRUCT.textbuf = C2D_TextBufNew(4096);
    STRUCT.header.py = 4;
    STRUCT.header.sy = .5;
    C2D_TextParse(&STRUCT.header.text, STRUCT.textbuf, "Updates");
    C2D_TextParse(&STRUCT.exitText, STRUCT.textbuf, "\uE001 Back");
    buttonSetupCB(&STRUCT.exitBtn, 0, 200, 128, 40, menuUpdatesTop__ExitBtnRender);
    buttonSetup(&STRUCT.dmy1Btn, 15, 10, 90, 40);
    buttonSetup(&STRUCT.dmy2Btn, 115, 10, 90, 40);
    buttonSetup(&STRUCT.dmy3Btn, 215, 10, 90, 40);
    buttonSetup(&STRUCT.dmy4Btn, 15, 60, 90, 40);
    buttonSetup(&STRUCT.dmy5Btn, 115, 60, 90, 40);
    buttonSetup(&STRUCT.dmy6Btn, 215, 60, 90, 40);
}

void menuUpdatesTop__Exit() {
    C2D_TextBufDelete(STRUCT.textbuf);
}

int menuUpdatesTop__Act() {

    if (buttonTick(&STRUCT.exitBtn) || (HID_BTNPRESSED & KEY_B)) {
        soundPlay(SND(SND_BACK));
        return menuNext(MENUID_MAIN);
    }

    if (buttonTick(&STRUCT.dmy1Btn)) {
        if R_FAILED(aptJumpToEManual())
            drawError("Failed to open the electronic manual.", true, KEY_A);
    }
    if (buttonTick(&STRUCT.dmy2Btn)) {
        if R_FAILED(aptJumpToThemeShop())
            drawError("Failed to open the HOME Menu for the Theme Shop.", true, KEY_A);
    }
    if (buttonTick(&STRUCT.dmy3Btn)) {
        if R_FAILED(aptJumpToBadgeBox())
            drawError("Failed to open the HOME Menu for the badge box.", true, KEY_A);
    }
    if (buttonTick(&STRUCT.dmy4Btn)) {
        //char aptjmpasd[0x400] = {0};
        aptLaunchSystemApplet(APPID_WEB, NULL, 0, 0);
    }
    if (buttonTick(&STRUCT.dmy5Btn)) {
        //char aptjmpasd[0x400] = {0};
        aptLaunchSystemApplet(APPID_MIIVERSE, NULL, 0, 0);
    }
    if (buttonTick(&STRUCT.dmy6Btn)) {
        
    }
    
    return MENUREACT_CONTINUE;
}

void menuUpdatesTop__Render(gfxScreen_t screen) {
    if (screen == GFX_TOP) {
        STRUCT.buttonLabelFG = C2D_FloatToU8(ALPHA)<<24 | 0xFFFFFF;
        STRUCT.buttonLabelBG = C2D_FloatToU8(ALPHA)<<22 | 0xFFFFFF;
        u32 headerBannerBack = C2D_Color32f(0, 0, 0, ALPHA / 2);
        C2D_DrawRectangle(  0, STRUCT.header.py - 8, 0, 200, 16 + (30 * STRUCT.header.sy), headerBannerBack, headerBannerBack, 0, headerBannerBack);
        C2D_DrawRectangle(200, STRUCT.header.py - 8, 0, 200, 16 + (30 * STRUCT.header.sy), headerBannerBack, headerBannerBack, headerBannerBack, 0);
        C2D_DrawText(&STRUCT.header.text, C2D_WithColor|C2D_AlignCenter, 200, STRUCT.header.py, 0, .75, STRUCT.header.sy, C2D_Color32f(1, 1, 1, ALPHA));
    } else {
        buttonRender(&STRUCT.exitBtn);
        
        buttonRender(&STRUCT.dmy1Btn);
        buttonRender(&STRUCT.dmy2Btn);
        buttonRender(&STRUCT.dmy3Btn);
        buttonRender(&STRUCT.dmy4Btn);
        buttonRender(&STRUCT.dmy5Btn);
        buttonRender(&STRUCT.dmy6Btn);
    }
}

bool menuUpdatesTop__AnimIn() {
    ALPHA += .125f;
    STRUCT.exitBtn.pos.y = 200 + (1 - ALPHA) * 4;
    STRUCT.header.sy += .030625f;
    //STRUCT.header.py -= 1.5f;
    STRUCT.header.py += .5f;
    return ALPHA >= 1.f;
}

bool menuUpdatesTop__AnimOut() {
    ALPHA -= .125f;
    STRUCT.exitBtn.pos.y = 200 + (1 - ALPHA) * 4;
    STRUCT.header.sy -= .030625f;
    //STRUCT.header.py += 1.5f;
    STRUCT.header.py -= .5f;
    return ALPHA <= 0.f;
}