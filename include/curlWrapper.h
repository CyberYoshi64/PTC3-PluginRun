#pragma once

#include "main.h"

enum CURLTaskType {
    APPTASK_VACANT = 0,
    APPTASK_DOWN_FILE,
    APPTASK_DOWN_RAW,
    APPTASK_FORMAT_SAVE,
    APPTASK_FORMAT_EXTDATA,
    APPTASK_COPY_FILE,
};

typedef struct {
    u32     type;
    int     done;
    int     rc;
    char    url[256];
    union {
        char**  downData;
        Handle  file;
        struct {
            char    dest[256];
            bool    isDir;
        } fileC;
        struct {
            u64             titleID;
            FS_MediaType    mediaType;
            u32             files;
            u32             dirs;
            bool            dupData;
        } format;
    } data;
} CTR_ALIGN(256) AppTask;

#define APPTASK_SIZE           sizeof(AppTask)
#define APPTASKS_MAX           32
#define APPTASK_STRUCTSIZE     APPTASKS_MAX * APPTASK_SIZE

int appTaskInit(void);
void appTaskExit(void);
void appTaskThread(void* arg);

extern char CURL_lastErrorCode[];
extern u64 curl_progress_dltotal;
extern u64 curl_progress_dlnow;
extern u64 curl_progress_ultotal;
extern u64 curl_progress_ulnow;

int downloadFile(const char* URL, Handle fd);
int downloadString(const char* URL, char** out);

float curlGetDownloadPercentage(void);
float curlGetUploadPercentage(void);
void curlGetDownloadState(u64* now, u64* total, float* perc);
void curlGetUploadState(u64* now, u64* total, float* perc);

int appTask_DownloadData(const char* url, char** out);
int appTask_DownloadFile(const char* url, Handle fd);
int appTask_FormatSave(u64 titleID, FS_MediaType mediaType, u32 files, u32 dirs, bool dupData);
int appTask_FormatExtData(u64 titleID, FS_MediaType mediaType, u32 files, u32 dirs);
int appTask_CopyFile(const char* src, const char* dest, bool isDir);

int appTask_GetResult(u32 index);
int appTask_IsDone(u32 index);
u32 appTask_IsWaiting(void);
u32 appTask_PausedOn(void);
void appTask_Continue(void);
u32 appTask_GetCurrentTask(void);
int appTask_Clear(u32 index);