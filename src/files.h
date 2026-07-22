#pragma once

#include <filesystem>
#include <string_view>

#include <stdio.h>
#include <time.h>

enum class FILEMODE {
    READONLY = 0x01,
    WRITEONLY = 0x02,
    READWRITE = 0x04,
    APPEND = 0x08
};

enum class FILETYPE {
    PERM = 0x10,
    OPTIONAL = 0x20,
    CONFIG = 0x40
};

enum class FILEATTR {
    NONE = 0x0,
    DIRECTORY = 0x0100,
    READABLE = 0x0200,
    WRITABLE = 0x0400
};

inline FILEATTR operator|(FILEATTR left, FILEATTR right)
{
    return FILEATTR(left | right);
}

inline FILEATTR operator|=(FILEATTR left, FILEATTR right)
{
    return left | right;
}

inline FILEATTR operator&(FILEATTR left, FILEATTR right)
{
    return FILEATTR(left & right);
}

inline FILEATTR operator&=(FILEATTR left, FILEATTR right)
{
    return left & right;
}

typedef struct GameDirectoryFile
{
    std::string filename;
    FILEATTR attr;
    time_t timestamp;
} GameDirectoryFile;

int SetGameDirectories(const std::filesystem::path &local, const std::filesystem::path &global);
FILE *OpenGameFile(const std::filesystem::path &filename, FILEMODE mode, FILETYPE type);
int CloseGameFile(FILE *pfd);
FILEATTR GetGameFileAttributes(const std::filesystem::path &filename, FILETYPE type);
bool DeleteGameFile(const std::filesystem::path &filename);
bool CreateGameDirectory(const std::filesystem::path &dirname);
void *OpenGameDirectory(const std::filesystem::path &dirname, const std::string &pattern, FILETYPE type);
GameDirectoryFile *ScanGameDirectory(void *dir);
int CloseGameDirectory(void *dir);
void InitGameDirectories(const std::string_view argv0);
