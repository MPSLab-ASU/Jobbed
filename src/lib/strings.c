#include "../lib/strings.h"

unsigned long strlen(char* s) {
	unsigned long len = 0;
	while (s[len] != 0) {
		len += 1;
	}
	return len;
}

unsigned char strcmp(char* a, char* b) {
	unsigned long idx = 0;
	while (a[idx] != 0 && b[idx] != 0) {
		if (a[idx] != b[idx]) {
			return 0;
		}
		idx += 1;
	}
	return a[idx] == b[idx];
}
