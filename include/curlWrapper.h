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

/**
 * @brief Start the app tasks thread and initialize cURL
 */
int appTaskInit(void);

/**
 * @brief End the app task thread and clean up cURL
 */
void appTaskExit(void);

// App tasks thread
void appTaskThread(void* arg);

extern char CURL_lastErrorCode[];   // Last error code string
extern u64 curl_progress_dltotal;   // Download total
extern u64 curl_progress_dlnow;     // Download current position
extern u64 curl_progress_ultotal;   // Upload total
extern u64 curl_progress_ulnow;     // Upload current position

float curlGetDownloadPercentage(void);
float curlGetUploadPercentage(void);
void curlGetDownloadState(u64* now, u64* total, float* perc);
void curlGetUploadState(u64* now, u64* total, float* perc);

/// Task creators

int appTask_DownloadData(const char* url, char** out);
int appTask_DownloadFile(const char* url, Handle fd);
int appTask_FormatSave(u64 titleID, FS_MediaType mediaType, u32 files, u32 dirs, bool dupData);
int appTask_FormatExtData(u64 titleID, FS_MediaType mediaType, u32 files, u32 dirs);
int appTask_CopyFile(const char* src, const char* dest, bool isDir);

/// Task handlers

int appTask_GetResult(u32 index);
int appTask_IsDone(u32 index);
u32 appTask_IsWaiting(void);
u32 appTask_PausedOn(void);
void appTask_Continue(void);
u32 appTask_GetCurrentTask(void);
int appTask_Clear(u32 index);