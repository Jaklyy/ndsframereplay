#include "menu.h"
#include "font.h"




s16 menucursor = 0;

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
            s16 temp = ((menucursor >> 5)) << 5; // divide/multiply by 32
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
    numchars--;

    for (u16 i = numchars; i > 0; i--)
    {
        switch (text[i])
        {
        case '\n':
            break;
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

void menuRender(s32 sel, struct MenuDat mdat)
{
    menuClear();
    s32 j = 0;
    for (int i = 0; i < mdat.header; i++)
    {
        menuWrite(mdat.strings[j++]);
        sel++;
    }

    mdat.numstr -= (mdat.footerL + mdat.footerR);
    for (;j < mdat.numstr; j++)
    {
        if (j == sel) mapWrite(caret, 0);
        else mapWrite(0, 0);
        menuWrite(mdat.strings[j]);
    }

    for (int i = 0; i < mdat.footerL; i++)
    {
        menucursor = MAP_AREA - (MAP_WIDTH * (mdat.footerL - i));
        menuWrite(mdat.strings[j++]);
    }
    for (int i = 0; i < mdat.footerR; i++)
    {
        menucursor = MAP_AREA - (MAP_WIDTH * (mdat.footerR - i - 1) + 1);
        menuWriteRev(mdat.strings[j++]);
    }
}

void menuEdit(void (*addr)(u32, u8), u8 addroffs, u32* toedit, u8 numbits, u8 shift, u8 mode, struct MenuDat mdat, u8** values)
{
    u16 numentries = mdat.numstr - mdat.header - mdat.footerL - mdat.footerR;

    // set up a bit mask
    u32 mask = 0;
    for (int i = 0; i < numbits; i++)
    {
        mask <<= 1;
        mask |= 0x1;
    }

    u8* strend[16];
    u32 variable[16];
    
    for (int i = 0; i < numentries; i++)
    {
        strend[i] = strrchr(mdat.strings[i+1], ' ');
        variable[i] = ((*toedit >> shift) >> (numbits * i)) & mask;
    }
    
    if (mode == 1)
    {
        BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] = variable[0] | variable[1] << 5 | variable[2] << 10;
        BG_PALETTE_SUB[(PALETTE_SIZE*15)+2] = variable[0] & mask;
        BG_PALETTE_SUB[(PALETTE_SIZE*15)+3] = (variable[1] & mask) << 5;
        BG_PALETTE_SUB[(PALETTE_SIZE*15)+4] = (variable[2] & mask) << 10;
    }

    bool menudirty = true;
    scanKeys();
    u16 prevkeys = keysHeld();
    s32 cursor = 0;
    while (true)
    {
        swiWaitForVBlank();
        if (menudirty)
        {
            menuRender((numentries <= 1 ? -1 : cursor), mdat);
            menudirty = false;
        }

        scanKeys();
        u16 keys = keysHeld();

        if (keys & KEY_RIGHT && !(prevkeys & KEY_RIGHT))
        {
            variable[cursor] = (variable[cursor] + 1) & mask;

            if (values == NULL || values[variable[cursor]] == NULL)
                sprintf(strend[cursor], " %li\n", variable[cursor]);
            else strcpy(strend[cursor], values[variable[cursor]]);

            if (mode == 1)
            {
                BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] = variable[0] | variable[1] << 5 | variable[2] << 10;
                BG_PALETTE_SUB[(PALETTE_SIZE*15)+cursor+2] = variable[cursor] << (cursor * 5);
                
                *toedit = variable[0] | variable[1] << 5 | variable[2] << 10;
            }
            else
            {
                *toedit &= ~(mask << shift);
                *toedit |= (variable[cursor] << shift);
            }
            if (addr != NULL) (*addr)(*toedit, addroffs);
            menudirty = true;
        }
        else if (keys & KEY_LEFT && !(prevkeys & KEY_LEFT))
        {
            variable[cursor] = (variable[cursor] - 1) & mask;

            if (values == NULL || values[variable[cursor]] == NULL)
                sprintf(strend[cursor], " %li\n", variable[cursor]);
            else strcpy(strend[cursor], values[variable[cursor]]);


            if (mode == 1)
            {
                BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] = variable[0] | variable[1] << 5 | variable[2] << 10;
                BG_PALETTE_SUB[(PALETTE_SIZE*15)+cursor+2] = variable[cursor] << (cursor * 5);
                
                *toedit = variable[0] | variable[1] << 5 | variable[2] << 10;
            }
            else
            {
                *toedit &= ~(mask << shift);
                *toedit |= (variable[cursor] << shift);
            }
            if (addr != NULL) (*addr)(*toedit, addroffs);
            menudirty = true;
        }
        else if (keys & KEY_SELECT && !(prevkeys & KEY_SELECT))
        {
            runDump(true);
        }
        else if (keys & KEY_B && !(prevkeys & KEY_B))
        {
            return;
        }
        if (numbits > 1)
        {
            if (keys & KEY_R && !(prevkeys & KEY_R))
            {
                if (variable[cursor] != mask)
                {
                    variable[cursor] += 10;
                    if (variable[cursor] > mask) variable[cursor] = mask;
                }
                else variable[cursor] = 0;

                if (values == NULL || values[variable[cursor]] == NULL)
                    sprintf(strend[cursor], " %li\n", variable[cursor]);
                else strcpy(strend[cursor], values[variable[cursor]]);

                if (mode == 1)
                {
                    BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] = variable[0] | variable[1] << 5 | variable[2] << 10;
                    BG_PALETTE_SUB[(PALETTE_SIZE*15)+cursor+2] = variable[cursor] << (cursor * 5);

                    *toedit = variable[0] | variable[1] << 5 | variable[2] << 10;
                }
                else
                {
                    *toedit &= ~(mask << shift);
                    *toedit |= (variable[cursor] << shift);
                }
                if (addr != NULL) (*addr)(*toedit, addroffs);
                menudirty = true;
            }
            else if (keys & KEY_L && !(prevkeys & KEY_L))
            {
                if (variable[cursor] != 0)
                {
                    variable[cursor] -= 10;
                    if (variable[cursor] > mask) variable[cursor] = 0;
                }
                else variable[cursor] = mask;

                if (values == NULL || values[variable[cursor]] == NULL)
                    sprintf(strend[cursor], " %li\n", variable[cursor]);
                else strcpy(strend[cursor], values[variable[cursor]]);

                if (mode == 1)
                {
                    BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] = variable[0] | variable[1] << 5 | variable[2] << 10;
                    BG_PALETTE_SUB[(PALETTE_SIZE*15)+cursor+2] = variable[cursor] << (cursor * 5);

                    *toedit = variable[0] | variable[1] << 5 | variable[2] << 10;
                }
                else
                {
                    *toedit &= ~(mask << shift);
                    *toedit |= (variable[cursor] << shift);
                }
                if (addr != NULL) (*addr)(*toedit, addroffs);
                menudirty = true;
            }
        }
        if (numentries > 1)
        {
            if (keys & KEY_UP && !(prevkeys & KEY_UP))
            {
                cursor -= 1;
                if (cursor < 0) cursor = numentries-1;
                menudirty = true;
            }
            else if (keys & KEY_DOWN && !(prevkeys & KEY_DOWN))
            {
                cursor += 1;
                if (cursor > numentries-1) cursor = 0;
                menudirty = true;
            }
        }

        prevkeys = keys;
    }
}

u32 menuInputs(s32* cursor, u16 startID, struct InputIDs inputids, struct MenuDat mdat)
{
    u16 numentries = mdat.numstr - mdat.header - mdat.footerL - mdat.footerR;
    bool menudirty = true;
    scanKeys();
    u16 prevkeys = keysHeld();
    while (true)
    {   
        swiWaitForVBlank();
        if (menudirty)
        {
            menuRender(*cursor, mdat);
            menudirty = false;
        }

        scanKeys();
        u16 keys = keysHeld();
        if (keys & KEY_A && !(prevkeys & KEY_A))
        {
            return *cursor + startID;
        }
        else if (inputids.B != 0 && keys & KEY_B && !(prevkeys & KEY_B))
        {
            return inputids.B;
        }
        else if (inputids.Select != 0 && keys & KEY_SELECT && !(prevkeys & KEY_SELECT))
        {
            return inputids.Select;
        }
        else if (inputids.R != 0 && keys & KEY_R && !(prevkeys & KEY_R))
        {
            return inputids.R;
        }
        else if (keys & KEY_UP && !(prevkeys & KEY_UP))
        {
            (*cursor)--;
            if (*cursor < 0) *cursor = numentries-1;
            menudirty = true;
        }
        else if (keys & KEY_DOWN && !(prevkeys & KEY_DOWN))
        {
            (*cursor)++;
            if (*cursor > numentries-1) *cursor = 0;
            menudirty = true;
        }
        prevkeys = keys;
    }
}
