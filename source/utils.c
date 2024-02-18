#include "utils.h"

// https://github.com/ihaveamac/save-data-copy-tool/blob/main/source/main.c#L138-L150
u32 saveDataGetBuckets(u32 count) {
	if (count < 20)
		return (count < 4) ? 3 : count | 1;

	for (u32 i = 0; i < 100; ++i) {
		u32 ret = count + i;
		if (ret & 1 && ret % 3 && ret % 5 && ret % 7 && ret % 11 && ret % 13 && ret % 17)
			return ret;
	}
	return count | 1;
}

Result formatSave(u64 titleID, FS_MediaType mt, u32 files, u32 directories, bool duplicateData) {
    return FSUSER_FormatSaveData(
        ARCHIVE_USER_SAVEDATA,
        (FS_Path){PATH_BINARY, 12, (u32[]){mt, titleID, (titleID>>32)}},
        0x200,
        directories,
        files,
        saveDataGetBuckets(directories),
        saveDataGetBuckets(files),
        duplicateData
    );
}

Result formatExtData(u64 titleID, FS_MediaType mt, u32 files, u32 directories) {
    return FSUSER_CreateExtSaveData(
        (FS_ExtSaveDataInfo){
            0,
            .mediaType = mt,
            .saveId = (u32)(titleID & 0xFFFFFF00)>>8
        },
        directories,
        files,
        -1,
        ptc3_smdh_bin_len,
        (u8*)ptc3_smdh_bin
    );
}

void C3D_SetScissor$(GPU_SCISSORMODE mode, u32 x1, u32 y1, u32 x2, u32 y2) {
    C3D_SetScissor(mode, 240 - y2, x1, 239 - y1, x2 - 1);
}

void C2D_FontGetTextSize(const char *str, float w, float h, float *width, float *height) {
	C2D_Text text;
    C2D_TextBuf tb = C2D_TextBufNew(strlen(str));
	C2D_TextFontParse(&text, NULL, tb, str);
	C2D_TextGetDimensions(&text, w, h, width, height);
    C2D_TextBufDelete(tb);
}

u32 acIsConnected(void) {
    u32 id = 0;
    //Result res = ACU_GetStatus(&id);
    Result res = ACU_GetWifiStatus(&id);
    return id;
}