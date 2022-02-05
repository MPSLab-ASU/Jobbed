#include <lib/kmem.h>
#include <lib/strings.h>

unsigned long strlen(string_t s)
{
	unsigned long len = 0;
	while (s[len] != 0) {
		len += 1;
	}
	return len;
}

unsigned char strcmp(string_t a, string_t b)
{
	unsigned long idx = 0;
	while (a[idx] != 0 && b[idx] != 0) {
		if (a[idx] != b[idx]) {
			return 0;
		}
		idx += 1;
	}
	return a[idx] == b[idx];
}

unsigned char strcmpn(string_t a, string_t b, unsigned int n)
{
	unsigned long idx = 0;
	while (a[idx] != 0 && b[idx] != 0 && idx+1 < n) {
		if (a[idx] != b[idx]) {
			return 0;
		}
		idx += 1;
	}
	return a[idx] == b[idx];
}

char* zhex32_to_str(unsigned long value)
{
	char* data = kcalloc(9);
	char tmp = 0;
	char isz = -1;
	for (int i = 0; i < 8; i++) {
		tmp = (value >> 4*(8-i-1))&0xF;
		if (isz == 0xFF && tmp != 0)
			isz = i;
		if(tmp > 0x9)
			tmp += 7;
		tmp += 0x30;
		data[i] = tmp;
	}
	return data+isz;
}

char* hex32_to_str(unsigned long value)
{
	char* data = kcalloc(9);
	char tmp = 0;
	for (int i = 0; i < 8; i++) {
		tmp = (value >> 4*(8-i-1))&0xF;
		if(tmp > 0x9)
			tmp += 7;
		tmp += 0x30;
		data[i] = tmp;
	}
	return data;
}

char* u32_to_str(unsigned long value)
{
	unsigned long t = value;
	unsigned long c;
	char* data = kcalloc(11);
	char* dptr = data + 9;
	for (int i = 0; i <= 10; i++) {
		c = t%10;
		*dptr = 0x30 + (c&0xF);
		t /= 10;
		if (t==0)
			break;
		dptr -= 1;
	}
	return dptr;
}

char* s32_to_str(unsigned long value)
{
	long t = value;
	unsigned long c;
	char is_neg = 0;
	if (t < 0) {
		t = -t;
		is_neg = 1;
	}
	char* data = kcalloc(12);
	char* dptr = data + 10;
	for (int i = 0; i <= 10; i++) {
		c = t%10;
		*dptr = 0x30 + (c&0xF);
		t /= 10;
		if (t==0)
			break;
		dptr -= 1;
	}
	if (is_neg) {
		dptr -= 1;
		*dptr = '-';
	}
	return dptr;
}
