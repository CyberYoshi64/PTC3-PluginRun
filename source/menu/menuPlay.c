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
    Dialog* dlg;

    if (menuPlay__AppTaskFormat >= 0) {
        if (appTask_IsDone(menuPlay__AppTaskFormat)) {
            res = appTask_GetResult(menuPlay__AppTaskFormat);
            appTask_Clear(menuPlay__AppTaskFormat);
            menuPlay__AppTaskFormat = -1;
            if (res) {
                if (res > 0) sprintf(buf, "Error code %ld", res);
                if (res < 0) sprintf(buf, "Error code 0x%08lX", res);
                dlg = dialogNewTemp(DIALOG_ENABLE_BUTTON1|DIALOG_TITLE);
                dialogTitle(dlg, buf);
                dialogMessage(dlg, "An error has occured while starting CYX.\n\nNote down the error code and contact the publisher of this application.");
                dialogPrepare(dlg);
                dialogShow(dlg);
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
    Dialog* dlg;
    char err[32];
    AM_TitleEntry amEntry = {0};
    u64 titleID;
    bool bool1 = false;

    switch (menuPlay__PrepareDialogState) {
    case 0:
        if (R_FAILED(res = amInit())) {
            dlg = dialogNewTemp(DIALOG_ENABLE_BUTTON1|DIALOG_TITLE);
            snprintf(err, 32, "Error code AM-%08lX", res);
            dialogTitle(dlg, err);
            dialogMessage(dlg, "Couldn't check for SmileBASIC installations.\n\nPlease try again later. If issues persist, restart the system or contact CyberYoshi64.");
            dialogPrepare(dlg);
            dialogShow(dlg);
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
    } else
        menuPlay__StartGame = -1;
    return true;
}
bool menuPlay__CYXSetupCallback1(s32 rc) {
    if (rc == 1) {
        menuPlay__StartGame = -1;
        saveCopy__Clear();
        if R_FAILED(archMount(ARCHIVE_USER_SAVEDATA, MEDIATYPE_SD, bootTitle, "save", 0)) {
            Dialog* dlg = dialogNewTemp(DIALOG_ENABLE_BUTTON1|DIALOG_TITLE);
            dialogTitle(dlg, "Unable to copy data");
            dialogMessage(dlg, "The save data for this version of SmileBASIC doesn't exist.\n\nStart SmileBASIC once to create the data or skip copying it.");
            dialogPrepare(dlg);
            dialogShow(dlg);
            return true;
        };
        archMount(ARCHIVE_EXTDATA, MEDIATYPE_SD, bootTitle, "data", 0);
        
        Handle d; u32 nextEntry;
        FS_DirectoryEntry e;
        char buf[2][0x200];
        if R_SUCCEEDED(archDirOpen(&d, "data:/")) {
            while (true) {
                FSDIR_Read(d, &nextEntry, 1, &e);
                if (!nextEntry) break;
                memset(buf, 0, sizeof(buf));
                sprintf(buf[0], "%s", "data:/");
                sprintf(buf[1], "%s", SDMC_PREFIX SAVE_PATH);
                utf16_to_utf8((u8*)buf[0]+strlen(buf[0]), e.name, 192);
                utf16_to_utf8((u8*)buf[1]+strlen(buf[1]), e.name, 192);
                saveCopy__Add(buf[0], buf[1]);
            }
            FSDIR_Close(d);
        }
        if (saveCopy__Add("save:/config.dat", SDMC_PREFIX SAVE_PATH "config.dat")) {
            Dialog* dlg = dialogNewTemp(DIALOG_ENABLE_BUTTON1|DIALOG_TITLE);
            dialogTitle(dlg, "Unable to copy data");
            dialogMessage(dlg, "The file 'config.dat' is missing on SmileBASIC doesn't exist.\n\nStart SmileBASIC once to create the file or skip copying it.");
            dialogPrepare(dlg);
            dialogShow(dlg);
            saveCopy__Clear();
            archUnmount("save");
            archUnmount("data");
            return true;
        }
        archDirCreateRecursive(SDMC_PREFIX SAVE_PATH "###", false);
        menuNext(MENUID_SAVEFS_COPY);
    } else {
        Dialog* dlg = dialogNewTemp(DIALOG_ENABLE_BUTTON2);
        dialogMessage(dlg, "CYX will not inherit save data from any version of SmileBASIC.\n\nContinue to start CYX anyway?");
        dialogSetButtonCallback(dlg, menuPlay__CYXSetupCallback2);
        dialogPrepare(dlg);
        dialogShow(dlg);
    }
    return true;
}

void menuPlay__Init() {
    STRUCT.textbuf = C2D_TextBufNew(4096);
    STRUCT.header.py = 4;
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

    Dialog* dlg = NULL;
    if (!menuPlay__AvailableGameMask) {
        dlg = dialogNewTemp(DIALOG_WAIT);
        dialogSetWaitCallback(dlg, menuPlay__PrepareWaitCallback);
        dialogMessage(dlg, "Preparing...");
        dialogPrepare(dlg);
        dialogShow(dlg);
    }
    STRUCT.badSaveDialog = dialogNew(DIALOG_ENABLE_BUTTON1|DIALOG_TITLE);
    dialogTitle(STRUCT.badSaveDialog, "Unable to launch CYX.");
    dialogMessage(STRUCT.badSaveDialog, "The save data for this version of SmileBASIC doesn't exist.\nPlease start SmileBASIC to create the save data.\n\nCitra unfortunately prohibits creating it here.");
    dialogPrepare(STRUCT.badSaveDialog);
    STRUCT.outdatedGameDialog = dialogNew(DIALOG_ENABLE_BUTTON1|DIALOG_TITLE);
    dialogTitle(STRUCT.outdatedGameDialog, "Unable to launch CYX.");
    dialogMessage(STRUCT.outdatedGameDialog, "This version of SmileBASIC is not up-to-date. Please obtain the update from the Nintendo eShop.");
    dialogPrepare(STRUCT.outdatedGameDialog);
    STRUCT.prepareGameDialog = dialogNew(DIALOG_WAIT|DIALOG_PROGRESS);
    dialogMessage(STRUCT.prepareGameDialog, "Preparing...");
    dialogSetWaitCallback(STRUCT.prepareGameDialog, menuPlay__PrepareGameWaitCallback);
    dialogPrepare(STRUCT.prepareGameDialog);
    STRUCT.cyxSaveRootSetupDialog = dialogNew(DIALOG_ENABLE_BUTTON2);
    dialogMessage(STRUCT.cyxSaveRootSetupDialog, "CYX is currently not fully set up yet.\n\nDo you want to copy the save data from this version of SmileBASIC to CYX?");
    dialogSetButtonCallback(STRUCT.cyxSaveRootSetupDialog, menuPlay__CYXSetupCallback1);
    dialogPrepare(STRUCT.cyxSaveRootSetupDialog);

    menuPlay__IsCYXSaveRootPresent = archFileExists(SDMC_PREFIX SAVE_PATH "config.dat") && archDirExists(SDMC_PREFIX SAVE_PATH "###");
    archUnmount("save");
    archUnmount("data");
}

void menuPlay__Exit() {
    C2D_TextBufDelete(STRUCT.textbuf);
    dialogFree(STRUCT.badSaveDialog);
    dialogFree(STRUCT.outdatedGameDialog);
    dialogFree(STRUCT.prepareGameDialog);
    dialogFree(STRUCT.cyxSaveRootSetupDialog);
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
        bootTitle = bootableTID[menuPlay__StartGame];
        if (!menuPlay__IsCYXSaveRootPresent) {
            dialogShow(STRUCT.cyxSaveRootSetupDialog);
        } else {
            if (menuPlay__BadSaveDataMask & BIT(menuPlay__StartGame)) {
                menuPlay__StartGame = -1;
                dialogShow(STRUCT.badSaveDialog);
                return MENUREACT_CONTINUE;
            }
            if (!(menuPlay__UsableGameMask & BIT(menuPlay__StartGame))) {
                menuPlay__StartGame = -1;
                dialogShow(STRUCT.outdatedGameDialog);
                return MENUREACT_CONTINUE;
            }
            menuPlay__StartGame = -1;
            menuPlay__PrepareGameDialogState = 0;
            dialogShow(STRUCT.prepareGameDialog);
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
        C2D_DrawText(&STRUCT.header.text, C2D_WithColor|C2D_AlignCenter, 200, STRUCT.header.py, 0, .75, STRUCT.header.sy, C2D_Color32f(1, 1, 1, ALPHA));
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