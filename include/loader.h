/**
 * @file loader.h
 * @brief LOADER Service
 */

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

#pragma once
#include <3ds/exheader.h>
#include <3ds/services/fs.h>

// Used by the custom loader command 0x101 (ControlApplicationMemoryModeOverride)
typedef struct ControlApplicationMemoryModeOverrideConfig {
    u32 query : 1; //< Only query the current configuration, do not update it.
    u32 enable_o3ds : 1; //< Enable o3ds memory mode override
    u32 enable_n3ds : 1; //< Enable n3ds memory mode override
    u32 o3ds_mode : 3; //< O3ds memory mode
    u32 n3ds_mode : 3; //< N3ds memory mode
} ControlApplicationMemoryModeOverrideConfig;

/// Initializes LOADER.
Result loaderInit(void);
/// Exits LOADER.
void loaderExit(void);
/**
 * @brief Loads a program and returns a process handle to the newly created process.
 * @param[out] process Pointer to output the process handle to.
 * @param[in] programHandle The handle of the program to load.
 */
Result LOADER_LoadProcess(Handle* process, u64 programHandle);
/**
 * @brief Registers a program (along with its update).
 * @param[out] programHandle Pointer to output the program handle to.
 * @param[in] programInfo The program info.
 * @param[in] programInfoUpdate The program update info.
 */
Result LOADER_RegisterProgram(u64* programHandle, const FS_ProgramInfo *programInfo, const FS_ProgramInfo *programInfoUpdate);
/**
 * @brief Unregisters a program (along with its update).
 * @param[in] programHandle The handle of the program to unregister.
 */
Result LOADER_UnregisterProgram(u64 programHandle);
/**
 * @brief Retrives a program's main NCCH extended header info (SCI + ACI, see @ref ExHeader_Info).
 * @param[out] exheaderInfo Pointer to output the main NCCH extended header info.
 * @param[in] programHandle The handle of the program to unregister
 */
Result LOADER_GetProgramInfo(ExHeader_Info* exheaderInfo, u64 programHandle);

/**
 * @brief Disables Luma 3DS game patching for the next application that is being chainloaded into.
 */
Result LOADER__Custom__DisableNextGamePatch(void);

/**
 * @brief Overrides the memory mode specified by next loaded application
 * @param[out] currentConfig Current memory configuration
 * @param[in] config Memory Override Configuration
 */
Result LOADER__Custom__ControlApplicationMemoryModeOverride(ControlApplicationMemoryModeOverrideConfig* currentConfig, ControlApplicationMemoryModeOverrideConfig* config);

