#ifndef GRAPHICS_DRAWER_H
#define GRAPHICS_DRAWER_H

#include <util/lock.h>

struct Drawer {
	unsigned int x;
	unsigned int y;
	struct Lock  l;
};

void write_cchar(struct Drawer* d, char s, unsigned int c);
void write_char(struct Drawer* d, char s);

void write_cstring(struct Drawer* d, char* s, unsigned int c);
void write_string(struct Drawer* d, char* s);

void write_chex32(struct Drawer* d, unsigned long val, unsigned int c);
void write_hex32(struct Drawer* d, unsigned long val);

void write_c10(struct Drawer* d, unsigned long val, unsigned int c);
void write_10(struct Drawer* d, unsigned long val);

void set_drawer(struct Drawer* d, unsigned int x, unsigned int y);

#endif
