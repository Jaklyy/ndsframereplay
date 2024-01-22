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

emun CommonReturnVals
{
    RetExit = 255;
};

struct InputIDs
{
    u8 Up;
    u8 Down;
    u8 Left;
    u8 Right;
    u8 A;
    u8 B;
    u8 X;
    u8 Y;
    u8 Start;
    u8 Select;
    u8 L;
    u8 R;
};

enum EntryType
{
    Entry_Dummy = 0,
    Entry_Button,
    Entry_Int,
    Entry_Color,
};

struct MenuEntry
{
    void (*addr)(u32, u8);
    u8* string;
    u8** values;
    u8 type;
    u8 typedat;
    u8 addroffs;
};

struct MenuDat
{
    s32* cursor;
    u8** Headers;
    u8** FooterLs;
    u8** FooterRs;
    struct MenuEntry* Entry;
    u16 numentries;
    struct InputIDs Inputs;
};

void mapWrite(u8 tile, u8 palette);
void menuWrite(u8* text);
void menuWriteSingle(u8* text);
void mapWriteRev(u8 tile, u8 palette);
void menuWriteRev(u8* text);
void menuClear();
void menuInit();
void menuRender(s32 sel, struct MenuDat mdat);
u32 menuInputs(struct MenuDat m);
//void menuEdit(void (*addr)(u32, u8), u8 addroffs, u32* toedit, u8 mode, struct MenuDat mdat, u8*** values, ...);
//u32 menuInputs(s32* cursor, u16 startID, struct InputIDs inputids, struct MenuDat mdat);
#endif
