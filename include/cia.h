#pragma once

#include <3ds.h>
#include <stdlib.h>

/**
 * @brief Launch a title
 * 
 * @param titleId Title ID
 * @param mediaType Media type
 */
Result CIA_LaunchTitle(u64 titleId, FS_MediaType mediaType);

/**
 * @brief Delete a title (used to install a CIA)
 * 
 * @param titleid Title ID
 * @param media Media type
 */
Result deletePrevious(u64 titleid, FS_MediaType media);

/**
 * @brief Install a CIA
 * 
 * @param fileHandle File handle
 * @param updateSelf Whether the title is intended to be the currently running application
 */
Result installCia(Handle fileHandle, bool updateSelf);
