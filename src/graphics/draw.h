#ifndef GRAPHICS_DRAW_H
#define GRAPHICS_DRAW_H

enum {
	SWIDTH = 640,
	SHEIGHT = 480,
	BPP = 16
};

extern unsigned long vram_base;
extern void clear_screen();
extern void draw_pix(unsigned long, unsigned long, unsigned short);

void draw_box(unsigned short, unsigned short, unsigned short, unsigned short, unsigned short);

#endif
