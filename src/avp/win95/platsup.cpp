/*

  Platform specific project specific C functions

*/

#include "3dc.h"
#include "module.h"
#include "inline.h"

#include "gameplat.h"
#include "gamedef.h"

#include "dynblock.h"
#include "dynamics.h"
#define UseLocalAssert false
#include "ourasert.h"

/*
	Externs from pc\io.c
*/

extern int InputMode;
extern unsigned char KeyboardInput[];

extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;
extern unsigned char *ScreenBuffer;

extern unsigned char KeyASCII;

/* External functions */

extern void D3D_Line(VECTOR2D *LineStart, VECTOR2D *LineEnd, int LineColour);
extern void Draw_Line_VMType_8(VECTOR2D *LineStart, VECTOR2D *LineEnd, int LineColour);

// Prototypes

int IDemandFireWeapon(void);

int IDemandNextWeapon(void);
int IDemandPreviousWeapon(void);

// Functions

void catpathandextension(char *dst, char *src)
{
    int len = lstrlen(dst);

    if ((len > 0 && (dst[len - 1] != '\\' && dst[len - 1] != '/')) && *src != '.') {
        lstrcat(dst, "/");
    }

    lstrcat(dst, src);

    /*
	The second null here is to support the use
	of SHFileOperation, which is a Windows 95
	addition that is uncompilable under Watcom
	with ver 10.5, but will hopefully become
	available later...
*/
    len = lstrlen(dst);
    dst[len + 1] = 0;
}

/* game platform definition of the Mouse Mode*/

int MouseMode = MouseVelocityMode;

/*

  Real PC control functions

*/
#if 1
int IDemandLookUp(void)
{
    return false;
}

int IDemandLookDown(void)
{
    return false;
}

int IDemandTurnLeft(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_LEFT])
        return true;
    return false;
}

int IDemandTurnRight(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_RIGHT])
        return true;
    return false;
}

int IDemandGoForward(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_UP])
        return true;
    return false;
}

int IDemandGoBackward(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_DOWN])
        return true;
    return false;
}

int IDemandJump(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_CAPS])
        return true;
    return false;
}

int IDemandCrouch(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_Z])
        return true;
    return false;
}

int IDemandSelect(void)
{
    InputMode = Digital;

    if (KeyboardInput[KEY_CR])
        return true;
    if (KeyboardInput[KEY_SPACE])
        return true;
    else
        return false;
}

int IDemandStop(void)
{
    return false;
}

int IDemandFaster(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_LEFTSHIFT])
        return true;
    return false;
}

int IDemandSideStep(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_LEFTALT])
        return true;
    return false;
}

int IDemandPickupItem(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_P])
        return true;
    return false;
}

int IDemandDropItem(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_D])
        return true;
    return false;
}

int IDemandMenu(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_M])
        return true;
    return false;
}

int IDemandOperate(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_SPACE])
        return true;
    return false;
}

int IDemandFireWeapon(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_CR])
        return true;
    return false;
}

/* KJL 11:29:12 10/07/96 - added by me */
int IDemandPreviousWeapon(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_1])
        return true;
    else
        return false;
}
int IDemandNextWeapon(void)
{
    InputMode = Digital;
    if (KeyboardInput[KEY_2])
        return true;
    else
        return false;
}
#endif

int IDemandChangeEnvironment()
{
    InputMode = Digital;

    if (KeyboardInput[KEY_F1])
        return 0;
    else if (KeyboardInput[KEY_F2])
        return 1;
    else if (KeyboardInput[KEY_F3])
        return 2;
    else if (KeyboardInput[KEY_F4])
        return 3;
    else if (KeyboardInput[KEY_F5])
        return 4;
    else
        return (-1);
}

/* KJL 15:53:52 05/04/97 - 
Loaders/Unloaders for language internationalization code in language.c */

char *LoadTextFile(char *filename)
{
    char *bufferPtr;
    long int save_pos, size_of_file;
    FILE *fp;
    fp = OpenGameFile(filename, FILEMODE_READONLY, FILETYPE_PERM);

    if (!fp)
        goto error;

    save_pos = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    size_of_file = ftell(fp);

    bufferPtr = AllocateMem(size_of_file);
    if (!bufferPtr) {
        memoryInitialisationFailure = 1;
        goto error;
    }

    fseek(fp, save_pos, SEEK_SET);

    if (!fread(bufferPtr, size_of_file, 1, fp)) {
        fclose(fp);
        goto error;
    }

    fclose(fp);
    return bufferPtr;

error: {
    /* error whilst trying to load file */
    textprint("Error! Can not load file %s.\n", filename);
    LOCALASSERT(0);
    return 0;
}
}

void UnloadTextFile(char *filename, char *bufferPtr)
{
    if (bufferPtr)
        DeallocateMem(bufferPtr);
}
