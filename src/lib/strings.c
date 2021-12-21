#include "../lib/strings.h"

unsigned long strlen(string_t s) {
	unsigned long len = 0;
	while (s[len] != 0) {
		len += 1;
	}
	return len;
}

unsigned char strcmp(string_t a, string_t b) {
	unsigned long idx = 0;
	while (a[idx] != 0 && b[idx] != 0) {
		if (a[idx] != b[idx]) {
			return 0;
		}
		idx += 1;
	}
	return a[idx] == b[idx];
}

unsigned char strcmpn(string_t a, string_t b, unsigned int n) {
	unsigned long idx = 0;
	while (a[idx] != 0 && b[idx] != 0 && idx+1 < n) {
		if (a[idx] != b[idx]) {
			return 0;
		}
		idx += 1;
	}
	return a[idx] == b[idx];
}
