#include "sarc.h"

u32 sarcHash(char* name, u32 multiplier) {
    if (!name) return 0;
    u32 res = 0, len = strlen(name);
    for (u32 i = 0; i < len; i++) {
        res = res * multiplier + name[i];
    }
    return res;
}
SARCWrapper* sarcOpen(Handle handle) {
    SARCWrapper* wrapper = malloc(sizeof(SARCWrapper));
    if (!wrapper) return wrapper;
    wrapper->fileHdl = handle;
    wrapper->sarc = memalign(sizeof(SARC), 0x100);
    if (!wrapper->sarc) goto fail1;
    u32 bytesRead, size; u64 off = 0; void* newptr;
    FSFILE_Read(handle, &bytesRead, 0, wrapper->sarc, sizeof(SARC));
    if (wrapper->sarc->bom != 0xFEFF) goto fail2;
    if (wrapper->sarc->signature != *(u32*)"SARC") goto fail2;
    size = wrapper->sarc->dataOff;
    newptr = realloc(wrapper->sarc, size);
    if (!newptr) goto fail2;
    wrapper->sarc = newptr;
    while (off < size) {
        FSFILE_Read(handle, &bytesRead, off, (u8*)wrapper->sarc + off, size);
        off += bytesRead;
    }
    wrapper->sarc->sfat = (SFAT*)(u8*)wrapper->sarc + wrapper->sarc->headerSize;
    if (wrapper->sarc->sfat->signature != *(u32*)"SFAT") goto fail2;
    wrapper->sarc->sfnt = (SFNT*)(u8*)wrapper->sarc + wrapper->sarc->headerSize + wrapper->sarc->sfat->headerSize + wrapper->sarc->sfat->nodeCount * sizeof(SFATEntry);
    if (wrapper->sarc->sfnt->signature != *(u32*)"SFNT") goto fail2;
    wrapper->sarc->sfat->entries = (SFATEntry*)(u8*)wrapper->sarc + wrapper->sarc->headerSize + wrapper->sarc->sfat->headerSize;
    for (int i = 0; i < wrapper->sarc->sfat->nodeCount; i++) {
        wrapper->sarc->sfat->entries[i].length -= wrapper->sarc->sfat->entries[i].dataBeg;
        wrapper->sarc->sfat->entries[i].dataBeg += wrapper->sarc->dataOff;
    }
    return wrapper;
fail2:
    free(wrapper->sarc);
fail1:
    free(wrapper);
    return NULL;
}
u32 __sarcGetIndex(SFAT *sfat, SFATEntry* out, u32 hash) {
    u32 index = sfat->nodeCount / 2, step = index;
    out = NULL;
    while (true) {
        if (hash == sfat->entries[index].name)
            break;
        else if (hash < sfat->entries[index].name)
            index -= step;
        else
            index += step;
        
        if (step > 2) step /= 2;
        if (index > sfat->nodeCount) return 0;
    }
    out = sfat->entries + index;
    return 1;
}
Handle sarcOpenFile(SARCWrapper* sarc, char* name) {
    if (!sarc || !sarc->sarc || !sarc->fileHdl) return 0;
    SFATEntry* sft = NULL; Handle out;
    if (__sarcGetIndex(sarc->sarc->sfat, sft, sarcHash(name, sarc->sarc->sfat->multiplier))) {
        if R_SUCCEEDED(FSFILE_OpenSubFile(sarc->fileHdl, &out, sft->dataBeg, sft->length)) {
            return out;
        }
    }
    return 0;
}
Handle sarcOpenFileH(SARCWrapper* sarc, u32 hash) {
    if (!sarc || !sarc->sarc || !sarc->fileHdl) return 0;
    SFATEntry* sft = NULL; Handle out;
    if (__sarcGetIndex(sarc->sarc->sfat, sft, hash)) {
        if R_SUCCEEDED(FSFILE_OpenSubFile(sarc->fileHdl, &out, sft->dataBeg, sft->length)) {
            return out;
        }
    }
    return 0;
}