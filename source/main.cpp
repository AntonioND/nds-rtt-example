/**************************************************************************************
***************************************************************************************
*****                                                                             *****
*****                       Render To Texture example                             *****
*****                       -------------------------                             *****
*****                                                                             *****
*****                                                                             *****
*****                                                                             *****
*****                   By Antonio Niño Díaz (AntonioND)                          *****
*****                                                                             *****
*****                                                                             *****
***************************************************************************************
**************************************************************************************/




#include <nds.h>
#include <stdio.h>

//texture_bin.h is created automagicaly from the texture.bin placed in arm9/resources
//texture.bin is a raw 128x128 16 bit image.  I will release a tool for texture conversion 
//later
#include "texture_bin.h"


#define USE_3D_MODE //If defined, demo uses MODE_5_3D, else MODE_FB2


//verticies for the cube
v16 CubeVectors[] = {
 		floattov16(-0.5), floattov16(-0.5), floattov16(0.5), 
		floattov16(0.5),  floattov16(-0.5), floattov16(0.5),
		floattov16(0.5),  floattov16(-0.5), floattov16(-0.5),
		floattov16(-0.5), floattov16(-0.5), floattov16(-0.5),
		floattov16(-0.5), floattov16(0.5),  floattov16(0.5), 
		floattov16(0.5),  floattov16(0.5),	floattov16(0.5),
		floattov16(0.5),  floattov16(0.5),  floattov16(-0.5),
		floattov16(-0.5), floattov16(0.5),  floattov16(-0.5)
};

//polys
u8 CubeFaces[] = {
	3,2,1,0,
	0,1,5,4,
	1,2,6,5,
	2,3,7,6,
	3,0,4,7,
	5,6,7,4
};

//texture coordinates
u32 uv[] =
{
	
	TEXTURE_PACK(inttot16(128), 0),
	TEXTURE_PACK(inttot16(128),inttot16(128)),
	TEXTURE_PACK(0, inttot16(128)),
	TEXTURE_PACK(0,0)
};

u32 normals[] =
{
	NORMAL_PACK(0,floattov10(-.97),0),
	NORMAL_PACK(0,0,floattov10(.97)),
	NORMAL_PACK(floattov10(.97),0,0),
	NORMAL_PACK(0,0,floattov10(-.97)),
	NORMAL_PACK(floattov10(-.97),0,0),
	NORMAL_PACK(0,floattov10(.97),0)
	
};

//draw a cube face at the specified color
 void drawQuad(int poly)
{	
	
	u32 f1 = CubeFaces[poly * 4] ;
	u32 f2 = CubeFaces[poly * 4 + 1] ;
	u32 f3 = CubeFaces[poly * 4 + 2] ;
	u32 f4 = CubeFaces[poly * 4 + 3] ;


	glNormal(normals[poly]);

	GFX_TEX_COORD = (uv[0]);
	glVertex3v16(CubeVectors[f1*3], CubeVectors[f1*3 + 1], CubeVectors[f1*3 +  2] );
	
	GFX_TEX_COORD = (uv[1]);
	glVertex3v16(CubeVectors[f2*3], CubeVectors[f2*3 + 1], CubeVectors[f2*3 + 2] );
	
	GFX_TEX_COORD = (uv[2]);
	glVertex3v16(CubeVectors[f3*3], CubeVectors[f3*3 + 1], CubeVectors[f3*3 + 2] );

	GFX_TEX_COORD = (uv[3]);
	glVertex3v16(CubeVectors[f4*3], CubeVectors[f4*3 + 1], CubeVectors[f4*3 + 2] );
}



void DrawCube(void)
{
int i;
//draw the obj
glBegin(GL_QUAD);
	for(i = 0; i < 6; i++)
		drawQuad(i);
}

int main()
{	
	
	int textureID;
	float rotateX = 0.0;
	float rotateY = 0.0;
	
	
	//irqs are nice
	irqEnable(IRQ_VBLANK);
	
	powerOn(POWER_ALL);
	
#ifdef USE_3D_MODE
	videoSetMode(MODE_5_3D | DISPLAY_BG2_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_2D_BMP_256);
	REG_BG0CNT |= BG_PRIORITY_1;
	REG_BG2CNT = BG_BMP16_256x256 | BG_PRIORITY_0;
	REG_BG2PA = 256;
	REG_BG2PB = 0;
	REG_BG2PC = 0;
	REG_BG2PD = 256;
	REG_BG2X = 0;
	REG_BG2Y = 0;
#else
	videoSetMode(MODE_FB2);
	vramSetBankC(VRAM_C_LCD);
#endif
	
	
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	vramSetBankH(VRAM_H_SUB_BG);
	REG_BG0CNT_SUB = BG_MAP_BASE(15);
	consoleInit(0,1, BgType_Text4bpp, BgSize_T_256x256, 15,0, false);
	BG_PALETTE_SUB[255] = 0xFFFF;
	iprintf("     RTT Demo by AntonioND");
	iprintf("\n\n  antoniond.drunkencoders.com");
	iprintf("\n\n\n\nA/B: Scale small cube.");
	iprintf("\nPad: Rotate small cube.");
	
	iprintf("\x1b[23;0HThanks to DiscoStew. ;)");


	
	glInit();

	glEnable(GL_TEXTURE_2D | GL_ANTIALIAS);
	
	// setup the rear plane
	glClearPolyID(63); // BG must have a unique polygon ID for AA to work
	glClearDepth(0x7FFF);
	
	vramSetBankA(VRAM_A_TEXTURE);
	
	glGenTextures(1, &textureID);
	glBindTexture(0, textureID);
	glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_128 , TEXTURE_SIZE_128, 0, TEXGEN_TEXCOORD, (u8*)texture_bin);
	
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	
	glMaterialf(GL_AMBIENT, RGB15(0,0,0));
	glMaterialf(GL_DIFFUSE, RGB15(31,31,31));
	glMaterialf(GL_SPECULAR, RGB15(0,0,0));
	glMaterialf(GL_EMISSION, RGB15(0,0,0));

	//not a real gl function and will likely change
	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0);
	
	glLight(0, RGB15(31,31,31) , floattov10(-0.6),	  floattov10(-0.6),   floattov10(-0.6));
	
	int frame = 0;
	float rx = 30, ry = 0;
	float scale = 1;
	
	while(1) {
		swiWaitForVBlank(); //This is here because of no$gba
		
		scanKeys();
		u16 keys = keysHeld();
				
		if((keys & KEY_UP)) rotateX += 3;
		if((keys & KEY_DOWN)) rotateX -= 3;
		if((keys & KEY_LEFT)) rotateY += 3;
		if((keys & KEY_RIGHT)) rotateY -= 3;	
	
		if(keys & KEY_A) scale += 0.01;
		if(keys & KEY_B) scale -= 0.01;

		rx += 1;
		ry += 1;

		if(frame & 1)            //--------'real' scene----------- 
			{ 
			vramSetBankB(VRAM_B_LCD);  
#ifdef USE_3D_MODE
			vramSetBankC(VRAM_C_MAIN_BG_0x06000000); 
#endif
			//Texture goes to VRAM_B 
			REG_DISPCAPCNT=DCAP_BANK(1)|DCAP_ENABLE|DCAP_SIZE(3)|DCAP_SRC(1); 
       
			glViewport(0,0,255,192); 
          
			glMatrixMode(GL_PROJECTION); 
			glLoadIdentity(); 
			gluPerspective(70, 256.0 / 192.0, 0.1, 40); 
          
			glMatrixMode(GL_MODELVIEW); 
			glLoadIdentity(); 
                
		//---------------------------------------------------------------------       
			
			glClearColor(0,0,31,31); 
			
			gluLookAt(   0.0, 0.0, 1.0,      //camera possition 
				0.0, 0.0, 0.0,      //look at 
				0.0, 1.0, 0.0);      //up 
			  
			//move it away from the camera 
			glTranslate3f32(0, 0, floattof32(-1)); 
			
			glRotateX(rx); 
			glRotateY(ry); 
			
			GFX_TEX_FORMAT = (GL_RGBA<<26) | ((TEXTURE_SIZE_128)<<20) | 
				((TEXTURE_SIZE_128)<<23) | (((int)VRAM_B) >> 3);       
		   
			DrawCube(); 
			
			glFlush(0); 
			} 
		else                    //---------texture scene--------- 
			{ 
			vramSetBankB(VRAM_B_TEXTURE); 
#ifdef USE_3D_MODE
			vramSetBankC(VRAM_C_LCD);  
#endif
			//Scene goes to VRAM_C 
			REG_DISPCAPCNT=DCAP_BANK(2)|DCAP_ENABLE|DCAP_SIZE(3)|DCAP_SRC(1); 	
			
			glViewport(0,64,128,192); 
			
			glMatrixMode(GL_PROJECTION); 
			
			glLoadIdentity(); 
			gluPerspective(70, 128/128, 0.1, 40); 
			
			glMatrixMode(GL_MODELVIEW); 
			glLoadIdentity(); 
			
		//--------------------------------------------------------------------- 
			
			glClearColor(0,31,0,31); 
			
			gluLookAt(   0.0, 0.0, 1.0,      //camera possition 
			   0.0, 0.0, 0.0,      //look at 
			   0.0, 1.0, 0.0);      //up 
			
			//move it away from the camera 
			glTranslate3f32(0, 0, floattof32(-1)); 
			
			glRotateX(rotateX); 
			glRotateY(rotateY); 
			
			glScalef(scale,scale,scale); 
			
			glBindTexture(0, textureID); 
			
			DrawCube(); 
			
			glFlush(0);
			}
		
		
		frame ^= 1;	
	}

	return 0;
}//end main 


