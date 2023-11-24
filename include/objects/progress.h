#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdlib.h>
#include "sheets/sheet_progress.h"

typedef struct ProgressBar_s {
    float   px, py;
    float   width;
    float   scale;
    u32     baseColor;
    u32     backColor;
    u32     tintColor[4];
    bool    useSpark;
    float   sparkAnim;
    float   alpha;
    float   progress;
    
    float   displayedProgress;
    u32     dispAnimTimer;
    
    float   lastProgress;   // Old progress to determine animation
    float   oldProgress;    // Old progress to compute animation
    float   progStep;       // Difference between old and current progress;
} ProgressBar;

#define PROGRESSBAR_STRUCTSIZE  sizeof(ProgressBar)
#define PROGRESSBAR_WIDTH       124.f
#define PROGRESSBAR_HEIGHT      12.f

extern C2D_SpriteSheet progressSheet;

/**
 * @brief Allocate a new progress bar
 * 
 * @return New progress bar; NULL on failure
 */
ProgressBar* progressBarNew();

/**
 * @brief Unallocate a progress bar from memory
 * 
 * @param self Progress bar struct
 */
void progressBarFree(ProgressBar* self);

/**
 * @brief Initialize a progress bar with default values
 * 
 * @param self Progress bar struct
 */
void progressBarInit(ProgressBar* self);

/**
 * @brief Render a progress bar
 * 
 * @param self Progress bar struct
 */
void progressBarRender(ProgressBar* self);

/**
 * @brief Set the frame color of a progress bar
 * 
 * @param self Progress bar struct
 * @param c Color code
 */
void progressBarSetBaseColor(ProgressBar* self, u32 c);

/**
 * @brief Set a corner's color of a progress bar
 * 
 * @param self Progress bar struct
 * @param corner Corners (see C2D_Corner, -1=all at once)
 * @param c Color code
 */
void progressBarSetTintColor(ProgressBar* self, s32 corner, u32 c);

/**
 * @brief Set the position of a progress bar
 * 
 * @param self Progress bar struct
 * @param x Horizontal offset
 * @param y Vertical offset
 */
void progressBarSetPosition(ProgressBar* self, float x, float y);

/**
 * @brief Set the width of a progress bar
 * 
 * @param self Progress bar struct
 * @param width Progress bar width
 */
void progressBarSetWidth(ProgressBar* self, float width);

/**
 * @brief Set the scale of a progress bar
 * 
 * @param self Progress bar struct
 * @param scale Scale (1.0 = 100%)
 */
void progressBarSetScale(ProgressBar* self, float scale);

/**
 * @brief Set the transparency of a progress bar
 * 
 * @param self Progress bar struct
 * @param alpha 1.0 = opaque, 0.0 = invisible
 */
void progressBarSetAlpha(ProgressBar* self, float alpha);

/**
 * @brief Set the progress of a progress bar
 * 
 * @param self Progress bar struct
 * @param p Progress percentage (0.0 - 1.0)
 */
void progressBarSetProgress(ProgressBar* self, float p);

/**
 * @brief Set whether to use spark animation on a progress bar
 * 
 * @note
 * This is an experimental animation, it is safe to use however.
 * 
 * @param self Progress bar struct
 * @param enabled `true` to enable, `false` to disable.
 */
void progressBarUseSparkAnimation(ProgressBar* self, bool enabled);