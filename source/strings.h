#include "main.h"




#ifndef STRINGS_H
#define STRINGS_H

// menu headers
extern u8 str_menu_seldir[];
extern u8 str_menu_selfile[];
extern u8 str_menu_generic[];
extern u8 str_menu_globalvars[];
extern u8 str_menu_ss[];

// edit menu sub sections
extern u8 str_sub_3ddispcnt[];
extern u8 str_sub_edgecolor[];
extern u8 str_sub_clearcolor[];
extern u8 str_sub_color[];
extern u8 str_sub_gx[];


// menu options
// directory menu
extern u8 str_opt_sd[];
extern u8 str_opt_fc[];
extern u8 str_opt_nitro[];

// main menu
extern u8 str_opt_edit[];
extern u8 str_opt_changefile[];
extern u8 str_opt_screenshot[];
extern u8 str_opt_quit[];

// edit menu
extern u8 str_opt_globalvars[];
extern u8 str_opt_initstate[];
extern u8 str_opt_cmdlist[];

// global vars submenu
extern u8 str_opt_3ddispcnt[];
extern u8 str_opt_edgecolor[];
extern u8 str_opt_alphatest[];
extern u8 str_opt_clearcolor[];
extern u8 str_opt_cleardepth[];
extern u8 str_opt_clearoffsetx[];
extern u8 str_opt_clearoffsety[];
extern u8 str_opt_fogcolor[];
extern u8 str_opt_fogoffset[];
extern u8 str_opt_fogtable[];
extern u8 str_opt_toontable[];

extern u8 str_opt_r[];
extern u8 str_opt_g[];
extern u8 str_opt_b[];
extern u8 str_opt_a[];
extern u8 str_opt_fog[];
extern u8 str_opt_id[];

// screenshotmenu 
extern u8 str_opt_ss_full_bmp[];
extern u8 str_opt_ss_norm_bmp[];


// menu footers
extern u8 str_hint_sel[];
extern u8 str_hint_edit[];
extern u8 str_hint_back[];
extern u8 str_hint_prev[];
extern u8 str_hint_next[];
extern u8 str_hint_subt[];
extern u8 str_hint_add[];
extern u8 str_hint_add10[];
extern u8 str_hint_subt10[];
extern u8 str_hint_reset[];
extern u8 str_hint_reload[];


// menu errors
extern u8 str_err_dir[];
extern u8 str_err_file[];
extern u8 str_err_toomanyss[];
extern u8 str_err_nofreebank[];
extern u8 str_err_invalidfile[];
extern u8 str_err_headerfail[];
extern u8 str_err_oldver[];
extern u8 str_err_timetravel[];


// misc
extern u8 str_edgecolor[];


// stuff i haven't put anywhere yet-
extern u8 str_gx[];
extern u8 str_cmd[];

extern u8 str_null[];


// lookup tables
extern u8* str_cmds[256];

extern u8 str_matrix[][8];

extern u8* str_state[];

extern u8 str_input_a[];
extern u8 str_input_b[];
extern u8 str_input_select[];
extern u8 str_input_start[];
extern u8 str_input_right[];
extern u8 str_input_left[];
extern u8 str_input_up[];
extern u8 str_input_down[];
extern u8 str_input_r[];
extern u8 str_input_l[];
extern u8 str_input_x[];
extern u8 str_input_y[];

extern u8* str_inputs[];

extern u8 str_3ddispcnt[][11];

extern u8* str_shading[];

extern u8* str_fogmode[];

extern u8* str_rearplane[];
#endif
