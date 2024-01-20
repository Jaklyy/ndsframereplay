#include "menus.h"




void inputEdgeColor(u32 var, u8 offset)
{

}

/*void menuEdgeColor()
{
    u8* ptr_array[] =
    {
    [0] = str_sub_edgecolor,
    [9] = str_hint_bback,
    [10] = str_hint_aedit,
    };
    for (int i = 0; i < 8; i++)
    {
        u8 color = 0xC0 | i;
        ptr_array[i+1] = malloc(17);
        sprintf(ptr_array[i+1], "EDGE COLOR %i:%c\xFF", i, color);
    }

    ptr_array2* [] =
    {
        str_sub_edgecolor,
        str_null,
        str_hint_bback,
        str_hint_upadd,
        str_hint_downsubt
    };

    s8* cursor = 0;
    while (true)
    {
        u8 sel = menuInputs(&cursor, 2, (struct InputIDs) {1, 0, 0}, 1, 1, 1, sizeof(ptr_array) / sizeof(ptr_array[0]), ptr_array);

        if (sel == 1) goto exit;
        else
        {
            sel -= 2;
            ptr_array2[1] = ptr_array[sel+1];
            menuEdit(&inputEdgeColor, sel, &edgecolor[8], 15, 0, 1, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2, NULL);
        }
    }

    exit:
    for (int i = 0+1; i < 8+1; i++)
        free(ptr_array[i]);
}*/

void input3DDispCnt(u32 var, __attribute__((unused)) u8 offset)
{
    GFX_CONTROL = var;
}

void menu3DDispCnt()
{
    u8 sttextures[16] = "";
    memcpy(sttextures, str_3ddispcnt[0], 11);
    strcat(sttextures, ((disp3dcnt >> 0) & 0x1) ? str_state[1] : str_state[0]);

    u8 stshading[22] = "";
    memcpy(stshading, str_3ddispcnt[1], 11);
    strcat(stshading, ((disp3dcnt >> 1) & 0x1) ? str_shading[1] : str_shading[0]);

    u8 stalphatest[16] = "";
    memcpy(stalphatest, str_3ddispcnt[2], 11);
    strcat(stalphatest, ((disp3dcnt >> 2) & 0x1) ? str_state[1] : str_state[0]);

    u8 stblend[16] = "";
    memcpy(stblend, str_3ddispcnt[3], 11);
    strcat(stblend, ((disp3dcnt >> 3) & 0x1) ? str_state[1] : str_state[0]);

    u8 stalias[16] = "";
    memcpy(stalias, str_3ddispcnt[4], 11);
    strcat(stalias, ((disp3dcnt >> 4) & 0x1) ? str_state[1] : str_state[0]);

    u8 stedge[16] = "";
    memcpy(stedge, str_3ddispcnt[5], 11);
    strcat(stedge, ((disp3dcnt >> 5) & 0x1) ? str_state[1] : str_state[0]);

    u8 stfogmode[23] = "";
    memcpy(stfogmode, str_3ddispcnt[6], 11);
    strcat(stfogmode, ((disp3dcnt >> 6) & 0x1) ? str_fogmode[1] : str_fogmode[0]);

    u8 stfog[16] = "";
    memcpy(stfog, str_3ddispcnt[7], 11);
    strcat(stfog, ((disp3dcnt >> 7) & 0x1) ? str_state[1] : str_state[0]);

    u8 stfogshift[16] = "";
    memcpy(stfogshift, str_3ddispcnt[8], 11);
    snprintf(stfogshift, 16, "%s 0\x80%X\n", stfogshift, (disp3dcnt >> 8) & 0xF);

    u8 stplane[19] = "";
    memcpy(stplane, str_3ddispcnt[9], 11);
    strcat(stplane, ((disp3dcnt >> 14) & 0x1) ? str_rearplane[1] : str_rearplane[0]);

    u8* ptr_array[] =
    {
        str_sub_3ddispcnt,
        sttextures,
        stshading,
        stalphatest,
        stblend,
        stalias,
        stedge,
        stfogmode,
        stfog,
        stfogshift,
        stplane,
        str_hint_bback,
        str_hint_aedit
    };

    s8 cursor = 0;
    while (true)
    {
        u8 sel = menuInputs(&cursor, 2, (struct InputIDs) {1, 0, 0}, 1, 1, 1, sizeof(ptr_array) / sizeof(ptr_array[0]), ptr_array);

        u8* ptr_array2[] = 
        {
            str_sub_3ddispcnt,
            str_null,
            str_hint_bback,
            str_hint_upadd,
            str_hint_downsubt
        };


        switch(sel)
        {
            case 1: // back
                return;
            case 2: // texture toggle
            {
                ptr_array2[1] = sttextures;
                u8* vals[] = 
                {
                    str_state[0],
                    str_state[1],
                };
                menuEdit(&input3DDispCnt, 0, &disp3dcnt, 1, 0, 0, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2, vals);
                break;
            }
            case 3: // shading mode
            {
                ptr_array2[1] = stshading;
                u8* vals[] =
                {
                    str_shading[0],
                    str_shading[1],
                };
                menuEdit(&input3DDispCnt, 0, &disp3dcnt, 1, 1, 0, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2, vals);
                break;
            }
            case 4: // alpha test toggle
            {
                ptr_array2[1] = stalphatest;
                u8* vals[] = 
                {
                    str_state[0],
                    str_state[1],
                };
                menuEdit(&input3DDispCnt, 0, &disp3dcnt, 1, 2, 0, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2, vals);
                break;
            }
            case 5: // translucency blend toggle
            {
                ptr_array2[1] = stblend;
                u8* vals[] = 
                {
                    str_state[0],
                    str_state[1],
                };
                menuEdit(&input3DDispCnt, 0, &disp3dcnt, 1, 3, 0, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2, vals);
                break;
            }
            case 6: // aa toggle
            {
                ptr_array2[1] = stalias;
                u8* vals[] = 
                {
                    str_state[0],
                    str_state[1],
                };
                menuEdit(&input3DDispCnt, 0, &disp3dcnt, 1, 4, 0, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2, vals);
                break;
            }
            case 7: // edge marking toggle
            {
                ptr_array2[1] = stedge;
                u8* vals[] = 
                {
                    str_state[0],
                    str_state[1],
                };
                menuEdit(&input3DDispCnt, 0, &disp3dcnt, 1, 5, 0, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2, vals);
                break;
            }
            case 8: // fog mode toggle
            {
                ptr_array2[1] = stfogmode;
                u8* vals[] = 
                {
                    str_fogmode[0],
                    str_fogmode[1],
                };
                menuEdit(&input3DDispCnt, 0, &disp3dcnt, 1, 6, 0, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2, vals);
                break;
            }
            case 9: // fog toggle
            {
                ptr_array2[1] = stfog;
                u8* vals[] = 
                {
                    str_state[0],
                    str_state[1],
                };
                menuEdit(&input3DDispCnt, 0, &disp3dcnt, 1, 7, 0, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2, vals);
                break;
            }
            case 10: // fog shift variable
            {
                ptr_array2[1] = stfogshift;
                menuEdit(&input3DDispCnt, 0, &disp3dcnt, 4, 8, 0, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2, NULL);
                break;
            }
            case 11: // rear plane mode toggle
            {
                ptr_array2[1] = stplane;
                u8* vals[] = 
                {
                    str_rearplane[0],
                    str_rearplane[1],
                };
                menuEdit(&input3DDispCnt, 0, &disp3dcnt, 1, 14, 0, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2, vals);
                break;
            }
        }
    }
}

void menuEditVars()
{
    u8* ptr_array[] =
    {
        str_menu_generic,
        str_menu_globalvars,
        str_opt_3ddispcnt,
        str_opt_edgecolor,
        str_opt_alphatest,
        str_opt_clearcolor,
        str_opt_cleardepth,
        str_opt_clearoffset,
        str_opt_fogcolor,
        str_opt_fogtable,
        str_opt_toontable,
        str_opt_initstate,
        str_opt_cmdlist,
        str_opt_reset,
        str_hint_rscreenshot,
        str_hint_bback,
        str_hint_asel,
    };
    
    s8 cursor = 0;
    while (true)
    {
        u8 sel = menuInputs(&cursor, 0, (struct InputIDs) {12, 0, 13}, 2, 2, 1, sizeof(ptr_array) / sizeof(ptr_array[0]), ptr_array);

        switch (sel)
        {
            case 0: // 3d display control
                menu3DDispCnt();
                break;
            case 1: // edge colors
                //menuEdgeColor();
                break;
            case 2: // alpha test ref
                break;
            case 3: // clear color
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
            case 11: // reset changes
                break;
            case 12: // exit
                return;
            case 13: // screenshot
                menuScreenshot();
                break;
        }
    }
}
