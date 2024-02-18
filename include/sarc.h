#pragma once

#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define SARC_DEFAULT_KEY    101

typedef struct SFATEntry_s {
    u32 name;
    u32 sfntOff;
    u32 dataBeg;
    u32 length;  // dataEnd
} CTR_PACKED SFATEntry;

typedef struct SFAT_s {
    u32 signature;
    u16 headerSize;
    u16 nodeCount;
    u32 multiplier;
    SFATEntry* entries;
} CTR_PACKED SFAT;

typedef struct SFNT_s {
    u32 signature;
    u16 headerSize;
    u16 reserved;
    u32* data;
} CTR_PACKED SFNT;

typedef struct SARC_s {
    u32 signature;
    u16 headerSize;
    u16 bom;
    u32 fileSize;
    u32 dataOff;
    u16 version;
    u16 reserved;
    SFAT* sfat;
    SFNT* sfnt;
} CTR_PACKED SARC;

typedef struct SARCWrapper_s {
    SARC* sarc;
    Handle fileHdl;
} SARCWrapper;

SARCWrapper* sarcOpen(Handle handle);
u32 __sarcGetIndex(SFAT *sfat, SFATEntry* out, u32 hash);
Handle sarcOpenFile(SARCWrapper* sarc, char* name);
Handle sarcOpenFileH(SARCWrapper* sarc, u32 hash);
u32 sarcHash(char* name, u32 multiplier);