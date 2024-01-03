#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <filesystem.h>



// file structure:

// misc bits
// note: the order of these bits is not always the same as the order of the variables in the file (might change this later)
// also note: some of these regs have more bits than used, they can(?) be written to but they dont do anything(?), so we'll just ignore them to save on file size
u8 bits1; // 0 disp3dcnt; 1 alpha test; 2-3 clear color low/hi; 4 clear dep; 5 clear offset; 6-7 fog color low/hi;
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
u16 lightvecvecmtx_mask[4];

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
u32 vtxcolor; // actually stores a diffuse/ambient cmd if the vtx colors were last set by one
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
u32 normal;
u32 normpolyattr;
u32 normtexmtx[16];
u32 normvecmtx[16];
u32 normdiffambi;
u32 normspecemis;
u32 normtexparams;
u32 normshininess[32];
u32 normlightvec[4];
u32 normlightvecvecmtx[4][16];
u32 normlightcolor[4];
u32 polygon;
u16 vtxx; // i think using only one mask for each vtx cmd is technically wrong actually-- but i dont feel like fixing it.
u16 vtxy;
u16 vtxz;
u32 texcoord;
u32 texparams;
u32 texpalette;
u32 diffambi;
u32 specemis;
u32 shininess[32];
u32 lightvec[4];
u32 lightvecvecmtx[4][16];
u32 lightcolor[4];
u32 swapbuffer; // has no mask because it has to occur for 3d rendering to take place
u8 vramcontrol[7]; // only 7 are actually saved/loaded/used banks h and i are ignored
// vram state:
// (up to) 608 KiB of vram state :DDDDD (i am not storing masks for these unless i have good reason to)
// note: vram state is ONLY saved/loaded for ACTIVE vram banks (ie. last bit of vramcnt must be set)

// other:
u32 numcmds;

// 30000 as a max is just a guess tbh
// i dont actually know how many commands the gpu can process in one frame at a maximum
// probably a *lot* more if you really try to
// this *should* cover most real games though
u8 cmd[30000];
u32 param[30000];


void initVars(FILE* file)
{
    // init most variables
    fread(&bits1, 1, sizeof(bits1), file);
    fread(&bits2, 1, sizeof(bits2), file);
    fread(&bits3, 1, sizeof(bits3), file);
    fread(&bits4, 1, sizeof(bits4), file);
    fread(&bits5, 1, sizeof(bits5), file);
    fread(&bits6, 1, sizeof(bits6), file);
    fread(&edgecolor_mask, 1, sizeof(edgecolor_mask), file);
    fread(fogtable_mask, 1, sizeof(fogtable_mask), file);
    fread(toontable_mask, 1, sizeof(toontable_mask), file);
    fread(&projstack_mask, 1, sizeof(projstack_mask), file);
    fread(posstack_mask, 1, sizeof(posstack_mask), file);
    fread(vecstack_mask, 1, sizeof(vecstack_mask), file);
    fread(&texstack_mask, 1, sizeof(texstack_mask), file);
    fread(&projmtx_mask, 1, sizeof(projmtx_mask), file);
    fread(&posmtx_mask, 1, sizeof(posmtx_mask), file);
    fread(&vecmtx_mask, 1, sizeof(vecmtx_mask), file);
    fread(&texmtx_mask, 1, sizeof(texmtx_mask), file);
    fread(&normtexmtx_mask, 1, sizeof(normtexmtx_mask), file);
    fread(&normvecmtx_mask, 1, sizeof(normvecmtx_mask), file);
    fread(&normshininess_mask, 1, sizeof(normshininess_mask), file);
    fread(normlightvecvecmtx_mask, 1, sizeof(normlightvecvecmtx_mask), file);
    fread(&shininess_mask, 1, sizeof(shininess_mask), file);
    fread(lightvecvecmtx_mask, 1, sizeof(lightvecvecmtx_mask), file);

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
    fread(&normal, 1, sizeof(normal), file);
    fread(&normpolyattr, 1, sizeof(normpolyattr), file);
    fread(normtexmtx, 1, sizeof(normtexmtx), file);
    fread(normvecmtx, 1, sizeof(normvecmtx), file);
    fread(&normdiffambi, 1, sizeof(normdiffambi), file);
    fread(&normspecemis, 1, sizeof(normspecemis), file);
    fread(&normtexparams, 1, sizeof(normtexparams), file);
    fread(normshininess, 1, sizeof(normshininess), file);
    fread(normlightvec, 1, sizeof(normlightvec), file);
    fread(normlightvecvecmtx, 1, sizeof(normlightvecvecmtx), file);
    fread(normlightcolor, 1, sizeof(normlightcolor), file);
    fread(&polygon, 1, sizeof(polygon), file);
    fread(&vtxx, 1, sizeof(vtxx), file);
    fread(&vtxy, 1, sizeof(vtxy), file);
    fread(&vtxz, 1, sizeof(vtxz), file);
    fread(&texcoord, 1, sizeof(texcoord), file);
    fread(&texparams, 1, sizeof(texparams), file);
    fread(&texpalette, 1, sizeof(texpalette), file);
    fread(&diffambi, 1, sizeof(diffambi), file);
    fread(&specemis, 1, sizeof(specemis), file);
    fread(shininess, 1, sizeof(shininess), file);
    fread(lightvec, 1, sizeof(lightvec), file);
    fread(lightvecvecmtx, 1, sizeof(lightvecvecmtx), file);
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

void initFrameState(bool trustemudefaultstate)
{
    bool t = trustemudefaultstate; // i really did not wanna type that out tbh
    // init frame state
    // please ignore the cursed nonsense im trying to do here i swear it makes sense
    if (bits1 & (1<<0) || t) GFX_CONTROL = disp3dcnt;

    for (int i = 0; i < 16; i+=2)
    {
        if (edgecolor_mask & (1<<i) || t) ((vu8*)0x04000330)[i] = ((u8*)edgecolor)[i];
        if (edgecolor_mask & (1<<(i+1)) || t) ((vu8*)0x04000330)[i+1] = ((u8*)edgecolor)[i+1];
    }

    if (bits1 & (1<<1) || t) GFX_ALPHA_TEST = alphatest;
    if (bits1 & (1<<2) || t) ((vu16*)0x04000350)[0] = (u16)(clearcolor & 0xFFFF);
    if (bits1 & (1<<3) || t) ((vu16*)0x04000350)[1] = (u16)(clearcolor >> 16);
    if (bits1 & (1<<4) || t) GFX_CLEAR_DEPTH = cleardepoff & 0xFFFF;
    if (bits1 & (1<<5) || t) GFX_CLRIMAGE_OFFSET = cleardepoff >> 16;
    if (bits1 & (1<<6) || t) ((vu16*)0x04000358)[0] = fogcolor & 0xFFFF;
    if (bits1 & (1<<7) || t) ((vu16*)0x04000358)[1] = fogcolor >> 16;
    if (bits2 & (1<<0) || t) GFX_FOG_OFFSET = fogoffset;

    for (int i = 0; i < 32; i++)
        if (fogtable_mask[i/8] & (1 << (i % 8)) || t) GFX_FOG_TABLE[i] = fogtable[i];
    for (int i = 0; i < 64; i++)
        if (toontable_mask[i/8] & (1 << (i % 8)) || t) ((vu8*)0x04000380)[i] = ((u8*)toontable)[i];

    if (bits2 & (1<<1) || t) GFX_CUTOFF_DEPTH = zdotdisp;
    if (bits2 & (1<<7) || t) GFX_VIEWPORT = viewport;


    // init regs relevant to the normal command
    if (bits3 & (1<<1) || t)
    {
    // init polyattr for normal
    if (bits5 & (1<<4) || t)
    {
        GFX_POLY_FORMAT = normpolyattr;
        GFX_BEGIN = 0; // dummy begin cmd
    }

    //init lights for normal
    if ((u64)normlightvecvecmtx_mask || t) MATRIX_CONTROL = 2;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            if (!(normlightvecvecmtx_mask[i] & (1<<j)) || t) continue;
            MATRIX_LOAD4x4 = normlightvecvecmtx[i][j];
        }
        if ((bits4 >> i) & 0x1 || t) GFX_LIGHT_VECTOR = normlightvec[i];
        if ((bits4 >> (i+4)) & 0x1 || t) GFX_LIGHT_COLOR = normlightcolor[i];
    }
    // init textures for normal
    if (bits3 & (1<<4) || t) GFX_TEX_FORMAT = normtexparams;
    
    if (normtexmtx_mask || t) MATRIX_CONTROL = 3;
    for (int i = 0; i < 16; i++)
        if (normtexmtx_mask & (1<<i) || t) MATRIX_LOAD4x4 = normtexmtx[i];
    // init diff/ambi for normal
    if (bits3 & (1<<2) || t) GFX_DIFFUSE_AMBIENT = normdiffambi;
    if (bits3 & (1<<3) || t) GFX_SPECULAR_EMISSION = normspecemis;

    // init vector matrix for normal
    if (normvecmtx_mask || t)
    {
        MATRIX_CONTROL = 2;
        for (int i = 0; i < 16; i++)
            if (normvecmtx_mask & (1<<i) || t) MATRIX_LOAD4x4 = normvecmtx[i];
    }
    
    // init shininess table too for the normal
    for (int i = 0; i < 32; i++)
        if (normshininess_mask & (1<<i) || t) GFX_SHININESS = normshininess[i];

    // actually do the normal at long last.
    GFX_NORMAL = normal;
    }

    
    if (bits2 & (1<<6) || t)
    {
        u8 vtxcolortype = ((bits2 >> 4) & 0b11);
        if (vtxcolortype == 0)
            GFX_COLOR = vtxcolor;
        else if (vtxcolortype == 1)
            GFX_DIFFUSE_AMBIENT = vtxcolor;
    }

    if (bits5 & (1<<2) || t) GFX_DIFFUSE_AMBIENT = diffambi;
    if (bits5 & (1<<3) || t) GFX_SPECULAR_EMISSION = specemis;

    for (int i = 0; i < 32; i++)
        if (shininess_mask & (1<<i) || t) GFX_SHININESS = shininess[i];

    // init lights
    if ((u64)lightvecvecmtx_mask || t) MATRIX_CONTROL = 2;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 16; j++)
            if (lightvecvecmtx_mask[i] & (1<<j) || t) MATRIX_LOAD4x4 = lightvecvecmtx[i][j];

        if ((bits6) & (1<<i) || t) GFX_LIGHT_VECTOR = lightvec[i];
        if ((bits6) & (1<<(i+4)) || t) GFX_LIGHT_COLOR = lightcolor[i];
    }
    

    // init matrix stack
    
    if (projstack_mask || t) 
    {
        MATRIX_CONTROL = 0;
        for (int i = 0; i < 16; i++)
        {
            if (!(projstack_mask & (1<<i)) || t) break;

            MATRIX_LOAD4x4 = projstack[i];
        }
        MATRIX_STORE = 0; 
    }

    {
    bool runpos = false;
    bool runvec = false;
    for (int i = 0; i < 32; i++)
    {
        if (posstack_mask[i] || t) runpos = true;
        if (vecstack_mask[i] || t) runvec = true;
    }
    if (runpos || runvec)
    for (int j = 0; j < 32; j++)
    {
        bool posveccheck = false;

        if (runpos)
        {
            MATRIX_CONTROL = 1;
            for (int i = 0; i < 16; i++)
            {
                if (!(posstack_mask[j] & (1<<i)) || t) break;

                posveccheck = true;
                MATRIX_LOAD4x4 = posstack[j][i];
            }
        }
        if (runvec)
        {
            MATRIX_CONTROL = 2;
            for (int i = 0; i < 16; i++)
            {
                if (!(vecstack_mask[j] & (1<<i)) || t) break;

                posveccheck = true;
                MATRIX_LOAD4x4 = vecstack[j][i];
            }
        }
        if (posveccheck) MATRIX_STORE = j;
    }
    }

    if (texstack_mask || t)
    {
        MATRIX_CONTROL = 3;
        for (int i = 0; i < 16; i++)
        {
            if (!(texstack_mask & (1<<i)) || t) break;

            MATRIX_LOAD4x4 = texstack[i];
        }
        MATRIX_STORE = 0; 
    }

    // init matrices
    if (projmtx_mask || t)
    {
        MATRIX_CONTROL = 0;
        for (int i = 0; i < 16; i++)
        {
            if (!(projmtx_mask & (1<<i)) || t) break;

            MATRIX_LOAD4x4 = projmtx[i];
        }
    }
    if (posmtx_mask || t)
    {
        MATRIX_CONTROL = 1;
        for (int i = 0; i < 16; i++)
        {
            if (!(posmtx_mask & (1<<i)) || t) break;

            MATRIX_LOAD4x4 = posmtx[i];
        }
    }
    if (vecmtx_mask || t)
    {
        MATRIX_CONTROL = 2;
        for (int i = 0; i < 16; i++)
        {
            if (!(vecmtx_mask & (1<<i)) || t) break;

            MATRIX_LOAD4x4 = vecmtx[i];
        }
    }
    if (texmtx_mask || t)
    {
        MATRIX_CONTROL = 3;
        for (int i = 0; i < 16; i++)
        {
            if (!(texmtx_mask & (1<<i)) || t) break;

            MATRIX_LOAD4x4 = texmtx[i];
        }
    }
    
    if (bits3 & (1<<0) || t) MATRIX_CONTROL = matrixmode;
    
    if (bits2 & (1<<2) || t) GFX_POLY_FORMAT = polyattr;
    // init polygon engine (that's a term right?)
    if (bits3 & (1<<5) || t) GFX_BEGIN = polygon;

    // init texture state
    if (bits3 & (1<<7) || t) GFX_TEX_COORD = texcoord;
    if (bits5 & (1<<0) || t) GFX_TEX_FORMAT = texparams;
    if (bits5 & (1<<1) || t) GFX_PAL_FORMAT = texpalette;

    // init some vertices just in case (render an entire polygon because otherwise it actually hangs entirely--)
    if (bits3 & (1<<6) || t)
    {
        u8 vtxnum;
        if (!(bits3 & (1<<5) || t)) vtxnum = 3; // just pray it defaults to tri otherwise we hang the entire gpu lol
        else if (polygon == 0 || polygon == 2) vtxnum = 3;
        else vtxnum = 4;
        for (int i = 0; i < vtxnum; i++)
        {
            GFX_VERTEX16 = ((u32)vtxx | ((u32)vtxy << 16));
            GFX_VERTEX16 = vtxz;
        }
    }
    // unset polygon format (will be properly set when the next polygon begin cmd gets sent)
    if (bits2 & (1<<3) || t) GFX_POLY_FORMAT = polyattrunset;

    GFX_FLUSH = swapbuffer;
}

template<bool load>
void runGX(FILE* file)
{
    for (u32 i = 0; i < numcmds; i++)
    {   
        u32 addr = 0x04000000;
        if (load) fread(&cmd[i], 1, sizeof(cmd[i]), file);

        if (!(cmd[i] < 128 || cmd[i] == 132)) break; // if an invalid address is entered ABORT.

        vu32* finaladdr = (vu32*)(addr | ((cmd[i] << 2) + 0x400));

        if (load) fread(&param[i], 1, sizeof(param[i]), file); 
        *finaladdr = param[i];
    }
}

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
    //"address: 0x0400" "gx: 0x" "cmd"
    // run gx commands
    videoSetMode(MODE_0_3D);
    runGX<true>(file);

    // loop until any input is recieved
    // todo: add screencap support AND a menu to toggle/edit stuffs
    scanKeys();
    while (!keysHeld())
    {
        scanKeys();
        swiWaitForVBlank();
    }
}