#include "menus.h"




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
    memcpy(st_r, str_opt_r, 10);
    memcpy(st_g, str_opt_g, 10);
    memcpy(st_b, str_opt_b, 10);

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
            snprintf(&st_r[8], 4, "%i\n", r);
            snprintf(&st_g[8], 4, "%i\n", g);
            snprintf(&st_b[8], 4, "%i\n", b);

            u32 var = edgecolor[sel];
            menuEdit(&inputEdgeColor, sel, &var, 5, 0, 1, (struct MenuDat) {1, 3, 3, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2}, NULL);
            edgecolor[sel] = var;
        }
    }

    exit:
    for (int i = 0+1; i < 8+1; i++)
        free(ptr_array[i]);
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
    sprintf(stfogshift, "%s %i\n", str_3ddispcnt[8], (disp3dcnt >> 8) & 0xF);

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
        str_hint_aedit,
        str_hint_selreload,
    };

    s32 cursor = 0;
    while (true)
    {
        u32 sel = menuInputs(&cursor, 3, (struct InputIDs) {1, 2, 0}, (struct MenuDat) {1, 1, 2, sizeof(ptr_array) / sizeof(ptr_array[0]), ptr_array});

        u8* ptr_array2[] = 
        {
            str_sub_3ddispcnt,
            str_null,
            str_hint_bback,
            str_hint_leftsubt,
            str_hint_selreload,
            str_hint_rightadd,
        };

        u8* ptr_array3[] =
        {
            str_sub_3ddispcnt,
            stfogshift,
            str_hint_bback,
            str_hint_leftsubt,
            str_hint_lsubt10,
            str_hint_selreload,
            str_hint_rightadd,
            str_hint_radd10,
        };

        u32 var = disp3dcnt;
        switch(sel)
        {
            case 1: // back
                return;
            case 2:
                runDump(true);
                break;
            case 3: // texture toggle
            {
                ptr_array2[1] = sttextures;
                u8* vals[] = 
                {
                    str_state[0],
                    str_state[1],
                };
                menuEdit(&input3DDispCnt, 0, &var, 1, 0, 0, (struct MenuDat) {1, 2, 2, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2}, vals);
                break;
            }
            case 4: // shading mode
            {
                ptr_array2[1] = stshading;
                u8* vals[] =
                {
                    str_shading[0],
                    str_shading[1],
                };
                menuEdit(&input3DDispCnt, 0, &var, 1, 1, 0, (struct MenuDat) {1, 2, 2, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2}, vals);
                break;
            }
            case 5: // alpha test toggle
            {
                ptr_array2[1] = stalphatest;
                u8* vals[] = 
                {
                    str_state[0],
                    str_state[1],
                };
                menuEdit(&input3DDispCnt, 0, &var, 1, 2, 0, (struct MenuDat) {1, 2, 2, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2}, vals);
                break;
            }
            case 6: // translucency blend toggle
            {
                ptr_array2[1] = stblend;
                u8* vals[] = 
                {
                    str_state[0],
                    str_state[1],
                };
                menuEdit(&input3DDispCnt, 0, &var, 1, 3, 0, (struct MenuDat) {1, 2, 2, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2}, vals);
                break;
            }
            case 7: // aa toggle
            {
                ptr_array2[1] = stalias;
                u8* vals[] = 
                {
                    str_state[0],
                    str_state[1],
                };
                menuEdit(&input3DDispCnt, 0, &var, 1, 4, 0, (struct MenuDat) {1, 2, 2, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2}, vals);
                break;
            }
            case 8: // edge marking toggle
            {
                ptr_array2[1] = stedge;
                u8* vals[] = 
                {
                    str_state[0],
                    str_state[1],
                };
                menuEdit(&input3DDispCnt, 0, &var, 1, 5, 0, (struct MenuDat) {1, 2, 2, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2}, vals);
                break;
            }
            case 9: // fog mode toggle
            {
                ptr_array2[1] = stfogmode;
                u8* vals[] = 
                {
                    str_fogmode[0],
                    str_fogmode[1],
                };
                menuEdit(&input3DDispCnt, 0, &var, 1, 6, 0, (struct MenuDat) {1, 2, 2, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2}, vals);
                break;
            }
            case 10: // fog toggle
            {
                ptr_array2[1] = stfog;
                u8* vals[] = 
                {
                    str_state[0],
                    str_state[1],
                };
                menuEdit(&input3DDispCnt, 0, &var, 1, 7, 0, (struct MenuDat) {1, 2, 2, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2}, vals);
                break;
            }
            case 11: // fog shift variable
            {
                menuEdit(&input3DDispCnt, 0, &var, 4, 8, 0, (struct MenuDat) {1, 3, 3, sizeof(ptr_array3) / sizeof(ptr_array3[0]), ptr_array3}, NULL);
                break;
            }
            case 12: // rear plane mode toggle
            {
                ptr_array2[1] = stplane;
                u8* vals[] = 
                {
                    str_rearplane[0],
                    str_rearplane[1],
                };
                menuEdit(&input3DDispCnt, 0, &var, 1, 14, 0, (struct MenuDat) {1, 2, 2, sizeof(ptr_array2) / sizeof(ptr_array2[0]), ptr_array2}, vals);
                break;
            }
        }
        disp3dcnt = var;
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
    
    s32 cursor = 0;
    while (true)
    {
        u32 sel = menuInputs(&cursor, 0, (struct InputIDs) {12, 0, 13}, (struct MenuDat) {2, 2, 1, sizeof(ptr_array) / sizeof(ptr_array[0]), ptr_array});

        switch (sel)
        {
            case 0: // 3d display control
                menu3DDispCnt();
                break;
            case 1: // edge colors
                menuEdgeColor();
                break;
            case 2: // alpha test ref
            {
                u32 var = alphatest;
                u8 st_alpharef[15];
                sprintf(st_alpharef, "ALPHA REF: %i", alphatest);
                u8* ptr_array2[] = 
                {
                    str_sub_editparam,
                    st_alpharef,
                    str_hint_bback,
                    str_hint_leftsubt,
                    str_hint_lsubt10,
                    str_hint_selreload,
                    str_hint_rightadd,
                    str_hint_radd10,
                };
                menuEdit(&inputAlphaRef, 0, &var, 5, 0, 0, (struct MenuDat) {1, 3, 3, sizeof(ptr_array2) / sizeof(ptr_array[0]), ptr_array2}, NULL);
                alphatest = var;
                break;
            }
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
