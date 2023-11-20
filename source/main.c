#include "main.h"

bool exiting = false;
bool runPlugin = false;
bool isCitra = false;
bool blockHOME = false;
u64 bootTitle = 0;
u32 mainCnt = 0, frameCnt;
float screenFadeAlpha = 1;

u64 __appID;

C3D_RenderTarget    *topScr;
C3D_RenderTarget    *botScr;
C2D_SpriteSheet     commonSheet;

C2D_TextBuf         errorTextBuf;
C2D_Text            errorTextStr;
char                errorTextData[4096] = {0};

MenuStruct*         menuStruct = NULL;
u32                 menuStructMode = 0;

MenuStructPointers* currMenuPtr;
MenuStructPointers* nextMenuPtr;
Dialog*             currDialog;
Dialog*             nextDialog;

MenuStructPointers  menuDefaultPtr =
    {NULL, NULL, menuDefault__Act, NULL, NULL, NULL};

char latestVersion[16] = {0};
char* latestVersionString;
int updateCURLTask = -1;

s64 checkCitra(){
    s64 output = 0;
    svcGetSystemInfo(&output, 0x20000, 0);
    return output;
}

void menuTick() {
    switch (menuStructMode)
    {
    case 0:
        if (currMenuPtr->Init)
            currMenuPtr->Init();
        menuStructMode++;
        break;
    case 1:
        if (!currMenuPtr->AnimIn || currMenuPtr->AnimIn())
            menuStructMode++;
        break;
    case 2:
        if (nextMenuPtr)
            menuStructMode++;
        else if (currMenuPtr->Act && !currDialog)
            currMenuPtr->Act();
        break;
    case 3:
        if (!currMenuPtr->AnimOut || currMenuPtr->AnimOut()) {
            if (currMenuPtr->Exit)
                currMenuPtr->Exit();
            memset(menuStruct, 0, MENUSTRUCT_SIZE);
            currMenuPtr = nextMenuPtr;
            nextMenuPtr = NULL;
            menuStructMode = 0;
        }
        break;
    
    default:
        break;
    }
    buttonAlpha = menuStruct->alpha;
}

int menuNext(MenuID id) {
    switch (id) {
    case MENUID_NONE:
        nextMenuPtr = &menuDefaultPtr;
        break;
    case MENUID_MAIN:
        nextMenuPtr = &menuMain__Ptr;
        break;
    case MENUID_PLAY:
        nextMenuPtr = &menuPlay__Ptr;
        break;
    case MENUID_UPDATES_TOP:
        nextMenuPtr = &menuUpdatesTop__Ptr;
        break;
    default:
        nextMenuPtr = &menuTemplate__Ptr;
        break;
    }
    return MENUREACT_NEXTMENU;
}

void dialogShow(Dialog* dlg) {
    if (!dlg) return;
    if (nextDialog) return;
    dlg->state = 0;
    dlg->rc = 0;
    nextDialog = dlg;
}

int menuDefault__Act() {
    exiting = true;
    return MENUREACT_CONTINUE;
}

void drawError(const char* error, bool standalone, u32 waitButton){
    if (standalone)
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    else
        C2D_Flush();

    if (error){
        memset(errorTextData, 0, sizeof(errorTextData));
        memcpy(errorTextData, error, strlen(error)+1);
    }
    C2D_TextBufClear(errorTextBuf);
    C2D_SceneBegin(botScr);
    C2D_TargetClear(botScr, 0xFF000000);
    C2D_TextParse(&errorTextStr, errorTextBuf, errorTextData);
    C2D_TextOptimize(&errorTextStr);
    C2D_DrawText(&errorTextStr, C2D_WithColor|C2D_WordWrap, 0, 0, 0, .45, .5, -1, 320.f);
    C2D_Flush();

    if (standalone)
        C3D_FrameEnd(0);
    
    if (waitButton)
        hidBlockUntilButton(waitButton);
}

bool updateDlgWaitCallback(u32 *buttons, float *progress) {
    if (appTask_GetCurrentTask() == updateCURLTask) {
        curlGetDownloadState(NULL, NULL, progress);
    }
    if (appTask_IsDone(updateCURLTask)) {
        *progress = 1.0f;
        if (appTask_GetResult(updateCURLTask)) {
            Dialog* s = dialogNewTemp(DIALOG_ENABLE_BUTTON1|DIALOG_TITLE);
            if (s) {
                char title[32];
                sprintf(title, "Error code %d", appTask_GetResult(updateCURLTask));
                dialogTitle(s, title);
                dialogMessage(s, "Could not check for the latest version.\n\nCURL returned error: ");
                dialogMessageAppend(s, CURL_lastErrorCode);
                dialogPrepare(s);
                dialogShow(s);
            }
            *buttons |= BIT(31);
        } else {
            strncpy(latestVersion, latestVersionString, sizeof(latestVersion)-1);
            free(latestVersionString);
        }
        appTask_Clear(updateCURLTask);
        updateCURLTask = -1;
        return true;
    }
    return false;
}

bool spawnUpdateCheckDialog() {
    if (updateCURLTask >= 0) return false;
    updateCURLTask = appTask_DownloadData(URL_UPDATEINFO, &latestVersionString);
    
    Dialog* updateDlg;
    
    if (updateCURLTask < 0) {
        updateDlg = dialogNewTemp(DIALOG_ENABLE_BUTTON1|DIALOG_TITLE);
        if (updateDlg) {
            dialogTitle(updateDlg, "An error has occured");
            dialogMessage(updateDlg, "Unable to download update data.\n\nPlease try again later.");
            dialogPrepare(updateDlg);
            dialogShow(updateDlg);
        }
        return false;
    } else {
        updateDlg = dialogNewTemp(DIALOG_WAIT|DIALOG_PROGRESS);
        if (updateDlg) {
            dialogMessage(updateDlg, "Checking for updates...");
            dialogSetWaitCallback(updateDlg, updateDlgWaitCallback);
            dialogPrepare(updateDlg);
            dialogShow(updateDlg);
        }
        return true;
    }
}

bool menuAskExitBtnCB(s32 rc) {
    if (rc == 1) menuNext(0);
    return true;
};

void menuAskExit() {
    Dialog* dlg;
    dlg = dialogNewTemp(DIALOG_ENABLE_BUTTON2);
    if (dlg) {
        dialogMessage(dlg, "Do you want to close the application?");
        dialogButton(dlg, 1, "No");
        dialogButton(dlg, 2, "Yes");
        dialogSetButtonCallback(dlg, menuAskExitBtnCB);
        dialogMessage(dlg, "Do you want to close the application?");
        dialogPrepare(dlg);
        dialogShow(dlg);
    }
}

int main(int argc, char const *argv[]) {
    Result res; bool fail = false;
    
    APT_GetProgramID(&__appID);

    gfxInit(GSP_RGB565_OES, GSP_RGB565_OES, false);
    romfsMountSelf("rom");
    ndspInit();
    httpcInit(0x1000);
    isCitra = checkCitra();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    
    errorTextBuf = C2D_TextBufNew(sizeof(errorTextData));
    
    u32 transFlags =    GX_TRANSFER_FLIP_VERT(false)|
                        GX_TRANSFER_OUT_TILED(false)|
                        GX_TRANSFER_RAW_COPY(false)|
                        GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8)|
                        GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB565)|
                        GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO);
    
    sprintf(errorTextData, "The application failed to initialize:\n\n");

	topScr = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH16);
	C3D_RenderTargetClear(topScr, C3D_CLEAR_ALL, 0, 0);
    C3D_RenderTargetSetOutput(topScr, GFX_TOP, GFX_LEFT, transFlags);
	
	botScr = C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH16);
    C3D_RenderTargetClear(botScr, C3D_CLEAR_ALL, 0, 0);
	C3D_RenderTargetSetOutput(botScr, GFX_BOTTOM, GFX_LEFT, transFlags);

    if (R_FAILED(res = archInit())) {
        sprintf(errorTextData + strlen(errorTextData),
            "archInit() → 0x%08lX\n", res
        );
        fail = true;
    }
    if (R_FAILED(res = archMount(ARCHIVE_SDMC, 0, 0, "sdmc", 0))) {
        sprintf(errorTextData + strlen(errorTextData),
            "archMount()[sdmc] → 0x%08lX\n", res
        );
        fail = true;
    }
    if (R_FAILED(res = archMount(ARCHIVE_ROMFS, 0, 0, "romfs", 0))) {
        sprintf(errorTextData + strlen(errorTextData),
            "archMount()[romfs] → 0x%08lX\n", res
        );
        fail = true;
    }
    if (R_FAILED(res = appTaskInit())) {
        sprintf(errorTextData + strlen(errorTextData),
            "appTaskInit() → %ld\n", res
        );
        fail = true;
    }
    if (!(menuStruct = malloc(MENUSTRUCT_SIZE))) {
        sprintf(errorTextData + strlen(errorTextData),
            "menuStruct = NULL\n"
        );
        fail = true;
    }
    if (!(dialogSheet = C2D_SpriteSheetLoad("rom:/gfx/dialog.t3x"))) {
        sprintf(errorTextData + strlen(errorTextData),
            "dialogSheet = NULL\n"
        );
        fail = true;
    }
    if (!(progressSheet = C2D_SpriteSheetLoad("rom:/gfx/progress.t3x"))) {
        sprintf(errorTextData + strlen(errorTextData),
            "progressSheet = NULL\n"
        );
        fail = true;
    }
    if (!(commonSheet = C2D_SpriteSheetLoad("rom:/gfx/common.t3x"))) {
        sprintf(errorTextData + strlen(errorTextData),
            "commonSheet = NULL\n"
        );
        fail = true;
    }
    if (fail){
        sprintf(errorTextData + strlen(errorTextData), "\nPlease get in contact with the creator of this application. Contact info can be found at https://cyberyoshi64.github.io \nPress START to exit the application.");
        drawError(NULL, true, KEY_START);
        return 1;
    }

    osSetSpeedupEnable(true);
    //MenuSound::Init();
    memset(menuStruct, 0, MENUSTRUCT_SIZE);
    currMenuPtr = &menuMain__Ptr;

    for (u32 i = 0; i < C2D_SpriteSheetCount(dialogSheet); i++)
        C3D_TexSetFilter(C2D_SpriteSheetGetImage(dialogSheet, i).tex, GPU_LINEAR, GPU_LINEAR);

    for (u32 i = 0; i < C2D_SpriteSheetCount(progressSheet); i++)
        C3D_TexSetFilter(C2D_SpriteSheetGetImage(progressSheet, i).tex, GPU_LINEAR, GPU_LINEAR);

    for (u32 i = 0; i < C2D_SpriteSheetCount(commonSheet); i++)
        C3D_TexSetFilter(C2D_SpriteSheetGetImage(commonSheet, i).tex, GPU_LINEAR, GPU_LINEAR);

    // spawnUpdateCheckDialog();

    while (aptMainLoop() && !exiting){
        hidRead();
        if (HID_BTNPRESSED & KEY_START) menuAskExit();
        
        HID_BTNPRESSED &= ~(KEY_START);

        if (HID_BTNPRESSED & KEY_Y)
            menuNext(1);
                
        if (currDialog) {
            if (dialog__Tick(currDialog)) {
                if (currDialog->mode & DIALOG_ONESHOT)
                    dialogFree(currDialog);
                currDialog = NULL;
            }
        }        
        menuTick();
        waitIconTick();
        
        if (nextDialog && !currDialog) {
            currDialog = nextDialog;
            nextDialog = NULL;
        }

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        u32 c1, c2, c3, c4;
        c1 = C2D_Color32(
            48 + sinf(mainCnt / 60.0) * 48,
            48 + sinf(mainCnt / 90.0) * 48,
            48 + sinf(mainCnt /120.0) * 48,
            255
        );
        c2 = C2D_Color32(
            48 + sinf(mainCnt /120.0) * 48,
            48 + sinf(mainCnt /150.0) * 48,
            48 + sinf(mainCnt /180.0) * 48,
            255
        );
        c3 = C2D_Color32(
            48 + sinf(mainCnt /180.0) * 48,
            48 + sinf(mainCnt /210.0) * 48,
            48 + sinf(mainCnt /240.0) * 48,
            255
        );
        c4 = C2D_Color32(
            48 + sinf(mainCnt /240.0) * 48,
            48 + sinf(mainCnt /120.0) * 48,
            48 + sinf(mainCnt / 90.0) * 48,
            255
        );

        C2D_SceneBegin(topScr);
        C2D_DrawRectangle(0, 0, 0, 400, 240, c1, c2, c3, c4);
        
        if (menuStructMode && currMenuPtr->Render)
            currMenuPtr->Render(GFX_TOP);
        
        if (currDialog)
            dialog__Render(currDialog, GFX_TOP);

        C2D_DrawRectSolid(0, 0, 0, 400, 240, C2D_Color32f(0, 0, 0, screenFadeAlpha));
        C2D_SceneBegin(botScr);
        C2D_DrawTriangle(  0,   0, c1,   0, 240, c2, 320, 240, c4, 0);
        C2D_DrawTriangle(  0,   0, c1, 320, 240, c4, 320,   0, c3, 0);

        if (menuStructMode && currMenuPtr->Render)
            currMenuPtr->Render(GFX_BOTTOM);
        
        if (currDialog)
            dialog__Render(currDialog, GFX_BOTTOM);

        C2D_DrawRectSolid(0, 0, 0, 320, 240, C2D_Color32f(0, 0, 0, screenFadeAlpha));
        C3D_FrameEnd(0);

        mainCnt++;
        frameCnt = C3D_FrameCounter(0);
        if (screenFadeAlpha > 0.f) screenFadeAlpha -= .03125f;
    }
    
    exiting = true; // In case it wasn't set before

    if (menuStructMode) {
        if (currMenuPtr->Exit)
            currMenuPtr->Exit();
    }
    
    if (currDialog && currDialog->mode & DIALOG_ONESHOT)
        dialogFree(currDialog);

    if (nextDialog && nextDialog->mode & DIALOG_ONESHOT)
        dialogFree(nextDialog);

    if (!aptShouldClose()) {
        blockHOME = true;
        for (; screenFadeAlpha < 1.0f; screenFadeAlpha += .03125f){
            C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
            C2D_SceneBegin(topScr);
            C2D_DrawRectSolid(0, 0, 0, 400, 240, C2D_Color32f(0, 0, 0, screenFadeAlpha));
            C2D_SceneBegin(botScr);
            C2D_DrawRectSolid(0, 0, 0, 320, 240, C2D_Color32f(0, 0, 0, screenFadeAlpha));
            C3D_FrameEnd(0);
        }
    }

    if (dialogSheet)    C2D_SpriteSheetFree(dialogSheet);
    if (progressSheet)  C2D_SpriteSheetFree(progressSheet);
    if (commonSheet)    C2D_SpriteSheetFree(commonSheet);
    appTaskExit();
    httpcExit();
    ndspExit();
    archExit();
    romfsUnmount("rom");
    C3D_RenderTargetDelete(botScr);
    C3D_RenderTargetDelete(topScr);
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    
    if (__ENABLE_PLUGIN_LOADER && runPlugin && !aptShouldClose()){
        u32 ret = 0;
        PluginLoadParameters plgparam = {0};
        plgparam.noFlash = true;
        plgparam.lowTitleId = (u32)bootTitle;
        strcpy(plgparam.path, PLUGIN_PATH);
        APT_PrepareToDoApplicationJump(0, bootTitle, MEDIATYPE_SD);
        if (!isCitra) {
            ret = loaderInit();
            if (R_FAILED(ret)) customBreak(0xDEADBEE0,0x00000001,ret);
            ret = LOADER__Custom__DisableNextGamePatch();
            if (R_FAILED(ret)) customBreak(0xDEADBEE0,0x00000002,ret);
            loaderExit();
        }
        ret = plgLdrInit();
        if (R_FAILED(ret)) customBreak(0xDEADBEE1,0x00000001,ret);
        ret = PLGLDR__SetPluginLoaderState(true);
        if (R_FAILED(ret)) customBreak(0xDEADBEE1,0x00000002,ret);
        ret = PLGLDR__SetPluginLoadParameters(&plgparam);
        if (R_FAILED(ret)) customBreak(0xDEADBEE1,0x00000003,ret);
        plgLdrExit();

        u8 hmac[0x20];
        APT_DoApplicationJump(NULL, 0, hmac);
        for(;;);
    }
    return 0;
}
