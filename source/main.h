



#ifndef MAIN_H
#define MAIN_H

#define CUR_FD_REV 0

enum Cmd_IDs
{
    //Nop = 0, // not stored
    Mode = 0x10,
    Push,
    Pop,
    Store,
    Restore,
    Identity,
    Load4x4,
    Load4x3,
    Mult4x4,
    Mult4x3,
    Mult3x3,
    Scale,
    Trans,
    Color = 0x20,
    Normal,
    Texcoord,
    Vtx16,
    Vtx10,
    VtxXY,
    VtxXZ,
    VTXYZ,
    VtxDiff,
    PolyAttr,
    TexParam,
    TexPalette,
    DiffAmbi = 0x30,
    SpecEmis,
    LightVec,
    LightColor,
    Shininess,
    BeginPoly = 0x40,
    EndPoly,
    SwapBuffer = 0x50,
    Viewport = 0x60,
    //BoxTest = 0x70 // not stored
    PosTest = 0x71,
    //VecTest // not stored
    ZDotDisp = 0x72, // stole the id from the vector test cmd so i could "borrow" it
};

void waitForInput();
u32 runDump(bool finish);

extern const u8 paramcount[256];

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
extern u16 disp3dcnt;
extern u16 edgecolor[8];
extern u8 alphatest;
extern u32 clearcolor;
extern u32 cleardepoff;
extern u32 fogcolor;
extern u16 fogoffset;
extern u8 fogtable[32];
extern u16 toontable[32];

// initial state variables:
// should be saved before the first command is sent
// should we only define the used bits of the command, or the full 32 bits?
extern u16 zdotdisp;
extern u32 polyattr;
extern u32 polyattrunset;
extern u32 vtxcolor;
extern u32 viewport;
extern u32 projstack[16];
extern u32 posstack[32][16];
extern u32 vecstack[32][16];
extern u32 texstack[16];
extern u32 projmtx[16];
extern u32 posmtx[16];
extern u32 vecmtx[16];
extern u32 texmtx[16];
extern u32 matrixmode;
extern u32 polygon;
extern u32 vtxxy;
extern u16 vtxz;
extern u32 texcoord;
extern u32 texparams;
extern u32 texpalette;
extern u32 diffambi;
extern u32 specemis;
extern u32 shininess[32];
extern u32 lightvec[4];
extern u32 lightcolor[4];
extern u32 swapbuffer;
extern u8 vramcontrol[7]; // only 7 are actually saved/loaded/used. banks h and i are ignored

// vram state:
// (up to) 608 KiB of vram state :DDDDD
// note: vram state is ONLY saved/loaded for ACTIVE vram banks that're being used for 3d textures/texture palettes
// (ie. last bit of vramcnt must be set and the value of the mst must be 3).
// (additionally banks H and I are ignored due to not being allocatable to the 3d engine).

// other:
extern u32 numcmds;
extern u32 numparams;

// only store a maximum of 500KB
// cmds should be stored in one consecutive block before all params are stored.
// nops and vec/box tests should not be added to any of these trackers
// id 255 is used to store zero dot disp reg writes
extern u8* cmd;
extern u32* param;

#endif
