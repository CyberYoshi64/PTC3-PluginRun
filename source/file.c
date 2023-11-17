#include "file.h"

FileArchiveHandles *archiveHandles;

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
}

Result archMount(FS_ArchiveID archID, FS_MediaType mediaType, u64 id, const char* prefix, u32 mode) {
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
Result archUnmount(const char* prefix) {
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
void archGetPath(FS_Archive *handle, FS_Path* path, const char* pathStr, u32* archType) {
    char* off = strchr(pathStr, ':');
    u32 prefix;
    FS_Path outPath;
    FileArchiveHandles *entry = archiveHandles;

    *handle = 0;
    if (archType) *archType = 0;

    if (!off) return;

    prefix = off - pathStr;
    outPath = fsMakePath(PATH_ASCII, off + 1);
    
    for (u32 i=0; i < FILEARCHIVEHANDLES_MAX; i++, entry++) {
        if (entry->type && strncmp(entry->name, pathStr, prefix)==0) {
            *handle = entry->archive;
            *path = outPath;
            if (archType) *archType = entry->type;
            return;
        }
    }
}

bool archDirExists(const char* path) {
    FS_Path fsPath; FS_Archive arch; u32 type;
    archGetPath(&arch, &fsPath, path, &type);
    if (!arch) return false;
    if (!(type & ARCHMODE_READ)) return false;
    
    Handle handle;
    if (R_SUCCEEDED(FSUSER_OpenDirectory(&handle, arch, fsPath))) {
        FSDIR_Close(handle);
        return true;
    }
    return false;
}

Result archDirCreate(const char* path) {
    FS_Path fsPath; FS_Archive arch; u32 type;
    archGetPath(&arch, &fsPath, path, &type);
    if (!arch) return 0xC8804478;

    if (!(type & ARCHMODE_WRITE)) return MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);
    return FSUSER_CreateDirectory(arch, fsPath, FS_ATTRIBUTE_DIRECTORY);
}

Result archDirDelete(const char* path) {
    FS_Path fsPath; FS_Archive arch; u32 type;
    archGetPath(&arch, &fsPath, path, &type);
    if (!arch) return 0xC8804478;

    if (!(type & ARCHMODE_WRITE)) return MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);
    return FSUSER_DeleteDirectory(arch, fsPath);
}

Result archDirDeleteRecursive(const char* path) {
    FS_Path fsPath; FS_Archive arch; u32 type;
    archGetPath(&arch, &fsPath, path, &type);
    if (!arch) return 0xC8804478;

    if (!(type & ARCHMODE_WRITE)) return MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);
    return FSUSER_DeleteDirectoryRecursively(arch, fsPath);
}

Result archDirRename(const char* path1, const char* path2) {
    FS_Path fsPath1; FS_Archive arch1; u32 type1;
    FS_Path fsPath2; FS_Archive arch2; u32 type2;

    archGetPath(&arch1, &fsPath1, path1, &type1);
    archGetPath(&arch2, &fsPath2, path2, &type2);

    if (!arch1) return 0xC8804478;
    if (!arch2) return 0xC8804478;

    if (!(type1 & ARCHMODE_WRITE)) return MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);
    if (!(type2 & ARCHMODE_WRITE)) return MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);

    return FSUSER_RenameDirectory(arch1, fsPath1, arch2, fsPath2);
}

Result archDirMove(const char* path1, const char* path2) {
    FS_Path fsPath1; FS_Archive arch1; u32 type1;
    FS_Path fsPath2; FS_Archive arch2; u32 type2;

    archGetPath(&arch1, &fsPath1, path1, &type1);
    archGetPath(&arch2, &fsPath2, path2, &type2);

    if (!arch1) return 0xC8804478;
    if (!arch2) return 0xC8804478;

    if (!(type1 & ARCHMODE_WRITE)) return MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);
    if (!(type2 & ARCHMODE_WRITE)) return MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);
    
    Handle handle;
    if (R_SUCCEEDED(FSUSER_OpenDirectory(&handle, arch2, fsPath2))) {
        FSDIR_Close(handle);
        FSUSER_DeleteDirectoryRecursively(arch2, fsPath2);
    }

    return FSUSER_RenameDirectory(arch1, fsPath1, arch2, fsPath2);
}

Result archDirOpen(Handle* handle, const char* path) {
    FS_Path fsPath; FS_Archive arch; u32 type;
    archGetPath(&arch, &fsPath, path, &type);
    if (!arch) return 0xC8804478;

    if (!(type & ARCHMODE_READ)) return MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);
    return FSUSER_OpenDirectory(handle, arch, fsPath);
}

bool archFileExists(const char* path) {
    FS_Path fsPath; FS_Archive arch; u32 type;
    archGetPath(&arch, &fsPath, path, &type);
    if (!arch) return false;
    if (!(type & ARCHMODE_READ)) return false;

    Handle handle;
    if (R_SUCCEEDED(FSUSER_OpenFile(&handle, arch, fsPath, FS_OPEN_READ, 0))) {
        FSFILE_Close(handle);
        return true;
    }
    return false;
}

Result archFileCreate(const char* path, u32 attr, u64 size) {
    FS_Path fsPath; FS_Archive arch; u32 type;
    archGetPath(&arch, &fsPath, path, &type);
    if (!arch) return 0xC8804478;

    if (!(type & ARCHMODE_WRITE)) return MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);
    return FSUSER_CreateFile(arch, fsPath, attr, size);
}

Result archFileDelete(const char* path) {
    FS_Path fsPath; FS_Archive arch; u32 type;
    archGetPath(&arch, &fsPath, path, &type);
    if (!arch) return 0xC8804478;

    if (!(type & ARCHMODE_WRITE)) return MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);
    return FSUSER_DeleteFile(arch, fsPath);
}

Result archFileRename(const char* path1, const char* path2) {
    FS_Path fsPath1; FS_Archive arch1; u32 type1;
    FS_Path fsPath2; FS_Archive arch2; u32 type2;

    archGetPath(&arch1, &fsPath1, path1, &type1);
    archGetPath(&arch2, &fsPath2, path2, &type2);

    if (!arch1) return 0xC8804478;
    if (!arch2) return 0xC8804478;

    if (!(type1 & ARCHMODE_WRITE)) return MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);
    if (!(type2 & ARCHMODE_WRITE)) return MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);

    return FSUSER_RenameFile(arch1, fsPath1, arch2, fsPath2);
}

Result archFileMove(const char* path1, const char* path2) {
    FS_Path fsPath1; FS_Archive arch1; u32 type1;
    FS_Path fsPath2; FS_Archive arch2; u32 type2;

    archGetPath(&arch1, &fsPath1, path1, &type1);
    archGetPath(&arch2, &fsPath2, path2, &type2);

    if (!arch1) return 0xC8804478;
    if (!arch2) return 0xC8804478;

    Handle handle;

    if (!(type1 & ARCHMODE_WRITE)) return MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);
    if (!(type2 & ARCHMODE_WRITE)) return MAKERESULT(RL_USAGE,RS_NOTSUPPORTED,RM_APPLICATION,RD_NOT_AUTHORIZED);
    
    if (R_SUCCEEDED(FSUSER_OpenFile(&handle, arch2, fsPath2, FS_OPEN_READ, 0))) {
        FSFILE_Close(handle);
        FSUSER_DeleteFile(arch2, fsPath2);
    }

    return FSUSER_RenameFile(arch1, fsPath1, arch2, fsPath2);
}

Result archFileOpen(Handle* handle, const char* path, u32 mode) {
    FS_Path fsPath; FS_Archive arch; u32 type;
    archGetPath(&arch, &fsPath, path, &type);
    if (!arch) return 0xC8804478;

    if (!mode) {
        mode =
            ((type & ARCHMODE_READ) ? FS_OPEN_READ : 0) |
            ((type & ARCHMODE_WRITE) ? FS_OPEN_WRITE : 0) |
            FS_OPEN_CREATE;
    }
    return FSUSER_OpenFile(handle, arch, fsPath, mode, 0);
}
