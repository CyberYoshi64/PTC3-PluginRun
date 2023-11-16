#pragma once

#include <3ds.h>
#include <stdlib.h>

Result CIA_LaunchTitle(u64 titleId, FS_MediaType mediaType);
Result deletePrevious(u64 titleid, FS_MediaType media);
Result installCia(Handle fileHandle, bool updateSelf);
