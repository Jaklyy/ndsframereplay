



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
#endif
