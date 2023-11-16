#pragma once
#include "main.h"

enum FileArchiveMode {
    ARCHMODE_VACANT     = 0,        // State: Vacant | Mount value: Default
    ARCHMODE_READ       = BIT( 0),  // Read
    ARCHMODE_WRITE      = BIT( 1),  // Write
    ARCHMODE_RW         = ARCHMODE_READ|ARCHMODE_WRITE, // Read & Write
};

typedef struct {
    FS_Archive  archive;
    char        name[20];
    u32         type;
} FileArchiveHandles;

#define FILEARCHIVEHANDLES_MAX  16
#define FILEARCHIVEHANDLES_SIZE sizeof(FileArchiveHandles)

extern FileArchiveHandles *archiveHandles;

Result  archInit();
void    archExit();
Result  archMount(FS_ArchiveID archID, FS_MediaType mediaType, u64 id, const char* prefix, u32 mask);
Result  archUnmount(const char* prefix);
void    archUnmountAll(void);
