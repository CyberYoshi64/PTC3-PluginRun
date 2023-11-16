#include "menuDialog.h"
#include "hidRead.h"
#include "utils.h"
#include "sheets/sheet_dialog.h"

extern u32 mainCnt;

C2D_SpriteSheet menuDialogSheet;

u32 menuDialogBackground = 0x00404000;

#define MENUDIAGLOG_MSGFONTSIZE .5625

MenuDialog* menuDialogNew(u32 mode) {
    MenuDialog* dlg = malloc(sizeof(MenuDialog));
    if (!dlg) return NULL;
    memset(dlg, 0, sizeof(MenuDialog));
    snprintf(dlg->buttonText[0], MENUDLG__BTN_SIZE, MENUDLG__BUTTON1);
    snprintf(dlg->buttonText[1], MENUDLG__BTN_SIZE, MENUDLG__BUTTON2);
    dlg->miscBuf    = C2D_TextBufNew(MENUDLG__MISCBUF_SIZE);
    dlg->msgBuf     = C2D_TextBufNew(MENUDLG__MSG_SIZE);
    sprintf(dlg->message, "This dialog was spawned without text.");
    dlg->mode = mode;
    return dlg;
}

MenuDialog* menuDialogNewTemp(u32 mode) {
    if (nextDialog) return NULL;
    MenuDialog* dlg = malloc(sizeof(MenuDialog));
    if (!dlg) return NULL;
    memset(dlg, 0, sizeof(MenuDialog));
    snprintf(dlg->buttonText[0], MENUDLG__BTN_SIZE, MENUDLG__BUTTON1);
    snprintf(dlg->buttonText[1], MENUDLG__BTN_SIZE, MENUDLG__BUTTON2);
    dlg->miscBuf    = C2D_TextBufNew(MENUDLG__MISCBUF_SIZE);
    dlg->msgBuf     = C2D_TextBufNew(MENUDLG__MSG_SIZE);
    sprintf(dlg->message, "This dialog was spawned without text.");
    dlg->mode = mode | MENUDIALOG_ONESHOT;
    return dlg;
}

void menuDialogMessage(MenuDialog* self, const char* message) {
    if (!self) return;
    snprintf(self->message, MENUDLG__MSG_SIZE, message);
}

void menuDialogMessageAppend(MenuDialog* self, const char* message) {
    if (!self) return;
    u32 off = strlen(self->message);
    if (off >= MENUDLG__MSG_SIZE) return;
    snprintf(self->message + off, MENUDLG__MSG_SIZE - off, message);
}

void menuDialogTitle(MenuDialog* self, const char* title) {
    if (!self) return;
    snprintf(self->title, MENUDLG__TITLE_SIZE, title);
}

void menuDialogButton(MenuDialog* self, int index, const char* text) {
    if (!self) return;
    if (index < 1 || index > MENUDLG__BTN_COUNT) return;
    if (text && *text) {
        snprintf(self->buttonText[index-1], MENUDLG__BTN_SIZE, text);
    } else {
        if (index == 1) snprintf(self->buttonText[0], MENUDLG__BTN_SIZE, MENUDLG__BUTTON1);
        if (index == 2) snprintf(self->buttonText[1], MENUDLG__BTN_SIZE, MENUDLG__BUTTON2);
    }
}

void menuDialogSetWaitCallback(MenuDialog* self, MenuDlgWaitCBF func) {
    if (!self) return;
    self->waitCallback = func;
}

void menuDialogSetButtonCallback(MenuDialog* self, MenuDlgButtonCBF func) {
    if (!self) return;
    self->buttonCallback = func;
}

void menuDialogPrepare(MenuDialog* self) {
    if (!self) return;
    char tb[64] = {0};
    C2D_TextParse(&self->titleT, self->miscBuf, self->title);
    snprintf(tb, 36, "\uE001 %s", self->buttonText[0]);
    C2D_TextParse(&self->buttonT[0], self->miscBuf, tb);
    snprintf(tb, 36, "\uE000 %s", self->buttonText[1]);
    C2D_TextParse(&self->buttonT[1], self->miscBuf, tb);
    char *buf = malloc(2048);
    if (!buf) return;
    memset(buf, 0, 2048);
    memset(tb, 0, sizeof(tb));
    float w;

    u32 i = 0, l = strlen(self->message);
    s32 lbOpp = -1; u32 uni = 0, lo = 0;
    char c;
    while (i < l) {
        c = self->message[i];
        if (strchr(" -~,", c)) lbOpp = strlen(tb);
        if (strchr("\r\n", c)) lbOpp = -2;
        tb[strlen(tb)] = c;
        if (uni) {
            uni--; i++; continue;
        } else if ((c & 0xF0) == 0xF0) {
            uni = 2; i++; continue;
        } else if ((c & 0xF0) == 0xE0) {
            uni = 1; i++; continue;
        } else if ((c & 0xF0) == 0xC0) {
            uni = 0; i++; continue;
        }
        C2D_FontGetTextSize(tb, MENUDIAGLOG_MSGFONTSIZE, 1.f, &w, NULL);
        if (w >= 264.f || strlen(tb)>60 || lbOpp<-1) {
            if (lbOpp == -1) {
                tb[strlen(tb)] = '\n';
            } else if (lbOpp<-1) {
                tb[strlen(tb)-1] = '\n';
            } else {
                if (tb[lbOpp] != ' ') {
                    tb[lbOpp + 1] = '\n';
                    tb[lbOpp + 2] = 0;
                } else {
                    tb[lbOpp] = '\n';
                    tb[lbOpp + 1] = 0;
                }
                i = lo + lbOpp;
            }
            memcpy(buf + strlen(buf), tb, strlen(tb));
            memset(tb, 0, sizeof(tb));
            lbOpp = -1; lo = i + 1;
        }
        i++;
    }
    if (strlen(tb)) memcpy(buf + strlen(buf), tb, strlen(tb));

    C2D_TextParse(&self->msgT, self->msgBuf, buf);
    free(buf);

    C2D_TextOptimize(&self->titleT);
    C2D_TextOptimize(&self->msgT);
    C2D_TextOptimize(&self->buttonT[0]);
    C2D_TextOptimize(&self->buttonT[1]);

    bool hasSecondBtn = (self->mode & MENUDIALOG_ENABLE_BUTTON__2);
    bool haveBtn = (self->mode & MENUDIALOG_ENABLE_BUTTON1);

    buttonSetup(&self->cancelBtn, 16, 187 + (!hasSecondBtn * 100), 144, 44);
    buttonSetup(&self->okayBtn, 16 + hasSecondBtn*144, 187 + (!haveBtn * 100), 144 * (2 - hasSecondBtn), 44);
}

void menuDialogFree(MenuDialog* self) {
    if (!self) return;
    C2D_TextBufDelete(self->miscBuf);
    C2D_TextBufDelete(self->msgBuf);
    free(self);
}

void menuDialog__Render(MenuDialog* self, gfxScreen_t screen) {
    C2D_DrawParams p = {0};
    C2D_ImageTint  t;
    C2D_Image      i;
    u32 btn1TC[2] = {
        C2D_Color32f( 1, 1, 1, self->boxAlpha),
        C2D_Color32f(.2,.2,.2, self->boxAlpha)
    };
    u32 btn2TC[2] = {
        C2D_Color32f( 1, 1, 1, self->boxAlpha),
        C2D_Color32f(.2,.2,.2, self->boxAlpha)
    };

    u32 titleBack  = C2D_Color32f(0, 0, 0, self->boxAlpha);
    u32 menuDialogBackgroundAlpha = menuDialogBackground | C2D_FloatToU8(self->boxAlpha)<<24;
    u32 fg = C2D_Color32f(1, 1, 1, self->boxAlpha);
    u32 btnColor[4] = {
        C2D_Color32f(.3, .7, 1, self->boxAlpha),
        C2D_Color32f(.2, .5, 1, self->boxAlpha),
        C2D_Color32f(1, .5, .6, self->boxAlpha),
        C2D_Color32f(1, .3, .5, self->boxAlpha)
    };
    u32 btnPColor[4] = {
        C2D_Color32f(.2, .3, .8, self->boxAlpha),
        C2D_Color32f(.3, .6, 1, self->boxAlpha),
        C2D_Color32f(.5, .1, .2, self->boxAlpha),
        C2D_Color32f(.9, .4, .5, self->boxAlpha)
    };
    u32 waitColor[4] = {
        C2D_Color32f(.05, .15, .15, self->boxAlpha),
        C2D_Color32f(.5, 1, 1, self->boxAlpha),
        C2D_Color32f(0, .75, 1, self->boxAlpha),
        C2D_Color32f(0, .3, .8, self->boxAlpha),
    };
    bool drawSplit = true;
    if (!(self->mode & MENUDIALOG_ENABLE_BUTTON__2)) {
        btnColor[2] = btnColor[0];
        btnColor[3] = btnColor[1];
        drawSplit = false;
    }
    u32 tmp;

    if (self->cancelBtn.selected && self->cancelBtn.inArea) {
        tmp = btn2TC[0];
        btn2TC[0] = btn2TC[1];
        btn2TC[1] = tmp;

        btnColor[2] = btnPColor[2];
        btnColor[3] = btnPColor[3];
    }
    if (self->okayBtn.selected && self->okayBtn.inArea) {
        tmp = btn1TC[0];
        btn1TC[0] = btn1TC[1];
        btn1TC[1] = tmp;

        btnColor[0] = btnPColor[0];
        btnColor[1] = btnPColor[1];
        if (!(self->mode & MENUDIALOG_ENABLE_BUTTON__2)) {
            btnColor[2] = btnColor[0];
            btnColor[3] = btnColor[1];
        }
    }

    float noTitle = !(self->mode & MENUDIALOG_TITLE) * 16;
    float noButtons = !(self->mode & MENUDIALOG_ENABLE_BUTTON1) * 36;
    float hasWaitIcon = !!(self->mode & MENUDIALOG_WAIT) * 40;
    float msgH;
    C2D_TextGetDimensions(&self->msgT, MENUDIAGLOG_MSGFONTSIZE, MENUDIAGLOG_MSGFONTSIZE, NULL, &msgH);
    C2D_Flush();
    C2D_SetTintMode(C2D_TintMult);
    if (screen == GFX_TOP) {
        if (self->mode & MENUDIALOG_FADE_TOP)
            C2D_DrawRectSolid(0, 0, 0, 400, 240, C2D_Color32f(0, 0, 0, self->boxAlpha / 2.f));
    } else {
        C2D_DrawRectSolid(0, 0, 0, 320, 240, C2D_Color32f(0, 0, 0, self->boxAlpha / 2.f));
        i = C2D_SpriteSheetGetImage(menuDialogSheet, sheet_dialog_base_idx);
        p.center.x = 150 * self->boxScale;
        p.center.y = i.subtex->height / 2.f * self->boxScale;
        p.pos.x = 160; p.pos.y = 120;
        p.pos.w = i.subtex->width * self->boxScale;
        p.pos.h = i.subtex->height * self->boxScale;
        C2D_PlainImageTint(&t, menuDialogBackgroundAlpha, 1);
        C2D_DrawImage(i, &p, &t);
        p.center.x = -120 * self->boxScale;
        p.center.y = i.subtex->height / 2.f * self->boxScale;
        p.pos.x = 160; p.pos.y = 120;
        p.pos.w = i.subtex->width * -self->boxScale;
        p.pos.h = i.subtex->height * self->boxScale;
        C2D_PlainImageTint(&t, menuDialogBackgroundAlpha, 1);
        C2D_DrawImage(i, &p, &t);

        i = C2D_SpriteSheetGetImage(menuDialogSheet, sheet_dialog_baseMiddle_idx);
        p.center.x = 120.f * self->boxScale;
        p.center.y = i.subtex->height / 2.f * self->boxScale;
        p.pos.x = 160; p.pos.y = 120;
        p.pos.w = 240.f * self->boxScale;
        p.pos.h = i.subtex->height * self->boxScale;
        C2D_PlainImageTint(&t, menuDialogBackgroundAlpha, 1);
        C2D_DrawImage(i, &p, &t);

        if (self->mode & MENUDIALOG_TITLE) {
            C2D_DrawRectangle(160 - 128 * self->boxScale, 120 - 104 * self->boxScale, 0, 32 * self->boxScale, 24 * self->boxScale, titleBack & 0xFFFFFF, titleBack, titleBack & 0xFFFFFF, titleBack);
            C2D_DrawRectSolid(160 - 96 * self->boxScale, 120 - 104 * self->boxScale, 0, 192 * self->boxScale, 24 * self->boxScale, titleBack);
            C2D_DrawRectangle(160 + 96 * self->boxScale, 120 - 104 * self->boxScale, 0, 32 * self->boxScale, 24 * self->boxScale, titleBack, titleBack & 0xFFFFFF, titleBack, titleBack & 0xFFFFFF);
            C2D_DrawText(&self->titleT, C2D_AlignCenter|C2D_WithColor, 160, 120 - 100 * self->boxScale, 0, self->boxScale * .5625, self->boxScale * .525, fg);
        }
        
        C2D_Flush();
        C3D_SetScissor$(
            GPU_SCISSOR_NORMAL,
            floorf(160 - 144 * self->boxScale), floorf(120 - (80 + noTitle) * self->boxScale),
            ceilf(160 + 144 * self->boxScale), ceilf(120 + (64 + noButtons - hasWaitIcon) * self->boxScale)
        );
        float textMiddle = (-16 - noTitle - hasWaitIcon + noButtons) / 2;
        float textHeight = (140 + noButtons + noTitle - hasWaitIcon);
        C2D_DrawText(&self->msgT, C2D_AlignCenter|C2D_WithColor, 160, 120 + (textMiddle - C2D_Clamp(msgH, 0, textHeight) / 2) * self->boxScale, 0, self->boxScale * MENUDIAGLOG_MSGFONTSIZE, self->boxScale * MENUDIAGLOG_MSGFONTSIZE, fg);

        C2D_DrawRectangle(0, 120 - (82 + noTitle) * self->boxScale, 0, 320, 10, menuDialogBackgroundAlpha, menuDialogBackgroundAlpha, menuDialogBackground, menuDialogBackground);
        C2D_DrawRectangle(0, 120 + (55 + noButtons - hasWaitIcon) * self->boxScale, 0, 320, 10, menuDialogBackground, menuDialogBackground, menuDialogBackgroundAlpha, menuDialogBackgroundAlpha);
        C2D_Flush();
        C3D_SetScissor(GPU_SCISSOR_DISABLE, 0, 0, 0, 0);

        if (hasWaitIcon) {
            bool hasProgress = !!(self->mode & MENUDIALOG_PROGRESS);
            p.center.x = 12 * self->boxScale;
            p.center.y = 12 * self->boxScale;
            p.pos.x = 160 + (hasProgress * 100) * self->boxScale; p.pos.y = 120 + (44 + noButtons) * self->boxScale;
            p.pos.w = 24 * self->boxScale;
            p.pos.h = 24 * self->boxScale;
            i = C2D_SpriteSheetGetImage(menuDialogSheet, sheet_dialog_waiticon_bg_idx);
            C2D_PlainImageTint(&t, waitColor[0], 1);
            C2D_DrawImage(i, &p, &t);
            p.angle = C3D_AngleFromDegrees((mainCnt * 10) % 360);
            i = C2D_SpriteSheetGetImage(menuDialogSheet, sheet_dialog_waiticon_idx);
            C2D_SetImageTint(&t, C2D_TopLeft, waitColor[1], 1);
            C2D_SetImageTint(&t, C2D_BotLeft, waitColor[2], 1);
            C2D_RightImageTint(&t, waitColor[3], 1);
            C2D_DrawImage(i, &p, &t);
            p.angle = 0;
            if (hasProgress) {
                C2D_DrawRectSolid(160 - 110 * self->boxScale, p.pos.y - 8 * self->boxScale, 0, 180 * self->boxScale, 16 * self->boxScale, waitColor[0]);
                C2D_DrawRectangle(160 - 110 * self->boxScale, p.pos.y - 8 * self->boxScale, 0, 180 * C2D_Clamp(self->progress, 0, 1) * self->boxScale, 16 * self->boxScale, waitColor[1], waitColor[2], waitColor[3], waitColor[3]);
            }
        }
        
        if ((self->mode & MENUDIALOG_ENABLE_BUTTON1)) {
            i = C2D_SpriteSheetGetImage(menuDialogSheet, sheet_dialog_button_idx);
            p.center.x = 150 * self->boxScale;
            p.center.y = -68 * self->boxScale;
            p.pos.x = 160; p.pos.y = 120;
            p.pos.w = i.subtex->width * self->boxScale;
            p.pos.h = i.subtex->height * self->boxScale;
            C2D_PlainImageTint(&t, btnColor[2], 1);
            C2D_BottomImageTint(&t, btnColor[3], 1);
            C2D_DrawImage(i, &p, &t);
            p.center.x = -120 * self->boxScale;
            p.center.y = -68 * self->boxScale;
            p.pos.x = 160; p.pos.y = 120;
            p.pos.w = i.subtex->width * -self->boxScale;
            p.pos.h = i.subtex->height * self->boxScale;
            C2D_PlainImageTint(&t, btnColor[0], 1);
            C2D_BottomImageTint(&t, btnColor[1], 1);
            C2D_DrawImage(i, &p, &t);

            i = C2D_SpriteSheetGetImage(menuDialogSheet, sheet_dialog_buttonMiddle_idx);
            p.center.x = 120 * self->boxScale;
            p.center.y = -68 * self->boxScale;
            p.pos.x = 160; p.pos.y = 120;
            p.pos.w = 120 * self->boxScale;
            p.pos.h = i.subtex->height * self->boxScale;
            C2D_PlainImageTint(&t, btnColor[2], 1);
            C2D_BottomImageTint(&t, btnColor[3], 1);
            C2D_DrawImage(i, &p, &t);
            p.center.x = 0;
            p.center.y = -68 * self->boxScale;
            p.pos.x = 160; p.pos.y = 120;
            p.pos.w = 120 * -self->boxScale;
            p.pos.h = i.subtex->height * self->boxScale;
            C2D_PlainImageTint(&t, btnColor[0], 1);
            C2D_BottomImageTint(&t, btnColor[1], 1);
            C2D_DrawImage(i, &p, &t);

            if (self->mode & MENUDIALOG_ENABLE_BUTTON__2) {
                C2D_DrawText(&self->buttonT[0], C2D_AlignCenter|C2D_WithColor, 160 - 72.5 * self->boxScale, 120 + 77.5 * self->boxScale, 0, self->boxScale * .7, self->boxScale * .7, btn2TC[0]);
                C2D_DrawText(&self->buttonT[0], C2D_AlignCenter|C2D_WithColor, 160 - 72.5 * self->boxScale, 120 + 76.0 * self->boxScale, 0, self->boxScale * .7, self->boxScale * .7, btn2TC[1]);
                C2D_DrawText(&self->buttonT[1], C2D_AlignCenter|C2D_WithColor, 160 + 72.5 * self->boxScale, 120 + 77.5 * self->boxScale, 0, self->boxScale * .7, self->boxScale * .7, btn1TC[0]);
                C2D_DrawText(&self->buttonT[1], C2D_AlignCenter|C2D_WithColor, 160 + 72.5 * self->boxScale, 120 + 76.0 * self->boxScale, 0, self->boxScale * .7, self->boxScale * .7, btn1TC[1]);
            } else {
                C2D_DrawText(&self->buttonT[1], C2D_AlignCenter|C2D_WithColor, 160, 120 + 77.5 * self->boxScale, 0, self->boxScale * .7, self->boxScale * .7, btn1TC[0]);
                C2D_DrawText(&self->buttonT[1], C2D_AlignCenter|C2D_WithColor, 160, 120 + 76.0 * self->boxScale, 0, self->boxScale * .7, self->boxScale * .7, btn1TC[1]);
            }
            i = C2D_SpriteSheetGetImage(menuDialogSheet, sheet_dialog_buttonSplit_idx);
            p.center.x = -65 * self->boxScale;
            p.center.y = 145 * self->boxScale;
            p.pos.x = 160; p.pos.y = 120;
            p.pos.w = 6 * self->boxScale;
            p.pos.h = 290 * self->boxScale;
            p.angle = C3D_AngleFromDegrees(90);
            C2D_PlainImageTint(&t, C2D_Color32f(1,1,1,self->boxAlpha), 0);
            C2D_DrawImage(i, &p, &t);
            if (drawSplit) {
                p.center.x = 3 * self->boxScale;
                p.center.y = -68 * self->boxScale;
                p.pos.h = 42 * self->boxScale;
                p.angle = 0;
                C2D_PlainImageTint(&t, C2D_Color32f(1,1,1,self->boxAlpha), 0);
                C2D_DrawImage(i, &p, &t);
            }
        }
        // buttonRender(&self->cancelBtn);
        // buttonRender(&self->okayBtn);
    }
}

bool menuDialog__Tick(MenuDialog* self) {
    bool hasButtons = self->mode & MENUDIALOG_ENABLE_BUTTON1;
    bool hasButton2 = (self->mode & MENUDIALOG_ENABLE_BUTTON2) == MENUDIALOG_ENABLE_BUTTON2;
    u32 state = self->state;
    u32 btn;
    switch (state) {
    case 0:
        if (self->boxAlpha < 1.f) {
            self->boxAlpha += .1f;
            self->boxScale = 1.f - ((1.f - self->boxAlpha) / 20.f);
        } else
            state++;
        break;
    case 1:
        if (hasButtons) {
            if (buttonTick(&self->okayBtn) || (HID_BTNPRESSED & KEY_A))
                self->rc = 1;
        }
        if (hasButton2) {
            if (buttonTick(&self->cancelBtn) || (HID_BTNPRESSED & KEY_B))
                self->rc = 2;
        }

        if (self->rc && self->buttonCallback) {
            if (!self->buttonCallback(self->rc))
                self->rc = 0;
        }
        if (self->waitCallback) {
            if (self->waitCallback(&btn, &self->progress)) {
                self->rc = 1+!!(btn & BIT(31));
            }
        } else if ((self->mode & MENUDIALOG_WAIT) && !(self->mode & MENUDIALOG_ENABLE_BUTTON1)) {
            self->rc = 2;
        }

        if (HID_BTNPRESSED & KEY_ZL)
            self->rc = 3;


        if (self->rc)
            state++;
        break;
    case 2:
        if (self->boxAlpha > 0.f) {
            self->boxAlpha -= .1f;
            self->boxScale = 1.f + ((1.f - self->boxAlpha) / 20.f) * (self->rc == 1 ? 1.f : -1.f);
        } else
            state++;
        break;
    default:
        return true;
    }
    self->state = state;
    return false;
}

