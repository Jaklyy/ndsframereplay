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

enum CommonReturnVals
{
    RetExit = 255,
    RetScreenshot = 254,
};

struct InputIDs
{
    u16 ScrollUp;
    u16 ScrollDown;

    u16 PageUp;
    u16 PageDown;

    u16 Select;
    u16 Exit;

    u16 Add1;
    u16 Sub1;
    u16 Add10;
    u16 Sub10;

    u16 Reload;
    u16 Screenshot;
    u16 Reset;
};

enum EntryType
{
    Entry_Dummy = 0,
    Entry_Button,
    Entry_Int,
    Entry_Color,
};

enum SubType
{
    // button; value returned when selected
    Sub_Cursor = 0, // use cursor position for return value

    // color; what color the entry represents
    Sub_Red = 0,
    Sub_Green,
    Sub_Blue,
    Sub_Alpha,
};

struct MenuEntry
{
    void (*Addr)(u32, u8);
    u8* String;
    u8** Values;
    u32* Var;
    u16 SubType;
    u8 Type;
    u8 Shift;
    u8 Mask;
    u8 AddrOffs;
};

struct MenuDat
{
    s32* Cursor;
    u8** Headers;
    struct MenuEntry* Entry;
    u16 NumEntries;
    struct InputIDs Inputs;
};

void mapWrite(u8 tile, u8 palette);
void menuWrite(u8* text);
void menuWriteSingle(u8* text);
void mapWriteRev(u8 tile, u8 palette);
void menuWriteRev(u8* text);
void menuClear();
void menuInit();
void menuRender(struct MenuDat m);
u32 menuInputs(struct MenuDat m);
//void menuEdit(void (*addr)(u32, u8), u8 addroffs, u32* toedit, u8 mode, struct MenuDat mdat, u8*** values, ...);
//u32 menuInputs(s32* cursor, u16 startID, struct InputIDs inputids, struct MenuDat mdat);
#endif
