#include <nds.h>
#include <stdarg.h>

#include "../main.h"



#ifndef MENU_H
#define MENU_H

#define MAP_OFFSET      8192
#define MAP_WIDTH       (256 / 8)
#define MAP_HEIGHT      (192 / 8)
#define MAP_AREA        (MAP_WIDTH * MAP_HEIGHT)
#define PALETTE_SIZE    16

#ifndef ARRSIZE
#define ARRSIZE(array) (sizeof(array)/sizeof(array[0]))
#endif

#define MENU_UP         KEY_UP
#define MENU_DOWN       KEY_DOWN
#define MENU_PAGEUP     KEY_LEFT
#define MENU_PAGEDOWN   KEY_RIGHT
#define MENU_ADD1       KEY_X
#define MENU_SUB1       KEY_Y
#define MENU_ADD10      KEY_R
#define MENU_SUB10      KEY_L
#define MENU_SELECT     KEY_A
#define MENU_BACK       KEY_B
#define MENU_RELOAD     KEY_SELECT
#define MENU_RESET      KEY_START
#define MENU_PROGRESS   (KEY_A | KEY_B)

enum Palettes
{
    Pal_Normal = 0,
    Pal_Error,
    Pal_Header,
};

enum CommonReturnVals
{
    Ret_Exit = 0xFFFF,
};

enum EntryType
{
    Entry_Dummy = 0,
    Entry_Button,
    Entry_Var,
};

enum SubType
{
    // button; value returned when selected
    Sub_Cursor = 0, // use cursor position for return value

    // entry int; what type the entry represents
    Sub_Normal = 0,
    // colors
    Sub_Red,
    Sub_Green,
    Sub_Blue,
    Sub_Alpha,
};

struct MenuInputs
{
    bool DisableReload;
    bool DisableReset;
    bool DisableExit;
};

struct MenuEntry
{
    void (*Addr)(u32, u8);
    u8* String;
    u8** Values;
    u32* Var;
    u32 VarFrag; // the part of the var that's actually incremented
    u16 SubType;
    u16 Mask;
    u8 Type;
    u8 Shift;
    u8 AddrOffs;
};

struct MenuDat
{
    s32* Cursor;
    u8** Headers;
    struct MenuEntry* Entry;
    u16 NumEntries;
    struct MenuInputs Inputs;
};

extern struct MenuInputs InputsCommon;

void menuWriteSingle(u8* text);
void menuClear();
void menuInit();
u32 menuInputs(struct MenuDat m);
//void menuEdit(void (*addr)(u32, u8), u8 addroffs, u32* toedit, u8 mode, struct MenuDat mdat, u8*** values, ...);
//u32 menuInputs(s32* cursor, u16 startID, struct InputIDs inputids, struct MenuDat mdat);
#endif
