// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008, 2019, 2024 Antonio Niño Díaz
//
// Render to Texture example
//
// http://www.skylyrac.net/
// https://github.com/AntonioND/
//
// Email: antonio_nd at outlook dot com

#include <stdio.h>

#include <nds.h>

// texture.h is created automagicaly from the texture.png and texture.grit files
// the graphics folder.
#include "texture.h"

// When the following define is enabled, the demo uses MODE_5_3D. When it isn't,
// it uses MODE_FB2.
#define USE_3D_MODE

// It uses VRAM banks as follows:
//
// - VRAM_A: Used for regular textures
// - VRAM_B: Used for the rendered texture
// - VRAM_C: Used to capture the final 3D scene
//
// The BGs of the main screen are used as follows:
//
// - If USE_3D_MODE is defined, it alternates between:
//
//   - BG 0: Rendered texture output, hidden under BG 2.
//   - BG 2: Renders the contents of VRAM_C captured during the previous frame.
//
//   and:
//
//   - BG 0: Displays the currently drawn 3D scene, which is captured to VRAM_C.
//
// - If USE_3D_MODE is not defined:
//
//   - MODE_FB2 renders the contents of VRAM_C as a bitmap on the screen. This
//     makes it easier to setup, but it's less flexible as you can't use other
//     layers on top of it.

// Vertices for the cube
v16 CubeVectors[] = {
    floattov16(-0.5), floattov16(-0.5), floattov16(0.5),
    floattov16(0.5),  floattov16(-0.5), floattov16(0.5),
    floattov16(0.5),  floattov16(-0.5), floattov16(-0.5),
    floattov16(-0.5), floattov16(-0.5), floattov16(-0.5),
    floattov16(-0.5), floattov16(0.5),  floattov16(0.5),
    floattov16(0.5),  floattov16(0.5),  floattov16(0.5),
    floattov16(0.5),  floattov16(0.5),  floattov16(-0.5),
    floattov16(-0.5), floattov16(0.5),  floattov16(-0.5)
};

// Polygons
u8 CubeFaces[] = {
    3, 2, 1, 0,
    0, 1, 5, 4,
    1, 2, 6, 5,
    2, 3, 7, 6,
    3, 0, 4, 7,
    5, 6, 7, 4
};

// Texture coordinates
u32 uv[] = {
    TEXTURE_PACK(inttot16(128), 0),
    TEXTURE_PACK(inttot16(128), inttot16(128)),
    TEXTURE_PACK(0,             inttot16(128)),
    TEXTURE_PACK(0,             0)
};

// Normals vectors
u32 normals[] = {
    NORMAL_PACK(0,                floattov10(-.97), 0),
    NORMAL_PACK(0,                0,                floattov10(.97)),
    NORMAL_PACK(floattov10(.97),  0,                0),
    NORMAL_PACK(0,                0,                floattov10(-.97)),
    NORMAL_PACK(floattov10(-.97), 0,                0),
    NORMAL_PACK(0,                floattov10(.97),  0)
};

// Draw a cube face at the specified color
void drawQuad(int poly)
{
    u32 f1 = CubeFaces[poly * 4];
    u32 f2 = CubeFaces[poly * 4 + 1];
    u32 f3 = CubeFaces[poly * 4 + 2];
    u32 f4 = CubeFaces[poly * 4 + 3];

    glNormal(normals[poly]);

    GFX_TEX_COORD = uv[0];
    glVertex3v16(CubeVectors[f1 * 3], CubeVectors[f1 * 3 + 1],
             CubeVectors[f1 * 3 + 2]);

    GFX_TEX_COORD = uv[1];
    glVertex3v16(CubeVectors[f2 * 3], CubeVectors[f2 * 3 + 1],
             CubeVectors[f2 * 3 + 2]);

    GFX_TEX_COORD = uv[2];
    glVertex3v16(CubeVectors[f3 * 3], CubeVectors[f3 * 3 + 1],
             CubeVectors[f3 * 3 + 2]);

    GFX_TEX_COORD = uv[3];
    glVertex3v16(CubeVectors[f4 * 3], CubeVectors[f4 * 3 + 1],
             CubeVectors[f4 * 3 + 2]);
}

void DrawCube(void)
{
    glBegin(GL_QUAD);

    for (int i = 0; i < 6; i++)
        drawQuad(i);
}

int main(void)
{
    int textureID;
    float rotateX = 0.0;
    float rotateY = 0.0;

    irqEnable(IRQ_VBLANK);

    powerOn(POWER_ALL);

#ifdef USE_3D_MODE
    videoSetMode(MODE_5_3D | DISPLAY_BG2_ACTIVE | DISPLAY_SPR_ACTIVE |
             DISPLAY_SPR_2D_BMP_256);
    REG_BG0CNT |= BG_PRIORITY_1;
    REG_BG2CNT = BG_BMP16_256x256 | BG_PRIORITY_0;
    REG_BG2PA = 1 << 8;
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2PD = 1 << 8;
    REG_BG2X = 0;
    REG_BG2Y = 0;
#else
    videoSetMode(MODE_FB2);
    vramSetBankC(VRAM_C_LCD);
#endif

    videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
    vramSetBankH(VRAM_H_SUB_BG);
    REG_BG0CNT_SUB = BG_MAP_BASE(15);
    consoleInit(NULL, 1, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false,
            true);
    BG_PALETTE_SUB[255] = 0xFFFF;
    printf("     RTT Demo by AntonioND\n");
    printf("\n");
    printf("    http://www.skylyrac.net\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("A/B: Scale small cube\n");
    printf("Pad: Rotate small cube");
    printf("\x1b[23;0HThanks to DiscoStew. ;)");

    glInit();

    glEnable(GL_TEXTURE_2D | GL_ANTIALIAS);

    // Setup the rear plane. It must have a unique polygon ID for AA to work
    glClearPolyID(63);
    glClearDepth(0x7FFF);

    vramSetBankA(VRAM_A_TEXTURE);

    glGenTextures(1, &textureID);
    glBindTexture(0, textureID);
    if (glTexImage2D(0, 0, GL_RGBA, 128, 128, 0, TEXGEN_TEXCOORD, textureBitmap) == 0)
    {
        printf("Failed to load texture\n");
        while (1)
            swiWaitForVBlank();
    }

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    glMaterialf(GL_AMBIENT,  RGB15(0, 0, 0));
    glMaterialf(GL_DIFFUSE,  RGB15(31, 31, 31));
    glMaterialf(GL_SPECULAR, RGB15(0, 0, 0));
    glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0);

    glLight(0, RGB15(31, 31, 31),
        floattov10(-0.6), floattov10(-0.6), floattov10(-0.6));

    int frame = 0;
    float rx = 30, ry = 0;
    float scale = 1;

    while (1)
    {
        // The video syncronization has to be done before any work is done in a
        // frame. This ensures that emulators and hardware have time to
        // synchronize during the first frame. If not, screens may be switched.
        swiWaitForVBlank();

        scanKeys();
        u16 keys = keysHeld();

        if ((keys & KEY_UP))
            rotateX += 3;
        if ((keys & KEY_DOWN))
            rotateX -= 3;
        if ((keys & KEY_LEFT))
            rotateY += 3;
        if ((keys & KEY_RIGHT))
            rotateY -= 3;

        if (keys & KEY_A)
            scale += 0.01;
        if (keys & KEY_B)
            scale -= 0.01;

        rx += 1;
        ry += 1;

        if (frame & 1) {
            vramSetBankB(VRAM_B_LCD);
#ifdef USE_3D_MODE
            vramSetBankC(VRAM_C_MAIN_BG_0x06000000);
#endif
            REG_DISPCAPCNT =
                // Destination is VRAM_B
                DCAP_BANK(DCAP_BANK_VRAM_B) |
                // Size = 256x192
                DCAP_SIZE(DCAP_SIZE_256x192) |
                // Capture source A only
                DCAP_MODE(DCAP_MODE_A) |
                // Source A = 3D rendered image
                DCAP_SRC_A(DCAP_SRC_A_3DONLY) |
                // Enable capture
                DCAP_ENABLE;

            glViewport(0, 0, 255, 192);

            // Real scene
            // ----------

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(70, 256.0 / 192.0, 0.1, 40);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            // Blue background
            glClearColor(0, 0, 31, 31);

            gluLookAt(0.0, 0.0, 1.0,    // Camera position
                  0.0, 0.0, 0.0,    // Look at
                  0.0, 1.0, 0.0);    // Up

            // Move cube away from the camera
            glTranslate3f32(0, 0, floattof32(-1));

            glRotateX(rx);
            glRotateY(ry);

            // When calling glBindTexture(), the GFX_TEX_FORMAT
            // command is only sent when the new texture isn't
            // currently active. By calling it with an invalid
            // texture we make sure that any future call to
            // glBindTexture() will actually send that command and
            // replace the manual command below.
            glBindTexture(0, -1);

            // The captured texture is 256x192, stored in VRAM_B,
            // and is in RGBA format. It is needed to use 256x256 as
            // size, as only power of two sizes are supported.
            GFX_TEX_FORMAT = (GL_RGBA << 26)
                           | (TEXTURE_SIZE_256 << 20)
                           | (TEXTURE_SIZE_256 << 23)
                           | ((((uintptr_t)VRAM_B) >> 3) & 0xFFFF);

            DrawCube();

            glFlush(0);
        }
        else
        {
            vramSetBankB(VRAM_B_TEXTURE);
#ifdef USE_3D_MODE
            vramSetBankC(VRAM_C_LCD);
#endif
            REG_DISPCAPCNT =
                // Destination is VRAM_C
                DCAP_BANK(DCAP_BANK_VRAM_C) |
                // Size = 256x192
                DCAP_SIZE(DCAP_SIZE_256x192) |
                // Capture source A only
                DCAP_MODE(DCAP_MODE_A) |
                // Source A = 3D rendered image
                DCAP_SRC_A(DCAP_SRC_A_3DONLY) |
                // Enable capture
                DCAP_ENABLE;

            glViewport(0, 64, 128, 192);

            // Texture scene
            // -------------

            glMatrixMode(GL_PROJECTION);

            glLoadIdentity();
            gluPerspective(70, 128 / 128, 0.1, 40);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            // Green background
            glClearColor(0, 31, 0, 31);

            gluLookAt(0.0, 0.0, 1.0,  // Camera position
                      0.0, 0.0, 0.0,  // Look at
                      0.0, 1.0, 0.0); // Up

            // Move cube away from the camera
            glTranslate3f32(0, 0, floattof32(-1));

            glRotateX(rotateX);
            glRotateY(rotateY);

            glScalef(scale, scale, scale);

            glBindTexture(0, textureID);

            DrawCube();

            glFlush(0);
        }

        frame ^= 1;
    }

    return 0;
}
