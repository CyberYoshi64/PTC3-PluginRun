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
            if (R_FAILED(
                res = FSUSER_OpenArchive(
                    &entry->archive,
                    archID,
                    (FS_Path){PATH_BINARY, 12, (u32[]){mediaType, id, (id>>32)}}
                ))) {
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