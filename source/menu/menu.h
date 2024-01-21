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

struct InputIDs
{
    u8 B;
    u8 Select;
    u8 R;
};

struct MenuDat
{
    u8 header;
    u8 footerL;
    u8 footerR;
    s32 numstr;
    u8** strings;
};

void mapWrite(u8 tile, u8 palette);
void menuWrite(u8* text);
void menuWriteSingle(u8* text);
void mapWriteRev(u8 tile, u8 palette);
void menuWriteRev(u8* text);
void menuClear();
void menuInit();
void menuRender(s32 sel, struct MenuDat mdat);
void menuEdit(void (*addr)(u32, u8), u8 addroffs, u32* toedit, u8 numbits, u8 shift, u8 mode, struct MenuDat mdat, u8** values);
u32 menuInputs(s32* cursor, u16 startID, struct InputIDs inputids, struct MenuDat mdat);
#endif
