#include "menu.h"

extern u32 mainCnt;

#define STRUCT menuStruct->saveCopy
#define ALPHA menuStruct->alpha
MenuStructPointers menuSaveCopy__Ptr = {menuSaveCopy__Init, menuSaveCopy__Exit, menuSaveCopy__Act, menuSaveCopy__Render, menuSaveCopy__AnimIn, menuSaveCopy__AnimOut};

#define SAVECOPYENTRY_MAX   1024

SaveCopyEntry saveCopyEntries[SAVECOPYENTRY_MAX] = {0};
static u32 saveCopyTotalTasks = 0;
static u32 saveCopyDoneTasks = 0;
static s32 saveCopyEndLastAppTask = -1;
static bool saveCopyWasCancelled = false;

void menuSaveCopy__ExitBtnRender(float x, float y, float w, float h, bool selected, bool touched, bool disabled) {
    menuMain__ButtonBackground(x, y, w, h, disabled);
    C2D_DrawText(&STRUCT.exitText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2 + 2.5f, 0, .7, .7, STRUCT.buttonLabelBG);
    C2D_DrawText(&STRUCT.exitText, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, x + w / 2, y + (h + 10) / 2, 0, .7, .7, STRUCT.buttonLabelFG);
}

bool saveCopyFailCB(s32 rc) {
    if (rc == 1) saveCopy__RetryFailed();
    if (rc == 2) saveCopy__Clear();
    return true;
}

bool saveCopyAbortCB(s32 rc) {
    if (rc == 2) {
        C2D_TextBufClear(STRUCT.fnamebuf);
        C2D_TextParse(&STRUCT.label1, STRUCT.fnamebuf, "Waiting for current operation\nto finish...");
        for (u32 i = 0; i < SAVECOPYENTRY_MAX; i++) {
            if (*saveCopyEntries[i].source && saveCopyEntries[i].appTaskId >= 0) {
                if (appTask_Clear(saveCopyEntries[i].appTaskId) < 0) saveCopyEndLastAppTask = saveCopyEntries[i].appTaskId;
            }
        }
        if (saveCopyEndLastAppTask >= 0) appTask_KillCurrent();
        saveCopy__Clear();
        saveCopyWasCancelled = true;
    }
    return true;
}

void menuSaveCopy__Init() {
    STRUCT.textbuf = C2D_TextBufNew(4096);
    STRUCT.fnamebuf = C2D_TextBufNew(256);
    STRUCT.header.py = 4;
    STRUCT.header.sy = .5;
    C2D_TextParse(&STRUCT.header.text, STRUCT.textbuf, "Copy data");
    C2D_TextParse(&STRUCT.exitText, STRUCT.textbuf, "\uE001 Cancel");
    C2D_TextParse(&STRUCT.label1, STRUCT.textbuf, "Please wait, while data is being copied.\nThis may take a few minutes.");
    buttonSetupCB(&STRUCT.exitBtn, 0, 200, 128, 40, menuSaveCopy__ExitBtnRender);
    waitIconInit(&STRUCT.waiticon);
    waitIconSetPosition(&STRUCT.waiticon, 160, 104);
    waitIconSetBaseColor(&STRUCT.waiticon, 0xC0000000);
    waitIconSetScale(&STRUCT.waiticon, 1);
    progressBarInit(&STRUCT.current);
    progressBarSetScale(&STRUCT.current, 2);
    progressBarSetWidth(&STRUCT.current, 256);
    progressBarSetPosition(&STRUCT.current, 32, 132);
    progressBarInit(&STRUCT.total);
    progressBarSetScale(&STRUCT.total, 2);
    progressBarSetWidth(&STRUCT.total, 256);
    progressBarSetPosition(&STRUCT.total, 32, 160);
    progressBarUseSparkAnimation(&STRUCT.total, true);
}

void menuSaveCopy__Exit() {
    C2D_TextBufDelete(STRUCT.textbuf);
    C2D_TextBufDelete(STRUCT.fnamebuf);
    if (saveCopyEndLastAppTask >= 0) appTask_Clear(saveCopyEndLastAppTask);
    saveCopyEndLastAppTask = -1;
    saveCopyWasCancelled = false;
}

int menuSaveCopy__Act() {
    Dialog* td;

    progressBarSetProgress(&STRUCT.current, curlGetUploadPercentage());
    progressBarSetProgress(&STRUCT.total, (saveCopyDoneTasks + curlGetDownloadPercentage()) / C2D_Clamp(saveCopyTotalTasks, 1, 9999));
    
    if (saveCopyDoneTasks < saveCopyTotalTasks) {
        if (buttonTick(&STRUCT.exitBtn)) {
            td = dialogNewTemp(DIALOG_ENABLE_BUTTON2);
            dialogButton(td, 1, "Abort");
            dialogButton(td, 2, "Continue");
            dialogMessage(td, "Are you sure you want to abort the copying?\n(Current files in the queue may not copy correctly.)");
            dialogSetButtonCallback(td, saveCopyAbortCB);
            dialogPrepare(td);
            dialogShow(td);
            soundPlay(SND(SND_BACK));
            return MENUREACT_CONTINUE;
        }
        Result res;
        for (u32 i = 0; i < SAVECOPYENTRY_MAX; i++) {
            if (*saveCopyEntries[i].source && saveCopyEntries[i].appTaskId >= 0 && !saveCopyEntries[i].done) {
                if (appTask_IsDone(saveCopyEntries[i].appTaskId)) {
                    saveCopyEntries[i].done = true;
                    res = appTask_GetResult(saveCopyEntries[i].appTaskId);
                    appTask_Clear(saveCopyEntries[i].appTaskId);
                    saveCopyEntries[i].appTaskId = -1;
                    saveCopyEntries[i].succeeded = !res;
                    saveCopyDoneTasks += saveCopyEntries[i].succeeded;
                    if (res) {
                        td = dialogNewTemp(DIALOG_ENABLE_BUTTON2|DIALOG_TITLE);
                        dialogButton(td, 1, "Abort");
                        dialogButton(td, 2, "Retry");
                        char buf[0x20];
                        if (res < 0) sprintf(buf, "Error code 0x%08lX",res);
                        if (res > 0) sprintf(buf, "Error code %ld",res);
                        dialogTitle(td, buf);
                        dialogMessage(td, "An error occured when copying item:\n");
                        dialogMessageAppend(td, saveCopyEntries[i].source);
                        dialogSetButtonCallback(td, saveCopyFailCB);
                        dialogPrepare(td);
                        dialogShow(td);
                    }
                }
            } else if (*saveCopyEntries[i].source && !saveCopyEntries[i].done && saveCopyEntries[i].appTaskId < 0) {
                saveCopyEntries[i].appTaskId = appTask_CopyFile(saveCopyEntries[i].source, saveCopyEntries[i].dest, saveCopyEntries[i].folder);
                if (appTask_IsWaiting()) appTask_Continue();
            }
        }
        C2D_TextBufClear(STRUCT.fnamebuf);
        C2D_TextParse(&STRUCT.label1, STRUCT.fnamebuf, CURL_lastErrorCode);
    } else if (appTask_IsWaiting()) {
        if (saveCopyEndLastAppTask >= 0) appTask_Clear(saveCopyEndLastAppTask);
        td = dialogNewTemp(DIALOG_ENABLE_BUTTON1);
        if (saveCopyWasCancelled)
            dialogMessage(td, "The process has been cancelled.\nSome data may be missing or corrupted.");
        else
            dialogMessage(td, "Data was successfully copied.");
        dialogPrepare(td);
        dialogShow(td);
        menuNext(MENUID_PLAY);
    }
    return MENUREACT_CONTINUE;
}

void menuSaveCopy__Render(gfxScreen_t screen) {
    if (screen == GFX_TOP) {
        STRUCT.buttonLabelFG = C2D_FloatToU8(ALPHA)<<24 | 0xFFFFFF;
        STRUCT.buttonLabelBG = C2D_FloatToU8(ALPHA)<<22 | 0xFFFFFF;
        u32 headerBannerBack = C2D_Color32f(0, 0, 0, ALPHA / 2);
        C2D_DrawRectangle(  0, STRUCT.header.py - 8, 0, 200, 16 + (30 * STRUCT.header.sy), headerBannerBack, headerBannerBack, 0, headerBannerBack);
        C2D_DrawRectangle(200, STRUCT.header.py - 8, 0, 200, 16 + (30 * STRUCT.header.sy), headerBannerBack, headerBannerBack, headerBannerBack, 0);
        C2D_DrawText(&STRUCT.header.text, C2D_WithColor|C2D_AlignCenter, 200, STRUCT.header.py, 0, .75, STRUCT.header.sy, C2D_Color32f(1, 1, 1, ALPHA));
    } else {
        buttonRender(&STRUCT.exitBtn);
        C2D_DrawText(&STRUCT.label1, C2D_AlignCenter|C2D_WithColor, 160, 48, 0, .6, .6, C2D_FloatToU8(ALPHA)<<24|0xFFFFFF);
        waitIconRender(&STRUCT.waiticon);
        progressBarRender(&STRUCT.current);
        progressBarRender(&STRUCT.total);
    }
}

bool menuSaveCopy__AnimIn() {
    ALPHA += .125f;
    STRUCT.exitBtn.pos.y = 200 + (1 - ALPHA) * 4;
    STRUCT.header.sy += .030625f;
    //STRUCT.header.py -= 1.5f;
    STRUCT.header.py += .5f;
    waitIconSetAlpha(&STRUCT.waiticon, ALPHA);
    progressBarSetAlpha(&STRUCT.current, ALPHA);
    progressBarSetAlpha(&STRUCT.total, ALPHA);
    return ALPHA >= 1.f;
}

bool menuSaveCopy__AnimOut() {
    ALPHA -= .125f;
    STRUCT.exitBtn.pos.y = 200 + (1 - ALPHA) * 4;
    STRUCT.header.sy -= .030625f;
    //STRUCT.header.py += 1.5f;
    STRUCT.header.py -= .5f;
    waitIconSetAlpha(&STRUCT.waiticon, ALPHA);
    progressBarSetAlpha(&STRUCT.current, ALPHA);
    progressBarSetAlpha(&STRUCT.total, ALPHA);
    return ALPHA <= 0.f;
}

bool saveCopy__Add(const char* src, const char* dest) {
    SaveCopyEntry* e = saveCopyEntries;
    for (u32 i=0; i<SAVECOPYENTRY_MAX; i++, e++) {
        if (!*e->source || e->succeeded) {
            e->folder = archDirExists(src);
            if (!e->folder && !archFileExists(src))
                return true;
            strncpy(e->source, src, 255);
            strncpy(e->dest, dest, 255);
            e->appTaskId = -1;
            e->done = false;
            e->succeeded = false;
            saveCopyTotalTasks++;
            break;
        }
    }
    return false;
}

void saveCopy__Clear() {
    memset(saveCopyEntries, 0, sizeof(saveCopyEntries));
    saveCopyTotalTasks = 0;
    saveCopyDoneTasks = 0;
}

void saveCopy__RetryFailed() {
    SaveCopyEntry* e = saveCopyEntries;
    for (u32 i=0; i<SAVECOPYENTRY_MAX; i++, e++) {
        if (e->done && !e->succeeded)
            e->done = false;
    }
}