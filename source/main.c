#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <filesystem.h>
#include <dirent.h>
#include <stdarg.h>

#include "main.h"
#include "menu/menu.h"
#include "strings.h"



// file structure:

// Header:
u8 compressiontype; // currently unused, should always be set to 0 (uncompressed)

// global variables:
// should be saved when registers are latched (ie. right before rendering)
u16 disp3dcnt;
u16 edgecolor[8];
u8 alphatest;
u32 clearcolor;
u32 cleardepoff;
u32 fogcolor;
u16 fogoffset;
u8 fogtable[32];
u16 toontable[32];

// track whether this one specific register has been written to, because i know for a fact melonDS inits it incorrectly.
// implementation detail: this bool is stored as a whole byte, skipping proper implementation of it is possible by simply writing an 0x01 or 0xFF byte.
// other write tracking bits *were* implemented and could be reimplemented, should demand for it exist.
// they were removed for the time being due to it overcomplicating implementation greatly for fairly limited benefits.
bool zdotdisp_track;

// initial state variables:
// should be saved before the first command is sent
// should we only define the used bits of the command, or the full 32 bits?
u16 zdotdisp;
u32 polyattr;
u32 polyattrunset;
u32 vtxcolor;
u32 viewport;
u32 projstack[16];
u32 posstack[32][16];
u32 vecstack[32][16];
u32 texstack[16];
u32 projmtx[16];
u32 posmtx[16];
u32 vecmtx[16];
u32 texmtx[16];
u32 matrixmode;
/*u32 normal; // init for normals scrapped
u32 normpolyattr;
u32 normtexmtx[16];
u32 normvecmtx[16];
u32 normdiffambi;
u32 normspecemis;
u32 normtexparams;
u32 normshininess[32];
u32 normlightvec[4];
u32 normlightvecvecmtx[4][16];
u32 normlightcolor[4];*/
u32 polygon;
u32 vtxxy;
u16 vtxz;
u32 texcoord;
u32 texparams;
u32 texpalette;
u32 diffambi;
u32 specemis;
u32 shininess[32];
u32 lightvec[4];
//u32 lightvecvecmtx[4][16]; also scrapped
u32 lightcolor[4];
u32 swapbuffer;
u8 vramcontrol[7]; // only 7 are actually saved/loaded/used. banks h and i are ignored
// vram state:
// (up to) 608 KiB of vram state :DDDDD
// note: vram state is ONLY saved/loaded for ACTIVE vram banks that're being used for 3d textures
// (ie. last bit of vramcnt must be set and the value of the mst must be 3).
// (additionally banks H and I are ignored due to not being allocatable to the 3d engine).

// other:
u32 numcmds;
u32 numparams;

// only store a maximum of 500KB
// nops and vec/box tests should not be added to any of these trackers
u8* cmd = NULL;
u32* param = NULL;

void initVars(FILE* file)
{
    // init most variables
    fread(&compressiontype, 1, sizeof(compressiontype), file);
    fread(&disp3dcnt, 1, sizeof(disp3dcnt), file);
    fread(edgecolor, 1, sizeof(edgecolor), file);
    fread(&alphatest, 1, sizeof(alphatest), file);
    fread(&clearcolor, 1, sizeof(clearcolor), file);
    fread(&cleardepoff, 1, sizeof(cleardepoff), file);
    fread(&fogcolor, 1, sizeof(fogcolor), file);
    fread(&fogoffset, 1, sizeof(fogoffset), file);
    fread(fogtable, 1, sizeof(fogtable), file);
    fread(toontable, 1, sizeof(toontable), file);

    fread(&zdotdisp_track, 1, 1, file);
    fread(&zdotdisp, 1, sizeof(zdotdisp), file);
    fread(&polyattr, 1, sizeof(polyattr), file);
    fread(&polyattrunset, 1, sizeof(polyattrunset), file);
    fread(&vtxcolor, 1, sizeof(vtxcolor), file);
    fread(&viewport, 1, sizeof(viewport), file);
    fread(projstack, 1, sizeof(projstack), file);
    fread(posstack, 1, sizeof(posstack), file);
    fread(vecstack, 1, sizeof(vecstack), file);
    fread(texstack, 1, sizeof(texstack), file);
    fread(projmtx, 1, sizeof(projmtx), file);
    fread(posmtx, 1, sizeof(posmtx), file);
    fread(vecmtx, 1, sizeof(vecmtx), file);
    fread(texmtx, 1, sizeof(texmtx), file);
    fread(&matrixmode, 1, sizeof(matrixmode), file);
    fread(&polygon, 1, sizeof(polygon), file);
    fread(&vtxxy, 1, sizeof(vtxxy), file);
    fread(&vtxz, 1, sizeof(vtxz), file);
    fread(&texcoord, 1, sizeof(texcoord), file);
    fread(&texparams, 1, sizeof(texparams), file);
    fread(&texpalette, 1, sizeof(texpalette), file);
    fread(&diffambi, 1, sizeof(diffambi), file);
    fread(&specemis, 1, sizeof(specemis), file);
    fread(shininess, 1, sizeof(shininess), file);
    fread(lightvec, 1, sizeof(lightvec), file);
    fread(lightcolor, 1, sizeof(lightcolor), file);
    fread(&swapbuffer, 1, sizeof(swapbuffer), file);
}

void initVram(FILE* file)
{
    // turn vram on and set to lcdc mode for easy writing
    fread(vramcontrol, 1, 7, file);
    for (int i = 0; i < 7; i++)
    {
        //if (i == 7) i++; // skip wramcnt
        vu8* address = (vu8*)((0x04000240 + i)); // vramcnt_a address
        *address = VRAM_ENABLE;
    }

    // init vram banks
    // ignore H & I because irrelevant to 3d gfx
    u16 vrambuffer[128*1024/2];
    // A
    if ((vramcontrol[0] & 0x83) == 0x83)
    {
        fread(vrambuffer, 2, 128*1024/2, file);
        memcpy(VRAM_A, vrambuffer, 128*1024);
    }
    // B
    if ((vramcontrol[1] & 0x83) == 0x83)
    {
        fread(vrambuffer, 2, 128*1024/2, file);
        memcpy(VRAM_B, vrambuffer, 128*1024);
    }
    // C
    if ((vramcontrol[2] & 0x87) == 0x83)
    {
        fread(vrambuffer, 2, 128*1024/2, file);
        memcpy(VRAM_C, vrambuffer, 128*1024);
    }
    // D
    if ((vramcontrol[3] & 0x87) == 0x83)
    {
        fread(vrambuffer, 2, 128*1024/2, file);
        memcpy(VRAM_D, vrambuffer, 128*1024);
    }
    // E
    if ((vramcontrol[4] & 0x87) == 0x83)
    {
        fread(vrambuffer, 2, 64*1024/2, file);
        memcpy(VRAM_E, vrambuffer, 64*1024);
    }
    // F
    if ((vramcontrol[5] & 0x87) == 0x83)
    {
        fread(vrambuffer, 2, 16*1024/2, file);
        memcpy(VRAM_F, vrambuffer, 16*1024);
    }
    // G
    if ((vramcontrol[6] & 0x87) == 0x83)
    {
        fread(vrambuffer, 2, 16*1024/2, file);
        memcpy(VRAM_G, vrambuffer, 16*1024);
    }

    // actually init vramcnt
    for (int i = 0; i < 7; i++)
    {
        //if (i == 7) i++;
        vu8* address = ((vu8*)(0x04000240 + i));
        *address = vramcontrol[i];
    }
}

void initGlobals()
{
    GFX_CONTROL = disp3dcnt;

    for (int i = 0; i < 8; i++)
        GFX_EDGE_TABLE[i] = edgecolor[i];

    GFX_ALPHA_TEST = alphatest;
    GFX_CLEAR_COLOR = clearcolor;
    *((vu32*)&GFX_CLEAR_DEPTH) = cleardepoff;
    GFX_FOG_COLOR = fogcolor;
    GFX_FOG_OFFSET = fogoffset;

    for (int i = 0; i < 32; i++)
    {
        GFX_FOG_TABLE[i] = fogtable[i];
        GFX_TOON_TABLE[i] = toontable[i];
    }
}

void initFrameState(bool trustemu)
{
    // init frame state
    if (zdotdisp_track || trustemu) GFX_CUTOFF_DEPTH = zdotdisp;
    GFX_VIEWPORT = viewport;
    GFX_COLOR = vtxcolor;
    GFX_DIFFUSE_AMBIENT = diffambi;
    GFX_SPECULAR_EMISSION = specemis;

    for (int i = 0; i < 32; i++)
        GFX_SHININESS = shininess[i];

    // init lights
    MATRIX_CONTROL = 2;
    MATRIX_IDENTITY = 0;
    for (int i = 0; i < 4; i++)
    {
        GFX_LIGHT_VECTOR = lightvec[i];
        GFX_LIGHT_COLOR = lightcolor[i];
    }
    
    // init texture state
    MATRIX_CONTROL = 3;
    MATRIX_IDENTITY = 0;
    
    GFX_TEX_COORD = texcoord;
    GFX_TEX_FORMAT = texparams;
    GFX_PAL_FORMAT = texpalette;

    // init matrix stack
    
    MATRIX_CONTROL = 0;
    for (int i = 0; i < 16; i++)
        MATRIX_LOAD4x4 = projstack[i];
    MATRIX_STORE = 0; 

    for (int j = 0; j < 32; j++)
    {
        MATRIX_CONTROL = 2;
        for (int i = 0; i < 16; i++)
            MATRIX_LOAD4x4 = vecstack[j][i];

        MATRIX_CONTROL = 1;
        for (int i = 0; i < 16; i++)
            MATRIX_LOAD4x4 = posstack[j][i];

        MATRIX_STORE = 0; 
    }

    MATRIX_CONTROL = 3;
    for (int i = 0; i < 16; i++)
        MATRIX_LOAD4x4 = texstack[i];
    MATRIX_STORE = 0; 


    // init matrices
    MATRIX_CONTROL = 0;
    for (int i = 0; i < 16; i++)
        MATRIX_LOAD4x4 = projmtx[i];

    MATRIX_CONTROL = 2;
    for (int i = 0; i < 16; i++)
        MATRIX_LOAD4x4 = vecmtx[i];
        
    MATRIX_CONTROL = 1;
    for (int i = 0; i < 16; i++)
        MATRIX_LOAD4x4 = posmtx[i];

    MATRIX_CONTROL = 3;
    for (int i = 0; i < 16; i++)
        MATRIX_LOAD4x4 = texmtx[i];
    
    MATRIX_CONTROL = matrixmode;
    
    GFX_POLY_FORMAT = polyattr;
    // init polygon engine (that's a term right?)
    GFX_BEGIN = polygon;

    // init some vertices just in case (render an entire polygon because otherwise it actually hangs entirely--)
    u8 vtxnum;
    if (polygon == 0 || polygon == 2) vtxnum = 3;
    else vtxnum = 4;
    for (int i = 0; i < vtxnum; i++)
    {
        GFX_VERTEX16 = vtxxy;
        GFX_VERTEX16 = vtxz;
    }
    // unset polygon format (will be properly set when the next polygon begin cmd gets sent)
    GFX_POLY_FORMAT = polyattrunset;

    GFX_FLUSH = swapbuffer;
}

void runGX()
{
    for (u32 i = 0, j = 0; i < numcmds; i++)
    {   
        // handle zerodotdisp reg writes
        if (cmd[i] == 255)
            GFX_CUTOFF_DEPTH = param[j++];
        else
        {
            vu32* finaladdr = (vu32*)((u32)&GFX_FIFO + (cmd[i] << 2));
            if (paramcount[cmd[i]] != 0)
                for (int k = 0; k < paramcount[cmd[i]]; k++)
                    *finaladdr = param[j++];
            else
                *finaladdr = 0;
        }
    }
}

void runDump()
{
    initFrameState(false);

    swiWaitForVBlank(); // wait for buffer swap to actually take place

    videoSetMode(MODE_0_3D);
    runGX();
}

bool loadFile(FILE* file)
{
    fseek(file, 0, SEEK_SET);
    initVars(file);
    initVram(file);
    initGlobals();
    
    fread(&numcmds, 1, sizeof(numcmds), file);
    fread(&numparams, 1, sizeof(numparams), file);
    
    s32 totalram = numcmds + (numparams * 4);
    if (totalram >= 500000 || totalram <= 0) return false;

    free(cmd);
    free(param);

    cmd = malloc(numcmds);
    param = malloc(numparams*4);

    fread(cmd, 1, numcmds, file);
    fread(param, 4, numparams, file);
    
    runDump();
    return true;
}

u8 menuDirSelect()
{
    bool opensd = false;
    bool usingfoldersd = false;
    bool openfat = false;
    bool usingfolderfat = false;
    bool opennitro = false;
    
    // test console sd
    DIR* testdir = opendir("sd:/framedumps");
    if (testdir == NULL) testdir = opendir("sd:/");
    else usingfoldersd = true;
    if (testdir != NULL)
    {
        opensd = true;
        closedir(testdir);
    }

    // test flash cart sd
    testdir = opendir("fat:/framedumps");
    if (testdir == NULL) testdir = opendir("fat:/");
    else usingfolderfat = true;
    if (testdir != NULL)
    {
        openfat = true;
        closedir(testdir);
    }

    // test romfs
    testdir = opendir("nitro:/");
    if (testdir != NULL)
    {
        opennitro = true;
        closedir(testdir);
    }

    // count found dirs
    u8 numfound = opensd + openfat + opennitro; 

    // if none opened
    if (numfound <= 1)
    {
        if (numfound == 0)
        {
            menuWrite(str_err_dir);
            while(true) swiWaitForVBlank();
        }
        else
        {
            if (opennitro) return 0;
            if (opensd) return (usingfoldersd ? 3 : 1);
            if (openfat) return (usingfolderfat ? 4 : 2);
        }
    }

    u8* ptr_array[6] = {[0] = str_menu_seldir};
    int i = 1;
    if (opennitro)
    {
        ptr_array[i] = str_opt_nitro;
        i++;
    }
    if (opensd)
    {
        ptr_array[i] = str_opt_sd;
        i++;
    }
    if (openfat)
    {
        ptr_array[i] = str_opt_fc;
        i++;
    }

    ptr_array[i] = str_hint_bback;
    i++;

    ptr_array[i] = str_hint_asel;
    i++;

    u16 selection = menuInputs(0, (struct InputIDs) {5,0,0}, 1, 1, 1, i, ptr_array);
    if (!opennitro && selection == 0) selection++;
    if ((!opennitro || !opensd) && selection == 1) selection++;

    if ((selection == 1 && usingfoldersd) || (selection == 2 && usingfolderfat)) selection++;

    return selection;
}

FILE* menuFileSelect(u16 dir)
{
    u8 dirr[18];
    if (dir == 0) strncpy(dirr, "nitro:/", sizeof(dirr));
    else if (dir == 1 || dir == 3) strncpy(dirr, "sd:/", sizeof(dirr));
    else if (dir == 2 || dir == 4) strncpy(dirr, "fat:/", sizeof(dirr));
    if (dir >= 3) strncpy(dirr, "/framedumps/", sizeof(dirr));

    u8* ptr_array[25];
    ptr_array[0] = str_menu_selfile;

    u8 numentries = 0;
    u8 locations[22] = {};
    for (u16 i = 0; i < 256; i++)
    {
        u8 name[28];
        snprintf(name, sizeof(name), "%sdump%i.fd", dirr, i);
        FILE* file = fopen(name, "rb");
        if (file != NULL)
        {
            ptr_array[numentries+1] = calloc(1, 13);
            locations[numentries] = i;
            (ptr_array[numentries+1])[0] = 0xF0;
            snprintf(&(ptr_array[numentries+1])[1], 12, "DUMP%i.FD\n", i);
            numentries++;
            fclose(file);
            if (numentries == 22) break;
        }
    }
    if (numentries == 0)
    {
        menuWrite(str_err_file);
        u16 keys = 0;
        while(keys & (KEY_A | KEY_B | KEY_START))
        {
            scanKeys();
            keys = keysHeld();
            swiWaitForVBlank();
        }
        return NULL;
    }

    u8 count = numentries+1;
    ptr_array[count++] = str_hint_bback;
    ptr_array[count++] = str_hint_asel;

    s16 selection = menuInputs(2, (struct InputIDs) {1,0,0}, 1, 1, 1, count, ptr_array) - 2;

    for (int i = 1; i < numentries+1; i++)
        free(ptr_array[i]);

    if (selection == -1) return NULL;

    u8 name[28];
    snprintf(name, sizeof(name), "%sdump%i.fd", dirr, locations[selection]);
    return fopen(name, "rb");
}
/*
u8 menuScreenshot()
{
    u8* ptr_array;
}*/

u8 menuMain(FILE** file)
{
    u8* ptr_array[] =
    {
        str_menu_generic,
        str_opt_changefile,
        str_opt_rerender,
        str_opt_gx,
        str_hint_rscreenshot,
        str_hint_asel
    };

    while (true)
    {
        u8 selection = menuInputs(2, (struct InputIDs) {0,0,1}, 1, 1, 1, (sizeof(ptr_array) / sizeof(ptr_array[0])), ptr_array);
        switch(selection)
        {
            case 1: // R button - screenshot
                //menuScreenshot();
                break;

            case 2: // change file
            {
                bool unloaded = false;
                while(true)
                {
                    FILE* newfile = NULL;
                    while (newfile == NULL)
                    {
                        u8 sel = menuDirSelect();
                        if (sel == 5)
                        {
                            if (newfile != NULL) fclose(newfile);
                            if (unloaded) loadFile(*file);
                            goto abort;
                        }
                        newfile = menuFileSelect(sel);
                    }
                    unloaded = true;
                    if (loadFile(newfile))
                    {
                        fclose(*file);
                        *file = newfile;
                        break;
                    }
                    else fclose(newfile);
                }
                break;
            }
            case 3: // rerender
                runDump();
                break;

            case 4: // gx menu
                //menuGX();
                break;
        }
        abort:
    }
}

int main()
{
    //consoleDebugInit(DebugDevice_NOCASH);
    // init 2d engine to a custom menu implementation
    menuInit();

    // enable 3d gpu
    powerOn(POWER_3D_CORE | POWER_MATRIX);

    // setup file
    fatInitDefault();
    nitroFSInit(NULL);

    FILE* file = NULL;

    while (true)
    {
        while (file == NULL)
        {
            u16 sel = menuDirSelect();
            if (sel == 5) return 0;
            file = menuFileSelect(sel);
        }
        menuClear();
        if (loadFile(file)) break;
        fclose(file);
    }

    menuMain(&file);
}
