
#include <pspiofilemgr.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <stdio.h>
#include <string.h>

PSP_MODULE_INFO("Screen Shot: BMP", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define CAPTURE_SAVE_DIR "ms0:/PSP/PHOTO/capture"

typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef long            LONG;

typedef struct tagBITMAPFILEHEADER {
   //    WORD    bfType;
        DWORD   bfSize;
        DWORD   bfReserved;
        DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
	DWORD	biSize;
	LONG	biWidth;
	LONG	biHeight;
	WORD	biPlanes;
	WORD	biBitCount;
	DWORD	biCompression;
	DWORD	biSizeImage;
	LONG	biXPelsPerMeter;
	LONG	biYPelsPerMeter;
	DWORD	biClrUsed;
	DWORD	biClrImportant;
} BITMAPINFOHEADER;



int nkThreadSuspend(SceUID thId);
int nkThreadResume(SceUID thId);

/***************************************

//Saving screen to 24bit BMP Image File

****************************************/
void screenshot_BMP(const char* filename, char imageSize){

	int x, y;
	int fd;
	int pwidth, pheight, bufferwidth, pixelformat, unk;
	unsigned int* vram32;
	unsigned short* vram16;


	//Get Display Info
	sceDisplayWaitVblankStart();
	sceDisplayGetMode(&unk, &pwidth, &pheight);
	sceDisplayGetFrameBuf((void*)&vram32, &bufferwidth, &pixelformat, &unk);
	vram16 = (unsigned short*) vram32;

	unsigned char padding;
	int pw = pwidth, ph = pheight;
	if (imageSize > 0) pw >>= 1, ph >>= 1;
	padding = (3 * (pw >> 1) % 4);

	int allocline = 0;
	int allocsize = 0;
	SceUID mem = -1;

	//Allocating Memory
	for (allocline = pheight; allocline > 0; allocline >>= 1)
	{
		allocsize = allocline * ((3 * pw + padding) * sizeof(unsigned char));
		mem = sceKernelAllocPartitionMemory(1, "block", 0, allocsize, NULL);
		if (mem >= 0) break;
	}

	if (allocline <= 0 || mem < 0) return;
	unsigned char *buf = (unsigned char*)sceKernelGetBlockHeadAddr(mem);

	fd = sceIoOpen(filename, PSP_O_CREAT|PSP_O_WRONLY|PSP_O_TRUNC, 0777);
	if (fd < 0) return;

	//Create a BMP Header
	BITMAPFILEHEADER h1;
	BITMAPINFOHEADER h2;
	unsigned char bm[2];

	bm[0] = 0x42;
	bm[1] = 0x4D;
	h1.bfSize	= (((24 * pw + padding) * ph) >> 3) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 2;
	h1.bfReserved	= 0;
	h1.bfOffBits	= 2 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	h2.biSize	= sizeof(BITMAPINFOHEADER);
	h2.biPlanes	= 1;
	h2.biBitCount	= 24;
	h2.biCompression 	= 0;
	h2.biWidth	= pw;
	h2.biHeight	= ph;
	h2.biSizeImage	= ((24 * pw + padding) * ph) >> 3;
	h2.biXPelsPerMeter	= 0xEC4;
	h2.biYPelsPerMeter	= 0xEC4;
	h2.biClrUsed		= 0;
	h2.biClrImportant	= 0;

	//Write header
	sceIoWrite(fd, bm, 2);
	sceIoWrite(fd, &h1, sizeof(BITMAPFILEHEADER));
	sceIoWrite(fd, &h2, sizeof(BITMAPINFOHEADER));


//Saving screen to 24bit BMP Image
	for(x = 0; x < padding; x++) buf[3 * pw + x] = 0;

	int i = 0, line = 1;

	for(y = (ph-1); y >= 0; y--, line++){
		for(x = 0; x < pw; x++){
			unsigned int color, offset = x + y*bufferwidth;
			unsigned char r = 0, g = 0, b = 0;
			if(imageSize) offset <<= 1;

			switch (pixelformat) {
				case 0:	// 16-bit RGB 5:6:5
					color = vram16[offset];
					vram16[offset] ^= 0xFFFF;	//Reverse the Color
					r = (color & 0x1f) << 3;
					g = ((color >> 5) & 0x3f) << 2 ;
					b = ((color >> 11) & 0x1f) << 3 ;
					break;

				case 1:// 16-bit RGBA 5:5:5:1
					color = vram16[offset];
					vram16[offset] ^= 0x7FFF;	//Reverse the Color
					r = (color & 0x1f) << 3;
					g = ((color >> 5) & 0x1f) << 3 ;
					b = ((color >> 10) & 0x1f) << 3 ;
					break;

				case 2:// 16-bit RGBA 4:4:4:4
					color = vram16[offset];
					vram16[offset] ^= 0x0FFF;	//Reverse the Color
					r = (color & 0xf) << 4;
					g = ((color >> 4) & 0xf) << 4 ;
					b = ((color >> 8) & 0xf) << 4 ;
					break;

				case 3:// 32-bit RGBA 8:8:8:8
					color = vram32[offset];
					vram32[offset] ^= 0x00FFFFFF;	//Reverse the Color
					r = color & 0xff;
					g = (color >> 8) & 0xff;
					b = (color >> 16) & 0xff;
					break;
			}
			buf[i++] = b;
			buf[i++] = g;
			buf[i++] = r;
		}
		if (line >= allocline)
		{
			sceIoWrite(fd, buf, allocsize);
			i = 0;
			line = 0;
		}
		else
		{
			i += padding;
		}
	}
	if (line > 0)
	{
		sceIoWrite(fd, buf, (3 * pw + padding) * sizeof(unsigned char) * line);
	}
//	sceIoWaitAsync(fd, NULL);
	sceKernelFreePartitionMemory(mem);
	sceIoClose(fd);

//êFÇå≥Ç…ñﬂÇ∑
	for(y = 0; y < ph; y++){
		int i;
		for(i = 0, x = 0; x < pw; x++){
			unsigned int offset = x + y*bufferwidth;
			if(imageSize) offset <<= 1;

			switch (pixelformat) {
				case 0:	// 16-bit RGB 5:6:5
					vram16[offset] ^= 0xFFFF;	//Reverse the Color
					break;

				case 1:// 16-bit RGBA 5:5:5:1
					vram16[offset] ^= 0x7FFF;	//Reverse the Color
					break;

				case 2:// 16-bit RGBA 4:4:4:4
					vram16[offset] ^= 0x0FFF;	//Reverse the Color
					break;

				case 3:// 32-bit RGBA 8:8:8:8
					vram32[offset] ^= 0x00FFFFFF;	//Reverse the Color
					break;
			}
		}
	}
}

/***************************************

Main Thread

****************************************/
int mainThread (SceSize args, void *argp){
	unsigned int paddata_old = 0;
	char file[64], flag = 0, size = 0;
	int fd, count = 0;
	SceCtrlData paddata;

	//Create Save Directory
	sceIoMkdir("ms0:/PSP/PHOTO",0777);
	sceIoMkdir(CAPTURE_SAVE_DIR,0777);
	
	while(1){
		sceCtrlPeekBufferPositive(&paddata, 1);
		
		if((paddata.Buttons != paddata_old) || (flag > 0)){
			if((flag > 0) && (paddata.Buttons & PSP_CTRL_NOTE)){
				flag = 0;
				size = 0;
			}
			else if((paddata.Buttons & PSP_CTRL_NOTE) || (flag > 0)){

				//If push Vol-, make 1/4 of size
				if(flag == 0){
					if(paddata.Buttons & 0x00200000) size = 1;
					else size = 0;
				}
				//If push NOTE+L, have a Rapid Snap
				if((paddata.Buttons & (PSP_CTRL_LTRIGGER | PSP_CTRL_NOTE)) == (PSP_CTRL_LTRIGGER | PSP_CTRL_NOTE)){
					flag = 1;//ON
				}

				//Suspend Thread = Pause Game
				nkThreadSuspend(sceKernelGetThreadId());

				while(1){
					sprintf(file, "%s/snap%03d.bmp", CAPTURE_SAVE_DIR, count);
					fd = sceIoOpen(file, PSP_O_RDONLY, 0644);
					if(fd < 0){
						sceIoClose(fd);
						break;
					}
					sceIoClose(fd);
					count++;
				}
				screenshot_BMP(file, size);

				//Resume thread = Resume Game
				nkThreadResume(sceKernelGetThreadId());

			}
		}
		paddata_old = paddata.Buttons;
		sceKernelDelayThread(10000);
	}
	return 0;
}

/***************************************

Module Start

****************************************/
int module_start (SceSize args, void *argp)
{
	SceUID thid = sceKernelCreateThread("screenshot", mainThread, 0x18, 0xE00, 0, NULL);
	if (thid >= 0) sceKernelStartThread(thid, args, argp);

	return 0;
}
