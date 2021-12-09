#include "../graphics/draw.h"
#include "../sys/core.h"

void draw_box(unsigned short color, unsigned short x, unsigned short y, unsigned short dx, unsigned short dy) {
	for(int i = 0; i < dy; i++) {
		for(int j = 0; j < dx; j++) {
			draw_pix(x+j, y+i, color);
		}
	}
}
