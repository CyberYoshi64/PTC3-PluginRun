#pragma once
#include "main.h"

u32     saveDataGetBuckets(u32 count);
Result  formatSave(u64 titleID, FS_MediaType mt, u32 files, u32 directories, bool duplicateData);
Result  formatExtData(u64 titleID, FS_MediaType mt, u32 files, u32 directories);
void C3D_SetScissor$(GPU_SCISSORMODE mode, u32 x1, u32 y1, u32 x2, u32 y2);
void C2D_FontGetTextSize(const char *str, float w, float h, float *width, float *height);