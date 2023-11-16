#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <math.h>

#ifdef __cplusplus
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include "memcpyctr.h"
#include "file.h"
#include "clock.h"
#include "utils.h"
#include "plgldr.h"
#include "loader.h"
#include "hidRead.h"
#include "curlWrapper.h"
#include "ptc3_smdh_bin.h"

#include "menu.h"
#include "menu/menuMain.h"

#ifdef __cplusplus
}
#endif

extern bool     exiting;    // Application is closing
extern bool     isCitra;    // Is the current host Citra?
extern bool     runPlugin;  // Flag denoting if plugin will run
extern u64      bootTitle;  // Boot title ID for plugin
extern bool     blockHOME;  // Block HOME when error is shown

extern C3D_RenderTarget    *topScr;     // Top screen render target
extern C3D_RenderTarget    *botScr;     // Bottom screen render target

int menuDefault__Act(void);

/**
 * @brief Display an error message on the bottom screen.
 * 
 * @param[in] error Error string to display
 * @param[in] standalone Whether the screen is rendered outside of the frame or not
 * @param[in] waitButton Waiting for a button before returning execution
 */
void drawError(const char* error, bool standalone, u32 waitButton);

/**
 * @brief Cause a user panic with defined values for exception handler
 * @param[in] r0 Value set in R0
 * @param[in] r1 Value set in R1
 * @param[in] r2 Value set in R2
 */
void customBreak(u32 r0, u32 r1, u32 r2);

#define M_PI		3.14159265358979323846
#define M_SQRT2		1.41421356237309504880
#define KEY_ABXYSS  KEY_A|KEY_B|KEY_SELECT|KEY_START|KEY_X|KEY_Y
#define KEY_ABXY    KEY_A|KEY_B|KEY_X|KEY_Y

#define assert(x) {if(!(x)){customBreak(0xDEADBEEF,0x69420187,0xF00D43D5)}}

#define __ENABLE_PLUGIN_LOADER  true

/// Main mod paths

#define MAIN_PATH       "/PTC3PLG/"                 // Main path of the modpack
#define CONFIG_PATH     MAIN_PATH"config/"          // Config path containing version and launch info
#define GAME_FSPATH     MAIN_PATH"datafs/"          // Path to game's romfs mirror
#define MODRES_PATH     MAIN_PATH"resources/"       // Plugin resources
#define SAVE_PATH       MAIN_PATH"savefs/"          // Path to mod's own save data

#define PLUGIN_PATH     MODRES_PATH"ptc3plg.3gx"    // Plugin path

/// Updates

#define GITHUB_REPO     "CyberYoshi64/SB3CYX-Updates" // GitHub repo in which updates are hosted

#define URL_UPDATEINFO  "https://raw.githubusercontent.com/" GITHUB_REPO "/master/updates/changeloglist"    // Changelog data; can use to check latest version as well
#define URL_UPDATEDATA  "https://github.com/" GITHUB_REPO "/releases/download/v%s/filelist.txt"             // Update file lists
#define URL_LATESTVER   "https://raw.githubusercontent.com/" GITHUB_REPO "/master/updates/latestVer"        // Update file lists
