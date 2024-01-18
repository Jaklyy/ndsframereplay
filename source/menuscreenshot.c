#include "menus.h"




u8 dispcapbank = 0xFF;
u16 sscount = 0;

void initDispCap()
{
    REG_DISPCAPCNT = DCAP_BANK(dispcapbank) | DCAP_SIZE(DCAP_SIZE_256x192) | DCAP_SRC_A(DCAP_SRC_A_3DONLY);
}

void checkDiff(bool* fin, u8 ref, u8 comp, bool pass)
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
    GFX_VERTEX16 = (u16)-32767 << 16 | (u16)-32767;
    GFX_VERTEX16 = 0; 
    GFX_VERTEX16 = (u16)-32767 << 16 | 32767;
    GFX_VERTEX16 = 0; 
    GFX_VERTEX16 = 32767 << 16 | 32767;
    GFX_VERTEX16 = 0; 
    GFX_VERTEX16 = 32767 << 16 | (u16)-32767;
    GFX_VERTEX16 = 0;
    GFX_FLUSH = swapbuffer;
}

void doScreenshot(bool color18, bool bitmap)
{
    if (sscount > 999) // abort if too many screen shots
    {
        menuWriteSingle(str_err_toomanyss);
        waitForInput();
        return;
    }

    u16* bank;
    FILE* pic;
    if (dispcapbank == 0xFF) // abort if no free bank is available
    {
        menuWriteSingle(str_err_nofreebank);
        waitForInput();
        return;
    }
    // convert from bank number to bank address
    bank = (u16*)(0x6800000 + (0x20000 * dispcapbank));
    REG_DISPCAPCNT |= DCAP_ENABLE; // enable display capture

    u8 name[11];
    u8 ext[5];
    if (bitmap) memcpy(ext, ".bmp", 5);
    else memcpy(ext, ".bin", 5);

    chdir(fatGetDefaultCwd());

    // find next unused screencap name
    // todo: use the filename of the framedump as a base?
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
            menuWriteSingle(str_err_toomanyss);
            waitForInput();
            return;
        }
    }
    pic = fopen(name, "wb");

    // create the header for a bitmap
    if (bitmap)
    {
        if(!color18)
        {   
            //             BM            file size: 98370       00      00      image offset: 0x42
            u8 header[] = {0x42, 0x4D,   0x42, 0x80, 0x01, 0,   0, 0,   0, 0,   0x42, 0, 0, 0,
            // header size: 40 bytes (BITMAPINFOHEADER), x res: 256, y res: 192, color planes: 1, 16 bpp, BI_BITFIELDS, raw image size: 98304, printing stuff: 256 & 256, num colors: 0, important colors: 0
            0x28, 0, 0, 0,   0x00, 0x01, 0, 0,   0xC0, 0, 0, 0,   1, 0,   0x10, 0,   3, 0, 0, 0,   0, 0x80, 0x01, 0,   0, 1, 0, 0,   0, 1, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
            // 5r           5g                   5b
            0x1F, 0, 0, 0,   0xE0, 0x03, 0, 0,   0, 0x7C, 0, 0};
            fwrite(header, 1, sizeof(header), pic);
        }
        else
        {
            //             BM            file size: 196674      00      00      image offset: 0x42
            u8 header[] = {0x42, 0x4D,   0x42, 0x00, 0x03, 0,   0, 0,   0, 0,   0x42, 0, 0, 0,
            // header size: 40 bytes (BITMAPINFOHEADER), x res: 256, y res: 192, color planes: 1, 32 bpp, BI_BITFIELDS, raw image size: 196608, printing stuff: 256 & 256, num colors: 0, important colors: 0
            0x28, 0, 0, 0,   0x00, 0x01, 0, 0,   0xC0, 0, 0, 0,   1, 0,   0x20, 0,   3, 0, 0, 0,   0, 0x00, 0x03, 0,   0, 1, 0, 0,   0, 1, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
            // 6r            6g                  6b
            0x3F, 0, 0, 0,   0xC0, 0x0F, 0, 0,   0, 0xF0, 0x03, 0};
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

void menuScreenshot()
{
    u8* ptr_array[] =
    {
        str_menu_ss,
        str_opt_ss_norm_bmp,
        str_opt_ss_full_bmp,
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
            doScreenshot(true, true);
            break;
    }
}
