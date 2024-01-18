#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <filesystem.h>

#include "menu/menu.h"
#include "strings.h"




#ifndef MENUS_H
#define MENUS_H

void menuScreenshot();
void initDispCap();

bool menuDirSelect();
FILE* menuFileSelect();

void menuEditVars();

extern u8 dispcapbank;

#endif
