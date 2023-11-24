#pragma once
#include "main.h"

typedef struct FS_PathUTF8_s {
    FS_Path     path;
    FS_Archive  arch;
    u32         type;
    void*       buf;
} FS_PathUTF8;

enum FileArchiveMode {
    ARCHMODE_VACANT     = 0,        // State: Vacant | Mount value: Default
    ARCHMODE_READ       = BIT( 0),  // Read
    ARCHMODE_WRITE      = BIT( 1),  // Write
    ARCHMODE_RW         = ARCHMODE_READ|ARCHMODE_WRITE, // Read & Write
};

typedef struct {
    FS_Archive  archive;
    char        name[20];
    u32         type;
} FileArchiveHandles;

#define FILEARCHIVEHANDLES_MAX  16
#define FILEARCHIVEHANDLES_SIZE sizeof(FileArchiveHandles)

extern FileArchiveHandles *archiveHandles;

/**
 * @brief Initialize archive handler
 */
Result  archInit();

/**
 * @brief Clean up archive handler
 */
void    archExit();

/**
 * @brief Mount an archive
 * 
 * @param archID Archive type
 * @param mediaType Media Type
 * @param id Archive ID (typically a title ID)
 * @param prefix Prefix to refer archive as
 * @param mask Mounting options (see enum FileArchiveMode)
 */
Result  archMount(FS_ArchiveID archID, FS_MediaType mediaType, u64 id, const char *prefix, u32 mask);

/**
 * @brief Unmount an archive
 * 
 * @param prefix Prefix of an archive
 */
Result  archUnmount(const char *prefix);

/**
 * @brief Unmount all archives of this archive handler
 */
void    archUnmountAll(void);

/**
 * @brief Get archive properties of a path
 * 
 * @param[out] handle Output archive handle
 * @param[out] path Output FS_Path object
 * @param[in]  pathStr Path string
 * @param[out] type Mounting bitmask
 */
void    archGetPath(FS_Archive *handle, FS_Path *path, const char *pathStr, u32 *type);

/**
 * @brief Check, if a directory exists
 * 
 * @param path Path to the directory
 * @return True, if directory exists, false otherwise.
 */
bool archDirExists(const char *path);

/**
 * @brief Create a directory
 * 
 * @param path Path to the directory
 */
Result archDirCreate(const char *path);

/**
 * @brief Create a directory and its parent folders if they don't exist
 * 
 * @param path Path to the directory
 * @param forFile Whether the end of the path is a file or an extra directory
 */
Result archDirCreateRecursive(const char *path, bool forFile);

/**
 * @brief Delete a directory (Fail, if directory isn't empty)
 * 
 * @param path Path to the directory
 */
Result archDirDelete(const char *path);

/**
 * @brief Delete a directory and its contents
 * 
 * @param path Path to the directory
 */
Result archDirDeleteRecursive(const char *path);

/**
 * @brief Rename a directory
 * 
 * @param path1 Path to the source directory
 * @param path2 Path to the destination directory
 */
Result archDirRename(const char *path1, const char *path2);

/**
 * @brief Move a directory (delete destination, if it exists)
 * 
 * @param path1 Path to the source directory
 * @param path2 Path to the destination directory
 */
Result archDirMove(const char *path1, const char *path2);

/**
 * @brief Open a directory
 * 
 * @param handle File handle
 * @param path Path to the directory
 */
Result archDirOpen(Handle *handle, const char *path);

/**
 * @brief Check, if a file exists
 * 
 * @param path Path to the file
 * @return True, if the file exists, false otherwise.
 */
bool archFileExists(const char *path);

/**
 * @brief Create a file
 * 
 * @param path Path to the file
 * @param attr Attributes to create the file with
 * @param size Base file size to create with
 */
Result archFileCreate(const char *path, u32 attr, u64 size);

/**
 * @brief Create a file and its parent folders if they don't exist
 * 
 * @param path Path to the file
 * @param attr Attributes to create the file with
 * @param size Base file size to create with
 */
Result archFileCreateRecursive(const char *path, u32 attr, u64 size);

/**
 * @brief Delete a file
 * 
 * @param path Path to the file
 */
Result archFileDelete(const char *path);

/**
 * @brief Rename a file
 * 
 * @param path1 Path to the source file
 * @param path2 Path to the destination file
 */
Result archFileRename(const char *path1, const char *path2);

/**
 * @brief Move a file (delete destination, if it exists)
 * 
 * @param path1 Path to the source file
 * @param path2 Path to the destination file
 */
Result archFileMove(const char *path1, const char *path2);

/**
 * @brief Open a file
 * 
 * @param handle File handle
 * @param path Path to the file
 * @param mode Mode to open file with (0 = default)
 */
Result archFileOpen(Handle *handle, const char *path, u32 mode);

/**
 * @brief Construct a `FS_PathUTF8` object
 * 
 * @param path Path string
 * @param expectedType Expected mounting flags
 * @return A `FS_PathUTF8` object pointer, NULL on failure
 */
FS_PathUTF8 *fsMakePath8(const char *path, u32 expectedType);

/**
 * @brief Deconstruct a `FS_PathUTF8` object
 * 
 * @param self `FS_PathUTF8` object pointer
 */
void fsFreePath8(FS_PathUTF8* self);