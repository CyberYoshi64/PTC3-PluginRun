#pragma once
#include "main.h"

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

Result  archInit();
void    archExit();
Result  archMount(FS_ArchiveID archID, FS_MediaType mediaType, u64 id, const char* prefix, u32 mask);
Result  archUnmount(const char* prefix);
void    archUnmountAll(void);
void    archGetPath(FS_Archive *handle, FS_Path* path, const char* pathStr, u32* type);
bool    archDirExists(const char* path);
bool    archFileExists(const char* path);







/**
 * @brief Check, if a directory exists
 * 
 * @param path Path to the directory
 * @return True, if directory exists, false otherwise.
 */
bool archDirExists(const char* path);

/**
 * @brief Create a directory
 * 
 * @param path Path to the directory
 * @param attr Attribute to create directory with
 */
Result archDirCreate(const char* path);

/**
 * @brief Delete a directory (Fail, if directory isn't empty)
 * 
 * @param path Path to the directory
 */
Result archDirDelete(const char* path);

/**
 * @brief Delete a directory and its contents
 * 
 * @param path Path to the directory
 */
Result archDirDeleteRecursive(const char* path);

/**
 * @brief Rename a directory
 * 
 * @param path1 Path to the source directory
 * @param path2 Path to the destination directory
 */
Result archDirRename(const char* path1, const char* path2);

/**
 * @brief Move a directory (delete destination, if it exists)
 * 
 * @param path1 Path to the source directory
 * @param path2 Path to the destination directory
 */
Result archDirMove(const char* path1, const char* path2);

/**
 * @brief Open a directory
 * 
 * @param handle File handle
 * @param path Path to the directory
 */
Result archDirOpen(Handle* handle, const char* path);

/**
 * @brief Check, if a file exists
 * 
 * @param path Path to the file
 * @return True, if the file exists, false otherwise.
 */
bool archFileExists(const char* path);

/**
 * @brief Create a file
 * 
 * @param path Path to the file
 * @param attr Attributes to create the file with
 * @param size Base file size to create with
 */
Result archFileCreate(const char* path, u32 attr, u64 size);

/**
 * @brief Delete a file
 * 
 * @param path Path to the file
 */
Result archFileDelete(const char* path);

/**
 * @brief Rename a file
 * 
 * @param path1 Path to the source file
 * @param path2 Path to the destination file
 */
Result archFileRename(const char* path1, const char* path2);

/**
 * @brief Move a file (delete destination, if it exists)
 * 
 * @param path1 Path to the source file
 * @param path2 Path to the destination file
 */
Result archFileMove(const char* path1, const char* path2);

/**
 * @brief Open a file
 * 
 * @param handle File handle
 * @param path Path to the file
 * @param mode Mode to open file with (0 = default)
 */
Result archFileOpen(Handle* handle, const char* path, u32 mode);
