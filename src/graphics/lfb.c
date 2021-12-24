#include "../drivers/uart.h"
#include "../graphics/lfb.h"
#include "../graphics/mbox.h"
#include "../graphics/homer.h"
#include "../graphics/glyphs.h"

#define GRAPHICS_LFB_C
unsigned int width, height, pitch, isrgb;   /* dimensions and channel order */
unsigned char *lfb;                         /* raw frame buffer address */

/**
 * Set screen resolution to 1024x768
 */
void lfb_init()
{
	mbox[0] = 35*4;
	mbox[1] = MBOX_REQUEST;

	mbox[2] = 0x48003;  //set phy wh
	mbox[3] = 8;
	mbox[4] = 8;
	mbox[5] = 1024;         //FrameBufferInfo.width
	mbox[6] = 768;          //FrameBufferInfo.height

	mbox[7] = 0x48004;  //set virt wh
	mbox[8] = 8;
	mbox[9] = 8;
	mbox[10] = 1024;        //FrameBufferInfo.virtual_width
	mbox[11] = 768;         //FrameBufferInfo.virtual_height

	mbox[12] = 0x48009; //set virt offset
	mbox[13] = 8;
	mbox[14] = 8;
	mbox[15] = 0;           //FrameBufferInfo.x_offset
	mbox[16] = 0;           //FrameBufferInfo.y.offset

	mbox[17] = 0x48005; //set depth
	mbox[18] = 4;
	mbox[19] = 4;
	mbox[20] = 32;          //FrameBufferInfo.depth

	mbox[21] = 0x48006; //set pixel order
	mbox[22] = 4;
	mbox[23] = 4;
	mbox[24] = 1;           //RGB, not BGR preferably

	mbox[25] = 0x40001; //get framebuffer, gets alignment on request
	mbox[26] = 8;
	mbox[27] = 8;
	mbox[28] = 4096;        //FrameBufferInfo.pointer
	mbox[29] = 0;           //FrameBufferInfo.size

	mbox[30] = 0x40008; //get pitch
	mbox[31] = 4;
	mbox[32] = 4;
	mbox[33] = 0;           //FrameBufferInfo.pitch

	mbox[34] = MBOX_TAG_LAST;

	//this might not return exactly what we asked for, could be
	//the closest supported resolution instead
	if(mbox_call(MBOX_CH_PROP) && mbox[20]==32 && mbox[28]!=0) {
		mbox[28]&=0x3FFFFFFF;   //convert GPU address to ARM address
		width=mbox[5];          //get actual physical width
		height=mbox[6];         //get actual physical height
		pitch=mbox[33];         //get number of bytes per line
		isrgb=mbox[24];         //get the actual channel order
		lfb=(void*)((unsigned long)mbox[28]);
	} else {
		uart_string("Unable to set screen resolution to 1024x768x32\n");
	}
}

/**
 * Show a picture
 */
void lfb_showpicture()
{
	unsigned int x,y;
	unsigned char *ptr=lfb;
	char *data=homer_data, pixel[4];

	ptr = lfb + (height-homer_height)*pitch + (width-homer_width)*4;
	for(y=0;y<homer_height;y++) {
		for(x=0;x<homer_width;x++) {
			HEADER_PIXEL(data, pixel);
			// the image is in RGB. So if we have an RGB framebuffer, we can copy the pixels
			// directly, but for BGR we must swap R (pixel[0]) and B (pixel[2]) channels.
			*((unsigned int*)ptr)=isrgb ? *((unsigned int *)&pixel) : (unsigned int)(pixel[0]<<16 | pixel[1]<<8 | pixel[2]);
			ptr+=4;
		}
		ptr+=pitch-homer_width*4;
	}
}

void draw_cbyte(unsigned char lx, unsigned char ly, unsigned char letter, unsigned int c) {
	unsigned int x, y;
	unsigned char* ptr = lfb;
	ptr += (pitch*ly*GLYPH_Y+lx*4*GLYPH_X);
	unsigned char ltr = (letter & 0xF) + 0x30;
	if (ltr > 0x39) {
		ltr += 7;
	}
	for(y=0; y<GLYPH_Y; y++) {
		for(x=0; x<GLYPH_X; x++) {
			if((0x80 >> ((GLYPH_X-1)-x)) & glyphs[y+GLYPH_Y*(ltr)]) {
				*((unsigned int*)ptr) = isrgb ? (unsigned int)((c&0xFF)<<16 | (c&0xFF00) | (c&0xFF0000)>>16) : c;
			} else {
				*((unsigned int*)ptr) = 0x000000;
			}
			ptr += 4;
		}
		ptr += pitch - GLYPH_X*4;
	}
}

void draw_cletter(unsigned char lx, unsigned char ly, unsigned char letter, unsigned int c) {
	unsigned int x, y;
	unsigned char* ptr = lfb;
	ptr += (pitch*ly*GLYPH_Y+lx*4*GLYPH_X);
	unsigned char ltr = letter & 0x7F;
	for(y=0; y<GLYPH_Y; y++) {
		for(x=0; x<GLYPH_X; x++) {
			if((0b10000000 >> ((GLYPH_X-1)-x)) & glyphs[y+GLYPH_Y*(ltr)]) {
				*((unsigned int*)ptr) = isrgb ? (unsigned int)((c&0xFF)<<16 | (c&0xFF00) | (c&0xFF0000)>>16) : c;
			} else {
				*((unsigned int*)ptr) = 0x000000;
			}
			ptr += 4;
		}
		ptr += pitch - GLYPH_X*4;
	}
}

void draw_cstring(unsigned int lx, unsigned int ly, char* s, unsigned int c) {
	unsigned int x = lx % GG_MAX_X, y = ly % GG_MAX_Y;
	unsigned int idx = 0;
	while(s[idx] != 0) {
		draw_cletter(x++, y, s[idx++], c);
		if (x > GG_MAX_X) {
			y += 1;
			x = 0;
		}
		// CHECK Y EVENTUALLY
	}
}

void draw_chex32(unsigned int lx, unsigned int ly, unsigned long val, unsigned int c) {
	unsigned int x = lx % GG_MAX_X, y = ly % GG_MAX_Y;
	for(unsigned int i = 0; i < GLYPH_X; i++) {
		draw_cbyte(x++, y, 0xF & (val >> ((GLYPH_X-1)-i)*4), c);
		if (x > GG_MAX_X) {
			y += 1;
			x = 0;
		}
		// CHECK Y EVENTUALLY
	}
}

void draw_byte(unsigned char lx, unsigned char ly, unsigned char letter) {
	draw_cbyte(lx, ly, letter, 0xFFFFFF);
}
void draw_letter(unsigned char lx, unsigned char ly, unsigned char letter) {
	draw_cletter(lx, ly, letter, 0xFFFFFF);
}
void draw_string(unsigned int lx, unsigned int ly, char* s){
	draw_cstring(lx, ly, s, 0xFFFFFF);
}
void draw_hex32(unsigned int lx, unsigned int ly, unsigned long val) {
	draw_chex32(lx, ly, val, 0xFFFFFF);
}
