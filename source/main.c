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
// note: data can optionally be embedded into a png, if so, it should be done in a chunk with the name: "ndSF"
// Header:
u8 compressiontype; // currently unused, should always be set to 0 (uncompressed) (this byte should not be compressed)

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

// =======================================================================================================
// not stored in file, but globals used by the below functions.

s8 dispcapbank = -127;
u16 sscount = 0;

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

    u8 ndSF[4] = {'n', 'd', 'S', 'F'};
    while (true)
    {
        if (fread(buffer, 1, 8, file) < 8) return false; // could not file valid fd data in png
        if (memcmp(&buffer[4], ndSF, sizeof(ndSF)) == 0) return true; // valid file; fd data chunk found

        // convert size value's endianness, also add 4 to skip past checksums
        int offset = (buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3]) + 4;
        fseek(file, offset, SEEK_CUR);
    }
}

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
    else dispcapbank = 0;
    // B
    if ((vramcontrol[1] & 0x83) == 0x83)
    {
        fread(vrambuffer, 2, 128*1024/2, file);
        memcpy(VRAM_B, vrambuffer, 128*1024);
    }
    else dispcapbank = 1;
    // C
    if ((vramcontrol[2] & 0x87) == 0x83)
    {
        fread(vrambuffer, 2, 128*1024/2, file);
        memcpy(VRAM_C, vrambuffer, 128*1024);
    }
    else dispcapbank = 2;
    // D
    if ((vramcontrol[3] & 0x87) == 0x83)
    {
        fread(vrambuffer, 2, 128*1024/2, file);
        memcpy(VRAM_D, vrambuffer, 128*1024);
    }
    else dispcapbank = 3;
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
        vu8* address = ((vu8*)(0x04000240 + i));
        if (dispcapbank == i) continue; // skip re-initing bank if going to be used for display capture
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

u32 runGX(bool finish)
{
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
    }
    return 0;
}

u32 runDump(bool finish)
{
    initFrameState(false);

    swiWaitForVBlank(); // wait for buffer swap to actually take place

    videoSetMode(MODE_0_3D);
    return runGX(finish);
}

bool loadFile(FILE* file)
{
    if (!handlePNG(file)) // probably shouldn't be done if the file isn't expected to be a png but im too lazy to rework this to handle that properly
        return false;

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

void initDispCap()
{
    REG_DISPCAPCNT = DCAP_BANK(dispcapbank) | DCAP_SIZE(DCAP_SIZE_256x192) | DCAP_SRC_A(DCAP_SRC_A_3DONLY);
}

void transOverlay(u32 swapbuffer, bool pass2)
{
    u8 col = pass2 ? 23 : 15;

    // reset matrixes to make this easier on me
    for (int i = 0; i < 4; i++)
    {
        MATRIX_CONTROL = i;
        MATRIX_IDENTITY = 0;
    }

    GFX_POLY_FORMAT = 3 << 6 | 1 << 16;
    GFX_COLOR = col << 10 | col << 5 | col;
    GFX_TEX_FORMAT = 0;

    GFX_BEGIN = 1;
    GFX_VERTEX16 = -32767 << 16 | -32767;
    GFX_VERTEX16 = 0; 
    GFX_VERTEX16 = -32767 << 16 | 32767;
    GFX_VERTEX16 = 0; 
    GFX_VERTEX16 = 32767 << 16 | 32767;
    GFX_VERTEX16 = 0; 
    GFX_VERTEX16 = 32767 << 16 | -32767;
    GFX_VERTEX16 = 0;
    GFX_FLUSH = swapbuffer;
}

void checkDiff(u8* fin, u8 ref, u8 comp, bool pass)
{
    if (!pass)
    {
        if (ref <= (15 >> 1))
            *fin = (ref != comp);
        else if ((ref >= (32 >> 1)) && (ref <= (47 >> 1)))
            *fin = (ref == comp);
    }
    else
    {
        if ((ref >= (16 >> 1)) && (ref <= (31 >> 1)))
            *fin = (ref != comp);
        else if (ref >= (48 >> 1))
            *fin = (ref == comp);
    }
}

void initbuff(u8* ref, u8* buff)
{
    // size is always 256*192 so we dont need to pass along a var for size
    for (int i = 0; i < 192*256; i++)
        if (ref[i] <= 31) buff[i] = false;
        else buff[i] = true;
}

void doScreenshot(bool color18, bool bitmap)
{
    if (sscount > 999)
    {
        return;
    }
    REG_DISPCAPCNT |= DCAP_ENABLE;

    u16* bank;
    FILE* pic;
    if (dispcapbank == 0) bank = VRAM_A;
    else if (dispcapbank == 1) bank = VRAM_B;
    else if (dispcapbank == 2) bank = VRAM_C;
    else if (dispcapbank == 3) bank = VRAM_D;
    else return;

    u8 name[11];
    u8 ext[5];
    if (bitmap) strcpy(ext, ".bmp");
    else strcpy(ext, ".bin");

    chdir(fatGetDefaultCwd());

    snprintf(name, 11, "cap%i%s", sscount, ext);
    pic = fopen(name, "rb");
    sscount++;
    for (; pic != NULL; sscount++)
    {
        fclose(pic);
        snprintf(name, 11, "cap%i%s", sscount, ext);
        pic = fopen(name, "rb");
        if (sscount > 999)
        {
            return;
        }
    }
    pic = fopen(name, "wb");

    if (bitmap)
    {
        if(!color18)
        {
            u8 header[] = {0x42, 0x4D,   0x42, 0x80, 0x01, 0,   0, 0,   0, 0,   0x42, 0, 0, 0,
            0x28, 0, 0, 0,   0x00, 0x01, 0, 0,   0xC0, 0, 0, 0,   1, 0,   0x10, 0,   3, 0, 0, 0,   0, 0xC0, 0, 0,   0, 1, 0, 0,   0, 1, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
            0x1F, 0, 0, 0,   0xE0, 0x03, 0, 0,   0, 0x7C, 0, 0};
            fwrite(header, 1, sizeof(header), pic);
        }
        else
        {
            u8 header[] = {0x42, 0x4D,   0x42, 0x00, 0x03, 0,   0, 0,   0, 0,   0x42, 0, 0, 0,
            0x28, 0, 0, 0,   0x00, 0x01, 0, 0,   0xC0, 0, 0, 0,   1, 0,   0x20, 0,   3, 0, 0, 0,   0, 0xC0, 0, 0,   0, 1, 0, 0,   0, 1, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
            0x3F, 0, 0, 0,   0xC0, 0x0F, 0, 0,   0, 0xF0, 0x03, 0}; // probably doesn't work?
            fwrite(header, 1, sizeof(header), pic);
        }
    }

    if (!color18) // 15 bit
    {
        while (REG_DISPCAPCNT & DCAP_ENABLE);

        for (int y = 191; y >= 0; y--)
            for (int x = 0; x < 256; x++)
                fwrite(&bank[x + (256*y)], 1, 2, pic);
    }
    else
    {
        while (REG_DISPCAPCNT & DCAP_ENABLE);

        u8 refr[192*256];
        u8 refg[192*256];
        u8 refb[192*256];
        for (int y = 0; y < 192; y++)
            for (int x = 0; x < 256; x++)
                if (!bitmap) fwrite(&bank[x + (256*y)], 1, 2, pic);
                else
                {
                    u32 offset = x+(256*y);
                    u16 c = bank[offset];
                    refr[offset] = (c >> 10) & 0x1F;
                    refg[offset] = (c >> 5) & 0x1F;
                    refb[offset] = c & 0x1F;
                }

        u32 swapbuffer = runDump(false);
        transOverlay(swapbuffer, false);
        REG_DISPCAPCNT |= DCAP_ENABLE;
        while (REG_DISPCAPCNT & DCAP_ENABLE);
        bool b6r[192*256];
        bool b6g[192*256];
        bool b6b[192*256];

        //initbuff(refr, b6r);
        //initbuff(refg, b6g);
        //initbuff(refb, b6b);

        for (int y = 0; y < 192; y++)
            for (int x = 0; x < 256; x++)
                if (!bitmap) fwrite(&bank[x + (256*y)], 1, 2, pic);
                else
                {
                    u32 offset = x+(256*y);
                    u16 c = bank[offset];
                    u8 r = (c >> 10) & 0x1F;
                    u8 g = (c >> 5) & 0x1F;
                    u8 b = c & 0x1F;

                    checkDiff(&b6r[offset], refr[offset], r, false);
                    checkDiff(&b6g[offset], refg[offset], g, false);
                    checkDiff(&b6b[offset], refb[offset], b, false);
                }

        swapbuffer = runDump(false);
        transOverlay(swapbuffer, true);
        REG_DISPCAPCNT |= DCAP_ENABLE;
        while (REG_DISPCAPCNT & DCAP_ENABLE);

        for (int y = 0; y < 192; y++)
            for (int x = 0; x < 256; x++)
                if (!bitmap) fwrite(&bank[x + (256*y)], 1, 2, pic);
                else
                {
                    u32 offset = x+(256*y);
                    u16 c = bank[offset];
                    u8 r = (c >> 10) & 0x1F;
                    u8 g = (c >> 5) & 0x1F;
                    u8 b = c & 0x1F;

                    checkDiff(&b6r[offset], refr[offset], r, true);
                    checkDiff(&b6g[offset], refg[offset], g, true);
                    checkDiff(&b6b[offset], refb[offset], b, true);
                }

        if (bitmap)
            for (int y = 191; y >= 0; y--)
                for (int x = 0; x < 256; x++)
                {
                    u32 offset = x+(256*y);
                    u8 r = (refr[offset] << 1) | b6r[offset];
                    u8 g = (refg[offset] << 1) | b6g[offset];
                    u8 b = (refb[offset] << 1) | b6b[offset];
                    u32 fin = r << 12 | g << 6 | b;
                    fwrite(&fin, 1, sizeof(fin), pic);
                }
    }

    fclose(pic);
    runDump(true);
}

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
        while(true) swiWaitForVBlank();
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

    u16 selection = menuInputs(0, (struct InputIDs) {3,0,0}, 1, 1, 1, i, ptr_array);
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
    for (int i = 0; counter < 23+1 && i < 256; i++)
    {
        char png[] = ".png";
        char ndsfd[] = ".ndsfd";

        struct dirent* dat = readdir(dir);
        if (dat == NULL) break;

        char* loc = strrchr(dat->d_name, '.');
        
        if (strcmp(loc, png) == 0 || strcmp(loc, ndsfd) == 0)
        {
            ptr_array[counter] = malloc(33);
            filename_ptrs[counter-1] = malloc(255);
            strcpy(filename_ptrs[counter-1], dat->d_name);
            strncpy(ptr_array[counter], dat->d_name, 31);
            strncat(ptr_array[counter++], "\n", 1);
        }
    }

    closedir(dir);

    if (counter == 1)
    {
        menuWriteSingle(str_err_file);
        u16 keys = 0;
        while(keys & (KEY_A | KEY_B | KEY_START))
        {
            scanKeys();
            keys = keysHeld();
            swiWaitForVBlank();
        }
        return NULL;
    }

    ptr_array[counter++] = str_hint_bback;
    ptr_array[counter++] = str_hint_asel;

    s16 selection = menuInputs(2, (struct InputIDs) {1,0,0}, 1, 1, 1, counter, ptr_array) - 2;

    if (selection == -1)
    {
        for (int i = 0; i < counter - 3;)
        {
            free(filename_ptrs[i++]);
            free(ptr_array[i]);
        }
        return NULL;
    }

    FILE* file = fopen(filename_ptrs[selection], "rb");

    if (file == NULL)
    {
        menuWriteSingle("OOPS\n");
        while (true) swiWaitForVBlank();
    }

    for (int i = 0; i < counter - 3;)
    {
        free(filename_ptrs[i++]);
        free(ptr_array[i]);
    }

    return file;
}


void menuScreenshot()
{
    u8* ptr_array[] =
    {
        str_menu_ss,
        str_opt_ss_norm_bmp,
        str_opt_ss_norm_raw,
        str_opt_ss_full_bmp,
        str_opt_ss_full_raw,
        str_hint_bback,
        str_hint_asel,
    };
    u8 selection = menuInputs(2, (struct InputIDs) {1,0,0}, 1, 1, 1, (sizeof(ptr_array) / sizeof(ptr_array[0])), ptr_array);
    switch(selection)
    {
        case 1:
            return;

        case 2:
            doScreenshot(false, true);
            break;

        case 3:
            doScreenshot(false, false);
            break;

        case 4:
            doScreenshot(true, true);
            break;

        case 5:
            doScreenshot(true, false);
            break;
    }
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
        str_hint_asel,
    };

    while (true)
    {
        u8 selection = menuInputs(2, (struct InputIDs) {0,0,1}, 1, 1, 1, (sizeof(ptr_array) / sizeof(ptr_array[0])), ptr_array);
        switch(selection)
        {
            case 1: // R button - screenshot
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
                runDump(true);
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
            if (!menuDirSelect()) return 0;
            file = menuFileSelect();
        }
        menuClear();
        if (loadFile(file)) break;
        fclose(file);
    }

    if (dispcapbank != -127) initDispCap();

    menuMain(&file);
}
