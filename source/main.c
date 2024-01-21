#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <filesystem.h>

#include "main.h"
#include "menu/menu.h"
#include "strings.h"
#include "menus.h"



// file structure:
// note: data can optionally be embedded into a png, if so, it should be done in a chunk with the name: "ndSf"

// Header:
// header should remain uncompressed.

/*
5 bytes arbitrary header -- should always be "NTRFD" (maybe we'll add TWLFD in the future if any key dsi specific quirks are discovered?)
2 bytes version -- check main.h for CUR_FD_REV define
1 byte compressiontype -- currently unused, should always be set to 0 (uncompressed)
*/

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
u32 lightcolor[4];
u32 swapbuffer;
u8 vramcontrol[7]; // only 7 are actually saved/loaded/used. banks h and i are ignored

// vram state:
// (up to) 608 KiB of vram state :DDDDD
// note: vram state is ONLY saved/loaded for ACTIVE vram banks that're being used for 3d textures/texture palettes
// (ie. last bit of vramcnt must be set and the value of the mst must be 3).
// (additionally banks H and I are ignored due to not being allocatable to the 3d engine).

// other:
u32 numcmds;
u32 numparams;

// only store a maximum of 500KB
// cmds should be stored in one consecutive block before all params are stored.
// nops and vec/box tests should not be added to any of these trackers
// id 255 is used to store zero dot disp reg writes
u8* cmd = NULL;
u32* param = NULL;


// misc global variables -------------------------------------------------------------------------------------
// not included in frame dump file

const u8 paramcount[256] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 1, 1, 1, 0,16,12,16,12, 9, 3, 3, 0, 0, 0,
    1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    1, 1, 1, 1,32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void waitForInput()
{
    scanKeys();
    u16 prevkeys = keysHeld();
    while(true)
    {
        scanKeys();
        u16 keys = keysHeld();
        if ((keys & KEY_START && !(prevkeys & KEY_START)) || (keys & KEY_A && !(prevkeys & KEY_A)) || (keys & KEY_B && !(prevkeys & KEY_B)))
            return;
        prevkeys = keys;
        swiWaitForVBlank();
    }
}

bool handlePNG(FILE* file)
{
    fseek(file, 0, SEEK_SET);

    u8 buffer[8];
    fread(buffer, 1, 8, file);

    u8 pngheader[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}; // check if png
    if(memcmp(buffer, pngheader, sizeof(pngheader)) != 0)
    {
        fseek(file, 0, SEEK_SET);
        return true; // valid file; not a png
    }

    u8 ndSf[4] = {'n', 'd', 'S', 'f'};
    while (true)
    {
        if (fread(buffer, 1, 8, file) < 8) return false; // could not file valid fd data in png
        if (memcmp(&buffer[4], ndSf, sizeof(ndSf)) == 0) return true; // valid file; fd data chunk found

        // convert size value's endianness, also add 4 to skip past checksums
        int offset = (buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3]) + 4;
        fseek(file, offset, SEEK_CUR);
    }
}

bool verifyHeader(FILE* file)
{
    u8 header[5];
    fread(header, 1, sizeof(header), file);
    if (memcmp(header, "NTRFD", sizeof(header)) != 0)
    {
        menuWriteSingle(str_err_headerfail);
        waitForInput();
        return false;
    }
    u16 version;
    fread(&version, 1 , sizeof(version), file);
    if (version < CUR_FD_REV)
    {
        menuWriteSingle(str_err_oldver);
        waitForInput();
        return false;
    }
    else if (version > CUR_FD_REV)
    {
        menuWriteSingle(str_err_timetravel);
        waitForInput();
        return false;
    }
    return true;
}

void initVars(FILE* file)
{
    // init most variables
    // todo: could this be done in one fread with a struct?
    fread(&disp3dcnt, 1, sizeof(disp3dcnt), file);
    fread(edgecolor, 1, sizeof(edgecolor), file);
    fread(&alphatest, 1, sizeof(alphatest), file);
    fread(&clearcolor, 1, sizeof(clearcolor), file);
    fread(&cleardepoff, 1, sizeof(cleardepoff), file);
    fread(&fogcolor, 1, sizeof(fogcolor), file);
    fread(&fogoffset, 1, sizeof(fogoffset), file);
    fread(fogtable, 1, sizeof(fogtable), file);
    fread(toontable, 1, sizeof(toontable), file);

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
    // nab vramcontrols
    fread(vramcontrol, 1, 7, file);

    // init vram banks
    // ignore H & I because irrelevant to 3d gfx
    // todo: avoid activating the first 4 banks if unneeded?
    u16 vrambuffer[128*1024/2];
    // A
    VRAM_A_CR = VRAM_ENABLE;
    if ((vramcontrol[0] & 0x83) == 0x83)
    {
        fread(vrambuffer, 2, 128*1024/2, file);
        memcpy(VRAM_A, vrambuffer, 128*1024);
        VRAM_A_CR = vramcontrol[0];
    }
    else dispcapbank = 0;
    // B
    VRAM_B_CR = VRAM_ENABLE;
    if ((vramcontrol[1] & 0x83) == 0x83)
    {
        fread(vrambuffer, 2, 128*1024/2, file);
        memcpy(VRAM_B, vrambuffer, 128*1024);
        VRAM_B_CR = vramcontrol[1];
    }
    else dispcapbank = 1;
    // C
    VRAM_C_CR = VRAM_ENABLE;
    if ((vramcontrol[2] & 0x87) == 0x83)
    {
        fread(vrambuffer, 2, 128*1024/2, file);
        memcpy(VRAM_C, vrambuffer, 128*1024);
        VRAM_C_CR = vramcontrol[2];
    }
    else dispcapbank = 2;
    // D
    VRAM_D_CR = VRAM_ENABLE;
    if ((vramcontrol[3] & 0x87) == 0x83)
    {
        fread(vrambuffer, 2, 128*1024/2, file);
        memcpy(VRAM_D, vrambuffer, 128*1024);
        VRAM_D_CR = vramcontrol[3];
    }
    else dispcapbank = 3;
    // E
    if ((vramcontrol[4] & 0x87) == 0x83)
    {
        VRAM_E_CR = VRAM_ENABLE;
        fread(vrambuffer, 2, 64*1024/2, file);
        memcpy(VRAM_E, vrambuffer, 64*1024);
        VRAM_E_CR = vramcontrol[4];
    }
    // F
    if ((vramcontrol[5] & 0x87) == 0x83)
    {
        VRAM_F_CR = VRAM_ENABLE;
        fread(vrambuffer, 2, 16*1024/2, file);
        memcpy(VRAM_F, vrambuffer, 16*1024);
        VRAM_F_CR = vramcontrol[5];
    }
    // G
    if ((vramcontrol[6] & 0x87) == 0x83)
    {
        VRAM_G_CR = VRAM_ENABLE;
        fread(vrambuffer, 2, 16*1024/2, file);
        memcpy(VRAM_G, vrambuffer, 16*1024);
        VRAM_G_CR = vramcontrol[6];
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

void initFrameState()
{
    // init frame state
    GFX_CUTOFF_DEPTH = zdotdisp;
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

u32 runGX(bool finish)
{
    transidmask = 0;
    u32 currtexparam = texparams;
    u32 currpolyattrunset = polyattrunset;
    u32 currpolyattr = polyattr;
    for (u32 i = 0, j = 0; i < numcmds; i++)
    {   
        // handle zerodotdisp reg writes
        if (cmd[i] == 255)
            GFX_CUTOFF_DEPTH = param[j++];
        else if (finish || cmd[i] != 0x50)
        {
            vu32* finaladdr = (vu32*)((u32)&GFX_FIFO + (cmd[i] << 2));
            if (paramcount[cmd[i]] != 0)
                for (int k = 0; k < paramcount[cmd[i]]; k++)
                    *finaladdr = param[j++];
            else
                *finaladdr = 0;
        }
        else return param[j];

        if (!finish)
        {
            if (cmd[i] == 0x29) currpolyattrunset = param[j-1]; // polyattr
            else if (cmd[i] == 0x40) currpolyattr = currpolyattrunset; // begin poly
            else if (cmd[i] == 0x2A) currtexparam = param[j-1]; // texparam
            else if (cmd[i] >= 0x23 && cmd[i] <= 0x28) // vtx cmds
            {
                // create a mask of translucent polygon ids
                u8 texmode = ((currtexparam >> 26) & 0x7); 
                u8 alpha = ((currpolyattr >> 16) && 0x1F); 
                if (texmode == 6 || texmode == 1 || (alpha != 31 && alpha != 0))
                {
                    u8 polyid = (currpolyattr >> 24) & 0x3F;
                    transidmask |= (u64)1 << polyid;
                }
            }
        }
    }
    return 0;
}

u32 runDump(bool finish)
{
    initFrameState();

    swiWaitForVBlank(); // wait for buffer swap to actually take place

    videoSetMode(MODE_0_3D);
    return runGX(finish);
}

bool loadFile(FILE* file)
{
    if (!handlePNG(file)) // probably shouldn't be done if the file isn't expected to be a png but im too lazy to rework this to handle that properly
        return false;

    if (!verifyHeader(file)) // check if header is valid
        return false;
    u8 compressiontype;
    fread(&compressiontype, 1, sizeof(compressiontype), file); // load dummy compression value

    // actually load the file
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
    
    runDump(true);
    return true;
}

void menuMain(FILE** file)
{
    u8* ptr_array[] =
    {
        str_menu_generic,
        str_opt_changefile,
        str_opt_rerender,
        str_opt_edit,
        str_opt_quit,
        str_hint_rscreenshot,
        str_hint_asel,
    };

    s32 cursor = 0;
    while (true)
    {
        u32 selection = menuInputs(&cursor, 2, (struct InputIDs) {0,0,1}, 1, 1, 1, (sizeof(ptr_array) / sizeof(ptr_array[0])), ptr_array);
        switch(selection)
        {
            case 1: // R button - screenshot
                // todo: allow for disabling screenshot button if they aren't possible
                menuScreenshot();
                break;

            case 2: // change file
            {
                bool unloaded = false;
                while(true)
                {
                    FILE* newfile = NULL;
                    while (newfile == NULL)
                    {
                        
                        if (!menuDirSelect())
                        {
                            fclose(newfile);
                            if (unloaded) loadFile(*file);
                            goto abort;
                        }
                        newfile = menuFileSelect();
                    }
                    menuClear();
                    unloaded = true;
                    if (loadFile(newfile))
                    {
                        fclose(*file);
                        *file = newfile;
                        break;
                    }
                    else
                    {
                        fclose(newfile);
                        menuWriteSingle(str_err_invalidfile);
                        waitForInput();
                    }
                }
                break;
            }
            case 3: // rerender
                runDump(true);
                break;

            case 4: // edit menu
                menuEditVars();
                break;
            
            case 5: // quit
                return;
        }
        abort:
    }
}

int main()
{
    //consoleDebugInit(DebugDevice_NOCASH);

    // init secondary 2d engine to a custom menu implementation
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
            if (!menuDirSelect()) return 0;
            file = menuFileSelect();
        }
        menuClear();
        if (loadFile(file)) break;
        fclose(file);
        file = NULL;
        menuWriteSingle(str_err_invalidfile);
        waitForInput();
    }

    // initialize display capture
    if (dispcapbank != 0xFF) initDispCap();

    menuMain(&file);
    exit(0);
}
