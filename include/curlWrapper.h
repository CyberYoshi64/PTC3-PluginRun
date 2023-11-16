#pragma once

#include "main.h"

enum CURLTaskType {
    CURLTASK_VACANT = 0,
    CURLTASK_DOWN_FILE,
    CURLTASK_DOWN_RAW,
};

typedef struct {
    u32 type;
    int done;
    int rc;
    char url[256];
    union {
        char** downData;
        Handle file;
    } data;
} CTR_ALIGN(512) CURLTask;

#define CURLTASK_SIZE           sizeof(CURLTask)
#define CURLTASKS_MAX           32
#define CURLTASK_STRUCTSIZE     CURLTASKS_MAX * CURLTASK_SIZE

int curlInit(void);
void curlExit(void);
void curlTaskThread(void* arg);

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

int curlTask_DownloadData(const char* url, char** out);
int curlTask_DownloadFile(const char* url, Handle fd);
int curlTask_GetResult(u32 index);
int curlTask_IsDone(u32 index);
u32 curlTask_IsWaiting(void);
u32 curlTask_PausedOn(void);
void curlTask_Continue(void);
u32 curlTask_GetCurrentTask(void);
int curlTask_Clear(u32 index);