#include <nds.h>




#ifndef MENU_H
#define MENU_H

#define MAP_OFFSET              8192
#define MAP_WIDTH               (256 / 8)
#define MAP_HEIGHT              (192 / 8)
#define MAP_AREA                (MAP_WIDTH * MAP_HEIGHT)
#define PALETTE_SIZE            16

struct InputIDs
{
    u8 B;
    u8 X;
    u8 R;
};

void mapWrite(u8 tile, u8 palette);
void menuWrite(u8* text);
void menuWriteSingle(u8* text);
void mapWriteRev(u8 tile, u8 palette);
void menuWriteRev(u8* text);
void menuClear();
void menuInit();
void menuRender(s8 sel, u8 header, u8 footerL, u8 footerR, int numstr, u8** strings);
void menuEdit(void (*addr)(u32), u32* toedit, u8 numbits, u8 shift, u8 mode, u8 numstr, u8** strings, u8** values);
u16 menuInputs(u16 startID, struct InputIDs inputids, u8 header, u8 footerL, u8 footerR, int numstr, u8** strings);
#endif
