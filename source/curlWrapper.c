#include "curlWrapper.h"
#include <curl/curl.h>
#include <curl/easy.h>

#define APP_USERAGENT   "Mozilla/5.0 (Nintendo 3DS; U; ; en) AppleWebKit/536.30 (KHTML, like Gecko) CTGP-7/1.0"
#define FILE_ALLOC_SIZE 0x20000

static Thread taskThread;
static AppTask *app_tasks = NULL;
static vu32 app_tasks_current = 0;
static vu64 app_tasks_mask = 0;
static vu32 app_tasks_pause = 0;
static vu32 app_tasks_pausedOn = 0;
u64 curl_progress_dltotal = 0;
u64 curl_progress_dlnow = 0;
u64 curl_progress_ultotal = 0;
u64 curl_progress_ulnow = 0;

int appTaskInit() {
    app_tasks = malloc(APPTASK_STRUCTSIZE);
    if (!app_tasks) {
        drawError("cURL could not be initialized.\n\nPress any key to continue.", true, KEY_ABXYSS);
    } else
        taskThread = threadCreate(appTaskThread, NULL, 0x400000, 0x18, -1, false);
    return curl_global_init(CURL_GLOBAL_ALL);
}

void appTaskExit() {
    if (app_tasks) free(app_tasks);
    if (taskThread) {
        threadJoin(taskThread, U64_MAX);
        threadFree(taskThread);
        taskThread = 0;
    }
    app_tasks = NULL;
    curl_global_cleanup();
}

float curlGetDownloadPercentage() {
	return curl_progress_dlnow / (float)(curl_progress_dltotal ? curl_progress_dltotal : 1);
}
float curlGetUploadPercentage() {
	return curl_progress_ulnow / (float)(curl_progress_ultotal ? curl_progress_ultotal : 1);
}
void curlGetDownloadState(u64* now, u64* total, float* perc) {
	if (now) *now = curl_progress_dlnow;
	if (total) *total = curl_progress_dltotal;
	if (perc) *perc = curlGetDownloadPercentage();
}
void curlGetUploadState(u64* now, u64* total, float* perc) {
	if (now) *now = curl_progress_ulnow;
	if (total) *total = curl_progress_ultotal;
	if (perc) *perc = curlGetUploadPercentage();
}

static volatile char *str_result_buf = NULL;
static volatile size_t str_result_sz = 0;
static volatile size_t str_result_written = 0;
static u32 retryingFromError = 0;
static u32 dataPosition = 0;
char CURL_lastErrorCode[CURL_ERROR_SIZE];
Handle downfile = 0;

int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
	curl_progress_dltotal = dltotal;
    curl_progress_dlnow = dlnow;
    curl_progress_ultotal = ultotal;
    curl_progress_ulnow = ulnow;
    if (!aptShouldClose()) return 0;
	else return 1;
};

void curlSetCommonOptions(CURL *hnd, const char* url) {
	curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, FILE_ALLOC_SIZE);
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, APP_USERAGENT);
	curl_easy_setopt(hnd, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(hnd, CURLOPT_FAILONERROR, 1L);
	curl_easy_setopt(hnd, CURLOPT_ACCEPT_ENCODING, "gzip");
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(hnd, CURLOPT_XFERINFOFUNCTION, progress_callback);
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(hnd, CURLOPT_TRANSFER_ENCODING, 1L);
	curl_easy_setopt(hnd, CURLOPT_ERRORBUFFER, CURL_lastErrorCode);
	curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(hnd, CURLOPT_STDERR, stdout);

}

static size_t handle_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
	(void)userdata;
	const size_t bsz = size * nmemb;
	
	bool needrealloc = false;
	while (bsz + str_result_written >= str_result_sz) {
		str_result_sz <<= 1;
		needrealloc = true;
	}
	if (needrealloc) {
		str_result_buf = realloc((void*)str_result_buf, str_result_sz);
		if (!str_result_buf) {
			return 0;
		}
	}
	if (!str_result_buf) return 0;
	memcpy((void*)(str_result_buf + str_result_written), ptr, bsz);
	str_result_written += bsz;
	return bsz;
}

static size_t file_buffer_pos = 0;
static size_t file_toCommit_size = 0;
static char* g_buffers[2] = { NULL };
static u8 g_index = 0;
static Thread fsCommitThread;
static LightEvent readyToCommit;
static LightEvent waitCommit;
static bool killThread = false;
static bool writeError = false;

bool filecommit() {
	if (!downfile) return false;
	if (!file_toCommit_size) return true;
	u64 fileSize; u32 byteswritten;
    FSFILE_GetSize(downfile, &fileSize);
    FSFILE_Write(downfile, &byteswritten, fileSize, g_buffers[!g_index], file_toCommit_size, FS_WRITE_FLUSH);
	if (byteswritten != file_toCommit_size) return false;
	file_toCommit_size = 0;
	return true;
}

static void commitToFileThreadFunc(void* args) {
	LightEvent_Signal(&waitCommit);
	while (true) {
		LightEvent_Wait(&readyToCommit);
		LightEvent_Clear(&readyToCommit);
		if (killThread) threadExit(0);
		writeError = !filecommit();
		LightEvent_Signal(&waitCommit);
	}
}

static size_t file_handle_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
	(void)userdata;
	const size_t bsz = size * nmemb;
	size_t tofill = 0;
	if (writeError) return 0;
	if (!g_buffers[g_index]) {

		LightEvent_Init(&waitCommit, RESET_STICKY);
		LightEvent_Init(&readyToCommit, RESET_STICKY);

		fsCommitThread = threadCreate(commitToFileThreadFunc, NULL, 0x8000, 0x18, -2, true);

		g_buffers[0] = memalign(0x1000, FILE_ALLOC_SIZE);
		g_buffers[1] = memalign(0x1000, FILE_ALLOC_SIZE);

		if (!fsCommitThread || !g_buffers[0] || !g_buffers[1]) return 0;
	}
	if (file_buffer_pos + bsz >= FILE_ALLOC_SIZE) {
		tofill = FILE_ALLOC_SIZE - file_buffer_pos;
		memcpy_ctr(g_buffers[g_index] + file_buffer_pos, ptr, tofill);
		
		LightEvent_Wait(&waitCommit);
		LightEvent_Clear(&waitCommit);
		file_toCommit_size = file_buffer_pos + tofill;
		file_buffer_pos = 0;
		svcFlushProcessDataCache(CUR_PROCESS_HANDLE, (u32)g_buffers[g_index], file_toCommit_size);
		g_index = !g_index;
		LightEvent_Signal(&readyToCommit);
	}
	memcpy_ctr(g_buffers[g_index] + file_buffer_pos, ptr + tofill, bsz - tofill);
	file_buffer_pos += bsz - tofill;
	return bsz;
}

int downloadFile(const char* URL, Handle fd) {
	int retcode = 0;
	aptSetHomeAllowed(false);
	aptSetSleepAllowed(false);
	
	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf) {
		sprintf(CURL_lastErrorCode, "Failed to allocate memory.");
		retcode = 1;
		goto exit;
	}
	
    u64 fileOgSize;
    downfile = fd;
	if (R_FAILED(FSFILE_GetSize(fd, &fileOgSize))) {
		sprintf(CURL_lastErrorCode, "Bad file handle");
		retcode = 4;
		goto exit;
	}

	CURL_lastErrorCode[0] = 0;

	while (true) {
		int res = socInit(socubuf, 0x100000);
		CURLcode cres;
		if (R_FAILED(res)) {
			sprintf(CURL_lastErrorCode, "socInit returned: 0x%08X", res);
			cres = 0xFF;
		}
		else {
			CURL* hnd = curl_easy_init();
			curlSetCommonOptions(hnd, URL);
			if (retryingFromError) {
				u64 gotSize;
                FSFILE_GetSize(downfile, &gotSize);
				gotSize -= fileOgSize;
				if (gotSize) {
					dataPosition = gotSize;
					char tmpRange[0x20];
					sprintf(tmpRange, "%lld-", gotSize);
					curl_easy_setopt(hnd, CURLOPT_RANGE, tmpRange);
				}
			}
			curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, file_handle_data);

			cres = curl_easy_perform(hnd);
			curl_easy_cleanup(hnd);
		}		
		if (cres != CURLE_OK) {
			if (retryingFromError < 30 && !aptShouldClose()) {
				Sleep(.5);
				retryingFromError++;
			} else {
				retryingFromError = 0;
				retcode = cres;
				goto exit;
			}
		}
		else retryingFromError = 0;

		if (fsCommitThread) {
			LightEvent_Wait(&waitCommit);
			LightEvent_Clear(&waitCommit);
		}
		
		if (cres == CURLE_OK) {
			file_toCommit_size = file_buffer_pos;
			svcFlushProcessDataCache(CUR_PROCESS_HANDLE, (u32)g_buffers[g_index], file_toCommit_size);
			g_index = !g_index;
			if (!filecommit()) {
				sprintf(CURL_lastErrorCode, "Couldn't commit to file.");
				retcode = 2;
				goto exit;
			}
		}

	exit:
		if (fsCommitThread) {
			killThread = true;
			LightEvent_Signal(&readyToCommit);
			threadJoin(fsCommitThread, U64_MAX);
			killThread = false;
			fsCommitThread = NULL;
		}

		socExit();

		if (!retryingFromError) {
			if (socubuf)
				free(socubuf);
			
			if (downfile) {
				FSFILE_Flush(downfile);
                downfile = 0;
			}
		}

		if (g_buffers[0]) {
			free(g_buffers[0]);
			g_buffers[0] = NULL;
		}
		if (g_buffers[1]) {
			free(g_buffers[1]);
			g_buffers[1] = NULL;
		}
		file_buffer_pos = 0;
		file_toCommit_size = 0;
		writeError = false;
		g_index = 0;

		if (!retryingFromError)
			return retcode;
		else Sleep(1);
	}
	aptSetHomeAllowed(true);
	aptSetSleepAllowed(true);
}

int downloadString(const char* URL, char** out) {

	aptSetHomeAllowed(false);
	aptSetSleepAllowed(false);

	*out = NULL;
	int retcode = 0;
	CURL_lastErrorCode[0] = 0;

	void *socubuf = memalign(0x1000, 0x100000);
	if (!socubuf) {
		sprintf(CURL_lastErrorCode, "Failed to allocate memory.");
		retcode = 1;
		goto exit;
	}
	int res = socInit(socubuf, 0x100000);
	if (R_FAILED(res)) {
		sprintf(CURL_lastErrorCode, "socInit returned: 0x%08X", res);
		retcode = 2;
		goto exit;
	}
	str_result_sz = 1 << 9;
	str_result_buf = memalign(0x1000, 1 << 9);
	if (!str_result_buf) {
		sprintf(CURL_lastErrorCode, "Failed to allocate memory.");
		retcode = 1;
		goto exit;
	}

	CURL *hnd = curl_easy_init();
	curlSetCommonOptions(hnd, URL);
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, handle_data);

	CURLcode cres = curl_easy_perform(hnd);
	curl_easy_cleanup(hnd);

	if (cres != CURLE_OK) {
		retcode = cres;
		goto exit;
	}

	if (str_result_buf)
		str_result_buf[str_result_written] = '\0';
	*out = (void*)str_result_buf;

exit:
	socExit();

	if (socubuf)
		free(socubuf);

	str_result_buf = NULL;
	str_result_written = 0;
	str_result_sz = 0;

	aptSetHomeAllowed(true);
	aptSetSleepAllowed(true);

	return retcode;
}

int appTask__GetVacant() {
    if (!app_tasks) return -1;
    for (u32 i = 0; i < APPTASKS_MAX; i++) {
        if (app_tasks[i].type == APPTASK_VACANT) return i;
    }
    return -1;
}

int appTask_DownloadData(const char* url, char** out) {
    int taskIndex = appTask__GetVacant();
    if (taskIndex < 0) return taskIndex;
    AppTask* task = app_tasks + taskIndex;
	strncpy(task->url, url, 255);
    task->data.downData = out;
    task->done = 0;
    task->type = APPTASK_DOWN_RAW;
	app_tasks_mask |= BIT(taskIndex);
	app_tasks_pause = 0;
    return taskIndex;
}

int appTask_DownloadFile(const char* url, Handle fd) {
    int taskIndex = appTask__GetVacant();
    if (taskIndex < 0) return taskIndex;
    AppTask* task = app_tasks + taskIndex;
	strncpy(task->url, url, 255);
    task->data.file = fd;
    task->done = 0;
    task->type = APPTASK_DOWN_FILE;
	app_tasks_mask |= BIT(taskIndex);
	app_tasks_pause = 0;
    return taskIndex;
}

int appTask_FormatSave(u64 titleID, FS_MediaType mediaType, u32 files, u32 dirs, bool dupData) {
	int taskIndex = appTask__GetVacant();
	if (taskIndex < 0) return taskIndex;
	AppTask *task = app_tasks + taskIndex;
	task->data.format.titleID = titleID;
	task->data.format.mediaType = mediaType;
	task->data.format.files = files;
	task->data.format.dirs = dirs;
	task->data.format.dupData = dupData;
	task->done = 0;
	task->type = APPTASK_FORMAT_SAVE;
	app_tasks_mask |= BIT(taskIndex);
	app_tasks_pause = 0;
	return taskIndex;
}

int appTask_FormatExtData(u64 titleID, FS_MediaType mediaType, u32 files, u32 dirs) {
	int taskIndex = appTask__GetVacant();
	if (taskIndex < 0) return taskIndex;
	AppTask *task = app_tasks + taskIndex;
	task->data.format.titleID = titleID;
	task->data.format.mediaType = mediaType;
	task->data.format.files = files;
	task->data.format.dirs = dirs;
	task->done = 0;
	task->type = APPTASK_FORMAT_EXTDATA;
	app_tasks_mask |= BIT(taskIndex);
	app_tasks_pause = 0;
	return taskIndex;
}

int appTask_GetResult(u32 index) {
    if (!app_tasks) return -1;
    if (index >= APPTASKS_MAX) return -2;
    AppTask* task = app_tasks + index;
    if (!task->done) return -3;
    return task->rc;
}
int appTask_IsDone(u32 index) {
    if (!app_tasks) return 0;
    if (index >= APPTASKS_MAX) return 0;
    AppTask* task = app_tasks + index;
    if (task->done) return 1;
    return 0;
}

u32 appTask_IsWaiting() {
    return app_tasks_pause;
}

u32 appTask_PausedOn() {
	return app_tasks_pausedOn;
}

void appTask_Continue() {
    app_tasks_pause = 0;
}

u32 appTask_GetCurrentTask() {
    return app_tasks_current;
}

int appTask_Clear(u32 index) {
    if (!app_tasks) return -1;
    if (index >= APPTASKS_MAX) return -2;
    AppTask* task = app_tasks + index;
    if (!task->type) return 0;
    if (!task->done) return -3;
    task->type = APPTASK_VACANT;
    return task->rc;
}

void appTaskThread(void* arg) {
    if (!app_tasks) threadExit(1);
    AppTask* currentTask = app_tasks;
    
    while (!exiting) {
        if (!app_tasks_pause) {
            app_tasks_pausedOn = -1U;
			if (!currentTask->done) {
                switch (currentTask->type) {
                    case APPTASK_DOWN_FILE:
                        currentTask->rc =
                            downloadFile(currentTask->url, currentTask->data.file);
                        break;
                    case APPTASK_DOWN_RAW:
                        currentTask->rc =
                            downloadString(currentTask->url, currentTask->data.downData);
                        break;
                    case APPTASK_FORMAT_SAVE:
                        currentTask->rc =
                            formatSave(currentTask->data.format.titleID, currentTask->data.format.mediaType, currentTask->data.format.files, currentTask->data.format.dirs, currentTask->data.format.dupData);
                        break;
                    case APPTASK_FORMAT_EXTDATA:
                        currentTask->rc =
                            formatExtData(currentTask->data.format.titleID, currentTask->data.format.mediaType, currentTask->data.format.files, currentTask->data.format.dirs);
                        break;
					default:
						break;
                }
				if (currentTask->rc) {
					app_tasks_pause = 2;
					app_tasks_pausedOn = app_tasks_current;
				}
                currentTask->done = 1;
            }
			if (currentTask->type && !currentTask->done)
				app_tasks_mask |= BIT(app_tasks_current);
            app_tasks_current++; currentTask++;
            if (app_tasks_current >= APPTASKS_MAX) {
                app_tasks_current = 0;
				if (!app_tasks_mask && !app_tasks_pause) app_tasks_pause = 1;
				app_tasks_mask = 0;
                currentTask = app_tasks;
            }
        }
        Sleep(.01666f);
    }
    threadExit(0);
}