#include <dirent.h>

#include "menus.h"



bool menuDirSelect()
{    
    bool usingfoldersd = false;
    bool usingfolderfat = false;

    // test console sd
    DIR* sddir = opendir("sd:/framedumps");
    if (sddir == NULL) sddir = opendir("sd:/");
    else usingfoldersd = true;

    // test flash cart sd
    DIR* fatdir = opendir("fat:/framedumps");
    if (fatdir == NULL) fatdir = opendir("fat:/");
    else usingfolderfat = true;

    // test romfs
    DIR* nitrodir = opendir("nitro:/");

    // check if any dir was found.
    if ((nitrodir != NULL) && (fatdir != NULL) && (sddir != NULL))
    {
        menuWriteSingle(str_err_dir);
        waitForInput();
        exit(0);
    }

    u8* ptr_array[6] = {[0] = str_menu_seldir};
    int i = 1;
    if (nitrodir != NULL)
    {
        ptr_array[i] = str_opt_nitro;
        i++;
    }
    if (sddir != NULL)
    {
        ptr_array[i] = str_opt_sd;
        i++;
    }
    if (fatdir != NULL)
    {
        ptr_array[i] = str_opt_fc;
        i++;
    }

    ptr_array[i] = str_hint_bback;
    i++;

    ptr_array[i] = str_hint_asel;
    i++;

    s32 cursor = 0;
    u32 selection = menuInputs(&cursor, 0, (struct InputIDs) {3,0,0}, (struct MenuDat) {1, 1, 1, i, ptr_array});
    if (selection == 3)
    {
        closedir(nitrodir);
        closedir(fatdir);
        closedir(sddir);
        return false;
    }
    else if (selection == 2)
    {
        closedir(nitrodir);
        closedir(fatdir);
        closedir(sddir);
        chdir((usingfolderfat ? "fat:/framedumps" : "fat:/"));
        return true;
    }
    else if (selection == 1)
    {
        if (nitrodir == NULL || sddir == NULL)
        {
            closedir(nitrodir);
            closedir(fatdir);
            closedir(sddir);
            chdir((usingfolderfat ? "fat:/framedumps" : "fat:/"));
            return true;
        }
        else // fat dir
        {
            closedir(nitrodir);
            closedir(fatdir);
            closedir(sddir);
            chdir((usingfoldersd ? "sd:/framedumps" : "sd:/"));
            return true;
        }
    }
    else // sel 0
    {
        if (nitrodir != NULL)
        {
            closedir(nitrodir);
            closedir(fatdir);
            closedir(sddir);
            chdir("nitro:/");
            return true;
        }
        else if (sddir != NULL)
        {
            closedir(fatdir);
            closedir(sddir);
            chdir((usingfoldersd ? "sd:/framedumps" : "sd:/"));
            return true;
        }
        else // only fat remains
        {
            closedir(fatdir);
            chdir((usingfolderfat ? "fat:/framedumps" : "fat:/"));
            return true;
        }
    }
}

FILE* menuFileSelect()
{
    u8* ptr_array[25];
    u8* filename_ptrs[22];
    ptr_array[0] = str_menu_selfile;
    DIR* dir = opendir(".");

    u8 counter = 1;
    for (int i = 0; counter <= 22+1 && i < 256; i++)
    {
        char png[] = ".png";
        char ndsfd[] = ".ndsfd";

        struct dirent* dat = readdir(dir);
        if (dat == NULL) break;

        char* loc = strrchr(dat->d_name, '.');
        
        if (strcmp(loc, png) == 0 || strcmp(loc, ndsfd) == 0)
        {
            ptr_array[counter] = calloc(33, sizeof(u8)); // needs to be a calloc or else it ubs
            filename_ptrs[counter-1] = malloc(255);
            memcpy(filename_ptrs[counter-1], dat->d_name, 255);
            memcpy(ptr_array[counter], dat->d_name, 31);
            strcat(ptr_array[counter], "\n"); // also ensures the string ends with a null character
            counter++;
        }
    }

    closedir(dir);

    if (counter == 1)
    {
        menuWriteSingle(str_err_file);
        waitForInput();
        return NULL;
    }

    ptr_array[counter++] = str_hint_bback;
    ptr_array[counter++] = str_hint_asel;
    
    s32 cursor = 0;
    s32 selection = menuInputs(&cursor, 2, (struct InputIDs) {1,0,0}, (struct MenuDat) {1, 1, 1, counter, ptr_array}) - 2;

    if (selection == -1)
    {
        for (int i = 0; i < counter - 3; i++)
        {
            free(filename_ptrs[i]);
            free(ptr_array[i+1]);
        }
        return NULL;
    }

    FILE* file = fopen(filename_ptrs[selection], "rb");

    if (file == NULL)
    {
        menuWriteSingle("OOPS\n");
        while (true) swiWaitForVBlank();
    }
    
    for (int i = 0; i < counter - 3; i++)
    {
        free(filename_ptrs[i]);
        free(ptr_array[i+1]);
    }

    return file;
}
