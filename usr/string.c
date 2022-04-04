char* ulong_to_string(unsigned long value, char* data)
{
	unsigned long t = value;
	unsigned long c;
	char* dptr = data + 10;
	for (int i = 0; i <= 10; i++) {
		c = t%10;
		*dptr = 0x30 + (c&0xF);
		t /= 10;
		if (t==0)
			break;
		dptr -= 1;
		// Add decimal point to distinguish orders more easily
		if (i == 2) {
			*dptr = '.';
			dptr -= 1;
		}
	}
	return dptr;
}
