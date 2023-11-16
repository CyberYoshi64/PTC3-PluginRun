#include "menu.h"

#define STRUCT menuStruct->main
#define ALPHA menuStruct->alpha
MenuStructPointers menuMain__Ptr = {menuMain__Init, menuMain__Exit, menuMain__Act, menuMain__Render, menuMain__AnimIn, menuMain__AnimOut};

void menuMain__ButtonBackground(float x, float y, float w, float h) {
    C2D_DrawRectangle(
        x, y, 0, w, h,
        0xFF281810&STRUCT.buttonLabelFG, 0xFF201808&STRUCT.buttonLabelFG,
        0xFF100800&STRUCT.buttonLabelFG, 0xFF100800&STRUCT.buttonLabelFG
    );
}

void menuMain__PlayBtnRender(float x, float y, float w, float h, bool selected, bool touched, bool disabled) {
    menuMain__ButtonBackground(x, y, w, h);
    C2D_DrawTriangle(x + 12, y + 8, STRUCT.buttonLabelFG, x + 12, y + h - 8, STRUCT.buttonLabelFG, x + h * .75 + 4, y + h / 2 , STRUCT.buttonLabelFG, 0);
    C2D_DrawText(&STRUCT.playText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + (w + (h * .5 + 8)) / 2, y + (h + 10) / 2 + 2.5f, 0, .7, .7, STRUCT.buttonLabelBG);
    C2D_DrawText(&STRUCT.playText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + (w + (h * .5 + 8)) / 2, y + (h + 10) / 2, 0, .7, .7, STRUCT.buttonLabelFG);
}

void menuMain__UpdateBtnRender(float x, float y, float w, float h, bool selected, bool touched, bool disabled) {
    menuMain__ButtonBackground(x, y, w, h);
    C2D_DrawRectSolid(x + h * .25 + 8, y + 8, 0, h / 4, h / 2 - 8, STRUCT.buttonLabelFG);
    C2D_DrawTriangle(x + 12, y + h / 2, STRUCT.buttonLabelFG, x + h * .375 + 8, y + h - 8, STRUCT.buttonLabelFG, x + h * .75 + 4, y + h / 2, STRUCT.buttonLabelFG, 0);
    C2D_DrawText(&STRUCT.updatesText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + (w + (h * .5 + 8)) / 2, y + (h + 10) / 2 + 2.5f, 0, .7, .7, STRUCT.buttonLabelBG);
    C2D_DrawText(&STRUCT.updatesText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + (w + (h * .5 + 8)) / 2, y + (h + 10) / 2, 0, .7, .7, STRUCT.buttonLabelFG);
}

void menuMain__ExitBtnRender(float x, float y, float w, float h, bool selected, bool touched, bool disabled) {
    menuMain__ButtonBackground(x, y, w, h);
    C2D_DrawText(&STRUCT.exitText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2 + 2.5f, 0, .7, .7, STRUCT.buttonLabelBG);
    C2D_DrawText(&STRUCT.exitText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2, 0, .7, .7, STRUCT.buttonLabelFG);
}

void menuMain__SettingsBtnRender(float x, float y, float w, float h, bool selected, bool touched, bool disabled) {
    menuMain__ButtonBackground(x, y, w, h);
    C2D_DrawRectSolid(x + h * .25 + 8, y + 8, 0, h / 4, h / 2 - 8, STRUCT.buttonLabelFG);
    C2D_DrawTriangle(x + 12, y + h / 2, STRUCT.buttonLabelFG, x + h * .375 + 8, y + h - 8, STRUCT.buttonLabelFG, x + h * .75 + 4, y + h / 2, STRUCT.buttonLabelFG, 0);
    C2D_DrawText(&STRUCT.settingsText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + (w + (h * .5 + 8)) / 2, y + (h + 10) / 2 + 2.5f, 0, .7, .7, STRUCT.buttonLabelBG);
    C2D_DrawText(&STRUCT.settingsText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + (w + (h * .5 + 8)) / 2, y + (h + 10) / 2, 0, .7, .7, STRUCT.buttonLabelFG);
}

void menuMain__Init() {
    STRUCT.textbuf = C2D_TextBufNew(4096);
    STRUCT.header.px = 200;
    STRUCT.header.py = 4;
    STRUCT.header.sx = .75;
    STRUCT.header.sy = .5;
    
    C2D_TextParse(&STRUCT.header.text, STRUCT.textbuf, "Main Menu");
    C2D_TextParse(&STRUCT.playText, STRUCT.textbuf, "Start CYX");
    C2D_TextParse(&STRUCT.updatesText, STRUCT.textbuf, "Updates");
    C2D_TextParse(&STRUCT.settingsText, STRUCT.textbuf, "Settings");
    C2D_TextParse(&STRUCT.exitText, STRUCT.textbuf, "\uE072 Back");
    
    buttonSetupCB(&STRUCT.play, 50, 32, 220, 64, menuMain__PlayBtnRender);
    buttonSetupCB(&STRUCT.updates, 50, 102, 220, 64, menuMain__UpdateBtnRender);
    buttonSetupCB(&STRUCT.settings, 120, 200, 200, 40, menuMain__SettingsBtnRender);
    buttonSetupCB(&STRUCT.exitBtn, 0, 200, 100, 40, menuMain__ExitBtnRender);
}

void menuMain__Exit() {
    C2D_TextBufDelete(STRUCT.textbuf);
}

int menuMain__Act() {
    MenuDialog* dmydlg = NULL;

    if (HID_BTNPRESSED & KEY_DLEFT) {
        dmydlg = menuDialogNewTemp(MENUDIALOG_ENABLE_BUTTON1|MENUDIALOG_WAIT);
        menuDialogMessage(dmydlg, "You pressed left.\n");
        menuDialogMessageAppend(dmydlg, "This is a looong line of text, I hope it ain't break. Oh wait... it did? What a shame!\n\n1234567890123 123456789012345678901234567890123456789012345678901234567890");
        menuDialogPrepare(dmydlg);
        menuDialogShow(dmydlg);
    }
    if (HID_BTNPRESSED & KEY_DUP) {
        dmydlg = menuDialogNewTemp(MENUDIALOG_PROGRESS|MENUDIALOG_WAIT);
        menuDialogMessage(dmydlg, "You pressed up.\n");
        menuDialogMessageAppend(dmydlg, "This is a looong line of text, I hope it ain't break. Oh wait... it did? What a shame!\n\n123456789012 123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
        menuDialogPrepare(dmydlg);
        menuDialogShow(dmydlg);
    }
    if (HID_BTNPRESSED & KEY_DRIGHT) {
        dmydlg = menuDialogNewTemp(MENUDIALOG_ENABLE_BUTTON2);
        menuDialogMessage(dmydlg, "You pressed right.\n");
        menuDialogMessageAppend(dmydlg, "This is a looong line of text, I hope it ain't break. Oh wait... it did? What a shame!\n\n12345678901 123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
        menuDialogPrepare(dmydlg);
        menuDialogShow(dmydlg);
    }
    if (HID_BTNPRESSED & KEY_DDOWN) {
        dmydlg = menuDialogNewTemp(MENUDIALOG_ENABLE_BUTTON1|MENUDIALOG_TITLE);
        menuDialogTitle(dmydlg, "Error Code 000-0000");
        menuDialogMessage(dmydlg, "You pressed down.\n");
        menuDialogMessageAppend(dmydlg, "This is a looong line of text, I hope it ain't break. Oh wait... it did? What a shame!\n\n12345678901234 123456789012345678901234567890");
        menuDialogPrepare(dmydlg);
        menuDialogShow(dmydlg);
    }
    if (HID_BTNPRESSED & KEY_ZR)
        spawnUpdateCheckDialog();

    if (buttonTick(&STRUCT.play)) {
        return menuNext(MENUID_PLAY);
    }

    if (buttonTick(&STRUCT.updates)) {
        dmydlg = menuDialogNewTemp(MENUDIALOG_ENABLE_BUTTON1|MENUDIALOG_TITLE);
        menuDialogTitle(dmydlg, "Feature not implemented");
        menuDialogMessage(dmydlg, "The updates page is not yet available. Please check for updates manually.");
        menuDialogPrepare(dmydlg);
        menuDialogShow(dmydlg);
    }

    if (buttonTick(&STRUCT.settings)) {
        dmydlg = menuDialogNewTemp(MENUDIALOG_ENABLE_BUTTON1|MENUDIALOG_TITLE);
        menuDialogTitle(dmydlg, "Feature not implemented");
        menuDialogMessage(dmydlg, "Settings is not yet available. Please check for updates manually.");
        menuDialogPrepare(dmydlg);
        menuDialogShow(dmydlg);
    }
    
    if (buttonTick(&STRUCT.exitBtn))
        menuAskExit();
    
    return MENUREACT_CONTINUE;
}

void menuMain__Render(gfxScreen_t screen) {
    if (screen == GFX_TOP) {
        STRUCT.buttonLabelFG = C2D_FloatToU8(ALPHA)<<24 | 0xFFFFFF;
        STRUCT.buttonLabelBG = C2D_FloatToU8(ALPHA)<<22 | 0xFFFFFF;
        u32 headerBannerBack = C2D_Color32f(0, 0, 0, ALPHA / 2);
        C2D_DrawRectangle(  0, STRUCT.header.py - 8, 0, 200, 16 + (30 * STRUCT.header.sy), headerBannerBack, headerBannerBack, 0, headerBannerBack);
        C2D_DrawRectangle(200, STRUCT.header.py - 8, 0, 200, 16 + (30 * STRUCT.header.sy), headerBannerBack, headerBannerBack, headerBannerBack, 0);
        C2D_DrawText(&STRUCT.header.text, C2D_WithColor|C2D_AlignCenter, STRUCT.header.px, STRUCT.header.py, 0, STRUCT.header.sx, STRUCT.header.sy, C2D_Color32f(1, 1, 1, ALPHA));
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