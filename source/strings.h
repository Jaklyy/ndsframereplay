#include <nds.h>




#ifndef STRINGS_H
#define STRINGS_H

// menu headers
u8 str_menu_seldir[] =      "\xF2""SELECT DIRECTORY:\n";
u8 str_menu_selfile[] =     "\xF2""SELECT FILE:\n";
u8 str_menu_generic[] =     "\xF2""SELECT OPTION:\n";
u8 str_menu_gx[] =          "\xF2""SELECT COMMAND:\n";
u8 str_menu_ss[] =          "\xF2""SELECT SCREENSHOT MODE:\n";

// menu options
// directory menu
u8 str_opt_sd[] =           "\xF0""SD\n";
u8 str_opt_fc[] =           "\xF0""FLASHCARD\n";
u8 str_opt_nitro[] =        "\xF0""ROMFS\n";
// main menu
u8 str_opt_gx[] =           "\xF0""VIEW GX\n";
u8 str_opt_rerender[] =     "\xF0""FORCE RERENDER\n";
u8 str_opt_changefile[] =   "\xF0""CHANGE FILE\n";
u8 str_opt_swapbuffer[] =   "\xF0""FORCE SWAP BUFFERS\n";
// gx menu

// screenshotmenu 
u8 str_opt_ss_full_bmp[] =  "\xF0""666 18 BIT BMP (EXPERIMENTAL)\n";
u8 str_opt_ss_full_raw[] =  "\xF0""666 18 BIT RAW (EXPERIMENTAL)\n";
u8 str_opt_ss_norm_bmp[] =  "\xF0""555 16 BIT BMP\n";
u8 str_opt_ss_norm_raw[] =  "\xF0""555 16 BIT RAW\n";


// menu footers
u8 str_hint_asel[] =        "\xF2""A: SELECT""\xF2";
u8 str_hint_aedit[] =       "\xF2""A: EDIT""\xF2";
u8 str_hint_bback[] =       "\xF2""B: BACK""\xF2";
u8 str_hint_leftprev[] =    "\xF2""LEFT: PREV""\xF2";
u8 str_hint_rightnext[] =   "\xF2""RIGHT: NEXT""\xF2";
u8 str_hint_upadd[] =       "\xF2""UP: PLUS 1""\xF2";
u8 str_hint_downsubt[] =    "\xF2""DOWN: MINUS 1""\xF2";
u8 str_hint_rscreenshot[] = "\xF2""R: SCREENSHOT""\xF2";

// menu errors
u8 str_err_dir[] =          "\xF1""ERROR: UNABLE TO OPEN DIRECTORY\nPLEASE TURN OFF THE SYSTEM\n";
u8 str_err_file[] =         "\xF1""ERROR: NO FILES FOUND\nTRY A DIFFERENT FOLDER\n";
u8 str_err_nofreebank[] =   "\xF1""ERROR: NO BANK AVAILABLE\nFOR DISPLAY CAPTURE\n";
u8 str_err_headerfail[] =   "\xF1""ERROR: FILE HEADER INVALID\n";
u8 str_err_oldver[] =       "\xF1""ERROR: FILE TOO OLD TO LOAD\n";
u8 str_err_timetravel[] =   "\xF1""ERROR: FILE FROM FUTURE\nCHECK FOR A NEW VERSION\n";

// stuff i haven't put anywhere yet-
u8 str_gx[] =               "\xF0""GX: 0x";
u8 str_cmd[] =              "\xF0""CMD: ";

u8 str_null[] =             "";

// lookup table for graphics command names
u8 str_cmds[256][10] =
{
[Mode] =       "\xF0""MTX MODE",
[Push] =       "\xF0""PUSH",
[Pop] =        "\xF0""POP",
[Store] =      "\xF0""STORE",
[Restore] =    "\xF0""RESTORE",
[Identity] =   "\xF0""IDENTITY",
[Load4x4] =    "\xF0""LOAD 4x4",
[Load4x3] =    "\xF0""LOAD 4X3",
[Mult4x4] =    "\xF0""MULT 4X4",
[Mult4x3] =    "\xF0""MULT 4X3",
[Mult3x3] =    "\xF0""MULT 3X3",
[Scale] =      "\xF0""SCALE",
[Trans] =      "\xF0""TRANS",
[Color] =      "\xF0""VTX COL",
[Normal] =     "\xF0""NORMAL",
[Texcoord] =   "\xF0""TEXCOORD",
[Vtx16] =      "\xF0""VTX 16",
[Vtx10] =      "\xF0""VTX 10",
[VtxXY] =      "\xF0""VTX XY",
[VtxXZ] =      "\xF0""VTX XZ",
[VTXYZ] =      "\xF0""VTX YZ",
[VtxDiff] =    "\xF0""VTX DIFF",
[PolyAttr] =   "\xF0""POLY ATR",
[TexParam] =   "\xF0""TEXPARAM",
[TexPalette] = "\xF0""TEX PAL",
[DiffAmbi] =   "\xF0""DIFFAMBI",
[SpecEmis] =   "\xF0""SPECEMIS",
[LightVec] =   "\xF0""LIGHTVEC",
[LightColor] = "\xF0""LIGHTCOL",
[Shininess] =  "\xF0""SHININES",
[BeginPoly] =  "\xF0""BGN POLY",
[EndPoly] =    "\xF0""END POLY",
[SwapBuffer] = "\xF0""SWP BUFF",
[Viewport] =   "\xF0""VIEWPORT",
[PosTest] =    "\xF0""POS TEST",
[ZDotDisp] =   "\xF0""0DOTDISP",
};

u8 str_matrix[][8] =
{
    "PRJ MTX",
    "POS MTX",
    "VEC MTX",
    "TEX MTX",
};

u8 str_dispcnt[][12] =
{
    " TEXTURES: ",
    "  SHADING: ",
    "ALPHATEST: ",
    "ANTIALIAS: ",
    " EDGEMARK: ",
    " FOG MODE: ",
    "      FOG: ",
    " FOGSHIFT: ",
    "REARPLANE: ",
};

u8 str_shading[][11] =
{
    "TOON\n",
    "HIGHLIGHT\n",
};

u8 str_fogmode[][13] =
{
    "ALPHA+COLOR\n",
    "ALPHA\n",
};

u8 str_rearplane[][8] =
{
    "BLANK\n",
    "BITMAP\n",
};
#endif
