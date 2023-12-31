#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <filesystem.h>



// file structure:

// global variables:
u32 disp3dcnt_mask;
u32 disp3dcnt;
u32 edgecolor_mask[8];
u32 edgecolor[8];
u32 alphatest_mask;
u32 alphatest;
u32 clearcolor_mask;
u32 clearcolor;
u32 cleardepoff_mask;
u32 cleardepoff;
u32 fogcolor_mask;
u32 fogcolor;
u32 fogoffset_mask;
u32 fogoffset;
u32 fogtable_mask[32];
u32 fogtable[32];
u32 toontable_mask[32];
u32 toontable[32];

// initial state variables:
u32 zdotdisp_mask;
u32 zdotdisp;
u32 polyattr_mask;
u32 polyattr;
u32 polyattrunset_mask;
u32 polyattrunset;
u32 vtxcolor_mask;
u32 vtxcolor;
u8 vtxcolortype;
u32 viewport_mask;
u32 viewport;
u32 projstack_mask[16];
u32 projstack[16];
u32 posstack_mask[32][16];
u32 posstack[32][16];
u32 vecstack_mask[32][16];
u32 vecstack[32][16];
u32 texstack_mask[16];
u32 texstack[16];
u32 projmtx_mask[16];
u32 projmtx[16];
u32 posmtx_mask[16];
u32 posmtx[16];
u32 vecmtx_mask[16];
u32 vecmtx[16];
u32 texmtx_mask[16];
u32 texmtx[16];
u32 matrixmode_mask;
u32 matrixmode;
u32 normal_mask;
u32 normal;
u32 normtexmtx_mask[16];
u32 normtexmtx[16];
u32 normvecmtx_mask[16];
u32 normvecmtx[16];
u32 normdiffambi_mask;
u32 normdiffambi;
u32 normspecemis_mask;
u32 normspecemis;
u32 normtexparams_mask;
u32 normtexparams;
u32 normshininess_mask[32];
u32 normshininess[32];
u8 normlightcolor_mask;
u32 normlightcolor[4];
u8 normlightvec_mask;
u32 normlightvec[4];
u32 normlightvecvecmtx_mask[4][16];
u32 normlightvecvecmtx[4][16];
u32 polygon_mask;
u32 polygon;
u32 vtxx_mask;
u32 vtxx;
u32 vtxy_mask;
u32 vtxy;
u32 vtxz_mask;
u32 vtxz;
u32 texcoord_mask;
u32 texcoord;
u32 texparams_mask;
u32 texparams;
u32 texpalette_mask;
u32 texpalette;
u32 diffambi_mask;
u32 diffambi;
u32 specemis_mask;
u32 specemis;
u32 shininess_mask[32];
u32 shininess[32];
u8 lightvec_mask;
u32 lightvec[4];
u32 lightvecvecmtx_mask[4][16];
u32 lightvecvecmtx[4][16];
u8 lightcolor_mask;
u32 lightcolor[4];
u32 swapbuffer_mask;
u32 swapbuffer;
// vram state:
// 656 KB of vram state :DDDDD (i am not storing masks for these unless i have good reason to)

// other:
u32 numcmds;
// 16 bits for cmd address
// 32 bits for what the command actually was


int main()
{
    // setup file
    //todo add a thingy for selecting which file to open from a menu.
    
    //consoleDebugInit(DebugDevice_NOCASH);
    fatInitDefault();
    FILE* file = fopen("fat:/dump0.fd", "rb");

    if (file == nullptr) 
    {
        nitroFSInit(NULL);
        file = fopen("nitro:/dump0.fd", "rb");
        if (file == nullptr)
        {
            consoleDemoInit();
            printf("Unable to load frame dump.\n\nPress Power to exit.");
            while (true)
            {
                swiWaitForVBlank();
            }
        }
    }

    // init variables
    fread(&disp3dcnt_mask, 4, 1, file);
    fread(&disp3dcnt, 4, 1, file);

    fread(edgecolor_mask, 4, 8, file);
    fread(edgecolor, 4, 8, file);

    fread(&alphatest_mask, 4, 1, file);
    fread(&alphatest, 4, 1, file);

    fread(&clearcolor_mask, 4, 1, file);
    fread(&clearcolor, 4, 1, file);

    fread(&cleardepoff_mask, 4, 1, file);
    fread(&cleardepoff, 4, 1, file);

    fread(&fogcolor_mask, 4, 1, file);
    fread(&fogcolor, 4, 1, file);

    fread(&fogoffset_mask, 4, 1, file);
    fread(&fogoffset, 4, 1, file);

    fread(fogtable_mask, 4, 32, file);
    fread(fogtable, 4, 32, file);

    fread(toontable_mask, 4, 32, file);
    fread(toontable, 4, 32, file);


    fread(&zdotdisp_mask, 4, 1, file);
    fread(&zdotdisp, 4, 1, file);

    fread(&polyattr_mask, 4, 1, file);
    fread(&polyattr, 4, 1, file);

    fread(&polyattrunset_mask, 4, 1, file);
    fread(&polyattrunset, 4, 1, file);

    fread(&vtxcolor_mask, 4, 1, file);
    fread(&vtxcolor, 4, 1, file);

    fread(&vtxcolortype, 1, 1, file);

    fread(&viewport_mask, 4, 1, file);
    fread(&viewport, 4, 1, file);

    fread(projstack_mask, 4, 16, file);
    fread(projstack, 4, 16, file);

    fread(posstack_mask, 4, 512, file);
    fread(posstack, 4, 512, file);

    fread(vecstack_mask, 4, 512, file);
    fread(vecstack, 4, 512, file);

    fread(texstack_mask, 4, 16, file);
    fread(texstack, 4, 16, file);

    fread(projmtx_mask, 4, 16, file);
    fread(projmtx, 4, 16, file);

    fread(posmtx_mask, 4, 16, file);
    fread(posmtx, 4, 16, file);

    fread(vecmtx_mask, 4, 16, file);
    fread(vecmtx, 4, 16, file);

    fread(texmtx_mask, 4, 16, file);
    fread(texmtx, 4, 16, file);

    fread(&matrixmode_mask, 4, 1, file);
    fread(&matrixmode, 4, 1, file);

    fread(&normal_mask, 4, 1, file);
    fread(&normal, 4, 1, file);

    fread(normtexmtx_mask, 4, 16, file);
    fread(normtexmtx, 4, 16, file);

    fread(normvecmtx_mask, 4, 16, file);
    fread(normvecmtx, 4, 16, file);

    fread(&normdiffambi_mask, 4, 1, file);
    fread(&normdiffambi, 4, 1, file);

    fread(&normspecemis_mask, 4, 1, file);
    fread(&normspecemis, 4, 1, file);

    fread(&normtexparams_mask, 4, 1, file);
    fread(&normtexparams, 4, 1, file);
    
    fread(normshininess_mask, 4, 32, file);
    fread(normshininess, 4, 32, file);

    fread(&normlightcolor_mask, 1, 1, file);
    fread(normlightcolor, 4, 4, file);

    fread(&normlightvec_mask, 1, 1, file);
    fread(normlightvec, 4, 4, file);

    fread(normlightvecvecmtx_mask, 4, 4*16, file);
    fread(normlightvecvecmtx, 4, 4*16, file);

    fread(&polygon_mask, 4, 1, file);
    fread(&polygon, 4, 1, file);

    fread(&vtxx_mask, 4, 1, file);
    fread(&vtxx, 4, 1, file);

    fread(&vtxy_mask, 4, 1, file);
    fread(&vtxy, 4, 1, file);

    fread(&vtxz_mask, 4, 1, file);
    fread(&vtxz, 4, 1, file);

    fread(&texcoord_mask, 4, 1, file);
    fread(&texcoord, 4, 1, file);

    fread(&texparams_mask, 4, 1, file);
    fread(&texparams, 4, 1, file);

    fread(&texpalette_mask, 4, 1, file);
    fread(&texpalette, 4, 1, file);

    fread(&diffambi_mask, 4, 1, file);
    fread(&diffambi, 4, 1, file);

    fread(&specemis_mask, 4, 1, file);
    fread(&specemis, 4, 1, file);

    fread(shininess_mask, 4, 32, file);
    fread(shininess, 4, 32, file);

    fread(&lightvec_mask, 1, 1, file);
    fread(lightvec, 4, 4, file);

    fread(lightvecvecmtx_mask, 4, 4*16, file);
    fread(lightvecvecmtx, 4, 4*16, file);

    fread(&lightcolor_mask, 1, 1, file);
    fread(lightcolor, 4, 4, file);

    fread(&swapbuffer_mask, 4, 1, file);
    fread(&swapbuffer, 4, 1, file);

    videoSetMode(MODE_0_3D);
    powerOn(POWER_3D_CORE | POWER_MATRIX);

    // turn vram on and set to lcdc mode for easy writing
    u8 vramcontrol[10];
    for (int i = 0; i < 10; i++)
    {
        if (i == 7) i++;
        fread(&vramcontrol[i], 1, 1, file);
        vu8* address = ((vu8*)0x04000240 + i);
        *address = 0b10000000;
    }

    // init vram banks
    // A
    for (int i = 0; i < 128 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6800000)[i/4] = vrambuffer;
    }
    // B
    for (int i = 0; i < 128 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6820000)[i/4] = vrambuffer;
    }
    // C
    for (int i = 0; i < 128 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6840000)[i/4] = vrambuffer;
    }
    // D
    for (int i = 0; i < 128 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6860000)[i/4] = vrambuffer;
    }
    // E
    for (int i = 0; i < 64 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6880000)[i/4] = vrambuffer;
    }
    // F
    for (int i = 0; i < 16 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6890000)[i/4] = vrambuffer;
    }
    // G
    for (int i = 0; i < 16 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6894000)[i/4] = vrambuffer;
    }
    // skip wram because idk if it's useful for this.
    // H
    for (int i = 0; i < 32 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x6898000)[i/4] = vrambuffer;
    }
    // I
    for (int i = 0; i < 16 * 1024; i += 4)
    {
        u32 vrambuffer;
        fread(&vrambuffer, 4, 1, file);
        ((u32*)0x68A0000)[i/4] = vrambuffer;
    }
    // actually init vramcnt
    for (int i = 0; i < 10; i++)
    {
        if (i == 7) i++;
        vu8* address = ((vu8*)0x04000240 + i);
        *address = vramcontrol[i];
    }

    // init frame state
    GFX_CONTROL = disp3dcnt & disp3dcnt_mask;

    for (int i = 0; i < 8; i++)
        GFX_EDGE_TABLE[i] = edgecolor[i] & edgecolor_mask[i];

    GFX_ALPHA_TEST = alphatest & alphatest_mask;
    GFX_CLEAR_COLOR = clearcolor & clearcolor_mask;
    (*(vu32*)0x04000354) = cleardepoff & cleardepoff_mask;
    GFX_FOG_COLOR = fogcolor & fogcolor_mask;
    GFX_FOG_OFFSET = fogoffset & fogoffset_mask;

    for (int i = 0; i < 32; i++)
        GFX_FOG_TABLE[i] = fogtable[i] & fogtable_mask[i];
    for (int i = 0; i < 32; i++)
        GFX_TOON_TABLE[i] = toontable[i] & toontable_mask[i];

    GFX_CUTOFF_DEPTH = zdotdisp & zdotdisp_mask;
    GFX_POLY_FORMAT = polyattr & polyattr_mask;
    GFX_VIEWPORT = viewport & viewport_mask;


    // init regs relevant to the normal command

    //init lights for normal
    MATRIX_CONTROL = 2;
    for (int i = 0; i < 4; i++)
    {
        if ((normlightcolor_mask >> i) & 0x1) GFX_LIGHT_COLOR = normlightcolor[i];
        for (int j = 0; j < 16; j++)
            MATRIX_LOAD4x4 = normlightvecvecmtx[i][j] & normlightvecvecmtx_mask[i][j];

        if ((normlightvec_mask >> i) & 0x1) GFX_LIGHT_VECTOR = normlightvec[i];
    }
    // init textures for normal
    GFX_TEX_FORMAT = normtexparams & normtexparams_mask;
    MATRIX_CONTROL = 3;
    for (int i = 0; i < 16; i++)
        MATRIX_LOAD4x4 = normtexmtx[i] & normtexmtx_mask[i];
    // init diff/ambi for normal
    GFX_DIFFUSE_AMBIENT = normdiffambi & normdiffambi_mask;
    GFX_SPECULAR_EMISSION = normspecemis & normspecemis_mask;

    // init vector matrix for normal
    MATRIX_CONTROL = 2;
    for (int i = 0; i < 16; i++)
        MATRIX_LOAD4x4 = normvecmtx[i] & normvecmtx_mask[i];

    // init shininess table too for the normal
    for (int i = 0; i < 32; i++)
        GFX_SHININESS = normshininess[i] & normshininess_mask[i];

    // actually do the normal at long last.
    GFX_NORMAL = normal & normal_mask;


    if (vtxcolortype == 0)
        GFX_COLOR = vtxcolor & vtxcolor_mask;
    else if (vtxcolortype == 1)
        GFX_DIFFUSE_AMBIENT = vtxcolor & vtxcolor_mask;

    GFX_DIFFUSE_AMBIENT = diffambi & diffambi_mask;
    GFX_SPECULAR_EMISSION = specemis & specemis_mask;

    for (int i = 0; i < 32; i++)
        GFX_SHININESS = shininess[i] & shininess_mask[i];

    // init lights
    MATRIX_CONTROL = 2;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 16; j++)
            MATRIX_LOAD4x4 = lightvecvecmtx[i][j] & lightvecvecmtx_mask[i][j];

        if ((lightvec_mask) & (0x1<<i)) GFX_LIGHT_VECTOR = lightvec[i];
        if ((lightcolor_mask) & (0x1<<i)) GFX_LIGHT_COLOR = lightcolor[i];
    }
    

    // init matrix stack
    MATRIX_CONTROL = 0;
    bool projcheck = false;
    for (int i = 0; i < 16; i++)
    {
        if (projstack_mask[i] == 0) break;
        else projcheck = 1;
        MATRIX_LOAD4x4 = projstack[i] & projstack_mask[i];
    }
    if (projcheck) MATRIX_STORE = 0; 

    for (int j = 0; j < 32; j++)
    {
        bool posveccheck = false;
        MATRIX_CONTROL = 1;
        for (int i = 0; i < 16; i++)
        {
            if (posstack_mask[j][i] == 0) break;
            else posveccheck = true;
            MATRIX_LOAD4x4 = posstack[j][i] & posstack_mask[j][i];
        }
        MATRIX_CONTROL = 2;
        for (int i = 0; i < 16; i++)
        {
            if (vecstack_mask[j][i] == 0) break;
            else posveccheck = true;
            MATRIX_LOAD4x4 = vecstack[j][i] & vecstack_mask[j][i];
        }
        if (posveccheck) MATRIX_STORE = j;
    }

    MATRIX_CONTROL = 3;
    bool texcheck = false;
    for (int i = 0; i < 16; i++)
    {
        if (texstack_mask[i] == 0) break;
        else texcheck = 1;
        MATRIX_LOAD4x4 = texstack[i] & texstack_mask[i];
    }
    if (texcheck) MATRIX_STORE = 0; 

    // init matrices
    MATRIX_CONTROL = 0;
    for (int i = 0; i < 16; i++)
    {
        if (projmtx_mask[i] == 0) break;
        MATRIX_LOAD4x4 = projmtx[i] & projmtx_mask[i];
    }
    MATRIX_CONTROL = 1;
    for (int i = 0; i < 16; i++)
    {
        if (posmtx_mask[i] == 0) break;
        MATRIX_LOAD4x4 = posmtx[i] & posmtx_mask[i];
    }
    MATRIX_CONTROL = 2;
    for (int i = 0; i < 16; i++)
    {
        if (vecmtx_mask[i] == 0) break;
        MATRIX_LOAD4x4 = vecmtx[i] & vecmtx_mask[i];
    }
    MATRIX_CONTROL = 3;
    for (int i = 0; i < 16; i++)
    {
        if (texmtx_mask[i] == 0) break;
        MATRIX_LOAD4x4 = texmtx[i] & texmtx_mask[i];
    }
    
    MATRIX_CONTROL = matrixmode & matrixmode_mask;
    
    // init polygon engine (that's a term right?)
    GFX_BEGIN = polygon & polygon_mask;

    u8 vtxnum;
    if (polygon == 0 || polygon == 2) vtxnum = 3;
    else vtxnum = 4;

    // init texture state
    GFX_TEX_FORMAT = texparams & texparams_mask;
    GFX_PAL_FORMAT = texpalette & texpalette_mask;

    // init some vertices just in case (render an entire polygon because otherwise it actually hangs entirely--)
    for (int i = 0; i < vtxnum; i++)
    {
        GFX_TEX_COORD = texcoord & texcoord_mask;
        GFX_VERTEX16 = ((u32)(vtxx & vtxx_mask) | ((u32)(vtxy & vtxy_mask) << 16));
        GFX_VERTEX16 = vtxz & vtxz_mask;
    }
    
    // unset polygon format (will be properly set when the next polygon begin cmd gets sent)
    GFX_POLY_FORMAT = polyattrunset & polyattrunset_mask;

    GFX_FLUSH = swapbuffer & swapbuffer_mask;
    fread(&numcmds, 4, 1, file);

    swiWaitForVBlank();

    // re-run gx commands
    for (u32 i = 0; i < numcmds; i++)
    {   
        u32 addr = 0x04000000;
        u16 addr2;
        fread(&addr2, 2, 1, file);
        if (((addr2 >= 0x400 && addr2 < 0x600) || addr2 == 0x610));
        else break; // if an invalid address is entered ABORT.
        if (addr2 >= 0x5C0 && addr2 <= 0x5C8) continue; // skip doing test cmds
        vu32* finaladdr = (vu32*)(addr | addr2);

        u32 param;
        fread(&param, 4, 1, file); 
        *finaladdr = param;
    }

    // loop until any input is recieved
    // todo: add screencap support AND a menu to toggle/edit stuffs
    scanKeys();
    while (!keysHeld())
    {
        scanKeys();
        swiWaitForVBlank();
    }
}