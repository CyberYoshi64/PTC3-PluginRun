#include "file.h"

FileArchiveHandles *archiveHandles;
char path_copy[0x400];
u16  path16[2][0x200];

Result __archError;

Result archInit() {
    archiveHandles = malloc(FILEARCHIVEHANDLES_SIZE * FILEARCHIVEHANDLES_MAX);
    if (!archiveHandles) return MAKERESULT(RL_FATAL, RS_OUTOFRESOURCE, RM_APPLICATION, RD_OUT_OF_MEMORY);
    return 0;
}

void archExit() {
    if (archiveHandles) {
        archUnmountAll();
        free(archiveHandles);
    }
    archiveHandles = NULL;
}

bool arch__isReadOnly(FS_ArchiveID id) {
    return
        (id == ARCHIVE_ROMFS) |
        (id == ARCHIVE_NAND_RO);
        //(id == ARCHIVE_EXTDATA); // iykyk
}

Result archMount(FS_ArchiveID archID, FS_MediaType mediaType, u64 id, const char *prefix, u32 mode) {
    Result res = MAKERESULT(RL_STATUS, RS_OUTOFRESOURCE, RM_APPLICATION, RD_OUT_OF_RANGE);
    FileArchiveHandles *entry = archiveHandles;
    for (u32 i=0; i < FILEARCHIVEHANDLES_MAX; i++, entry++) {
        if (!entry->type) {
            if (mode == ARCHMODE_VACANT) {
                if (arch__isReadOnly(archID))
                    entry->type = ARCHMODE_READ;
                else
                    entry->type = ARCHMODE_RW;
            } else {
                entry->type = ARCHMODE_RW;
                if (arch__isReadOnly(archID))
                    entry->type &= ~ARCHMODE_WRITE;
            }
            if (!(entry->type & ARCHMODE_RW)) {
                memset(entry, 0, sizeof(*entry));
                return MAKERESULT(
                    RL_USAGE,
                    RS_INVALIDARG,
                    RM_APPLICATION,
                    RD_INVALID_COMBINATION
                );
            }
            switch (archID) {
            case ARCHIVE_SDMC:
            case ARCHIVE_ROMFS:
            case ARCHIVE_SDMC_WRITE_ONLY:
            case ARCHIVE_NAND_CTR_FS:
            case ARCHIVE_NAND_RO:
            case ARCHIVE_NAND_RO_WRITE_ACCESS:
            case ARCHIVE_NAND_RW:
            case ARCHIVE_NAND_TWL_FS:
            case ARCHIVE_NAND_W_FS:
            case ARCHIVE_TWL_PHOTO:
            case ARCHIVE_TWL_SOUND:
                res = FSUSER_OpenArchive(
                    &entry->archive,
                    archID,
                    (FS_Path){PATH_EMPTY, 0, ""}
                );
                break;
            case ARCHIVE_SYSTEM_SAVEDATA:
                res = FSUSER_OpenArchive(
                    &entry->archive,
                    archID,
                    (FS_Path){PATH_BINARY, 8, (u32[]){mediaType, id}}
                );
                break;
            case ARCHIVE_EXTDATA:
                if (id & 0x800000000000LL)
                    res = FSUSER_OpenArchive(
                        &entry->archive,
                        archID,
                        (FS_Path){PATH_BINARY, 12, (u32[]){mediaType, id, id>>32}}
                    );
                else
                    res = FSUSER_OpenArchive(
                        &entry->archive,
                        archID,
                        (FS_Path){PATH_BINARY, 12, (u32[]){mediaType, (id>>8) & 0xFFFFF, 0}}
                    );
                break;
            default:
                res = FSUSER_OpenArchive(
                    &entry->archive,
                    archID,
                    (FS_Path){PATH_BINARY, 12, (u32[]){mediaType, id, (id>>32)}}
                );
                break;
            }
            if (R_FAILED(res)) {
                memset(entry, 0, sizeof(*entry));
                return res;
            }
            sprintf(archiveHandles[i].name, "%s", prefix);
            break;
        }
    }
    return res;
}

Result archUnmount(const char *prefix) {
    if (!prefix || !*prefix) return MAKERESULT(RL_USAGE, RS_INVALIDARG, RM_APPLICATION, RD_INVALID_COMBINATION);
    FileArchiveHandles *entry = archiveHandles;
    for (u32 i=0; i < FILEARCHIVEHANDLES_MAX; i++, entry++) {
        if (entry->type && strncmp(entry->name, prefix, 8)==0) {
            if (entry->type & ARCHMODE_WRITE)
                FSUSER_ControlArchive(entry->archive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
            FSUSER_CloseArchive(entry->archive);
            memset(entry, 0, sizeof(*entry));
            return 0;
        }
    }
    return 0xC8804478;
}

void archUnmountAll() {
    FileArchiveHandles *entry = archiveHandles;
    for (u32 i=0; i < FILEARCHIVEHANDLES_MAX; i++, entry++) {
        if (entry->type) {
            archUnmount(entry->name);
        }
    }
}

void archGetPath(FS_Archive *handle, FS_Path *path, const char *pathStr, u32 *archType) {
    *handle = 0;
    *path = (FS_Path){PATH_EMPTY,0,""};
    if (archType) *archType = 0;

    char *off = strchr(pathStr, ':');
    if (!off) return;

    u32 prefix = off - pathStr;
    *path = fsMakePath(PATH_ASCII, off + 1);
    
    FileArchiveHandles *entry = archiveHandles;
    for (u32 i=0; i < FILEARCHIVEHANDLES_MAX; i++, entry++) {
        if (entry->type && strncmp(entry->name, pathStr, prefix)==0) {
            *handle = entry->archive;
            if (archType) *archType = entry->type;
            return;
        }
    }
}

FS_PathUTF8 *fsMakePath8(const char *path, u32 expectedType) {
    FS_Path fsPath; FS_Archive arch; u32 type;
    archGetPath(&arch, &fsPath, path, &type);
    if (!arch) {
        __archError = 0xDEADBEEF;
        return NULL;
    }
    if (!(type & expectedType)) {
        __archError = MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);
        return NULL;
    }
    FS_PathUTF8 *p = malloc(sizeof(FS_PathUTF8));
    if (!p) {
        __archError = MAKERESULT(RL_FATAL,RS_OUTOFRESOURCE,RM_APPLICATION,RD_OUT_OF_MEMORY);
        return NULL;
    }
    p->buf = malloc(strlen(path)*3);
    if (!p->buf) {
        __archError = MAKERESULT(RL_FATAL,RS_OUTOFRESOURCE,RM_APPLICATION,RD_OUT_OF_MEMORY);
        free(p);
        return NULL;
    }
    memset(p->buf, 0, strlen(path)*3);
    u32 units = utf8_to_utf16(p->buf, fsPath.data, strlen(path)) + 1;
    p->path = (FS_Path){PATH_UTF16, units * 2, p->buf};
    p->arch = arch;
    p->type = type;
    return p;
}

void fsFreePath8(FS_PathUTF8 *self) {
    if (!self) return;
    if (self->buf) free(self->buf);
    free(self);
}

bool archDirExists(const char *path) {
    FS_PathUTF8 *fsPath = fsMakePath8(path, ARCHMODE_READ);
    if (!fsPath) return __archError;
    
    Handle handle;
    if (R_SUCCEEDED(FSUSER_OpenDirectory(&handle, fsPath->arch, fsPath->path))) {
        FSDIR_Close(handle);
        fsFreePath8(fsPath);
        return true;
    }
    fsFreePath8(fsPath);
    return false;
}

Result archDirCreate(const char *path) {
    FS_PathUTF8 *fsPath = fsMakePath8(path, ARCHMODE_WRITE);
    if (!fsPath) return __archError;
    Result res = FSUSER_CreateDirectory(fsPath->arch, fsPath->path, FS_ATTRIBUTE_DIRECTORY);
    fsFreePath8(fsPath);
    return res;
}

Result archDirCreateRecursive(const char *path, bool forFile) {
    FS_PathUTF8 *fsPath = fsMakePath8(path, ARCHMODE_WRITE);
    if (!fsPath) return __archError;

    Result res = 0;
    FS_Path fsPathTrim;
    fsPathTrim.type = PATH_UTF16;
    fsPathTrim.data = path16[0];

    strncpy(path_copy, path, 0x3FF);

    char *colon = strchr(path_copy, ':'); u32 len;
    int slashFound = (colon - path_copy) + 2; char *cres;
    while (true) {
        cres = strchr(path_copy + slashFound, '/');
        if (!cres) break;
        slashFound = (cres - path_copy);
        path_copy[slashFound] = 0;
        memset(path16[0], 0, 0x400);
        len = utf8_to_utf16(path16[0], (u8*)colon+1, 0x200) + 1;
        path_copy[slashFound++] = '/';
        fsPathTrim.size = len * 2;
        if R_FAILED(res = FSUSER_CreateDirectory(fsPath->arch, fsPathTrim, 0)) {
            if (res != 0xC82044BE) break; // Exists already? No problem!
            res = 0;
        }
    }
    if (path_copy[strlen(path_copy)-1]!='/' && !forFile) {
        if R_FAILED(res = FSUSER_CreateDirectory(fsPath->arch, fsPath->path, 0))
            if (res == 0xC82044BE) res = 0; // Exists already? That's fine!
    }
    fsFreePath8(fsPath);
    return res;
}

Result archDirDelete(const char *path) {
    FS_PathUTF8 *fsPath = fsMakePath8(path, ARCHMODE_WRITE);
    if (!fsPath) return __archError;
    Result res = FSUSER_DeleteDirectory(fsPath->arch, fsPath->path);
    fsFreePath8(fsPath);
    return res;
}

Result archDirDeleteRecursive(const char *path) {
    FS_PathUTF8 *fsPath = fsMakePath8(path, ARCHMODE_WRITE);
    if (!fsPath) return __archError;
    Result res = FSUSER_DeleteDirectoryRecursively(fsPath->arch, fsPath->path);
    fsFreePath8(fsPath);
    return res;
}

Result archDirRename(const char *path1, const char *path2) {
    FS_PathUTF8 *fsPath1 = NULL;
    FS_PathUTF8 *fsPath2 = NULL;
    Result res;

    if (!(fsPath1 = fsMakePath8(path1, ARCHMODE_WRITE))) {
        res = __archError;
        goto exit;
    }
    if (!(fsPath2 = fsMakePath8(path2, ARCHMODE_WRITE))) {
        res = __archError;
        goto exit;
    }
    
    res = FSUSER_RenameDirectory(fsPath1->arch, fsPath1->path, fsPath2->arch, fsPath2->path);
exit:
    fsFreePath8(fsPath1);
    fsFreePath8(fsPath2);
    return res;
}

Result archDirMove(const char *path1, const char *path2) {
    FS_PathUTF8 *fsPath1 = NULL;
    FS_PathUTF8 *fsPath2 = NULL;
    Result res;

    if (!(fsPath1 = fsMakePath8(path1, ARCHMODE_WRITE))) {
        res = __archError;
        goto exit;
    }
    if (!(fsPath2 = fsMakePath8(path2, ARCHMODE_WRITE))) {
        res = __archError;
        goto exit;
    }
    
    Handle handle;
    if (R_SUCCEEDED(FSUSER_OpenDirectory(&handle, fsPath2->arch, fsPath2->path))) {
        FSDIR_Close(handle);
        FSUSER_DeleteDirectoryRecursively(fsPath2->arch, fsPath2->path);
    }

    res = FSUSER_RenameDirectory(fsPath1->arch, fsPath1->path, fsPath2->arch, fsPath2->path);
exit:
    fsFreePath8(fsPath1);
    fsFreePath8(fsPath2);
    return res;
}

Result archDirOpen(Handle *handle, const char *path) {
    FS_PathUTF8 *fsPath = fsMakePath8(path, ARCHMODE_READ);
    if (!fsPath) return __archError;
    Result res = FSUSER_OpenDirectory(handle, fsPath->arch, fsPath->path);
    fsFreePath8(fsPath);
    return res;
}

bool archFileExists(const char *path) {
    FS_PathUTF8 *fsPath = fsMakePath8(path, ARCHMODE_READ);
    if (!fsPath) return false;

    Handle handle;
    if (R_SUCCEEDED(FSUSER_OpenFile(&handle, fsPath->arch, fsPath->path, FS_OPEN_READ, 0))) {
        FSFILE_Close(handle);
        fsFreePath8(fsPath);
        return true;
    }
    fsFreePath8(fsPath);
    return false;
}

Result archFileCreate(const char *path, u32 attr, u64 size) {
    FS_PathUTF8 *fsPath = fsMakePath8(path, ARCHMODE_READ);
    if (!fsPath) return __archError;
    Result res = FSUSER_CreateFile(fsPath->arch, fsPath->path, attr, size);
    fsFreePath8(fsPath);
    return res;
}

Result archFileCreateRecursive(const char *path, u32 attr, u64 size) {
    FS_PathUTF8 *fsPath = fsMakePath8(path, ARCHMODE_WRITE);
    if (!fsPath) return __archError;

    Result res = 0;
    FS_Path fsPathTrim;
    fsPathTrim.type = PATH_UTF16;
    fsPathTrim.data = path16[1];

    strncpy(path_copy, path, 0x3FF);

    char *colon = strchr(path_copy, ':'); u32 len;
    int slashFound = (colon - path_copy) + 2; char *cres;
    while (true) {
        cres = strchr(path_copy + slashFound, '/');
        if (!cres) break;
        slashFound = (cres - path_copy);
        path_copy[slashFound] = 0;
        memset(path16[1], 0, 0x400);
        len = utf8_to_utf16(path16[1], (u8*)colon+1, 0x200) + 1;
        path_copy[slashFound++] = '/';
        fsPathTrim.size = len * 2;
        if R_FAILED(res = FSUSER_CreateDirectory(fsPath->arch, fsPathTrim, 0)) {
            if (res != 0xC82044BE) break; // Exists already? No problem!
            res = 0;
        }
    }
    if (path_copy[strlen(path_copy)-1]=='/')
        res = MAKERESULT(RL_USAGE,RS_INVALIDARG,RM_APPLICATION,RD_NO_DATA);
    if R_SUCCEEDED(res)
        res = FSUSER_CreateFile(fsPath->arch, fsPath->path, attr, size);
    fsFreePath8(fsPath);
    return res;
}

Result archFileDelete(const char *path) {
    FS_PathUTF8 *fsPath = fsMakePath8(path, ARCHMODE_READ);
    if (!fsPath) return __archError;
    Result res = FSUSER_DeleteFile(fsPath->arch, fsPath->path);
    fsFreePath8(fsPath);
    return res;
}

Result archFileRename(const char *path1, const char *path2) {
    FS_PathUTF8 *fsPath1 = NULL;
    FS_PathUTF8 *fsPath2 = NULL;
    Result res;

    if (!(fsPath1 = fsMakePath8(path1, ARCHMODE_WRITE))) {
        res = __archError;
        goto exit;
    }
    if (!(fsPath2 = fsMakePath8(path2, ARCHMODE_WRITE))) {
        res = __archError;
        goto exit;
    }
    res = FSUSER_RenameFile(fsPath1->arch, fsPath1->path, fsPath2->arch, fsPath2->path);
exit:
    fsFreePath8(fsPath1);
    fsFreePath8(fsPath2);
    return res;
}

Result archFileMove(const char *path1, const char *path2) {
    FS_PathUTF8 *fsPath1 = NULL;
    FS_PathUTF8 *fsPath2 = NULL;
    Result res;

    if (!(fsPath1 = fsMakePath8(path1, ARCHMODE_WRITE))) {
        res = __archError;
        goto exit;
    }
    if (!(fsPath2 = fsMakePath8(path2, ARCHMODE_WRITE))) {
        res = __archError;
        goto exit;
    }
    
    Handle handle;
    if (R_SUCCEEDED(FSUSER_OpenFile(&handle, fsPath2->arch, fsPath2->path, FS_OPEN_READ, 0))) {
        FSFILE_Close(handle);
        FSUSER_DeleteFile(fsPath2->arch, fsPath2->path);
    }
    res = FSUSER_RenameFile(fsPath1->arch, fsPath1->path, fsPath2->arch, fsPath2->path);
exit:
    fsFreePath8(fsPath1);
    fsFreePath8(fsPath2);
    return res;
}

Result archFileOpen(Handle *handle, const char *path, u32 mode) {
    FS_PathUTF8 *fsPath = fsMakePath8(path, ARCHMODE_RW);
    if (!fsPath) return __archError;

    if (!mode) {
        mode = FS_OPEN_CREATE |
            ((fsPath->type & ARCHMODE_READ) ? FS_OPEN_READ : 0) |
            ((fsPath->type & ARCHMODE_WRITE) ? FS_OPEN_WRITE : 0);
    } else {
        if (!(fsPath->type & ARCHMODE_READ)) mode &= ~FS_OPEN_READ;
        if (!(fsPath->type & ARCHMODE_WRITE)) mode &= ~FS_OPEN_WRITE;
    }

    Result res = FSUSER_OpenFile(handle, fsPath->arch, fsPath->path, mode, 0);
    fsFreePath8(fsPath);
    return res;
}
