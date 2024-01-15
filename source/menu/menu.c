#include "menu.h"
#include "font.h"




s32 menucursor = 0;

void mapWrite(u8 tile, u8 palette)
{
    BG_GFX_SUB[MAP_OFFSET + menucursor++] = (palette << 12) | tile;
}

void menuWrite(u8* text)
{
    u8 palette = 0;
    for (u16 i = 0; text[i] != 0; i++)
    {
        switch (text[i])
        {
        case '\n':
            {
            s32 temp = ((menucursor >> 5)) << 5; // a way to divide/multiply by 32 without actually dividing/multiplying by 32
            if (temp != menucursor) menucursor = temp + 32;
            break;
            }
        case ((u8)'\xF0') ... ((u8)'\xFF'):
            palette = text[i];
            break;
        default:
            mapWrite(lookup[text[i]], palette);
            break;
        }
    }
}

void menuWriteSingle(u8* text)
{
    menuClear();
    menuWrite(text);
}

void mapWriteRev(u8 tile, u8 palette)
{
    BG_GFX_SUB[MAP_OFFSET + menucursor--] = (palette << 12) | tile;
}

void menuWriteRev(u8* text)
{
    u8 palette = 0;
    u8 numchars = 0;
    for (u16 i = 0; text[i] != 0; i++)
        numchars++;

    for (u16 i = numchars; i > 0; i--)
    {
        switch (text[i])
        {
        /*case '\n':
            menucursor = ((menucursor >> 5) << 5) - 1; // a way to divide/multiply by 32 without actually dividing/multiplying by 32
            break;*/
        case ((u8)'\xF0') ... ((u8)'\xFF'):
            palette = text[i];
            break;
        default:
            mapWriteRev(lookup[text[i]], palette);
            break;
        }
    }
}

void menuClear()
{
    // clear the visible area of the map
    for(int i = 0; i < MAP_AREA; i++)
        BG_GFX_SUB[MAP_OFFSET+i] = 0;
    menucursor = 0;
}

void menuInit()
{
    VRAM_H_CR = VRAM_ENABLE | VRAM_H_SUB_BG;
    VRAM_I_CR = VRAM_ENABLE | VRAM_I_SUB_BG_0x06208000;
    REG_DISPCNT_SUB = DISPLAY_BG0_ACTIVE | (1<<16);
    (*((vu16*)0x04001008)) = (8<<8); // could not find a define for any of the bgcnts
    
    menuClear();
    // allocate tileset to vram
    for(int i = 0; i < max; i++)
        for (int j = 0; j < 32; j+=2)
            BG_GFX_SUB[((16*i)+(j>>1))] = charset[i][j] | charset[i][j+1] << 8;
    // init relevant palettes
    BG_PALETTE_SUB[0] = 0;
    BG_PALETTE_SUB[1] = 0x7FFF;
    BG_PALETTE_SUB[PALETTE_SIZE+0] = 0;
    BG_PALETTE_SUB[PALETTE_SIZE+1] = 0x1F; // red
    BG_PALETTE_SUB[(PALETTE_SIZE*2)+0] = 0;
    BG_PALETTE_SUB[(PALETTE_SIZE*2)+1] = 0x3FF; // yellow
}

void menuRender(u8 sel, u8 header, u8 footerL, u8 footerR, int numstr, u8** strings)
{
    menuClear();
    int j = 0;
    for (int i = 0; i < header; i++)
    {
        menuWrite(strings[j++]);
        sel++;
    }

    numstr -= (footerL + footerR);
    for (;j < numstr; j++)
    {
        if (j == sel) mapWrite(caret, 0);
        else mapWrite(0, 0);
        menuWrite(strings[j]);
    }

    for (int i = 0; i < footerL; i++)
    {
        menucursor = MAP_AREA - (MAP_WIDTH * (footerL - i));
        menuWrite(strings[j++]);
    }
    for (int i = 0; i < footerR; i++)
    {
        menucursor = MAP_AREA - (MAP_WIDTH * (footerR - i + 1) - 1);
        menuWriteRev(strings[j++]);
    }
}

u16 menuInputs(u16 startID, struct InputIDs inputids, u8 header, u8 footerL, u8 footerR, int numstr, u8** strings)
{
    u16 numentries = numstr - header - footerL - footerR;
    s8 cursor = 0;
    bool menudirty = true;
    scanKeys();
    u16 prevkeys = keysHeld();
    while (true)
    {   
        swiWaitForVBlank();
        if (menudirty)
        {
            menuRender(cursor, header, footerL, footerR, numstr, strings);
            menudirty = false;
        }

        scanKeys();
        u16 keys = keysHeld();
        if (keys & KEY_A && !(prevkeys & KEY_A))
        {
            return cursor + startID;
        }
        else if (inputids.B != 0 && keys & KEY_B && !(prevkeys & KEY_B))
        {
            return inputids.B;
        }
        else if (inputids.X != 0 && keys & KEY_X && !(prevkeys & KEY_X))
        {
            return inputids.X;
        }
        else if (inputids.R != 0 && keys & KEY_R && !(prevkeys & KEY_R))
        {
            return inputids.R;
        }
        else if (keys & KEY_UP && !(prevkeys & KEY_UP))
        {
            cursor--;
            if (cursor < 0) cursor = numentries-1;
            menudirty = true;
        }
        else if (keys & KEY_DOWN && !(prevkeys & KEY_DOWN))
        {
            cursor++;
            if (cursor > numentries-1) cursor = 0;
            menudirty = true;
        }
        prevkeys = keys;
    }
}
