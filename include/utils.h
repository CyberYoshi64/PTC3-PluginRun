#pragma once
#include "main.h"

/**
 * @brief Calculate buckets for save data
 * 
 * @param count Number of files/directories
 * @return Number of buckets (for use with FSUSER_FormatSaveData)
 */
u32     saveDataGetBuckets(u32 count);

/**
 * @brief Format a title's user save data
 * 
 * @param titleID Title ID
 * @param mt Media Type
 * @param files Number of files that can be created
 * @param directories Number of directories that can be created
 * @param duplicateData Whether to store an internal duplicate of the data
 */
Result  formatSave(u64 titleID, FS_MediaType mt, u32 files, u32 directories, bool duplicateData);

/**
 * @brief Format a title's ext save data
 * 
 * @param titleID Title ID
 * @param mt Media Type
 * @param files Number of files that can be created
 * @param directories Number of directories that can be created
 */
Result  formatExtData(u64 titleID, FS_MediaType mt, u32 files, u32 directories);

/**
 * @brief Helper function for C3D_SetScissor
 * 
 * @note
 * The original function used the raw position within the render target.
 * This helper changes the arguments to be relative to the perceived position
 * 
 * @param mode  Scissor mode
 * @param x1    Left edge of the scissor box
 * @param y1    Top edge of the scissor box
 * @param x2    Right edge of the scissor box
 * @param y2    Bottom edge of the scissor box
 */
void    C3D_SetScissor$(GPU_SCISSORMODE mode, u32 x1, u32 y1, u32 x2, u32 y2);

/**
 * @brief Calculate the text dimensions of a string
 * 
 * @param str Text to calculate dimenstions of
 * @param w Horizontal scale
 * @param h Vertical scale
 * @param width Output width
 * @param height Output height
 */
void    C2D_FontGetTextSize(const char *str, float w, float h, float *width, float *height);

/**
 * @brief Check whether the console is conencted.
 */
u32 acIsConnected(void);