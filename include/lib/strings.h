#ifndef LIB_STRINGS_H
#define LIB_STRINGS_H

#define string_t char*

unsigned long strlen(string_t s);
unsigned char strcmp(string_t a, string_t b);
unsigned char strcmpn(string_t a, string_t b, unsigned int n);
char* u32_to_str(unsigned long value);
char* s32_to_str(unsigned long value);
char* zhex32_to_str(unsigned long value);
char* hex32_to_str(unsigned long value);

#endif
