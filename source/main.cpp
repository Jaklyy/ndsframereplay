#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <filesystem.h>

#include "main.h"
#include "font.h"




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

void initFrameState(bool trustemu)
{
    // init frame state
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

int main()
{
    // setup file
    //todo add a thingy for selecting which file to open from a menu.
    videoSetModeSub(MODE_0_2D);

    //consoleDebugInit(DebugDevice_NOCASH);
    fatInitDefault();
    FILE* file = fopen("dump0.fd", "rb");

    if (file == nullptr) 
    {
        nitroFSInit(NULL);
        file = fopen("nitro:/dump0.fd", "rb");
        if (file == nullptr)
        {
            consoleDemoInit();
            printf("Unable to load frame dump.\n\nPlease turn off the console.");
            while (true)
            {
                swiWaitForVBlank();
            }
        }
    }

    initVars(file);

    // enable 3d gpu
    powerOn(POWER_3D_CORE | POWER_MATRIX);

    initVram(file);

    initFrameState(false);
    
    fread(&numcmds, 1, sizeof(numcmds), file);

    swiWaitForVBlank(); // wait for swap buffer to actually take place

    //fprintf(stderr, "%li\n", ftell(file));
    for (u32 i = 0, j = 0; i < numcmds; i++)
    {
        fread(&cmd[i], 1, sizeof(cmd[0]), file);

        for (int k = 0; k < paramcount[cmd[i]]; k++)
        {
            fread(&param[j], 1, sizeof(param[0]), file);
            j++;
        }
    }

    videoSetMode(MODE_0_3D);
    runGX();

    // loop until any input is recieved
    // todo: add screencap support AND a menu to toggle/edit stuffs
    scanKeys();
    while (!keysHeld())
    {
        scanKeys();
        swiWaitForVBlank();
    }
}
