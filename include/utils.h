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

void    C3D_SetScissor$(GPU_SCISSORMODE mode, u32 x1, u32 y1, u32 x2, u32 y2);
void    C2D_FontGetTextSize(const char *str, float w, float h, float *width, float *height);