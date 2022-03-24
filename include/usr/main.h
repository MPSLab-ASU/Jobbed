#ifndef USR_MAIN_H
#define USR_MAIN_H

#ifdef RPI_BUILD
	#define USR_TIME 260
#else
	#define USR_TIME 2000
#endif

void main(void);
void handle_data(unsigned char);

#endif
