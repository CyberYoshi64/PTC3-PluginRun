#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

extern C2D_SpriteSheet commonSheet;

typedef struct WaitIcon_s {
    float px, py;
    float scale;
    float alpha;
    u32 baseColor;
    u32 tintColor[4];
} WaitIcon;

#define WAITICON_STRUCTSIZE   sizeof(WaitIcon)
#define WAITICON_NATIVE_SIZE  24.0f

/**
 * @brief Allocate a wait icon
 * 
 * @return New wait icon; NULL on failure
 */
WaitIcon *waitIconNew(void);

/**
 * @brief Unallocate a wait icon from memory
 * 
 * @param self Wait icon struct
 */
void waitIconFree(WaitIcon *self);

/**
 * @brief Initialize a wait icon with default values
 * 
 * @param self Wait icon struct
 */
void waitIconInit(WaitIcon *self);

/**
 * @brief Set the position of a wait icon
 * 
 * @note The wait icon is positioned based on its center
 * 
 * @param self Wait icon struct
 * @param x Horizontal offset
 * @param y Vertical offset
 */
void waitIconSetPosition(WaitIcon *self, float x, float y);

/**
 * @brief Set the scale of a wait icon
 * 
 * @note The wait icon is scaled from its center
 * 
 * @param self Wait icon struct
 * @param scale Scale (1.0 = 100%)
 */
void waitIconSetScale(WaitIcon *self, float scale);

/**
 * @brief Set the base color of a wait icon
 * 
 * @param self Wait icon struct
 * @param c Color code
 */
void waitIconSetBaseColor(WaitIcon *self, u32 color);

/**
 * @brief Set a corner's color of a wait icon's spinner
 * 
 * @param self Wait icon struct
 * @param corner Corners (see C2D_Corner, -1=all at once)
 * @param c Color code
 */
void waitIconSetTintColor(WaitIcon *self, s32 corner, u32 color);

/**
 * @brief Set the transparency of a wait icon
 * 
 * @param self Wait icon struct
 * @param alpha 1.0 = opaque, 0.0 = invisible
 */
void waitIconSetAlpha(WaitIcon *self, float alpha);

/**
 * @brief Render a wait icon
 * 
 * @param self Wait icon struct
 */
void waitIconRender(WaitIcon *self);

/**
 * @brief Handle wait icon animations
 */
void waitIconTick(void);
