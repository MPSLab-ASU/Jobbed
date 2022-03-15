#ifndef GRAPHICS_LFB_H
#define GRAPHICS_LFB_H

#define GG_MAX_X 128
#define GG_MAX_Y  100

#define DRAW64(x,y,v) draw_hex32(x,y,v>>32);draw_hex32(x+8,y,v);

void lfb_init(void);
void lfb_showpicture(void);

void clear_screen(void);

void draw_cpixel(unsigned int lx, unsigned int ly, unsigned int c);
void draw_cbox(unsigned int lx, unsigned int ly, unsigned int dx, unsigned int dy, unsigned int c);

void draw_cbyte(unsigned int lx, unsigned int ly, unsigned char letter, unsigned int c);
void draw_byte(unsigned int lx, unsigned int ly, unsigned char letter);

void draw_cletter(unsigned int lx, unsigned int ly, unsigned char letter, unsigned int c);
void draw_letter(unsigned int lx, unsigned int ly, unsigned char letter);

void draw_cstring(unsigned int lx, unsigned int ly, char* s, unsigned int c);
void draw_string(unsigned int lx, unsigned int ly, char* s);

void draw_chex32(unsigned int lx, unsigned int ly, unsigned long val, unsigned int c);
void draw_hex32(unsigned int lx, unsigned int ly, unsigned long val);

unsigned long draw_cu10(unsigned int lx, unsigned int ly, unsigned long val, unsigned int c);
unsigned long draw_u10(unsigned int lx, unsigned int ly, unsigned long val);

#endif
