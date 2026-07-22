#include <print>
#include <cassert>
#include <fstream>
#include <cstdio>
#include <chrono>

#include <fnmatch.h>

#include "files.h"
#include "globals.h"

namespace fs = std::filesystem;
using namespace std::string_view_literals;

/*
Sets the local and global directories used by the other functions.
Local = ~/.dir, where config and user-installed files are kept.
Global = installdir, where installed data is stored.
*/
int SetGameDirectories(const fs::path &local, const fs::path &global)
{
    auto &globals = Globals::instance();

    globals.local_dir = local;
    globals.global_dir = global;

    if (!fs::exists(local)) {
        std::println("Creating local directory {}...", local.string());

        fs::create_directories(local);
    }

    return 0;
}

#define DIR_SEPARATOR "/"

static std::string FixFilename(const std::string &filename, const std::string &prefix, int force)
{
    auto res = prefix + "/" + filename;
    for (auto &c : res)
        if (c == '\\')
            c = '/';
    return res;
}

/*
Open a file of type type, with mode mode.

Mode can be:
#define	FILEMODE::READONLY	0x01
#define	FILEMODE::WRITEONLY	0x02
#define	FILEMODE::READWRITE	0x04
#define FILEMODE::APPEND		0x08
Type is (mode = ReadOnly):
#define	FILETYPE::PERM		0x08 // try the global dir only
#define	FILETYPE::OPTIONAL	0x10 // try the global dir first, then try the local dir
#define	FILETYPE::CONFIG		0x20 // try the local dir only

Type is (mode = WriteOnly or ReadWrite):
FILETYPE::PERM: error
FILETYPE::OPTIONAL: error
FILETYPE::CONFIG: try the local dir only
*/
FILE *OpenGameFile(const fs::path &filename, FILEMODE mode, FILETYPE type)
{
    char *rfilename;
    std::string openmode;
    FILE *fp;

    if ((type != FILETYPE::CONFIG) && (mode != FILEMODE::READONLY))
        return NULL;

    switch (mode) {
    case FILEMODE::READONLY:
        openmode = "rb";
        break;
    case FILEMODE::WRITEONLY:
        openmode = "wb";
        break;
    case FILEMODE::READWRITE:
        openmode = "w+";
        break;
    case FILEMODE::APPEND:
        openmode = "ab";
        break;
    default:
        return NULL;
    }

    if (type != FILETYPE::CONFIG) {
        // rfilename = FixFilename(filename, global_dir, 0);

        fp = fopen((Globals::instance().global_dir / filename).string().c_str(), openmode.c_str());

        if (fp != NULL) {
            return fp;
        }

        // rfilename = FixFilename(filename, global_dir, 1);

        // fp = fopen(rfilename, openmode);

        // free(rfilename);

        // if (fp != NULL) {
        //     return fp;
        // }
    }

    if (type != FILETYPE::PERM) {
        // rfilename = FixFilename(filename, local_dir, 0);

        fp = fopen((Globals::instance().local_dir / filename).string().c_str(), openmode.c_str());

        if (fp != NULL) {
            return fp;
        }

        // rfilename = FixFilename(filename, local_dir, 1);

        // fp = fopen(rfilename, openmode);

        // free(rfilename);

        // return fp;
    }

    return NULL;
    // char *rfilename;
    // std::string openmode;
    // FILE *fp;

    // if ((type != FILETYPE::CONFIG) && (mode != FILEMODE::READONLY))
    //     return nullptr;

    // switch (mode) {
    // case FILEMODE::READONLY:
    //     openmode = "rb";
    //     break;
    // case FILEMODE::WRITEONLY:
    //     openmode = "wb";
    //     break;
    // case FILEMODE::READWRITE:
    //     // openmode = std::ios_base::in | std::ios_base::out | std::ios_base::binary;
    //     openmode = "rb+";
    //     break;
    // case FILEMODE::APPEND:
    //     openmode = std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::ate;
    //     openmode = ""
    //     break;
    // default:
    //     return {};
    // }

    // if (type != FILETYPE::CONFIG)
    //     return {Globals::instance().global_dir / filename, openmode};

    // if (type != FILETYPE::PERM)
    //     return {Globals::instance().local_dir / filename, openmode};

    // return {};
}

int CloseGameFile(FILE *pfd)
{
    return fclose(pfd);
}

/*
Get the filesystem attributes of a file

#define	FILEATTR::DIRECTORY	0x0100
#define FILEATTR::READABLE	0x0200
#define FILEATTR::WRITABLE	0x0400

Error or can't access it: return value of 0 (What is the game going to do about it anyway?)
*/
static FILEATTR GetFA(const fs::path &path)
{
    FILEATTR attr = FILEATTR::NONE;
    auto perms = fs::status(path).permissions();

    if (fs::is_directory(path))
        attr |= FILEATTR::DIRECTORY;

    if ((perms & fs::perms::owner_read) != fs::perms::none)
        attr |= FILEATTR::READABLE;

    if ((perms & fs::perms::owner_write) != fs::perms::none)
        attr |= FILEATTR::WRITABLE;
    return attr;
}

static time_t GetTS(const fs::path &filename)
{
#warning FIX ME: CHANGE TO STD::FILESYSTEM::FILE_TIME_TYPE
    const auto fTime = fs::last_write_time(filename);
    const auto chronoTime = std::chrono::clock_cast<std::chrono::system_clock>(fTime);
    return std::chrono::system_clock::to_time_t(chronoTime);
}

FILEATTR GetGameFileAttributes(const fs::path &filename, FILETYPE type)
{
    if (type != FILETYPE::CONFIG)
        return GetFA(Globals::instance().global_dir / filename);

    if (type != FILETYPE::PERM)
        return GetFA(Globals::instance().local_dir / filename);

    return FILEATTR::NONE;
}

/*
Delete a file: local dir only
*/
bool DeleteGameFile(const fs::path &filename)
{
    try {
        fs::remove(Globals::instance().local_dir / filename);
    } catch(...) {
        return false;
    }
    return true;
}

/*
Create a directory: local dir only

TODO: maybe also mkdir parent directories, if they do not exist?
*/
bool CreateGameDirectory(const fs::path &dirname)
{
    try {
        fs::create_directories(Globals::instance().local_dir / dirname);
    } catch (...) {
        return false;
    }
    return true;
}

/* This struct is private. */
typedef struct GameDirectory
{

    fs::path localdir;
    fs::path globaldir;

    std::string pat; /* pattern to match */

    GameDirectoryFile tmp; /* Temp space */
} GameDirectory;

/*
"Open" a directory dirname, with type type
Returns a pointer to a directory datatype

Pattern is the pattern to match
*/
void *OpenGameDirectory(const fs::path &dirname, const std::string &pattern, FILETYPE type)
{
    GameDirectory *gd;

    fs::path globaldir;
    fs::path localdir;

    if (type != FILETYPE::CONFIG)
        globaldir = Globals::instance().global_dir / dirname;

    if (type != FILETYPE::PERM)
        localdir = Globals::instance().local_dir / dirname;

    if (localdir.empty() && globaldir.empty())
        return nullptr;

    gd = new GameDirectory;

    gd->localdir = localdir;
    gd->globaldir = globaldir;

    gd->pat = pattern;

    return gd;
}

/*
This struct is public.

typedef struct GameDirectoryFile
{
	char *filename;
	int attr;
} GameDirectoryFile;
*/

/*
Returns the next match of pattern with the contents of dir

f is the current file
*/
GameDirectoryFile *ScanGameDirectory(void *dir)
{
    // char *ptr;
    // struct dirent *file;
    GameDirectory *directory;

    directory = (GameDirectory *) dir;

    if (!directory->globaldir.empty()) {
        for (auto file : fs::directory_iterator{directory->globaldir}) {
            if (fnmatch(directory->pat.c_str(), file.path().filename().string().c_str(), FNM_PATHNAME) == 0) {
                directory->tmp.attr = GetFA(file.path());

                directory->tmp.filename = file.path().filename().string();

                return &directory->tmp;
            }
        }
        directory->globaldir.clear();
    }

    if (!directory->localdir.empty()) {
        for (auto file : fs::directory_iterator{directory->localdir}) {
            if (fnmatch(directory->pat.c_str(), file.path().filename().string().c_str(), FNM_PATHNAME) == 0) {
                directory->tmp.attr = GetFA(file.path());

                directory->tmp.timestamp = GetTS(file.path());

                directory->tmp.filename = file.path().filename().string();

                return &directory->tmp;
            }
        }
        directory->localdir.clear();
    }

    return NULL;
}

/*
Close directory
*/
int CloseGameDirectory(void *dir)
{
    GameDirectory *directory = (GameDirectory *) dir;
    delete directory;
    return directory ? 0 : -1;
}

/*
  Game-specific helper function.
 */
static bool check_game_directory(const fs::path &dir)
{
    if (dir.empty())
        return false;

    if (!fs::exists(dir / "avp_huds"sv))
        return false;

    if (!fs::exists(dir / "avp_huds/alien.rif"sv))
        return false;

    if (!fs::exists(dir / "avp_rifs"sv))
        return false;

    if (!fs::exists(dir / "avp_rifs/temple.rif"sv))
        return false;

    if (!fs::exists(dir / "fastfile"sv))
        return false;

    if (!fs::exists(dir / "fastfile/ffinfo.txt"sv))
        return false;

    return true;
}

/*
  Game-specific initialization
 */
void InitGameDirectories(const std::string_view argv0)
{
#warning FIX ME: MOVE THIS CRAP TO GLOBALS
    extern char *SecondTex_Directory;
    extern char *SecondSoundDir;

    SecondTex_Directory = "graphics/";
    SecondSoundDir = "sound/";

    const auto safeenv = [](std::string_view name) -> std::string_view {
        const auto env = getenv(name.data());
        return env ? env : ""sv;
    };
    const fs::path homedir{safeenv("HOME")};
    auto localdir = homedir / ".avp"sv;


    /*
	1. $AVP_DATA overrides all
	2. executable path from argv[0]
	3. realpath of executable path from argv[0]
	4. $PATH
	5. current directory
	*/

    /* 1. $AVP_DATA */
    fs::path gamedir{safeenv("AVP_DATA")};

    /* $AVP_DATA overrides all, so no check */

    if (gamedir.empty()) {
        /* 2. executable path from argv[0] */
        fs::path exePath{argv0};

        if (exePath.empty()) {
            /* ... */
            std::println(stderr, "InitGameDirectories failure");
            std::abort();
        }

        gamedir = exePath.parent_path();

        if (!gamedir.empty()) {
            // gamedir = tmp;

            if (!check_game_directory(gamedir)) {
                gamedir.clear();
            }
        }
    }

    if (gamedir.empty()) {
        /* 3. realpath of executable path from argv[0] */

        try {
            gamedir = fs::read_symlink(argv0).parent_path();
        } catch (const std::filesystem::filesystem_error  &err) {
            std::println(stderr, "Error {}", err.what());
        }

        if (!check_game_directory(gamedir)) {
            gamedir.clear();
        }
    }

    // if (gamedir.empty()) {
    //     /* 4. $PATH */
    //     std::abort();
    //     // path = getenv("PATH");
    //     // if (path) {
    //     //     while (*path) {
    //     //         len = strcspn(path, ":");

    //     //         copylen = min(len, (size_t) (PATH_MAX - 1));

    //     //         strncpy(tmppath, path, copylen);
    //     //         tmppath[copylen] = 0;

    //     //         if (check_game_directory(tmppath)) {
    //     //             gamedir = tmppath;
    //     //             break;
    //     //         }

    //     //         path += len;
    //     //         path += strspn(path, ":");
    //     //     }
    //     // }
    // }

    if (gamedir.empty()) {
        /* 5. current directory */
        gamedir = fs::current_path();
    }

    /* last chance sanity check */
    if (!check_game_directory(gamedir)) {
        std::println(stderr, "Unable to find the AvP gamedata.");
        std::println(stderr, "The directory last examined was: {}", gamedir.string());
        std::println(stderr, "Has the game been installed and");
        std::println(stderr, "are all game files lowercase?");
        std::abort();
    }

    SetGameDirectories(localdir, gamedir);

    /* delete some log files */
    DeleteGameFile("dx_error.log");
}

#ifdef FILES_DRIVER
int main(int argc, char *argv[])
{
    FILE *fp;
    char buf[64];
    void *dir;

    SetGameDirectories("tmp1", "tmp2");

    fp = OpenGameFile("tester", FILEMODE::WRITEONLY, FILETYPE::CONFIG);

    fputs("test\n", fp);

    CloseGameFile(fp);

    CreateGameDirectory("yaya");
    CreateGameDirectory("tester2");
    CreateGameDirectory("tester2/blah");

    fp = OpenGameFile("tester", FILEMODE::READONLY, FILETYPE::OPTIONAL);
    printf("Read: %s", fgets(buf, 60, fp));
    CloseGameFile(fp);

    fp = OpenGameFile("tester", FILEMODE::READONLY, FILETYPE::CONFIG);
    printf("Read: %s", fgets(buf, 60, fp));
    CloseGameFile(fp);

    dir = OpenGameDirectory(".", "*", FILETYPE::OPTIONAL);
    if (dir != NULL) {
        GameDirectoryFile *gd;

        while ((gd = ScanGameDirectory(dir)) != NULL) {
            printf("Name: %s, Attr: %08X\n", gd->filename, gd->attr);
        }

        CloseGameDirectory(dir);
    } else {
        printf("Could not open the directory...\n");
    }

    DeleteGameFile("tester");

    return 0;
}
#endif
