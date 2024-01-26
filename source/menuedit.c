#include "menus.h"




void inputFogOffset(u32 var, __attribute__((unused)) u8 offset)
{
    GFX_FOG_OFFSET = var;
}

void inputClearDepth(u32 var, u8 offset)
{
    (&GFX_CLEAR_DEPTH)[offset] = var;
}

void inputClearColor(u32 var, __attribute__((unused)) u8 offset)
{
    GFX_CLEAR_COLOR = var;
}

void inputAlphaRef(u32 var, __attribute__((unused)) u8 offset)
{
    GFX_ALPHA_TEST = var;
}

void inputEdgeColor(u32 var, u8 offset)
{
    GFX_EDGE_TABLE[offset] = var;
}

void input3DDispCnt(u32 var, __attribute__((unused)) u8 offset)
{
    GFX_CONTROL = var;
}

void menuEditGX()
{
    struct MenuEntry* entries = calloc(numcmds, sizeof(struct MenuEntry));
    for (u32 i = 0; i < numcmds; i++)
        entries[i].String = str_cmds[cmd[i]];

    u8* headers[] =
    {
        str_sub_gx,
        NULL,
    };

    s32 cursor = 0;
    menuInputs((struct MenuDat){&cursor, headers, entries, numcmds, InputsCommon});

    free(entries);
}

void menuClearColor()
{
    u8* headers[] = 
    {
        str_sub_clearcolor,
        NULL,
    };

    struct MenuEntry entries[] =
    {
        {
            .Addr = inputClearColor,
            .String = str_opt_r,
            .Type = Entry_Var,
            .SubType = Sub_Red,
            .Var = &clearcolor,
            .Shift = 0,
            .Mask = 0x1F,
        },
        {
            .Addr = inputClearColor,
            .String = str_opt_g,
            .Type = Entry_Var,
            .SubType = Sub_Green,
            .Var = &clearcolor,
            .Shift = 5,
            .Mask = 0x1F,
        },
        {
            .Addr = inputClearColor,
            .String = str_opt_b,
            .Type = Entry_Var,
            .SubType = Sub_Blue,
            .Var = &clearcolor,
            .Shift = 10,
            .Mask = 0x1F,
        },
        {
            .Addr = inputClearColor,
            .String = str_opt_fog,
            .Values = str_state,
            .Type = Entry_Var,
            .Var = &clearcolor,
            .Shift = 15,
            .Mask = 0x1,
        },
        {
            .Addr = inputClearColor,
            .String = str_opt_a,
            .Type = Entry_Var,
            .SubType = Sub_Alpha,
            .Var = &clearcolor,
            .Shift = 16,
            .Mask = 0x1F,
        },
        {
            .Addr = inputClearColor,
            .String = str_opt_id,
            .Type = Entry_Var,
            .Var = &clearcolor,
            .Shift = 24,
            .Mask = 0x3F,
        },
    };
    s32 cursor = 0;
    menuInputs((struct MenuDat){&cursor, headers, entries, ARRSIZE(entries), InputsCommon});
}

void menuEdgeColor()
{

    u8* headers[] = 
    {
        str_sub_edgecolor,
        NULL,
    };
    struct MenuEntry entries[8];
    memset(entries, 0, sizeof(entries));

    for (int i = 0; i < 8; i++)
    {
        u8 color = 0xC0 | (i+1);
        entries[i].String = malloc(18);
        entries[i].Type = Entry_Button;
        sprintf(entries[i].String, "EDGE COLOR %i: \xFF%c\n", i, color);
    }

    u8* headers2[] = 
    {
        str_sub_color,
        NULL,
    };

    s32 cursor = 0;
    while (true)
    {
        BG_PALETTE_SUB[(PALETTE_SIZE*15)+1] = edgecolor[0];
        BG_PALETTE_SUB[(PALETTE_SIZE*15)+2] = edgecolor[1];
        BG_PALETTE_SUB[(PALETTE_SIZE*15)+3] = edgecolor[2];
        BG_PALETTE_SUB[(PALETTE_SIZE*15)+4] = edgecolor[3];
        BG_PALETTE_SUB[(PALETTE_SIZE*15)+5] = edgecolor[4];
        BG_PALETTE_SUB[(PALETTE_SIZE*15)+6] = edgecolor[5];
        BG_PALETTE_SUB[(PALETTE_SIZE*15)+7] = edgecolor[6];
        BG_PALETTE_SUB[(PALETTE_SIZE*15)+8] = edgecolor[7];

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
}

void menu3DDispCnt()
{
    u8* headers[] =
    {
        str_sub_3ddispcnt,
        NULL,
    };

    u32 var = disp3dcnt;
    struct MenuEntry entries[] =
    {
        {
            .Addr = input3DDispCnt,
            .String = str_3ddispcnt[0],
            .Values = str_state,
            .Var = &var,
            .Type = Entry_Var,
            .Shift = 0,
            .Mask = 0x1,
        },
        {
            .Addr = input3DDispCnt,
            .String = str_3ddispcnt[1],
            .Values = str_shading,
            .Var = &var,
            .Type = Entry_Var,
            .Shift = 1,
            .Mask = 0x1,
        },
        {
            .Addr = input3DDispCnt,
            .String = str_3ddispcnt[2],
            .Values = str_state,
            .Var = &var,
            .Type = Entry_Var,
            .Shift = 2,
            .Mask = 0x1,
        },
        {
            .Addr = input3DDispCnt,
            .String = str_3ddispcnt[3],
            .Values = str_state,
            .Var = &var,
            .Type = Entry_Var,
            .Shift = 3,
            .Mask = 0x1,
        },
        {
            .Addr = input3DDispCnt,
            .String = str_3ddispcnt[4],
            .Values = str_state,
            .Var = &var,
            .Type = Entry_Var,
            .Shift = 4,
            .Mask = 0x1,
        },
        {
            .Addr = input3DDispCnt,
            .String = str_3ddispcnt[5],
            .Values = str_state,
            .Var = &var,
            .Type = Entry_Var,
            .Shift = 5,
            .Mask = 0x1,
        },
        {
            .Addr = input3DDispCnt,
            .String = str_3ddispcnt[6],
            .Values = str_fogmode,
            .Var = &var,
            .Type = Entry_Var,
            .Shift = 6,
            .Mask = 0x1,
        },
        {
            .Addr = input3DDispCnt,
            .String = str_3ddispcnt[7],
            .Values = str_state,
            .Var = &var,
            .Type = Entry_Var,
            .Shift = 7,
            .Mask = 0x1,
        },
        {
            .Addr = input3DDispCnt,
            .String = str_3ddispcnt[8],
            .Var = &var,
            .Type = Entry_Var,
            .Shift = 8,
            .Mask = 0xF,
        },
        {
            .Addr = input3DDispCnt,
            .String = str_3ddispcnt[9],
            .Values = str_rearplane,
            .Var = &var,
            .Type = Entry_Var,
            .Shift = 14,
            .Mask = 0x1,
        },
    };

    s32 cursor = 0;
    menuInputs((struct MenuDat){&cursor, headers, entries, ARRSIZE(entries), InputsCommon});

    disp3dcnt = var;
}

void menuEditGlobals()
{
    u32 varat = alphatest;
    u32 varofs = fogoffset;
    struct MenuEntry entries[] =
    {
        {
            .String = str_opt_3ddispcnt,
            .Type = Entry_Button,
        },
        {
            .String = str_opt_edgecolor,
            .Type = Entry_Button,
        },
        {
            .Addr = inputAlphaRef,
            .String = str_opt_alphatest,
            .Var = &varat,
            .Type = Entry_Var,
            .Mask = 0x1F,
        },
        {
            .String = str_opt_clearcolor,
            .Type = Entry_Button,
        },
        {
            .Addr = inputClearDepth,
            .AddrOffs = 0,
            .String = str_opt_cleardepth,
            .Type = Entry_Var,
            .Var = &cleardepoff,
            .Mask = 0x7FFF,
        },
        {
            .Addr = inputClearDepth,
            .AddrOffs = 1,
            .String = str_opt_clearoffsetx,
            .Type = Entry_Var,
            .Var = &cleardepoff,
            .Mask = 0xFF,
            .Shift = 16,
        },
        {
            .Addr = inputClearDepth,
            .AddrOffs = 1,
            .String = str_opt_clearoffsety,
            .Type = Entry_Var,
            .Var = &cleardepoff,
            .Mask = 0xFF,
            .Shift = 24,
        },
        {
            .String = str_opt_fogcolor,
            .Type = Entry_Button,
        },
        {
            .Addr = inputFogOffset,
            .String = str_opt_fogoffset,
            .Type = Entry_Var,
            .Var = &varofs,
            .Mask = 0x7FFF,
        },
        {
            .String = str_opt_fogtable,
            .Type = Entry_Button,
        },
        {
            .String = str_opt_toontable,
            .Type = Entry_Button,
        },
    };

    u8* headers[] =
    {
        str_menu_globalvars,
        NULL,
    };

    s32 cursor = 0;
    while (true)
    {
        u32 sel = menuInputs((struct MenuDat){&cursor, headers, entries, ARRSIZE(entries), InputsCommon});

        switch (sel)
        {
            case 0: // 3d display control
                menu3DDispCnt();
                break;
            case 1: // edge colors
                menuEdgeColor();
                break;
            case 3: // clear color
                menuClearColor();
                break;
            case 7: // fog color
                //menuFogColor();
                break;
            case 9: // fog table
                //menuFogTable();
                break;
            case 10: // toon table
                //menuToonTable();
                break;
            case Ret_Exit: // exit
                alphatest = varat;
                fogoffset = varofs;
                return;
        }
    }
}

void menuEditVars()
{
    struct MenuEntry entries[] =
    {
        {
            .String = str_opt_globalvars,
            .Type = Entry_Button
        },
        {
            .String = str_opt_initstate,
            .Type = Entry_Button,
        },
        {
            .String = str_opt_cmdlist,
            .Type = Entry_Button,
        },
    };

    u8* headers[] =
    {
        str_menu_generic,
        NULL,
    };

    s32 cursor = 0;
    while (true)
    {
        u32 sel = menuInputs((struct MenuDat){&cursor, headers, entries, ARRSIZE(entries), InputsCommon});

        switch (sel)
        {
            case 0: // global vars
                menuEditGlobals();
                break;
            case 1: // initial state vars
                break;
            case 2: // 3d gfx commands
                menuEditGX();
                break;
            case Ret_Exit: // exit
                return;
        }
    }
}
