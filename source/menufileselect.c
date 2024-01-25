#include <dirent.h>

#include "menus.h"



bool menuDirSelect(bool exit)
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
    if ((nitrodir == NULL) && (fatdir == NULL) && (sddir == NULL))
    {
        menuWriteSingle(str_err_dir);
        while(true) swiWaitForVBlank();
    }

    u8* headers[] =
    {
        str_menu_seldir,
        NULL,
    };

    struct MenuEntry ent_nitro =
    {
        .String = str_opt_nitro,
        .SubType = 1,
        .Type = Entry_Button,
    };

    struct MenuEntry ent_sd =
    {
        .String = str_opt_sd,
        .SubType = 2,
        .Type = Entry_Button,
    };

    struct MenuEntry ent_fc =
    {
        .String = str_opt_fc,
        .SubType = 3,
        .Type = Entry_Button,
    };

    struct MenuEntry entries[3];

    int i = 0;
    if (nitrodir != NULL)
    {
        entries[i] = ent_nitro;
        i++;
    }
    if (sddir != NULL)
    {
        entries[i] = ent_sd;
        i++;
    }
    if (fatdir != NULL)
    {
        entries[i] = ent_fc;
        i++;
    }

    s32 cursor = 0;
    u32 selection = menuInputs((struct MenuDat) {&cursor, headers, entries, i,
        (struct InputIDs) {.ScrollUp = KEY_UP, .ScrollDown = KEY_DOWN, .Select = KEY_A, .Exit = (exit ? KEY_B : 0)}});

    switch (selection)
    {
        case 1:
            closedir(nitrodir);
            closedir(fatdir);
            closedir(sddir);
            chdir("nitro:/");
            return true;
        case 2:
            closedir(nitrodir);
            closedir(fatdir);
            closedir(sddir);
            chdir((usingfoldersd ? "sd:/framedumps" : "sd:/"));
            return true;
        case 3:
            closedir(nitrodir);
            closedir(fatdir);
            closedir(sddir);
            chdir((usingfolderfat ? "fat:/framedumps" : "fat:/"));
            return true;
        case RetExit:
            closedir(nitrodir);
            closedir(fatdir);
            closedir(sddir);
            return false;
        default:
            __builtin_unreachable();
    }
}

FILE* menuFileSelect()
{
    struct MenuEntry* entries = calloc(16, sizeof(struct MenuEntry));
    u8** filename_ptrs = malloc(sizeof(u8**) * 16);

    for (u32 i = 0; i < 16; i++)
        entries[i].Type = Entry_Button;

    DIR* dir = opendir(".");
    u8 arraysize = 16;
    u8 counter = 0;
    for (int i = 0; i < 256; i++)
    {
        char png[] = ".png";
        char ndsfd[] = ".ndsfd";

        struct dirent* dat = readdir(dir);
        if (dat == NULL) break;

        char* loc = strrchr(dat->d_name, '.');
        
        if (strcmp(loc, png) == 0 || strcmp(loc, ndsfd) == 0)
        {
            entries[counter].String = calloc(33, sizeof(u8)); // needs to be a calloc or else it ubs
            filename_ptrs[counter] = malloc(strlen(dat->d_name)+1);
            strcpy(filename_ptrs[counter], dat->d_name);
            memcpy(entries[counter].String, dat->d_name, 31);
            strcat(entries[counter].String, "\n"); // also ensures the string ends with a null character
            counter++;
            if (counter >= arraysize)
            {
                arraysize += 16;

                entries = realloc(entries, sizeof(struct MenuEntry) * arraysize);
                filename_ptrs = realloc(filename_ptrs, sizeof(u8**) * arraysize);

                memset(&entries[arraysize-16], 0, sizeof(struct MenuEntry) * 16);
                for (u32 i = arraysize - 16; i < arraysize; i++)
                    entries[i].Type = Entry_Button;
            }
        }
    }

    closedir(dir);

    if (counter == 0)
    {
        menuWriteSingle(str_err_file);
        waitForInput();
        free(entries);
        free(filename_ptrs);
        return NULL;
    }
    
    u8* headers[] =
    {
        str_menu_selfile,
        NULL,
    };

    s32 cursor = 0;
    s32 selection = menuInputs((struct MenuDat) {&cursor, headers, entries, counter,
        (struct InputIDs) {.ScrollUp = KEY_UP, .ScrollDown = KEY_DOWN, .Select = KEY_A, .Exit = KEY_B}});

    if (selection == RetExit)
    {
        for (int i = 0; i < counter; i++)
        {
            free(filename_ptrs[i]);
            free(entries[i].String);
        }
        free(entries);
        free(filename_ptrs);
        return NULL;
    }

    FILE* file = fopen(filename_ptrs[selection], "rb");

    if (file == NULL)
    {
        menuWriteSingle("\xF2""FILE OPEN ERROR: OOPSIE\n");
        while (true) swiWaitForVBlank();
    }
    
    for (int i = 0; i < counter; i++)
    {
        free(filename_ptrs[i]);
        free(entries[i].String);
    }
    free(entries);
    free(filename_ptrs);

    return file;
}
