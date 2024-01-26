#include "menu.h"
#include "font.h"
#include "../strings.h"



s16 menucursor = 0;

struct MenuInputs InputsCommon = {0, 0, 0};

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
    menuWrite(text, Pal_Normal);
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

void menuRender(struct MenuDat m)
{
    #define ENTRY   m.Entry[i]
    menuClear();
    // add menu footers
    int i = 0;
    u8 string[32] = "";
    if (!m.Inputs.DisableExit)
    {
        sprintf(string, "%s: %s", str_input_b, str_hint_back);

        menucursor = MAP_AREA - (MAP_WIDTH * (i+1));
        menuWrite(string, Pal_Header);
        i++;
    }
    if (!m.Inputs.DisableReload)
    {
        sprintf(string, "%s: %s", str_input_select, str_hint_reload);

        menucursor = MAP_AREA - (MAP_WIDTH * (i+1));
        menuWrite(string, Pal_Header);
        i++;
    }
    if (m.Entry[*m.Cursor].Type == Entry_Var)
    {
        sprintf(string, "%s: %s", str_input_l, str_hint_subt10);

        menucursor = MAP_AREA - (MAP_WIDTH * (i+1));
        menuWrite(string, Pal_Header);
        i++;

        sprintf(string, "%s: %s", str_input_y, str_hint_subt);

        menucursor = MAP_AREA - (MAP_WIDTH * (i+1));
        menuWrite(string, Pal_Header);
        i++;
    }

    int j = 0;
    if (m.Entry[*m.Cursor].Type == Entry_Button)
    {
        sprintf(string, "%s: %s", str_input_a, str_hint_sel);

        menucursor = MAP_AREA - (MAP_WIDTH * j + 1);
        menuWriteRev(string, Pal_Header);
        j++;
    }
    if (!m.Inputs.DisableReset)
    {
        sprintf(string, "%s: %s", str_input_start, str_hint_reset);
        menucursor = MAP_AREA - (MAP_WIDTH * j + 1);
        menuWriteRev(string, Pal_Header);
        j++;
    }
    if (m.Entry[*m.Cursor].Type == Entry_Var)
    {
        sprintf(string, "%s: %s", str_input_r, str_hint_add10);

        menucursor = MAP_AREA - (MAP_WIDTH * j + 1);
        menuWriteRev(string, Pal_Header);
        j++;

        sprintf(string, "%s: %s", str_input_x, str_hint_add);

        menucursor = MAP_AREA - (MAP_WIDTH * j + 1);
        menuWriteRev(string, Pal_Header);
        j++;
    }

    // menu headers
    menucursor = 0;
    int k = 0;
    for (; m.Headers[k] != NULL; k++)
    {
        menuWrite(m.Headers[k], Pal_Header);
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

        if (ENTRY.Type == Entry_Var)
        {
            if (ENTRY.SubType != Sub_Normal)
            {
                u8 string[31] = "";
                u8 palette = 0;
                u16 color = ENTRY.VarFrag;
                if (ENTRY.Mask > 31) color >>= 1; // assumes 6 bit color is the max
                switch (ENTRY.SubType)
                {
                    case Sub_Red:
                        palette = '\xC2';
                        BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] = (BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] & ~0x1F) | color;
                        break;
                    case Sub_Green:
                        palette = '\xC3';
                        color <<= 5;
                        BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] = (BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] & ~(0x1F<<5)) | color;
                        break;
                    case Sub_Blue:
                        palette = '\xC4';
                        color <<= 10;
                        BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] = (BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] & ~(0x1F<<10)) | color;
                        break;
                    case Sub_Alpha:
                        palette = '\xC5';
                        color = color | color << 5 | color << 10;
                        break;
                }
                BG_PALETTE_SUB[(PALETTE_SIZE*15) + 1 + ENTRY.SubType] = color;
                
                sprintf(string, "%s \xFF%c\xF0 %li\n", ENTRY.String, palette, ENTRY.VarFrag);
                menuWrite(string, Pal_Normal);
            }
            else if (ENTRY.Type == Entry_Var)
            {
                u8 string[31] = "";
                if (ENTRY.Values == NULL || ENTRY.Values[ENTRY.VarFrag] == NULL)
                    sprintf(string, "%s %li\n", ENTRY.String, ENTRY.VarFrag);
                else sprintf(string, "%s %s\n", ENTRY.String, ENTRY.Values[ENTRY.VarFrag]);
                menuWrite(string, Pal_Normal);
            }
        }
        else menuWrite(ENTRY.String, Pal_Normal);
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
            if (MENU_UP & keysrep)
            {
                (*m.Cursor)--;
                if (*m.Cursor < 0) *m.Cursor = m.NumEntries-1;
                menudirty = true;
            }
            if (MENU_DOWN & keysrep)
            {
                (*m.Cursor)++;
                if (*m.Cursor > m.NumEntries-1 || *m.Cursor < 0) *m.Cursor = 0;
                menudirty = true;
            }
            if (MENU_PAGEUP & keysrep)
            {
                *m.Cursor -= 10;
                if (*m.Cursor < 0) *m.Cursor = 0;
                menudirty = true;
            }
            if (MENU_PAGEDOWN & keysrep)
            {
                *m.Cursor += 10;
                if (*m.Cursor > m.NumEntries-1 || *m.Cursor < 0) *m.Cursor = m.NumEntries-1;
                menudirty = true;
            }
        }
        if ((ENTRY.Type == Entry_Button))
        {
            if (MENU_SELECT & keys)
            {
                if (ENTRY.SubType == 0) return *m.Cursor;
                else return ENTRY.SubType;
            }
        }
        else if (ENTRY.Type == Entry_Var)
        {
            // check if the variable should be edited
            bool update = false;
            if (MENU_ADD1 & keysrep)
            {   
                // increment variable
                ENTRY.VarFrag++;
                ENTRY.VarFrag &= ENTRY.Mask;

                update = true;
            }
            if (MENU_SUB1 & keysrep)
            {
                ENTRY.VarFrag--;
                ENTRY.VarFrag &= ENTRY.Mask;

                update = true;
            }
            if (MENU_ADD10 & keysrep)
            {
                if (ENTRY.VarFrag != ENTRY.Mask)
                {
                    ENTRY.VarFrag += 10;
                    if (ENTRY.VarFrag > ENTRY.Mask) ENTRY.VarFrag = ENTRY.Mask;
                }
                else ENTRY.VarFrag = 0;

                update = true;
            }
            if (MENU_SUB10 & keysrep)
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
        if (!m.Inputs.DisableReload && MENU_RELOAD & keys)
        {
            runDump(true);
        }
        if (!m.Inputs.DisableReset && MENU_RESET & keys)
        {
            // TODO
        }
        if (!m.Inputs.DisableExit && MENU_BACK & keys)
        {
            return Ret_Exit;
        }
    }
    #ifdef ENTRY
    #undef ENTRY
    #endif
}

/*void menuColor(u8* header, u8* string, u16* colorvar, u16 numcolors)
{
    u8* headers[] = 
    {
        header,
        NULL,
    };
    struct MenuEntry entries* = malloc(numcolors);
    memset(entries, 0, sizeof(struct MenuEntry) * numcolors);

    for (int i = 0; i < numcolors; i++)
    {
        u8 color = 0xC0 | (i+1);
        entries[i].String = malloc(31);
        entries[i].Type = Entry_Button;
        sprintf(entries[i].String, "%s %i: \xFF%c\n", string, i, color);
    }

    u8* headers2[] = 
    {
        str_sub_color,
        NULL,
    };

    s32 cursor = 0;
    while (true)
    {
        for (int i = 0; i < numcolors; i++)
            BG_PALETTE_SUB[(PALETTE_SIZE*15)+1+i] = colorvar[i];

        u32 sel = menuInputs((struct MenuDat){&cursor, headers, entries, ARRSIZE(entries), InputsCommon});

        if (sel == Ret_Exit) break;
        else
        {

            u32 var = edgecolor[sel];
            struct MenuEntry entries2[] =
            {
                {
                    .Addr = inputEdgeColor,
                    .AddrOffs = sel,
                    .String = str_opt_r,
                    .Type = Entry_Var,
                    .SubType = Sub_Red,
                    .Var = &var,
                    .Shift = 0,
                    .Mask = 0x1F,
                },
                {
                    .Addr = inputEdgeColor,
                    .AddrOffs = sel,
                    .String = str_opt_g,
                    .Type = Entry_Var,
                    .SubType = Sub_Green,
                    .Var = &var,
                    .Shift = 5,
                    .Mask = 0x1F,
                },
                {
                    .Addr = inputEdgeColor,
                    .AddrOffs = sel,
                    .String = str_opt_b,
                    .Type = Entry_Var,
                    .SubType = Sub_Blue,
                    .Var = &var,
                    .Shift = 10,
                    .Mask = 0x1F,
                },
            };
            u32 cursor2 = 0;
            menuInputs((struct MenuDat){&cursor2, headers2, entries2, ARRSIZE(entries2), InputsCommon});
            edgecolor[sel] = var;
        }
    }

    for (int i = 0; i < 8; i++)
        free(entries[i].String);
}*/
