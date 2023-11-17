#include "menu.h"

#define STRUCT menuStruct->play
#define ALPHA menuStruct->alpha
MenuStructPointers menuPlay__Ptr = {menuPlay__Init, menuPlay__Exit, menuPlay__Act, menuPlay__Render, menuPlay__AnimIn, menuPlay__AnimOut};

u32 menuPlay__AvailableGameMask = 0;
u32 menuPlay__UsableGameMask = 0;
u32 menuPlay__BadSaveDataMask = 0;
u32 menuPlay__PrepareDialogState = 0;
u32 menuPlay__PrepareGameDialogState = 0;
u32 menuPlay__IsCYXSaveRootPresent = 0;
int menuPlay__AppTaskFormat = 0;
int menuPlay__StartGame = -1;

u64 bootableTID[] = {
    0x0004000000117200ULL,
    0x000400000016DE00ULL,
    0x00040000001A1C00ULL,
};
u64 bootableUpdateMask = 0x0000000E00000000ULL;

void menuPlay__ExitBtnRender(float x, float y, float w, float h, bool selected, bool touched, bool disabled) {
    menuMain__ButtonBackground(x, y, w, h, disabled);
    C2D_DrawText(&STRUCT.exitText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2 + 2.5f, 0, .7, .7, STRUCT.buttonLabelBG);
    C2D_DrawText(&STRUCT.exitText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2, 0, .7, .7, STRUCT.buttonLabelFG);
}

void menuPlay__LncJPNBtnRender(float x, float y, float w, float h, bool selected, bool touched, bool disabled) {
    menuMain__ButtonBackground(x, y, w, h, disabled);
    C2D_DrawText(&STRUCT.launchJPNText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2 + 2.5f, 0, .7, .7, STRUCT.buttonLabelBG);
    C2D_DrawText(&STRUCT.launchJPNText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2, 0, .7, .7, STRUCT.buttonLabelFG);
}

void menuPlay__LncUSABtnRender(float x, float y, float w, float h, bool selected, bool touched, bool disabled) {
    menuMain__ButtonBackground(x, y, w, h, disabled);
    C2D_DrawText(&STRUCT.launchUSAText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2 + 2.5f, 0, .7, .7, STRUCT.buttonLabelBG);
    C2D_DrawText(&STRUCT.launchUSAText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2, 0, .7, .7, STRUCT.buttonLabelFG);
}

void menuPlay__LncEURBtnRender(float x, float y, float w, float h, bool selected, bool touched, bool disabled) {
    menuMain__ButtonBackground(x, y, w, h, disabled);
    C2D_DrawText(&STRUCT.launchEURText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2 + 2.5f, 0, .7, .7, STRUCT.buttonLabelBG);
    C2D_DrawText(&STRUCT.launchEURText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2, 0, .7, .7, STRUCT.buttonLabelFG);
}

bool menuPlay__PrepareGameWaitCallback(u32* buttons, float* progress) {
    
    Result res;
    char buf[0x30];
    MenuDialog* dlg;

    if (menuPlay__AppTaskFormat >= 0) {
        if (appTask_IsDone(menuPlay__AppTaskFormat)) {
            res = appTask_GetResult(menuPlay__AppTaskFormat);
            appTask_Clear(menuPlay__AppTaskFormat);
            menuPlay__AppTaskFormat = -1;
            if (res) {
                if (res > 0) sprintf(buf, "Error code %ld", res);
                if (res < 0) sprintf(buf, "Error code 0x%08lX", res);
                dlg = menuDialogNewTemp(MENUDIALOG_ENABLE_BUTTON1|MENUDIALOG_TITLE);
                menuDialogTitle(dlg, buf);
                menuDialogMessage(dlg, "An error has occured while starting CYX.\n\nNote down the error code and contact the publisher of this application.");
                menuDialogPrepare(dlg);
                menuDialogShow(dlg);
                return true;
            }
        }
        return false;
    }

    *progress = (float)menuPlay__PrepareGameDialogState / 2.f;

    switch (menuPlay__PrepareGameDialogState) {
    case 0:
        if (R_FAILED(archMount(ARCHIVE_USER_SAVEDATA, MEDIATYPE_SD, bootTitle, "tmp", 0))) {
            menuPlay__AppTaskFormat = appTask_FormatSave(bootTitle, MEDIATYPE_SD, 10, 5, true);
        } else archUnmount("tmp");
        menuPlay__PrepareGameDialogState++;
        break;
    case 1:
        if (R_FAILED(archMount(ARCHIVE_EXTDATA, MEDIATYPE_SD, bootTitle, "tmp", 0))) {
            menuPlay__AppTaskFormat = appTask_FormatExtData(bootTitle, MEDIATYPE_SD, 8196, 1026);
        } else archUnmount("tmp");
        menuPlay__PrepareGameDialogState++;
        break;
    default:
        menuPlay__PrepareGameDialogState = 0;
        runPlugin = true;
        menuNext(0);
        return true;
    }
    
    return false;
}

bool menuPlay__PrepareWaitCallback(u32* buttons, float* progress) {
    
    Result res;
    MenuDialog* dlg;
    char err[32];
    AM_TitleEntry amEntry = {0};
    u64 titleID;
    bool bool1 = false;

    switch (menuPlay__PrepareDialogState) {
    case 0:
        if (R_FAILED(res = amInit())) {
            dlg = menuDialogNewTemp(MENUDIALOG_ENABLE_BUTTON1|MENUDIALOG_TITLE);
            snprintf(err, 32, "Error code AM-%08lX", res);
            menuDialogTitle(dlg, err);
            menuDialogMessage(dlg, "Couldn't check for SmileBASIC installations.\n\nPlease try again later. If issues persist, restart the system or contact CyberYoshi64.");
            menuDialogPrepare(dlg);
            menuDialogShow(dlg);
            menuNext(MENUID_MAIN);
            return true;
        }
        menuPlay__AvailableGameMask = 0;
        menuPlay__UsableGameMask = 0;
        menuPlay__BadSaveDataMask = 0;
        menuPlay__PrepareDialogState++;
        break;
    case 1: case 2: case 3:
        titleID = bootableTID[menuPlay__PrepareDialogState - 1];
        if (AM_GetTitleInfo(MEDIATYPE_SD, 1, &titleID, &amEntry) >= 0 && (u32)(amEntry.titleID))
            menuPlay__AvailableGameMask |= BIT(menuPlay__PrepareDialogState - 1);
        menuPlay__PrepareDialogState++;
        break;
    case 4: case 5: case 6:
        titleID = bootableTID[menuPlay__PrepareDialogState - 4] | bootableUpdateMask;
        if (AM_GetTitleInfo(MEDIATYPE_SD, 1, &titleID, &amEntry) >= 0 && (u32)(amEntry.titleID)) {
            if (menuPlay__PrepareDialogState == 4) {
                bool1 = (amEntry.version == 15696);
            } else if (menuPlay__PrepareDialogState == 5) {
                bool1 = (amEntry.version == 5248);
            } else if (menuPlay__PrepareDialogState == 6) {
                bool1 = (amEntry.version == 1040);
            }
            if (bool1)
                menuPlay__UsableGameMask |= BIT(menuPlay__PrepareDialogState - 4);
        }
        menuPlay__PrepareDialogState++;
        break;
    case 7:
        amExit();
        menuPlay__PrepareDialogState++;
        break;
    case 8: case 9: case 10:
        titleID = bootableTID[menuPlay__PrepareDialogState - 8];
        if (R_FAILED(archMount(ARCHIVE_USER_SAVEDATA, MEDIATYPE_SD, titleID, "save", 0))) {
            //if (isCitra)
            //    menuPlay__BadSaveDataMask |= BIT(menuPlay__PrepareDialogState - 8);
        }
        archUnmount("save");
        menuPlay__PrepareDialogState++;
        break;
    default:
        menuPlay__PrepareDialogState = 0;
        return true;
    }
    return false;
}

bool menuPlay__CYXSetupCallback2(s32 rc) {
    if (rc == 1) {
        menuPlay__IsCYXSaveRootPresent = true;
        archDirCreate(SDMC_PREFIX SAVE_PATH);
    } else
        menuPlay__StartGame = -1;
    return true;
}
bool menuPlay__CYXSetupCallback1(s32 rc) {
    if (rc == 1) {
        menuPlay__StartGame = -1;
        menuNext(MENUID_SAVEFS_COPY);
    } else {
        MenuDialog* dlg = menuDialogNewTemp(MENUDIALOG_ENABLE_BUTTON2);
        menuDialogMessage(dlg, "CYX will not inherit save data from any version of SmileBASIC.\n\nContinue to start CYX anyway?");
        menuDialogSetButtonCallback(dlg, menuPlay__CYXSetupCallback2);
        menuDialogPrepare(dlg);
        menuDialogShow(dlg);
    }
    return true;
}

void menuPlay__Init() {
    STRUCT.textbuf = C2D_TextBufNew(4096);
    STRUCT.header.px = 200;
    STRUCT.header.py = 4;
    STRUCT.header.sx = .75;
    STRUCT.header.sy = .5;
    
    C2D_TextParse(&STRUCT.header.text, STRUCT.textbuf, "Start CYX");
    C2D_TextParse(&STRUCT.exitText, STRUCT.textbuf, "\uE001 Back");
    
    C2D_TextParse(&STRUCT.launchHeading, STRUCT.textbuf, "Please select the base application.");
    C2D_TextParse(&STRUCT.launchJPNText, STRUCT.textbuf, "JPN");
    C2D_TextParse(&STRUCT.launchUSAText, STRUCT.textbuf, "USA");
    C2D_TextParse(&STRUCT.launchEURText, STRUCT.textbuf, "EUR");

    buttonSetupCB(&STRUCT.launchJPNBtn, 30, 100, 80, 40, menuPlay__LncJPNBtnRender);
    buttonSetupCB(&STRUCT.launchUSABtn, 120, 100, 80, 40, menuPlay__LncUSABtnRender);
    buttonSetupCB(&STRUCT.launchEURBtn, 210, 100, 80, 40, menuPlay__LncEURBtnRender);
    buttonSetEnabled(&STRUCT.launchJPNBtn, false);
    buttonSetEnabled(&STRUCT.launchUSABtn, false);
    buttonSetEnabled(&STRUCT.launchEURBtn, false);

    buttonSetupCB(&STRUCT.exitBtn, 0, 200, 100, 40, menuPlay__ExitBtnRender);

    MenuDialog* dlg = NULL;
    if (!menuPlay__AvailableGameMask) {
        dlg = menuDialogNewTemp(MENUDIALOG_WAIT);
        menuDialogSetWaitCallback(dlg, menuPlay__PrepareWaitCallback);
        menuDialogMessage(dlg, "Preparing...");
        menuDialogPrepare(dlg);
        menuDialogShow(dlg);
    }
    STRUCT.badSaveDialog = menuDialogNew(MENUDIALOG_ENABLE_BUTTON1|MENUDIALOG_TITLE);
    menuDialogTitle(STRUCT.badSaveDialog, "Unable to launch CYX.");
    menuDialogMessage(STRUCT.badSaveDialog, "The save data for this version of SmileBASIC doesn't exist.\nPlease start SmileBASIC to create the save data.\n\nCitra unfortunately prohibits creating it here.");
    menuDialogPrepare(STRUCT.badSaveDialog);
    STRUCT.outdatedGameDialog = menuDialogNew(MENUDIALOG_ENABLE_BUTTON1|MENUDIALOG_TITLE);
    menuDialogTitle(STRUCT.outdatedGameDialog, "Unable to launch CYX.");
    menuDialogMessage(STRUCT.outdatedGameDialog, "This version of SmileBASIC is not up-to-date. Please obtain the update from the Nintendo eShop.");
    menuDialogPrepare(STRUCT.outdatedGameDialog);
    STRUCT.prepareGameDialog = menuDialogNew(MENUDIALOG_WAIT|MENUDIALOG_PROGRESS);
    menuDialogMessage(STRUCT.prepareGameDialog, "Preparing...");
    menuDialogSetWaitCallback(STRUCT.prepareGameDialog, menuPlay__PrepareGameWaitCallback);
    menuDialogPrepare(STRUCT.prepareGameDialog);
    STRUCT.cyxSaveRootSetupDialog = menuDialogNew(MENUDIALOG_ENABLE_BUTTON2);
    menuDialogMessage(STRUCT.cyxSaveRootSetupDialog, "CYX is currently not fully set up yet.\n\nDo you want to copy the save data from this version of SmileBASIC to CYX?");
    menuDialogSetButtonCallback(STRUCT.cyxSaveRootSetupDialog, menuPlay__CYXSetupCallback1);
    menuDialogPrepare(STRUCT.cyxSaveRootSetupDialog);

    menuPlay__IsCYXSaveRootPresent = archDirExists(SDMC_PREFIX SAVE_PATH);
}

void menuPlay__Exit() {
    C2D_TextBufDelete(STRUCT.textbuf);
    menuDialogFree(STRUCT.badSaveDialog);
    menuDialogFree(STRUCT.outdatedGameDialog);
    menuDialogFree(STRUCT.prepareGameDialog);
    menuDialogFree(STRUCT.cyxSaveRootSetupDialog);
}

int menuPlay__Act() {
    buttonSetEnabled(&STRUCT.launchJPNBtn, menuPlay__AvailableGameMask & BIT(GAMEREG_JPN));
    buttonSetEnabled(&STRUCT.launchUSABtn, menuPlay__AvailableGameMask & BIT(GAMEREG_USA));
    buttonSetEnabled(&STRUCT.launchEURBtn, menuPlay__AvailableGameMask & BIT(GAMEREG_EUR));

    if (buttonTick(&STRUCT.launchJPNBtn)) {
        menuPlay__StartGame = GAMEREG_JPN;
    }
    if (buttonTick(&STRUCT.launchUSABtn)) {
        menuPlay__StartGame = GAMEREG_USA;
    }
    if (buttonTick(&STRUCT.launchEURBtn)) {
        menuPlay__StartGame = GAMEREG_EUR;
    }

    if (menuPlay__StartGame >= 0) {
        if (!menuPlay__IsCYXSaveRootPresent) {
            menuDialogShow(STRUCT.cyxSaveRootSetupDialog);
        } else {
            if (menuPlay__BadSaveDataMask & BIT(menuPlay__StartGame)) {
                menuPlay__StartGame = -1;
                menuDialogShow(STRUCT.badSaveDialog);
                return MENUREACT_CONTINUE;
            }
            if (!(menuPlay__UsableGameMask & BIT(menuPlay__StartGame))) {
                menuPlay__StartGame = -1;
                menuDialogShow(STRUCT.outdatedGameDialog);
                return MENUREACT_CONTINUE;
            }
            bootTitle = bootableTID[menuPlay__StartGame];
            menuPlay__StartGame = -1;
            menuPlay__PrepareGameDialogState = 0;
            menuDialogShow(STRUCT.prepareGameDialog);
        }
        return MENUREACT_CONTINUE;
    }

    if (buttonTick(&STRUCT.exitBtn) || (HID_BTNPRESSED & KEY_B))
        return menuNext(MENUID_MAIN);
    
    return MENUREACT_CONTINUE;
}

void menuPlay__Render(gfxScreen_t screen) {
    if (screen == GFX_TOP) {
        STRUCT.buttonLabelFG = C2D_FloatToU8(ALPHA)<<24 | 0xFFFFFF;
        STRUCT.buttonLabelBG = C2D_FloatToU8(ALPHA)<<22 | 0xFFFFFF;
        u32 headerBannerBack = C2D_Color32f(0, 0, 0, ALPHA / 2);
        C2D_DrawRectangle(  0, STRUCT.header.py - 8, 0, 200, 16 + (30 * STRUCT.header.sy), headerBannerBack, headerBannerBack, 0, headerBannerBack);
        C2D_DrawRectangle(200, STRUCT.header.py - 8, 0, 200, 16 + (30 * STRUCT.header.sy), headerBannerBack, headerBannerBack, headerBannerBack, 0);
        C2D_DrawText(&STRUCT.header.text, C2D_WithColor|C2D_AlignCenter, STRUCT.header.px, STRUCT.header.py, 0, STRUCT.header.sx, STRUCT.header.sy, C2D_Color32f(1, 1, 1, ALPHA));
    } else {
        buttonRender(&STRUCT.exitBtn);
        C2D_DrawText(&STRUCT.launchHeading, C2D_AlignCenter|C2D_AtBaseline|C2D_WithColor, 160, 70, 0, .6, .6, C2D_Color32f(1, 1, 1, ALPHA));
        buttonRender(&STRUCT.launchJPNBtn);
        buttonRender(&STRUCT.launchUSABtn);
        buttonRender(&STRUCT.launchEURBtn);
    }
}

bool menuPlay__AnimIn() {
    ALPHA += .125f;
    STRUCT.exitBtn.pos.y = 200 + (1 - ALPHA) * 4;
    STRUCT.header.sy += .030625f;
    //STRUCT.header.py -= 1.5f;
    STRUCT.header.py += .5f;
    return ALPHA >= 1.f;
}

bool menuPlay__AnimOut() {
    ALPHA -= .125f;
    STRUCT.exitBtn.pos.y = 200 + (1 - ALPHA) * 4;
    STRUCT.header.sy -= .030625f;
    //STRUCT.header.py += 1.5f;
    STRUCT.header.py -= .5f;
    return ALPHA <= 0.f;
}