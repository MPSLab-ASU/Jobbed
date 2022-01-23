#ifndef GRAPHICS_LFB_H
#define GRAPHICS_LFB_H

#define GG_MAX_X 128
#define GG_MAX_Y  46

void lfb_init(void);
void lfb_showpicture(void);

void clear_screen(void);

void draw_cpixel(unsigned long lx, unsigned long ly, unsigned int c);
void draw_cbox(unsigned long lx, unsigned long ly, unsigned char dx, unsigned char dy, unsigned int c);

void draw_cbyte(unsigned char lx, unsigned char ly, unsigned char letter, unsigned int c);
void draw_byte(unsigned char lx, unsigned char ly, unsigned char letter);

void draw_cletter(unsigned char lx, unsigned char ly, unsigned char letter, unsigned int c);
void draw_letter(unsigned char lx, unsigned char ly, unsigned char letter);

void draw_cstring(unsigned int lx, unsigned int ly, char* s, unsigned int c);
void draw_string(unsigned int lx, unsigned int ly, char* s);

void draw_chex32(unsigned int lx, unsigned int ly, unsigned long val, unsigned int c);
void draw_hex32(unsigned int lx, unsigned int ly, unsigned long val);

#endif
