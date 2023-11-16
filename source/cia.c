#include "cia.h"

Result CIA_LaunchTitle(u64 titleId, FS_MediaType mediaType) {
    Result ret = 0;
    u8 param[0x300] = {0};
    u8 hmac[0x20] = {0};

    if (R_FAILED(ret = APT_PrepareToDoApplicationJump(0, titleId, mediaType)))
        return ret;

    if (R_FAILED(ret = APT_DoApplicationJump(param, sizeof(param), hmac)))
        return ret;

    return 0;
}

Result deletePrevious(u64 titleid, FS_MediaType media) {
    Result ret = 0;

    u32 titles_amount = 0;
    ret = AM_GetTitleCount(media, &titles_amount);
    if (R_FAILED(ret))
        return ret;

    u32 read_titles = 0;
    u64 * titleIDs = (u64*)malloc(titles_amount * sizeof(u64));
    ret = AM_GetTitleList(&read_titles, media, titles_amount, titleIDs);
    if (R_FAILED(ret)) {
        free(titleIDs);
        return ret;
    }

    for (u32 i = 0; i < read_titles; i++) {
        if (titleIDs[i] == titleid) {
            ret = AM_DeleteAppTitle(media, titleid);
            break;
        }
    }

    free(titleIDs);
    if (R_FAILED(ret))
        return ret;

    return 0;
}

FS_MediaType getTitleDestination(u64 titleId) {
    u16 platform = (u16) ((titleId >> 48) & 0xFFFF);
    u16 category = (u16) ((titleId >> 32) & 0xFFFF);
    u8 variation = (u8) (titleId & 0xFF);

    //     DSiWare                3DS                    DSiWare, System, DLP         Application           System Title
    return platform == 0x0003 || (platform == 0x0004 && ((category & 0x8011) != 0 || (category == 0x0000 && variation == 0x02))) ? MEDIATYPE_NAND : MEDIATYPE_SD;
}

// Variables.
u64 installSize = 0, installOffset = 0;

Result installCia(Handle fileHandle, bool updatingSelf) {
    u32 bytes_read = 0, bytes_written;
    installSize = 0, installOffset = 0; u64 size = 0;
    Handle ciaHandle;
    AM_TitleEntry info;
    Result ret = 0;
    FS_MediaType media = MEDIATYPE_SD;

    ret = AM_GetCiaFileInfo(media, &info, fileHandle);
    if (R_FAILED(ret))
        return ret;

    media = getTitleDestination(info.titleID);

    if (!updatingSelf) {
        ret = deletePrevious(info.titleID, media);
        if (R_FAILED(ret))
            return ret;
    }

    ret = FSFILE_GetSize(fileHandle, &size);
    if (R_FAILED(ret))
        return ret;

    ret = AM_StartCiaInstall(media, &ciaHandle);
    if (R_FAILED(ret))
        return ret;

    u32 toRead = 0x200000;
    u8 *buf = (u8*)malloc(toRead);
    if (!buf)
        return -1;

    installSize = size;
    do {
        FSFILE_Read(fileHandle, &bytes_read, installOffset, buf, toRead);
        FSFILE_Write(ciaHandle, &bytes_written, installOffset, buf, toRead, FS_WRITE_FLUSH);
        installOffset += bytes_read;
    } while(installOffset < installSize);
    free(buf);

    ret = AM_FinishCiaInstall(ciaHandle);
    if (R_FAILED(ret))
        return ret;

    if (updatingSelf) {
        if (R_FAILED(ret = CIA_LaunchTitle(info.titleID, MEDIATYPE_SD)))
            return ret;
    }
    return 0;
}