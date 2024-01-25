#include <nds.h>

#include "strings.h"




// menu headers
u8 str_menu_seldir[] =      "SELECT DIRECTORY:\n";
u8 str_menu_selfile[] =     "SELECT FILE:\n";
u8 str_menu_generic[] =     "SELECT OPTION:\n";
u8 str_menu_globalvars[] =  "GLOBAL VARS:\n";
u8 str_menu_ss[] =          "SELECT SCREENSHOT MODE:\n";

// edit menu sub sections
u8 str_sub_3ddispcnt[] =    "EDIT 3DDISPCNT:\n";
u8 str_sub_edgecolor[] =    "EDIT EDGE COLORS:\n";
u8 str_sub_clearcolor[] =   "EDIT CLEAR COLOR: \xFF\xC1\n";
u8 str_sub_color[] =        "EDIT COLOR: \xFF\xC1\n";


// menu options
// directory menu
u8 str_opt_sd[] =           "SD\n";
u8 str_opt_fc[] =           "FLASHCARD\n";
u8 str_opt_nitro[] =        "ROMFS\n";

// main menu
u8 str_opt_edit[] =         "VIEW OR EDIT COMMANDS\n";
u8 str_opt_changefile[] =   "CHANGE FILE\n";
u8 str_opt_screenshot[] =   "SCREENSHOT\n";
u8 str_opt_quit[] =         "VIBE CHECK\n";

// edit menu
u8 str_opt_globalvars[] =   "GLOBAL VARS\n";
u8 str_opt_initstate[] =    "INITAL STATE\n";
u8 str_opt_cmdlist[] =      "COMMAND LIST\n";

// global vars submenu
u8 str_opt_3ddispcnt[] =    "3DDISPCNT\n";
u8 str_opt_edgecolor[] =    "EDGE COLOR\n";
u8 str_opt_alphatest[] =    "ALPHA TEST:";
u8 str_opt_clearcolor[] =   "CLEAR COLOR\n";
u8 str_opt_cleardepth[] =   "CLEAR DEPTH\n";
u8 str_opt_clearoffset[] =  "CLEAR OFFSET\n";
u8 str_opt_fogcolor[] =     "FOG COLOR\n";
u8 str_opt_fogtable[] =     "FOG TABLE\n";
u8 str_opt_toontable[] =    "TOON TABLE\n";

u8 str_opt_r[] =            "R:";
u8 str_opt_g[] =            "G:";
u8 str_opt_b[] =            "B:";
u8 str_opt_a[] =            "A:";
u8 str_opt_fog[] =          "FOG:";
u8 str_opt_id[] =           "ID:";

// screenshotmenu 
u8 str_opt_ss_full_bmp[] =  "666 18 BIT BMP (EXPERIMENTAL)\n";
u8 str_opt_ss_norm_bmp[] =  "555 15 BIT BMP\n";


// menu footers
u8 str_hint_sel[] =         "SELECT\n";
u8 str_hint_edit[] =        "EDIT\n";
u8 str_hint_back[] =        "BACK\n";
u8 str_hint_prev[] =        "PREV\n";
u8 str_hint_next[] =        "NEXT\n";
u8 str_hint_subt[] =        "MINUS 1\n";
u8 str_hint_add[] =         "PLUS 1\n";
u8 str_hint_add10[] =       "PLUS 10\n";
u8 str_hint_subt10[] =      "MINUS 10\n";
u8 str_hint_reset[] =       "RESET\n";
u8 str_hint_reload[] =      "RELOAD\n";

// menu errors
u8 str_err_dir[] =          "\xF1""ERROR: UNABLE TO OPEN DIRECTORY\nPLEASE TURN OFF THE SYSTEM\n";
u8 str_err_file[] =         "\xF1""ERROR: NO FILES FOUND\nTRY A DIFFERENT FOLDER\n";
u8 str_err_toomanyss[] =    "\xF1""ERROR: TOO MANY SCREENSHOTS\n";
u8 str_err_nofreebank[] =   "\xF1""ERROR: NO BANK AVAILABLE\nFOR DISPLAY CAPTURE\n";
u8 str_err_invalidfile[] =  "\xF1""ERROR: INVALID FILE\n";
u8 str_err_headerfail[] =   "\xF1""ERROR: FILE HEADER INVALID\n";
u8 str_err_oldver[] =       "\xF1""ERROR: FILE TOO OLD TO LOAD\n";
u8 str_err_timetravel[] =   "\xF1""ERROR: FILE FROM FUTURE\nCHECK FOR A NEW VERSION\n";

// stuff i haven't put anywhere yet-
u8 str_gx[] =               "GX: 0x";
u8 str_cmd[] =              "CMD: ";

u8 str_null[] =             "";

// lookup table for graphics command names
u8 str_cmds[256][10] =
{
[Mode] =       "MTX MODE",
[Push] =       "PUSH",
[Pop] =        "POP",
[Store] =      "STORE",
[Restore] =    "RESTORE",
[Identity] =   "IDENTITY",
[Load4x4] =    "LOAD 4x4",
[Load4x3] =    "LOAD 4X3",
[Mult4x4] =    "MULT 4X4",
[Mult4x3] =    "MULT 4X3",
[Mult3x3] =    "MULT 3X3",
[Scale] =      "SCALE",
[Trans] =      "TRANS",
[Color] =      "VTX COL",
[Normal] =     "NORMAL",
[Texcoord] =   "TEXCOORD",
[Vtx16] =      "VTX 16",
[Vtx10] =      "VTX 10",
[VtxXY] =      "VTX XY",
[VtxXZ] =      "VTX XZ",
[VTXYZ] =      "VTX YZ",
[VtxDiff] =    "VTX DIFF",
[PolyAttr] =   "POLY ATR",
[TexParam] =   "TEXPARAM",
[TexPalette] = "TEX PAL",
[DiffAmbi] =   "DIFFAMBI",
[SpecEmis] =   "SPECEMIS",
[LightVec] =   "LIGHTVEC",
[LightColor] = "LIGHTCOL",
[Shininess] =  "SHININES",
[BeginPoly] =  "BGN POLY",
[EndPoly] =    "END POLY",
[SwapBuffer] = "SWP BUFF",
[Viewport] =   "VIEWPORT",
[PosTest] =    "POS TEST",
[ZDotDisp] =   "0DOTDISP",
};

u8 str_matrix[][8] =
{
    "PRJ MTX",
    "POS MTX",
    "VEC MTX",
    "TEX MTX",
};

u8* str_state[] = 
{
    "OFF\n",
    "ON\n",
};


u8 str_input_a[] = "A";
u8 str_input_b[] = "B";
u8 str_input_select[] = "SEL";
u8 str_input_start[] = "STR";
u8 str_input_right[] = "RIG";
u8 str_input_left[] = "LFT";
u8 str_input_up[] = "UP";
u8 str_input_down[] = "DWN";
u8 str_input_r[] = "R";
u8 str_input_l[] = "L";
u8 str_input_x[] = "X";
u8 str_input_y[] = "Y";

u8* str_inputs[] =
{
    "A",
    "B",
    "SEL",
    "STR",
    "RIG",
    "LFT",
    "UP",
    "DWN",
    "R",
    "L",
    "X",
    "Y",
};

u8 str_3ddispcnt[][11] =
{
    " TEXTURES:",
    "  SHADING:",
    "ALPHATEST:",
    "    BLEND:",
    "ANTIALIAS:",
    " EDGEMARK:",
    " FOG MODE:",
    "      FOG:",
    " FOGSHIFT:",
    "REARPLANE:",
};

u8* str_shading[] =
{
    "TOON\n",
    "HIGHLIGHT\n",
};

u8* str_fogmode[] =
{
    "ALPHACOLOR\n",
    "ALPHA ONLY\n",
};

u8* str_rearplane[] =
{
    "BLANK\n",
    "BITMAP\n",
};
