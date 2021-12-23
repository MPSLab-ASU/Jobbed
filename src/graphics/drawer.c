#include "../graphics/lfb.h"
#include "../graphics/drawer.h"

static struct Drawer g_Drawer = {x: 0, y: 0};

void write_cstring(struct Drawer* d, char* s, unsigned int c) {
	d->x %= GG_MAX_X;
	d->y %= GG_MAX_Y;
	unsigned int idx = 0;
	while(s[idx] != 0) {
		if (s[idx] == 0x0A) {
			d->y += 1;
			d->x = 0;
			idx++;
		} else {
			draw_cletter(d->x++, d->y, s[idx++], c);
			if (d->x > GG_MAX_X) {
				d->y += 1;
				d->x = 0;
			}
		}
		// CHECK Y EVENTUALLY
	}
}

void write_string(struct Drawer* d, char* s) {
	write_cstring(d, s, 0xFFFFFF);
}

void set_drawer(struct Drawer* d, unsigned int x, unsigned int y) {
	d->x = x % GG_MAX_X;
	d->y = y % GG_MAX_Y;
}

void write_chex32(struct Drawer* d, unsigned long val, unsigned int c) {
	draw_chex32(d->x, d->y, val, c);
	d->x += 8;
	if (d->x > GG_MAX_X) {
		d->y += 1;
		d->x %= GG_MAX_X;
	}
}

void write_hex32(struct Drawer* d, unsigned long val) {
	write_chex32(d, val, 0xFFFFFF);
}
