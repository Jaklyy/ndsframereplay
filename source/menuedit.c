#include "menus.h"




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

/*void menuClearColor()
{
    u8 st_r[12];
    u8 st_g[12];
    u8 st_b[12];
    u8 st_fog[9];
    u8 st_a[12];
    u8 st_id[8];

    u8 r = clearcolor & 0x1F;
    u8 g = (clearcolor >> 5) & 0x1F;
    u8 b = (clearcolor >> 10) & 0x1F;
    bool fog = (clearcolor >> 15) & 0x1;
    u8 a = (clearcolor >> 16) & 0x1F;
    u8 id = (clearcolor >> 24) & 0x3F;

    sprintf(st_r, "%s%i\n", str_opt_r, r);
    sprintf(st_g, "%s%i\n", str_opt_g, g);
    sprintf(st_b, "%s%i\n", str_opt_b, b);
    sprintf(st_a, "%s%i\n", str_opt_a, a);
    sprintf(st_fog, "%s%s", str_opt_fog, str_state[fog]);
    sprintf(st_id, "%s%i\n", str_opt_id, id);

    u8* ptr_array[] =
    {
        str_sub_clearcolor,
        st_r,
        st_g,
        st_b,
        st_fog,
        st_a,
        st_id,
        str_hint_bback,
        str_hint_leftsubt,
        str_hint_lsubt10,
        str_hint_selreload,
        str_hint_rightadd,
        str_hint_radd10,
    };

    u8* vals[] =
    {
        str_state[0],
        str_state[1],
    };

    u8** vals_array[] =
    {
        NULL,
        NULL,
        NULL,
        vals,
        NULL,
        NULL,
    };

    menuEdit(&inputClearColor, 0, &clearcolor, 2, (struct MenuDat) {1, 3, 3, sizeof(ptr_array) / sizeof(ptr_array[0]), ptr_array}, vals_array, 5, 0, 5, 5, 5, 10, 1, 15, 5, 16, 6, 24);
}

void menuAlphaTest()
{
    u32 var = alphatest;
    u8 st_alpharef[15];
    sprintf(st_alpharef, "ALPHA REF: %i", alphatest);
    u8* ptr_array[] = 
    {
        str_sub_alpharef,
        st_alpharef,
        str_hint_bback,
        str_hint_leftsubt,
        str_hint_lsubt10,
        str_hint_selreload,
        str_hint_rightadd,
        str_hint_radd10,
    };

    menuEdit(&inputAlphaRef, 0, &var, 0, (struct MenuDat) {1, 3, 3, sizeof(ptr_array) / sizeof(ptr_array[0]), ptr_array}, NULL, 5, 0);
    alphatest = var;
}

void menuEdgeColor()
{
    u8* ptr_array[] =
    {
    [0] = str_sub_edgecolor,
    [9] = str_hint_bback,
    [10] = str_hint_aedit,
    [11] = str_hint_selreload,
    };
    for (int i = 0+1; i < 8+1; i++)
    {
        u8 color = 0xC0 | i;
        ptr_array[i] = malloc(19);
        sprintf(ptr_array[i], "\xF0""EDGE COLOR %i: \xFF%c\n", i, color);
    }

    u8 st_r[12] = "";
    u8 st_g[12] = "";
    u8 st_b[12] = "";

    u8* ptr_array2[] =
    {
        str_sub_color,
        st_r,
        st_g,
        st_b,
        str_hint_bback,
        str_hint_leftsubt,
        str_hint_lsubt10,
        str_hint_selreload,
        str_hint_rightadd,
        str_hint_radd10,
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

        u32 sel = menuInputs(&cursor, 3, (struct InputIDs) {1, 2, 0}, (struct MenuDat) {1, 1, 2, sizeof(ptr_array) / sizeof(ptr_array[0]), ptr_array});

        if (sel == 1) goto exit;
        else if (sel == 2) runDump(true); 
        else
        {
            sel -= 3;
            u8 r = (edgecolor[sel] & 0x1F);
            u8 g = ((edgecolor[sel] >> 5) & 0x1F);
            u8 b = ((edgecolor[sel] >> 10) & 0x1F);
            sprintf(st_r, "%s%i\n", str_opt_r, r);
            sprintf(st_g, "%s%i\n", str_opt_g, g);
            sprintf(st_b, "%s%i\n", str_opt_b, b);

            u32 var = edgecolor[sel];
            menuEdit(&inputEdgeColor, sel, &var, 1, (struct MenuDat) {1, 3, 3, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2}, NULL, 5, 0, 5, 5, 5, 10);
            edgecolor[sel] = var;
        }
    }

    exit:
    for (int i = 0+1; i < 8+1; i++)
        free(ptr_array[i]);
}*/

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
            .Addr = &input3DDispCnt,
            .String = str_3ddispcnt[0],
            .Values = str_state,
            .Var = &var,
            .Type = Entry_Int,
            .Shift = 0,
            .Mask = 0x1,
        },
        {
            .Addr = &input3DDispCnt,
            .String = str_3ddispcnt[1],
            .Values = str_shading,
            .Var = &var,
            .Type = Entry_Int,
            .Shift = 1,
            .Mask = 0x1,
        },
        {
            .Addr = &input3DDispCnt,
            .String = str_3ddispcnt[2],
            .Values = str_state,
            .Var = &var,
            .Type = Entry_Int,
            .Shift = 2,
            .Mask = 0x1,
        },
        {
            .Addr = &input3DDispCnt,
            .String = str_3ddispcnt[3],
            .Values = str_state,
            .Var = &var,
            .Type = Entry_Int,
            .Shift = 3,
            .Mask = 0x1,
        },
        {
            .Addr = &input3DDispCnt,
            .String = str_3ddispcnt[4],
            .Values = str_state,
            .Var = &var,
            .Type = Entry_Int,
            .Shift = 4,
            .Mask = 0x1,
        },
        {
            .Addr = &input3DDispCnt,
            .String = str_3ddispcnt[5],
            .Values = str_state,
            .Var = &var,
            .Type = Entry_Int,
            .Shift = 5,
            .Mask = 0x1,
        },
        {
            .Addr = &input3DDispCnt,
            .String = str_3ddispcnt[6],
            .Values = str_fogmode,
            .Var = &var,
            .Type = Entry_Int,
            .Shift = 6,
            .Mask = 0x1,
        },
        {
            .Addr = &input3DDispCnt,
            .String = str_3ddispcnt[7],
            .Values = str_state,
            .Var = &var,
            .Type = Entry_Int,
            .Shift = 7,
            .Mask = 0x1,
        },
        {
            .Addr = &input3DDispCnt,
            .String = str_3ddispcnt[8],
            .Var = &var,
            .Type = Entry_Int,
            .Shift = 8,
            .Mask = 0xF,
        },
        {
            .Addr = &input3DDispCnt,
            .String = str_3ddispcnt[9],
            .Values = str_rearplane,
            .Var = &var,
            .Type = Entry_Int,
            .Shift = 14,
            .Mask = 0x1,
        },
    };

    s32 cursor = 0;
    menuInputs((struct MenuDat){&cursor, headers, entries, sizeof(entries)/sizeof(entries[0]),
        (struct InputIDs){.ScrollUp = KEY_UP, .ScrollDown = KEY_DOWN, .Exit = KEY_B, .Add1 = KEY_RIGHT, .Sub1 = KEY_LEFT, .Add10 = KEY_R, .Sub10 = KEY_L, .Reload = KEY_Y, .Reset = KEY_X}});

    disp3dcnt = var;
}

void menuEditVars()
{
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
            .String = str_opt_alphatest,
            .Type = Entry_Button,
        },
        {
            .String = str_opt_clearcolor,
            .Type = Entry_Button,
        },
        {
            .String = str_opt_cleardepth,
            .Type = Entry_Button,
        },
        {
            .String = str_opt_clearoffset,
            .Type = Entry_Button,
        },
        {
            .String = str_opt_fogcolor,
            .Type = Entry_Button,
        },
        {
            .String = str_opt_fogtable,
            .Type = Entry_Button,
        },
        {
            .String = str_opt_toontable,
            .Type = Entry_Button,
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
        str_menu_globalvars,
        NULL,
    };

    s32 cursor = 0;
    while (true)
    {
        u32 sel = menuInputs((struct MenuDat){&cursor, headers, entries, sizeof(entries)/sizeof(entries[0]),
            (struct InputIDs){.ScrollUp = KEY_UP, .ScrollDown = KEY_DOWN, .Select = KEY_A, .Exit = KEY_B, .Reload = KEY_Y, .Reset = KEY_X}});

        switch (sel)
        {
            case 0: // 3d display control
                menu3DDispCnt();
                break;
            case 1: // edge colors
                //menuEdgeColor();
                break;
            case 2: // alpha test ref
                //menuAlphaTest();
                break;
            case 3: // clear color
                //menuClearColor();
                break;
            case 4: // clear depth
                break;
            case 5: // clear offset
                break;
            case 6: // fog color
                break;
            case 7: // fog table
                break;
            case 8: // toon table
                break;
            case 9: // initial state vars
                break;
            case 10: // 3d gfx commands
                break;
            case RetExit: // exit
                return;
        }
    }
}
