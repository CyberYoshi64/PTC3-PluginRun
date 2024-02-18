#include "objects/ctrhud.h"

#define HUDCTR_BGCOLOR  0x40988078

HUD_CTR_global *hudCtrInit(void) {
    HUD_CTR_global *s = malloc(sizeof(HUD_CTR_global));
    if (!s) return NULL;
    memset(s, 0, sizeof(HUD_CTR_global));
    s->textBuf = C2D_TextBufNew(32);
    if (!s->textBuf) svcBreak(USERBREAK_PANIC);
    s->graphics = C2D_SpriteSheetLoad("rom:/gfx/hud.t3x");
    if (!s->graphics) svcBreak(USERBREAK_PANIC);
    sprintf(s->names[NTWKSTATE_ENABLED_INIT], "Enabled");
    sprintf(s->names[NTWKSTATE_ENABLED_SWITCHING], "Enabled");
    sprintf(s->names[NTWKSTATE_INTERNET], "Internet");
    sprintf(s->names[NTWKSTATE_LOCAL], "Local");
    sprintf(s->names[NTWKSTATE_LOCAL1], "Local");
    sprintf(s->names[NTWKSTATE_STREETPASS], "StreetPass");
    sprintf(s->names[NTWKSTATE_LOCAL2], "Local");
    sprintf(s->names[NTWKSTATE_DISABLED], "Disabled");

    for (u32 i = 0; i < C2D_SpriteSheetCount(s->graphics); i++)
        C3D_TexSetFilter(C2D_SpriteSheetGetImage(s->graphics, i).tex, GPU_LINEAR, GPU_LINEAR);

    u32 iconColor[NTWKSTATE_LENGTH] = {
        0xFFD8D8D8,
        0xFFD8D8D8,
        0xFFFFA020,
        0xFF1890FF,
        0xFF1890FF,
        0xFF20C040,
        0xFF1890FF,
        0xFF888888
    };

    u32 maskColor[NTWKSTATE_LENGTH] = {
        0xFF888888,
        0xFF888888,
        0xFF78300C,
        0xFF003880,
        0xFF003880,
        0xFF0C3318,
        0xFF003880,
        0xFF404040
    };

    u32 textColor[NTWKSTATE_LENGTH] = {
        0xFF404040,
        0xFF404040,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF
    };

    u32 frameColor[NTWKSTATE_LENGTH] = {
        0xFF404040,
        0xFF404040,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF
    };

    memcpy(s->iconColor, iconColor, sizeof(iconColor));
    memcpy(s->maskColor, maskColor, sizeof(maskColor));
    memcpy(s->textColor, textColor, sizeof(textColor));
    memcpy(s->frameColor, frameColor, sizeof(frameColor));

    return s;
}

HUD_CTR *hudCtrIconInit(HUD_CTR_global* global) {
    if (!global) return NULL;
    HUD_CTR* s = malloc(sizeof(HUD_CTR));
    if (!s) return NULL;
    memset(s, 0, sizeof(HUD_CTR));
    s->networkState = 255;
    s->global = global;
    s->x = s->y = 2;
    s->width = 92;
    s->height = 20;
    return s;
}

void hudCtrFree(HUD_CTR_global* self) {
    if (!self) return;
    C2D_TextBufDelete(self->textBuf);
    C2D_SpriteSheetFree(self->graphics);
    free(self); self = NULL;
}
void hudCtrIconFree(HUD_CTR* self) {
    if (!self) return;
    free(self); self = NULL;
}

void hudCtrSetPosition(HUD_CTR* self, float x, float y) {
    if (!self) return;
    self->x = x;
    self->y = y;
}

void hudCtrSetBoxSize(HUD_CTR* self, float width, float height) {
    if (!self) return;
    self->width = width;
    self->height = height;
}

void hudCtrTick(HUD_CTR* self) {
    if (!self) return;
    bool change = self->networkState != OS_SharedConfig->network_state;
    self->networkState = OS_SharedConfig->network_state;
    self->strength = OS_SharedConfig->wifi_strength;
    self->timer++;
    if (change) {
        self->timer = 0;
        C2D_TextBufClear(self->global->textBuf);
        C2D_TextParse(&self->text, self->global->textBuf, self->global->names[self->networkState % NTWKSTATE_LENGTH]);
        C2D_TextOptimize(&self->text);
    }
}

void hudCtrRender(HUD_CTR* self) {
    if (!self) return;
    C2D_SpriteSheet sheet = self->global->graphics;
    C2D_Image i; C2D_ImageTint t;
    u8 ns = self->networkState, ws = self->strength;
    C2D_DrawParams p = {{self->x,self->y,7,18},{0,0},0,0};
    float tw;
    
    C2D_Flush();
    C2D_SetTintMode(C2D_TintSolid);
    
    C2D_PlainImageTint(&t, 0xFF000000, 1);
    i = C2D_SpriteSheetGetImage(sheet, sheet_hud_box_le_idx);
    p.pos.x += 25; p.pos.w = 8;
    C2D_DrawImage(i, &p, &t);
    i = C2D_SpriteSheetGetImage(sheet, sheet_hud_box_me_idx);
    p.pos.x += 8; p.pos.w = self->width;
    C2D_DrawImage(i, &p, &t);
    i = C2D_SpriteSheetGetImage(sheet, sheet_hud_box_le_idx);
    p.pos.x += self->width; p.pos.w = -8;
    C2D_DrawImage(i, &p, &t);
    
    C2D_PlainImageTint(&t, self->global->maskColor[ns & 7], 1);
    i = C2D_SpriteSheetGetImage(sheet, sheet_hud_box_l_idx);
    p.pos.x = self->x + 25; p.pos.w = 8;
    C2D_DrawImage(i, &p, &t); p.pos.x += 8;
    if (self->width) {
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_box_m_idx);
        p.pos.w = self->width;
        C2D_DrawImage(i, &p, &t);
    }
    i = C2D_SpriteSheetGetImage(sheet, sheet_hud_box_l_idx);
    p.pos.x += self->width; p.pos.w = -8;
    C2D_DrawImage(i, &p, &t);

    C2D_PlainImageTint(&t, self->global->iconColor[ns & 7], 1);
    i = C2D_SpriteSheetGetImage(sheet, sheet_hud_box_lf_idx);
    p.pos.x = self->x + 25; p.pos.w = 8;
    C2D_DrawImage(i, &p, &t); p.pos.x += 8;
    if (self->width) {
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_box_mf_idx);
        p.pos.w = self->width;
        C2D_DrawImage(i, &p, &t);
    }
    i = C2D_SpriteSheetGetImage(sheet, sheet_hud_box_lf_idx);
    p.pos.x += self->width; p.pos.w = -8;
    C2D_DrawImage(i, &p, &t);
    if (self->width > 32) {
        C2D_TextGetDimensions(&self->text, .5, .5, &tw, NULL);
        if (tw < 1.f) tw = 1.f;
        C2D_DrawText(&self->text, C2D_AlignCenter|C2D_WithColor|C2D_AtBaseline, self->x + 33 + self->width/2, p.pos.y + 13.5f, 0, .5 * C2D_Clamp((self->width + 8) / tw, .1, 1.f), .5, self->global->textColor[ns & 7]);
    }

    p.pos.x = self->x; p.pos.w = 7;
    C2D_PlainImageTint(&t, 0xFF000000, 1);
    switch (ns) {
    case NTWKSTATE_INTERNET:
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_internet_bg_idx);
        C2D_DrawImage(i, &p, &t);
        p.pos.x += 7; p.pos.w = 16;
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_bar0_bg_idx + (ws & 3));
        C2D_DrawImage(i, &p, &t);
        p.pos.x -= 7; p.pos.w = 7;
        C2D_PlainImageTint(&t, self->global->iconColor[ns & 7], 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_internet_idx);
        C2D_DrawImage(i, &p, &t);
        C2D_PlainImageTint(&t, HUDCTR_BGCOLOR, 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_mask_bar_idx);
        p.pos.x += 7; p.pos.w = 16;
        C2D_DrawImage(i, &p, &t);
        C2D_PlainImageTint(&t, self->global->iconColor[ns & 7], 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_bar0_idx + (ws & 3));
        C2D_DrawImage(i, &p, &t);
        break;
    case NTWKSTATE_STREETPASS:
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_cecman_bg_idx);
        C2D_DrawImage(i, &p, &t);
        p.pos.x += 7; p.pos.w = 15;
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_cecwave0_bg_idx + ((self->timer>>6) & 3));
        C2D_DrawImage(i, &p, &t);
        p.pos.x -= 7; p.pos.w = 7;
        C2D_PlainImageTint(&t, self->global->iconColor[ns & 7], 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_cecman_idx);
        C2D_DrawImage(i, &p, &t);
        C2D_PlainImageTint(&t, HUDCTR_BGCOLOR, 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_mask_cecwave_idx);
        p.pos.x += 7; p.pos.w = 15;
        C2D_DrawImage(i, &p, &t);
        C2D_PlainImageTint(&t, self->global->iconColor[ns & 7], 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_cecwave0_idx + ((self->timer>>6) & 3));
        C2D_DrawImage(i, &p, &t);
        break;
    case NTWKSTATE_LOCAL:
    case NTWKSTATE_LOCAL1:
    case NTWKSTATE_LOCAL2:
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_local_bg_idx);
        C2D_DrawImage(i, &p, &t);
        p.pos.x += 7; p.pos.w = 16;
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_bar0_bg_idx + (ws & 3));
        C2D_DrawImage(i, &p, &t);
        p.pos.x -= 7; p.pos.w = 7;
        C2D_PlainImageTint(&t, self->global->iconColor[ns & 7], 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_local_idx);
        C2D_DrawImage(i, &p, &t);
        C2D_PlainImageTint(&t, HUDCTR_BGCOLOR, 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_mask_bar_idx);
        p.pos.x += 7; p.pos.w = 16;
        C2D_DrawImage(i, &p, &t);
        C2D_PlainImageTint(&t, self->global->iconColor[ns & 7], 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_bar0_idx + (ws & 3));
        C2D_DrawImage(i, &p, &t);
        break;
    case NTWKSTATE_ENABLED_INIT:
    case NTWKSTATE_ENABLED_SWITCHING:
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_generic_bg_idx);
        C2D_DrawImage(i, &p, &t);
        //p.pos.x += 7; p.pos.w = 16;
        //i = C2D_SpriteSheetGetImage(sheet, sheet_hud_bar0_bg_idx);
        //C2D_DrawImage(i, &p, &t);
        //p.pos.x -= 7; p.pos.w = 7;
        C2D_PlainImageTint(&t, self->global->iconColor[ns & 7], 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_generic_idx);
        C2D_DrawImage(i, &p, &t);
        C2D_PlainImageTint(&t, HUDCTR_BGCOLOR, 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_mask_bar_idx);
        p.pos.x += 7; p.pos.w = 16;
        C2D_DrawImage(i, &p, &t);
        C2D_PlainImageTint(&t, self->global->iconColor[ns & 7], 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_bar0_idx);
        C2D_DrawImage(i, &p, &t);
        break;
    case NTWKSTATE_DISABLED:
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_generic_bg_idx);
        C2D_DrawImage(i, &p, &t);
        p.pos.x += 7; p.pos.w = 16;
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_noconnect_bg_idx);
        C2D_DrawImage(i, &p, &t);
        p.pos.x -= 7; p.pos.w = 7;
        C2D_PlainImageTint(&t, self->global->iconColor[ns & 7], 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_generic_idx);
        C2D_DrawImage(i, &p, &t);
        C2D_PlainImageTint(&t, self->global->maskColor[ns & 7], 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_noconnect_idx);
        p.pos.x += 7; p.pos.w = 16;
        C2D_DrawImage(i, &p, &t);
        break;
    default:
        C2D_PlainImageTint(&t, 0xFF0040FF, 1);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_generic_idx);
        C2D_DrawImage(i, &p, &t);
        i = C2D_SpriteSheetGetImage(sheet, sheet_hud_noconnect_idx);
        p.pos.x += 7; p.pos.w = 14;
        C2D_DrawImage(i, &p, &t);
        break;
    }
}

void hudCtrTickFake(HUD_CTR* self, u8 networkState, u8 strength) {
    if (!self) return;
    bool change = self->networkState != networkState;
    self->networkState = networkState;
    self->strength = strength;
    self->timer++;
    if (change) {
        self->timer = 0;
        C2D_TextBufClear(self->global->textBuf);
        C2D_TextParse(&self->text, self->global->textBuf, self->global->names[self->networkState % NTWKSTATE_LENGTH]);
        C2D_TextOptimize(&self->text);
    }
}