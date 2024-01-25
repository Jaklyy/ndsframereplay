#include "menu.h"
#include "font.h"
#include "../strings.h"



s16 menucursor = 0;

void mapWrite(u8 tile, u8 palette)
{
    BG_GFX_SUB[MAP_OFFSET + menucursor++] = (palette << 12) | tile;
}

void menuWrite(u8* text, u8 palette)
{
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
    menuWrite(text, PalNormal);
}

void mapWriteRev(u8 tile, u8 palette)
{
    BG_GFX_SUB[MAP_OFFSET + menucursor--] = (palette << 12) | tile;
}

void menuWriteRev(u8* text, u8 palette)
{
    u8 numchars = 0;
    for (u16 i = 0; text[i] != 0; i++)
        numchars++;
    numchars--;

    for (s32 i = numchars; i >= 0; i--)
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

u8 lookupKey(u16 input)
{
    switch (input)
    {
        case 0:
            return 12;
        case KEY_A:
            return 0;
        case KEY_B:
            return 1;
        case KEY_SELECT:
            return 2;
        case KEY_START:
            return 3;
        case KEY_RIGHT:
            return 4;
        case KEY_LEFT:
            return 5;
        case KEY_UP:
            return 6;
        case KEY_DOWN:
            return 7;
        case KEY_R:
            return 8;
        case KEY_L:
            return 9;
        case KEY_X:
            return 10;
        case KEY_Y:
            return 11;
        default:
            return 12;
    }
}

void menuRender(struct MenuDat m)
{
    #define ENTRY   m.Entry[i]

    menuClear();
    // add menu footers
    int i = 0;
    u8 string[32] = "";
    u8 input = lookupKey(m.Inputs.Exit);
    if (input < 12)
    {
        sprintf(string, "%s: %s", str_inputs[input], str_hint_back);

        menucursor = MAP_AREA - (MAP_WIDTH * (i+1));
        menuWrite(string, PalHeader);
        i++;
    }
    input = lookupKey(m.Inputs.Reload);
    if (input < 12)
    {
        sprintf(string, "%s: %s", str_inputs[input], str_hint_reload);

        menucursor = MAP_AREA - (MAP_WIDTH * (i+1));
        menuWrite(string, PalHeader);
        i++;
    }
    input = lookupKey(m.Inputs.Sub10);
    if (input < 12)
    {
        sprintf(string, "%s: %s", str_inputs[input], str_hint_subt10);

        menucursor = MAP_AREA - (MAP_WIDTH * (i+1));
        menuWrite(string, PalHeader);
        i++;
    }
    input = lookupKey(m.Inputs.Sub1);
    if (input < 12)
    {
        sprintf(string, "%s: %s", str_inputs[input], str_hint_subt);

        menucursor = MAP_AREA - (MAP_WIDTH * (i+1));
        menuWrite(string, PalHeader);
        i++;
    }

    int j = 0;
    input = lookupKey(m.Inputs.Select);
    if (input < 12)
    {
        sprintf(string, "%s: %s", str_inputs[input], str_hint_sel);

        menucursor = MAP_AREA - (MAP_WIDTH * j + 1);
        menuWriteRev(string, PalHeader);
        j++;
    }
    input = lookupKey(m.Inputs.Screenshot);
    if (input < 12)
    {
        sprintf(string, "%s: %s", str_inputs[input], str_hint_screenshot);
        menucursor = MAP_AREA - (MAP_WIDTH * j + 1);
        menuWriteRev(string, PalHeader);
        j++;
    }
    input = lookupKey(m.Inputs.Add10);
    if (input < 12)
    {
        sprintf(string, "%s: %s", str_inputs[input], str_hint_add10);

        menucursor = MAP_AREA - (MAP_WIDTH * j + 1);
        menuWriteRev(string, PalHeader);
        j++;
    }
    input = lookupKey(m.Inputs.Add1);
    if (input < 12)
    {
        sprintf(string, "%s: %s", str_inputs[input], str_hint_add);

        menucursor = MAP_AREA - (MAP_WIDTH * j + 1);
        menuWriteRev(string, PalHeader);
        j++;
    }

    // menu headers
    menucursor = 0;
    int k = 0;
    for (; m.Headers[k] != NULL; k++)
    {
        menuWrite(m.Headers[k], PalHeader);
    }
    
    // menu entries
    int numfooters = (i > j) ? i : j;
    int menustart;
    int menuend;
    int menusize = MAP_HEIGHT - numfooters - k;
    int bothalf = menusize >> 1;
    int tophalf = (bothalf * 2 + numfooters + k == MAP_HEIGHT) ? bothalf : (bothalf+1);
    if (m.NumEntries > menusize)
    {
        if (*m.Cursor <= bothalf)
        {
            menustart = 0;
            menuend = menusize;
        }
        else if (*m.Cursor >= (m.NumEntries - tophalf))
        {
            menustart = m.NumEntries - menusize;
            menuend = m.NumEntries;
        }
        else
        {
            menustart = *m.Cursor - bothalf;
            menuend = *m.Cursor + tophalf;
        }
    }
    else
    {
        menustart = 0;
        menuend = m.NumEntries;
    }

    for (int i = menustart; i < menuend; i++)
    {
        if (i == *m.Cursor) mapWrite(caret, 0);
        else mapWrite(0, 0);

        if (ENTRY.Type == Entry_Color)
        {
            u8 string[31] = "";
            u8 sub = ENTRY.SubType;
            u8 palette = 0;
            u16 color = (*ENTRY.Var >> ENTRY.Shift) & ENTRY.Mask;
            if (ENTRY.Mask > 31) color >>= 1; // assumes 6 bit color is the max
            switch (sub)
            {
                case Sub_Red:
                    palette = '\xF2';
                    BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] = (BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] & ~0x1F) | color;
                    break;
                case Sub_Green:
                    palette = '\xF3';
                    color <<= 5;
                    BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] = (BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] & ~(0x1F<<5)) | color;
                    break;
                case Sub_Blue:
                    palette = '\xF4';
                    BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] = (BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] & ~(0x1F<<10)) | color;
                    color <<= 10;
                    break;
                case Sub_Alpha:
                    palette = '\xF5';
                    color = color | color << 5 | color << 10;
                    break;
            }
            BG_PALETTE_SUB[(PALETTE_SIZE*15) + 2 + sub] = color;
            
            sprintf(string, "%s \xFF%c\xF0 %i", ENTRY.String, palette, color);
            menuWrite(string, PalNormal);
        }
        else if (ENTRY.Type == Entry_Int)
        {
            u8 string[31] = "";
            if (ENTRY.Values == NULL || ENTRY.Values[ENTRY.VarFrag] == NULL)
                sprintf(string, "%s %li\n", ENTRY.String, ENTRY.VarFrag);
            else sprintf(string, "%s %s\n", ENTRY.String, ENTRY.Values[ENTRY.VarFrag]);
            menuWrite(string, PalNormal);
        }
        else menuWrite(ENTRY.String, PalNormal);
    }

    #ifdef ENTRY
    #undef ENTRY
    #endif
}

u32 menuInputs(struct MenuDat m)
{
    #define ENTRY   m.Entry[*m.Cursor]

    bool menudirty = true;

    // deconstruct variable
    for (int i = 0; i < m.NumEntries; i++)
        m.Entry[i].VarFrag = (*m.Entry[i].Var >> m.Entry[i].Shift) & m.Entry[i].Mask;

    while (true)
    {
        swiWaitForVBlank();
        if (menudirty)
        {
            menuRender(m);
            menudirty = false;
        }

        scanKeys();
        u16 keys = keysDown();
        u16 keysrep = keysDownRepeat();
        
        if (m.NumEntries > 1)
        {
            if (m.Inputs.ScrollUp & keysrep)
            {
                (*m.Cursor)--;
                if (*m.Cursor < 0) *m.Cursor = m.NumEntries-1;
                menudirty = true;
            }
            if (m.Inputs.ScrollDown & keysrep)
            {
                (*m.Cursor)++;
                if (*m.Cursor > m.NumEntries-1) *m.Cursor = 0;
                menudirty = true;
            }
            if (m.Inputs.PageUp & keysrep)
            {
                // todo
            }
            if (m.Inputs.PageDown & keysrep)
            {
                // todo
            }
        }
        if ((ENTRY.Type == Entry_Button) && (m.Inputs.Select & keys))
        {
            if (ENTRY.SubType == 0) return *m.Cursor;
            else return ENTRY.SubType;
        }
        if ((ENTRY.Type == Entry_Int || ENTRY.Type == Entry_Color))
        {
            // check if the variable should be edited
            bool update = false;
            if (m.Inputs.Add1 & keysrep)
            {   
                // increment variable
                ENTRY.VarFrag++;
                ENTRY.VarFrag &= ENTRY.Mask;

                update = true;
            }
            if (m.Inputs.Sub1 & keysrep)
            {
                ENTRY.VarFrag--;
                ENTRY.VarFrag &= ENTRY.Mask;

                update = true;
            }
            if (m.Inputs.Add10 & keysrep)
            {
                if (ENTRY.VarFrag != ENTRY.Mask)
                {
                    ENTRY.VarFrag += 10;
                    if (ENTRY.VarFrag > ENTRY.Mask) ENTRY.VarFrag = ENTRY.Mask;
                }
                else ENTRY.VarFrag = 0;

                update = true;
            }
            if (m.Inputs.Sub10 & keysrep)
            {
                if (ENTRY.VarFrag != 0)
                {
                    ENTRY.VarFrag -= 10;
                    if (ENTRY.VarFrag > ENTRY.Mask) ENTRY.VarFrag = 0;
                }
                else ENTRY.VarFrag = ENTRY.Mask;

                update = true;
            }
            // update vars if an update is queued
            if (update)
            {
                // reconstruct variable to update
                *ENTRY.Var = (*ENTRY.Var & ~(ENTRY.Mask << ENTRY.Shift)) | ENTRY.VarFrag << ENTRY.Shift;

                // update associated register if provided
                if (ENTRY.Addr != NULL) (*ENTRY.Addr)(*ENTRY.Var, ENTRY.AddrOffs);

                menudirty = true;
            }
        }
        if (m.Inputs.Reload & keys)
        {
            runDump(true);
        }
        if (m.Inputs.Screenshot & keys)
        {
            return RetScreenshot;
        }
        if (m.Inputs.Exit & keys)
        {
            return RetExit;
        }
    }
    #ifdef ENTRY
    #undef ENTRY
    #endif
}
