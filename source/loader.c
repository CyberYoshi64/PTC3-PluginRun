/*
loader.c (devkitPro/libctru)

This software is provided 'as-is', without any express
or implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it
and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
   you must not claim that you wrote the original software.
   If you use this software in a product, an acknowledgment
   in the product documentation would be appreciated but is
   not required.
2. Altered source versions must be plainly marked as such,
   and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source
   distribution.
*/

/*
  Changes applied:
  - Adding support for custom loader commands from the
    Luma 3DS extensions
   (https://github.com/LumaTeam/Luma3DS/commit/c055fb6f5e20c4b4ff205bf25e520f14b190800d#diff-3c0ce07c331ab8707953a67eb1c149f16e0d880b1375b98fb54201e0959592f5R584-R597)
*/

#include <string.h>
#include "loader.h"
#include <3ds/result.h>
#include <3ds/svc.h>
#include <3ds/srv.h>
#include <3ds/synchronization.h>
#include <3ds/ipc.h>

static Handle loaderHandle;
static int loaderRefCount;
Result loaderInit(void) {
	Result res;
	if (AtomicPostIncrement(&loaderRefCount)) return 0;
	res = srvGetServiceHandle(&loaderHandle, "Loader");
	if (R_FAILED(res)) AtomicDecrement(&loaderRefCount);
	return res;
}

void loaderExit(void) {
	if (AtomicDecrement(&loaderRefCount)) return;
	svcCloseHandle(loaderHandle);
}

Result LOADER_LoadProcess(Handle* process, u64 programHandle) {
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();
	cmdbuf[0] = IPC_MakeHeader(1, 2, 0); // 0x10080
	cmdbuf[1] = (u32)programHandle;
	cmdbuf[2] = (u32)(programHandle >> 32);
	if(R_FAILED(ret = svcSendSyncRequest(loaderHandle))) return ret;
	*process = cmdbuf[3];
	return (Result)cmdbuf[1];
}

Result LOADER_RegisterProgram(u64* programHandle, const FS_ProgramInfo *programInfo, const FS_ProgramInfo *programInfoUpdate) {
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();
	cmdbuf[0] = IPC_MakeHeader(2, 8, 0); // 0x20200
	memcpy(&cmdbuf[1], programInfo, sizeof(FS_ProgramInfo));
	memcpy(&cmdbuf[5], programInfoUpdate, sizeof(FS_ProgramInfo));
	if(R_FAILED(ret = svcSendSyncRequest(loaderHandle))) return ret;
	*programHandle = ((u64)cmdbuf[3] << 32) | cmdbuf[2];
	return (Result)cmdbuf[1];
}

Result LOADER_UnregisterProgram(u64 programHandle) {
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();
	cmdbuf[0] = IPC_MakeHeader(3, 2, 0); // 0x30080
	cmdbuf[1] = (u32)programHandle;
	cmdbuf[2] = (u32)(programHandle >> 32);
	if(R_FAILED(ret = svcSendSyncRequest(loaderHandle))) return ret;
	return (Result)cmdbuf[1];
}

Result LOADER_GetProgramInfo(ExHeader_Info* exheaderInfo, u64 programHandle) {
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();
	u32 *staticbufs = getThreadStaticBuffers();
	cmdbuf[0] = IPC_MakeHeader(4, 2, 0); // 0x40080
	cmdbuf[1] = (u32)programHandle;
	cmdbuf[2] = (u32)(programHandle >> 32);
	u32 staticbufscpy[2] = {staticbufs[0], staticbufs[1]};
	staticbufs[0] = IPC_Desc_StaticBuffer(0x400, 0);
	staticbufs[1] = (u32)exheaderInfo;
	if(R_FAILED(ret = svcSendSyncRequest(loaderHandle))) return ret;
	staticbufs[0] = staticbufscpy[0];
	staticbufs[1] = staticbufscpy[1];
	return (Result)cmdbuf[1];
}

Result LOADER__Custom__DisableNextGamePatch() {
    Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();
	cmdbuf[0] = IPC_MakeHeader(0x100, 0, 0);
	if(R_FAILED(ret = svcSendSyncRequest(loaderHandle))) return ret;
	return (Result)cmdbuf[1];
}

Result LOADER__Custom__ControlApplicationMemoryModeOverride(ControlApplicationMemoryModeOverrideConfig* currentConfig, ControlApplicationMemoryModeOverrideConfig* config) {
    Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();
	cmdbuf[0] = IPC_MakeHeader(0x101, 1, 0);
	memcpy(&cmdbuf[1], config, sizeof(ControlApplicationMemoryModeOverrideConfig));
	if(R_FAILED(ret = svcSendSyncRequest(loaderHandle))) return ret;
	currentConfig = (ControlApplicationMemoryModeOverrideConfig*)cmdbuf[2];
	return (Result)cmdbuf[1];
}
