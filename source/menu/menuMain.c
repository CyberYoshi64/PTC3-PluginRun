#include "menu.h"

#define STRUCT menuStruct->main
#define ALPHA menuStruct->alpha
MenuStructPointers menuMain__Ptr = {menuMain__Init, menuMain__Exit, menuMain__Act, menuMain__Render, menuMain__AnimIn, menuMain__AnimOut};

static u32 menuMain__isCYXPresent;

void menuMain__ButtonBackground(float x, float y, float w, float h, bool disabled) {
    if (!disabled)
        C2D_DrawRectangle(
            x, y, 0, w, h,
            0x281810|(C2D_FloatToU8(ALPHA)<<24), 0x201808|(C2D_FloatToU8(ALPHA)<<24),
            0x100800|(C2D_FloatToU8(ALPHA)<<24), 0x100800|(C2D_FloatToU8(ALPHA)<<24)
        );
    else
        C2D_DrawRectangle(
            x, y, 0, w, h,
            0x281810|(C2D_FloatToU8(ALPHA/2)<<24), 0x201808|(C2D_FloatToU8(ALPHA/2)<<24),
            0x100800|(C2D_FloatToU8(ALPHA/2)<<24), 0x100800|(C2D_FloatToU8(ALPHA/2)<<24)
        );
}

void menuMain__PlayBtnRender(float x, float y, float w, float h, bool selected, bool touched, bool disabled) {
    menuMain__ButtonBackground(x, y, w, h, disabled);
    C2D_DrawTriangle(x + 12, y + 8, STRUCT.buttonLabelFG, x + 12, y + h - 8, STRUCT.buttonLabelFG, x + h * .75 + 4, y + h / 2 , STRUCT.buttonLabelFG, 0);
    C2D_DrawText(&STRUCT.playText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + (w + (h * .5 + 8)) / 2, y + (h + 10) / 2 + 2.5f, 0, .7, .7, STRUCT.buttonLabelBG);
    C2D_DrawText(&STRUCT.playText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + (w + (h * .5 + 8)) / 2, y + (h + 10) / 2, 0, .7, .7, STRUCT.buttonLabelFG);
}

void menuMain__UpdateBtnRender(float x, float y, float w, float h, bool selected, bool touched, bool disabled) {
    menuMain__ButtonBackground(x, y, w, h, disabled);
    C2D_DrawRectSolid(x + h * .25 + 8, y + 8, 0, h / 4, h / 2 - 8, STRUCT.buttonLabelFG);
    C2D_DrawTriangle(x + 12, y + h / 2, STRUCT.buttonLabelFG, x + h * .375 + 8, y + h - 8, STRUCT.buttonLabelFG, x + h * .75 + 4, y + h / 2, STRUCT.buttonLabelFG, 0);
    C2D_DrawText(&STRUCT.updatesText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + (w + (h * .5 + 8)) / 2, y + (h + 10) / 2 + 2.5f, 0, .7, .7, STRUCT.buttonLabelBG);
    C2D_DrawText(&STRUCT.updatesText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + (w + (h * .5 + 8)) / 2, y + (h + 10) / 2, 0, .7, .7, STRUCT.buttonLabelFG);
}

void menuMain__ExitBtnRender(float x, float y, float w, float h, bool selected, bool touched, bool disabled) {
    menuMain__ButtonBackground(x, y, w, h, disabled);
    C2D_DrawText(&STRUCT.exitText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2 + 2.5f, 0, .7, .7, STRUCT.buttonLabelBG);
    C2D_DrawText(&STRUCT.exitText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2, 0, .7, .7, STRUCT.buttonLabelFG);
}

void menuMain__SettingsBtnRender(float x, float y, float w, float h, bool selected, bool touched, bool disabled) {
    menuMain__ButtonBackground(x, y, w, h, disabled);
    C2D_DrawRectSolid(x + h * .25 + 8, y + 8, 0, h / 4, h / 2 - 8, STRUCT.buttonLabelFG);
    C2D_DrawTriangle(x + 12, y + h / 2, STRUCT.buttonLabelFG, x + h * .375 + 8, y + h - 8, STRUCT.buttonLabelFG, x + h * .75 + 4, y + h / 2, STRUCT.buttonLabelFG, 0);
    C2D_DrawText(&STRUCT.settingsText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + (w + (h * .5 + 8)) / 2, y + (h + 10) / 2 + 2.5f, 0, .7, .7, STRUCT.buttonLabelBG);
    C2D_DrawText(&STRUCT.settingsText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + (w + (h * .5 + 8)) / 2, y + (h + 10) / 2, 0, .7, .7, STRUCT.buttonLabelFG);
}

void menuMain__Init() {
    STRUCT.textbuf = C2D_TextBufNew(4096);
    STRUCT.header.py = 4;
    STRUCT.header.sy = .5;
    
    C2D_TextParse(&STRUCT.header.text, STRUCT.textbuf, "Main Menu");
    C2D_TextParse(&STRUCT.playText, STRUCT.textbuf, "Start CYX");
    C2D_TextParse(&STRUCT.updatesText, STRUCT.textbuf, "Updates");
    C2D_TextParse(&STRUCT.settingsText, STRUCT.textbuf, "Settings");
    C2D_TextParse(&STRUCT.exitText, STRUCT.textbuf, "\uE072 Back");
    
    buttonSetupCB(&STRUCT.play, 50, 32, 220, 64, menuMain__PlayBtnRender);
    buttonSetupCB(&STRUCT.updates, 50, 102, 220, 64, menuMain__UpdateBtnRender);
    buttonSetupCB(&STRUCT.settings, 144, 200, 186, 40, menuMain__SettingsBtnRender);
    buttonSetupCB(&STRUCT.exitBtn, 0, 200, 128, 40, menuMain__ExitBtnRender);

    menuMain__isCYXPresent =
        archFileExists(SDMC_PREFIX PLUGIN_PATH) &&
        archDirExists(SDMC_PREFIX GAME_FSPATH);
}

void menuMain__Exit() {
    C2D_TextBufDelete(STRUCT.textbuf);
}

int menuMain__Act() {
    Dialog* dmydlg = NULL;

    if (HID_BTNPRESSED & KEY_ZR)
        spawnUpdateCheckDialog();

    if (buttonTick(&STRUCT.play)) {
        soundPlay(SND(SND_SELECT));
        if (!menuMain__isCYXPresent) {
            dmydlg = dialogNewTemp(DIALOG_ENABLE_BUTTON1);
            dialogMessage(dmydlg, "CYX is either not installed or broken.\n\nPlease go to Updates to download the missing data.");
            dialogPrepare(dmydlg);
            dialogShow(dmydlg);
            return MENUREACT_CONTINUE;
        }
        return menuNext(MENUID_PLAY);
    }

    if (buttonTick(&STRUCT.updates)) {
        soundPlay(SND(SND_SELECT));
        return menuNext(MENUID_UPDATES_TOP);
    }

    if (buttonTick(&STRUCT.settings)) {
        soundPlay(SND(SND_SELECT));
        return menuNext(MENUID_SETTING_TOP);
    }
    
    if (buttonTick(&STRUCT.exitBtn)) {
        soundPlay(SND(SND_BACK));
        menuAskExit();
    }
    
    return MENUREACT_CONTINUE;
}

void menuMain__Render(gfxScreen_t screen) {
    if (screen == GFX_TOP) {
        STRUCT.buttonLabelFG = C2D_FloatToU8(ALPHA)<<24 | 0xFFFFFF;
        STRUCT.buttonLabelBG = C2D_FloatToU8(ALPHA)<<22 | 0xFFFFFF;
        u32 headerBannerBack = C2D_Color32f(0, 0, 0, ALPHA / 2);
        C2D_DrawRectangle(  0, STRUCT.header.py - 8, 0, 200, 16 + (30 * STRUCT.header.sy), headerBannerBack, headerBannerBack, 0, headerBannerBack);
        C2D_DrawRectangle(200, STRUCT.header.py - 8, 0, 200, 16 + (30 * STRUCT.header.sy), headerBannerBack, headerBannerBack, headerBannerBack, 0);
        C2D_DrawText(&STRUCT.header.text, C2D_WithColor|C2D_AlignCenter, 200, STRUCT.header.py, 0, .75, STRUCT.header.sy, C2D_Color32f(1, 1, 1, ALPHA));
    } else {
        buttonRender(&STRUCT.play);
        buttonRender(&STRUCT.updates);
        buttonRender(&STRUCT.settings);
        buttonRender(&STRUCT.exitBtn);
    }
}

bool menuMain__AnimIn() {
    ALPHA += .125f;
    STRUCT.exitBtn.pos.y = 200 + (1 - ALPHA) * 4;
    STRUCT.settings.pos.y = 200 + (1 - ALPHA) * 4;
    STRUCT.header.sy += .030625f;
    //STRUCT.header.py -= 1.5f;
    STRUCT.header.py += .5f;
    return ALPHA >= 1.f;
}

bool menuMain__AnimOut() {
    ALPHA -= .125f;
    STRUCT.exitBtn.pos.y = 200 + (1 - ALPHA) * 4;
    STRUCT.settings.pos.y = 200 + (1 - ALPHA) * 4;
    STRUCT.header.sy -= .030625f;
    //STRUCT.header.py += 1.5f;
    STRUCT.header.py -= .5f;
    return ALPHA <= 0.f;
}