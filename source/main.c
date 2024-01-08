#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <filesystem.h>
#include <dirent.h>
#include <stdarg.h>

#include "font.h"
#include "main.h"




// file structure:

// misc bits -- SCRAPPED
// note: the order of these bits is not always the same as the order of the variables in the file (might change this later)
// also note: some of these regs have more bits than used, they can(?) be written to but they dont do anything(?), so we'll just ignore them to save on file size
/*u8 bits1; // 0 disp3dcnt; 1 alpha test; 2-3 clear color low/hi; 4 clear dep; 5 clear offset; 6-7 fog color low/hi;
u8 bits2; // 0 fog offset; 1 zero dot disp; 2 polyattr; 3 unset polyattr; 4-5 vtxcolor type; 6 vtxcolor; 7 viewport;
u8 bits3; // 0 matrixmode; 1 normal; 2 nordifamb; 3 norspecemis; 4 nortexparam; 5 polygon; 6 vtx; 7 texcoord;
u8 bits4; // 0-3 norligvec; 4-7 norligcol;
u8 bits5; // 0 texparam; 1 texpalette; 2 diffambi; 3 specemis; 4 norpolyattr;
u8 bits6; // 0-3 lightvector; 4-7 lightcolor;
u16 edgecolor_mask;
u8 fogtable_mask[4];
u8 toontable_mask[8];
u16 projstack_mask; // can matrixes even be partially defined? what even happens when you do that?
u16 posstack_mask[32];
u16 vecstack_mask[32];
u16 texstack_mask;
u16 projmtx_mask;
u16 posmtx_mask;
u16 vecmtx_mask;
u16 texmtx_mask;
u16 normtexmtx_mask;
u16 normvecmtx_mask;
u32 normshininess_mask;
u16 normlightvecvecmtx_mask[4];
u32 shininess_mask;
u16 lightvecvecmtx_mask[4];*/

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
// nops should not be added to any of these trackers
u32 numcmds;
// 30000 as a max is just a guess tbh
// i dont actually know how many commands the gpu can process in one frame at a maximum
// probably a *lot* more if you really try to
// this *should* cover most real games though
u8 cmd[30000];
u32 param[90000];

#define MAP_OFFSET              8192
#define MAP_WIDTH               (256 / 8)
#define MAP_HEIGHT              (192 / 8)
#define MAP_AREA                (MAP_WIDTH * MAP_HEIGHT)
#define PALETTE_SIZE            16
#define DISABLE_BACK_BUTTON     0

u16 menucursor = 0;


void initVars(FILE* file)
{
    // init most variables
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
    for (int i = 0; i < 7; i++)
    {
        //if (i == 7) i++; // skip wramcnt
        fread(&vramcontrol[i], 1, 1, file);
        vu8* address = (vu8*)((0x04000240 + i)); // vramcnt_a address
        *address = VRAM_ENABLE;
    }
    // init vram banks
    // A
    if ((vramcontrol[0] & 0x83) == 0x83)
    for (int i = 0; i < 128 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6800000)[i/4] = vrambuffer; // vram a lcdc address
    }
    // B
    if ((vramcontrol[1] & 0x83) == 0x83)
    for (int i = 0; i < 128 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6820000)[i/4] = vrambuffer;
    }
    // C
    if ((vramcontrol[2] & 0x87) == 0x83)
    for (int i = 0; i < 128 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6840000)[i/4] = vrambuffer;
    }
    // D
    if ((vramcontrol[3] & 0x87) == 0x83)
    for (int i = 0; i < 128 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6860000)[i/4] = vrambuffer;
    }
    // E
    if ((vramcontrol[4] & 0x87) == 0x83)
    for (int i = 0; i < 64 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6880000)[i/4] = vrambuffer;
    }
    // F
    if ((vramcontrol[5] & 0x87) == 0x83)
    for (int i = 0; i < 16 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6890000)[i/4] = vrambuffer;
    }
    // G
    if ((vramcontrol[6] & 0x87) == 0x83)
    for (int i = 0; i < 16 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6894000)[i/4] = vrambuffer;
    }
    // ignore H & I because irrelevant to 3d gfx

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
        // override vector test's cmd id since it does nothing but waste cycles to implement support for the zerodotdisp register with only 8 bits
        if (cmd[i] == 0x72)
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

void loadFile(FILE* file)
{
    initVars(file);
    initVram(file);
    initGlobals();
    
    fread(&numcmds, 1, sizeof(numcmds), file);

    for (u32 i = 0, j = 0; i < numcmds; i++)
    {
        fread(&cmd[i], 1, sizeof(cmd[0]), file);

        for (int k = 0; k < paramcount[cmd[i]]; k++)
        {
            fread(&param[j], 1, sizeof(param[0]), file);
            j++;
        }
    }
    
    runDump();
}

void mapWrite(u8 tile, u8 palette)
{
    BG_GFX_SUB[MAP_OFFSET + menucursor++] = (palette << 12) | tile;
}

void mapWriteRev(u8 tile, u8 palette)
{
    BG_GFX_SUB[MAP_OFFSET + menucursor--] = (palette << 12) | tile;
}

void menuWrite(u8* text)
{
    u8 palette = 0;
    for (u16 i = 0; text[i] != 0; i++)
    {
        switch (text[i])
        {
        case '\n':
            menucursor = ((menucursor >> 5) + 1) << 5; // a way to divide/multiply by 32 without actually dividing/multiplying by 32
            break;
        case ((u8)'\xF0') ... ((u8)'\xFF'):
            palette = text[i];
            break;
        default:
            mapWrite(lookup[text[i]], palette);
            break;
        }
    }
}

void menuWriteRev(u8* text)
{
    u8 palette = 0;
    u8 numchars = 0;
    for (u16 i = 0; text[i] != 0; i++)
        numchars++;

    for (u16 i = numchars; i > 0; i--)
    {
        switch (text[i])
        {
        /*case '\n':
            menucursor = ((menucursor >> 5) << 5) - 1; // a way to divide/multiply by 32 without actually dividing/multiplying by 32
            break;*/
        case ((u8)'\xF0') ... ((u8)'\xFF'):
            palette = text[i];
            break;
        default:
            mapWriteRev(lookup[text[i]], palette);
            break;
        }
    }
}

void menuClear()
{
    // clear the visible area of the map
    for(int i = 0; i < MAP_AREA; i++)
        BG_GFX_SUB[MAP_OFFSET+i] = 0;
    menucursor = 0;
}

void menuInit()
{
    VRAM_H_CR = VRAM_ENABLE | VRAM_H_SUB_BG;
    VRAM_I_CR = VRAM_ENABLE | VRAM_I_SUB_BG_0x06208000;
    REG_DISPCNT_SUB = DISPLAY_BG0_ACTIVE | (1<<16);
    (*((vu16*)0x04001008)) = (8<<8); // could not find a define for any of the bgcnts
    
    menuClear();
    // allocate tileset to vram
    for(int i = 0; i < max; i++)
        for (int j = 0; j < 32; j+=2)
            BG_GFX_SUB[((16*i)+(j>>1))] = charset[i][j] | charset[i][j+1] << 8;
    // init relevant palettes
    BG_PALETTE_SUB[0] = 0;
    BG_PALETTE_SUB[1] = 0x7FFF;
    BG_PALETTE_SUB[PALETTE_SIZE+0] = 0;
    BG_PALETTE_SUB[PALETTE_SIZE+1] = 0x1F; // red
    BG_PALETTE_SUB[(PALETTE_SIZE*2)+0] = 0;
    BG_PALETTE_SUB[(PALETTE_SIZE*2)+1] = 0x3FF; // yellow
}

void menuRender(u8 sel, u8 header, u8 footerL, u8 footerR, int numstr, u8** strings)
{
    menuClear();
    int j = 0;
    for (int i = 0; i < header; i++)
    {
        menuWrite(strings[j++]);
        sel++;
    }

    numstr -= ((u8)footerL + footerR);
    for (;j < numstr; j++)
    {
        if (j == sel) mapWrite(caret, 0);
        else mapWrite(0, 0);
        menuWrite(strings[j]);
    }

    for (int i = 0; i < footerL; i++)
    {
        menucursor = MAP_AREA - (MAP_WIDTH * (footerL - i));
        menuWrite(strings[j++]);
    }
    for (int i = 0; i < footerR; i++)
    {
        menucursor = MAP_AREA - (MAP_WIDTH * (footerL - i + 1) - 1);
        menuWriteRev(strings[j++]);
    }
}

u16 menuInputs(u16 startID, struct InputIDs inputids, u8 header, u8 footerL, u8 footerR, int numstr, u8** strings)
{
    u16 numentries = numstr - header - footerL - footerR;
    s8 cursor = 0;
    bool menudirty = true;
    scanKeys();
    u16 prevkeys = keysHeld();
    while (true)
    {   
        swiWaitForVBlank();
        if (menudirty)
        {
            menuRender(cursor, header, footerL, footerR, numstr, strings);
            menudirty = false;
        }

        scanKeys();
        u16 keys = keysHeld();
        if (keys & KEY_A && !(prevkeys & KEY_A))
        {
            return cursor + startID;
        }
        else if (inputids.B != 0 && keys & KEY_B && !(prevkeys & KEY_B))
        {
            return inputids.B;
        }
        else if (inputids.X != 0 && keys & KEY_B && !(prevkeys & KEY_B))
        {
            return inputids.X;
        }
        else if (inputids.R != 0 && keys & KEY_B && !(prevkeys & KEY_B))
        {
            return inputids.R;
        }
        else if (keys & KEY_UP && !(prevkeys & KEY_UP))
        {
            cursor--;
            if (cursor < 0) cursor = numentries-1;
            menudirty = true;
        }
        else if (keys & KEY_DOWN && !(prevkeys & KEY_DOWN))
        {
            cursor++;
            if (cursor > numentries-1) cursor = 0;
            menudirty = true;
        }
        prevkeys = keys;
    }
}

u8 menuDirSelect(bool back)
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

    u8* ptr_array[5] = {[0] = str_menu_seldir};
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

    u8 backid = DISABLE_BACK_BUTTON;
    if (back)
    {
        ptr_array[i] = str_hint_bback;
        i++;
        backid = 5;
    }
    ptr_array[i] = str_hint_asel;
    i++;


    u16 selection = menuInputs(0, (struct InputIDs) {backid,0,0}, 1, 1, back, i, ptr_array);
    if (!opennitro && selection == 0) selection++;
    if ((!opennitro || !opensd) && selection == 1) selection++;

    if ((selection == 1 && usingfoldersd) || (selection == 2 && usingfolderfat)) selection++;

    return selection;
}

FILE* menuFileSelect(u8 dir)
{
    u8 numentries = 0;
    u8* ptr_array[24];
    u8 locations[24] = {};
    u8 dirr[18];

    if (dir == 0) strncpy(dirr, "nitro:/", sizeof(dirr));
    else if (dir == 1 || dir == 3) strncpy(dirr, "sd:/", sizeof(dirr));
    else if (dir == 2 || dir == 4) strncpy(dirr, "fat:/", sizeof(dirr));
    if (dir >= 3) strncpy(dirr, "/framedumps/", sizeof(dirr));

    ptr_array[0] = str_menu_selfile;

    for (u16 i = 0; i < 256; i++)
    {
        u8 name[28];
        snprintf(name, sizeof(name), "%sdump%i.fd", dirr, i);
        FILE* file = fopen(name, "rb");
        if (file != NULL)
        {
            u8 name2[13] = {};
            ptr_array[numentries+1] = calloc(1, sizeof(name2));
            locations[numentries] = i;
            name2[0] = 0xF0;
            snprintf(&name2[1], sizeof(name2) - 1, "DUMP%i.FD\n", i);
            strncpy(ptr_array[numentries+1], name2, sizeof(name2));
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

    numentries++;
    ptr_array[numentries++] = str_hint_bback;
    ptr_array[numentries++] = str_hint_asel;

    s16 selection = menuInputs(2, (struct InputIDs) {1,0,0}, 1, 1, 1, numentries, ptr_array) - 2;

    for (int i = 1; i < numentries+1; i++)
        free(ptr_array[i]);

    if (selection == -1) return NULL;

    u8 name[28];
    snprintf(name, sizeof(name), "%sdump%i.fd", dirr, locations[selection]);
    return fopen(name, "rb");
}

u8 menuScreenshot()
{
    u8* ptr_array;
}

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
                while(file == NULL)
                {
                    u8 sel = menuDirSelect(true);
                    if (sel == 5) break;
                    file = menuFileSelect(sel);
                }
                loadFile(file);
                break;

            case 3: // rerender
                runDump();
                break;

            case 4: // gx menu
                //menuGX();
                break;
        }
    }
}

int main()
{
    //consoleDebugInit(DebugDevice_NOCASH);
    menuInit();

    // setup file
    fatInitDefault();
    nitroFSInit(NULL);

    FILE* file = NULL;// = fopen("dump0.fd", "rb");

    while (file == NULL) file = menuFileSelect(menuDirSelect(false));
    menuClear();

    // enable 3d gpu
    powerOn(POWER_3D_CORE | POWER_MATRIX);
    loadFile(file);

    menuMain(&file);
}
