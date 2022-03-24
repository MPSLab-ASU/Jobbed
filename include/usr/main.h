#ifndef USR_MAIN_H
#define USR_MAIN_H

#ifdef RPI_BUILD
	#define USR_TIME 40
#else
	#define USR_TIME 2000
#endif

void main(void);

#endif
