#ifndef LIB_STRINGS_H
#define LIB_STRINGS_H

#define string_t char*

unsigned long strlen(string_t s);
unsigned char strcmp(string_t a, string_t b);
unsigned char strcmpn(string_t a, string_t b, unsigned int n);

#endif
