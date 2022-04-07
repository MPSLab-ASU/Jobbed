enum {
	GLYPH_X = 8,
	GLYPH_Y = 16,
	GLYPH_MAX = 0x80,
};
unsigned char font[GLYPH_MAX][GLYPH_Y] = {
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x14, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x48, 0x48, 0x68, 0xfe, 0x24, 0x24, 0x7f, 0x14, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x10, 0x7c, 0x92, 0x12, 0x1c, 0x70, 0x90, 0x92, 0x7c, 0x10, 0x10, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x06, 0x09, 0x09, 0x46, 0x38, 0x66, 0x90, 0x90, 0x60, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x38, 0x04, 0x04, 0x0c, 0x92, 0xb2, 0xa2, 0x46, 0xbc, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x30, 0x10, 0x10, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x10, 0x10, 0x20, 0x00, 0x00, 0x00},
	{0x00, 0x0c, 0x08, 0x08, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x08, 0x08, 0x0c, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x10, 0x92, 0x7c, 0x38, 0xd6, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x7f, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x08, 0x04, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x40, 0x20, 0x20, 0x10, 0x10, 0x18, 0x08, 0x08, 0x04, 0x04, 0x02, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x38, 0x44, 0x82, 0x82, 0x92, 0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x1c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7c, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x7c, 0xc2, 0x80, 0x80, 0x40, 0x30, 0x18, 0x04, 0xfe, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x7c, 0x82, 0x80, 0xc0, 0x38, 0xc0, 0x80, 0xc2, 0x7c, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x60, 0x50, 0x58, 0x48, 0x44, 0x42, 0xfe, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x7e, 0x02, 0x02, 0x3e, 0xc0, 0x80, 0x80, 0xc2, 0x3c, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x78, 0x84, 0x02, 0x7a, 0xc6, 0x82, 0x82, 0xc4, 0x78, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0xfe, 0x40, 0x40, 0x20, 0x20, 0x10, 0x18, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x7c, 0x82, 0x82, 0x82, 0x7c, 0x82, 0x82, 0x86, 0x7c, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x3c, 0x46, 0x82, 0x82, 0xc6, 0xbc, 0x80, 0x42, 0x3c, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x08, 0x04, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x70, 0x0e, 0x0e, 0x70, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x1c, 0xe0, 0xe0, 0x1c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x1c, 0x22, 0x20, 0x10, 0x08, 0x08, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x78, 0xcc, 0x84, 0xe2, 0x92, 0x92, 0x92, 0xe2, 0x04, 0x0c, 0x78, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x10, 0x28, 0x28, 0x28, 0x44, 0x44, 0x7c, 0xc6, 0x82, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x7e, 0x82, 0x82, 0x82, 0x7e, 0x82, 0x82, 0x82, 0x7e, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x78, 0x84, 0x02, 0x02, 0x02, 0x02, 0x02, 0x84, 0x78, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x3e, 0x42, 0x82, 0x82, 0x82, 0x82, 0x82, 0x42, 0x3e, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0xfe, 0x02, 0x02, 0x02, 0xfe, 0x02, 0x02, 0x02, 0xfe, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0xfe, 0x02, 0x02, 0x02, 0xfe, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x78, 0x84, 0x02, 0x02, 0xc2, 0x82, 0x82, 0x84, 0x78, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x82, 0x82, 0x82, 0x82, 0xfe, 0x82, 0x82, 0x82, 0x82, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x38, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x42, 0x22, 0x12, 0x0a, 0x0e, 0x12, 0x22, 0x22, 0x42, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0xfe, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0xc6, 0xc6, 0xaa, 0xaa, 0xaa, 0x92, 0x82, 0x82, 0x82, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x86, 0x86, 0x8a, 0x8a, 0x92, 0xa2, 0xa2, 0xc2, 0xc2, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x7e, 0xc2, 0x82, 0x82, 0xc2, 0x7e, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x82, 0x44, 0x78, 0x60, 0x40, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x7e, 0xc2, 0x82, 0x82, 0x7e, 0x42, 0x82, 0x82, 0x02, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x7c, 0x86, 0x02, 0x06, 0x7c, 0xc0, 0x80, 0xc2, 0x7d, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x7f, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x7c, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x82, 0xc6, 0x44, 0x44, 0x44, 0x28, 0x28, 0x28, 0x10, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x81, 0x81, 0x81, 0x5a, 0x5a, 0x5a, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0xc6, 0x44, 0x28, 0x38, 0x10, 0x28, 0x6c, 0x44, 0x82, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x41, 0x22, 0x14, 0x14, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0xfe, 0xc0, 0x60, 0x20, 0x10, 0x08, 0x0c, 0x06, 0xfe, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x02, 0x04, 0x04, 0x08, 0x08, 0x18, 0x10, 0x10, 0x20, 0x20, 0x40, 0x00, 0x00},
	{0x00, 0x1c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1c, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x08, 0x14, 0x22, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00},
	{0x00, 0x00, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x44, 0x40, 0x7c, 0x42, 0x62, 0x5c, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x02, 0x02, 0x02, 0x02, 0x3e, 0x66, 0x42, 0x42, 0x42, 0x66, 0x3e, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x44, 0x02, 0x02, 0x02, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x40, 0x40, 0x40, 0x40, 0x7c, 0x66, 0x42, 0x42, 0x42, 0x66, 0x7c, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x42, 0x7e, 0x02, 0x46, 0x3c, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x30, 0x08, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x66, 0x42, 0x42, 0x42, 0x66, 0x5c, 0x40, 0x44, 0x38, 0x00},
	{0x00, 0x02, 0x02, 0x02, 0x02, 0x3a, 0x46, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x08, 0x00, 0x00, 0x00, 0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x10, 0x00, 0x00, 0x00, 0x1c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0e, 0x00},
	{0x00, 0x02, 0x02, 0x02, 0x02, 0x22, 0x12, 0x0a, 0x0e, 0x12, 0x22, 0x42, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x70, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 0x46, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x42, 0x42, 0x42, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x66, 0x42, 0x42, 0x42, 0x66, 0x3e, 0x02, 0x02, 0x02, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x66, 0x42, 0x42, 0x42, 0x66, 0x5c, 0x40, 0x40, 0x40, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x4c, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x02, 0x3c, 0x40, 0x42, 0x3c, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x08, 0x08, 0x7e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x70, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x62, 0x5c, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x66, 0x24, 0x24, 0x3c, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x81, 0x5a, 0x5a, 0x5a, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x24, 0x18, 0x18, 0x18, 0x24, 0x66, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x44, 0x24, 0x24, 0x28, 0x18, 0x10, 0x10, 0x08, 0x0c, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x40, 0x20, 0x18, 0x04, 0x02, 0x7e, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x38, 0x08, 0x08, 0x08, 0x08, 0x06, 0x08, 0x08, 0x08, 0x08, 0x08, 0x30, 0x00, 0x00, 0x00},
	{0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00},
	{0x00, 0x0e, 0x08, 0x08, 0x08, 0x08, 0x30, 0x08, 0x08, 0x08, 0x08, 0x08, 0x06, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9c, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
};
unsigned int src_graphics_font_bin_len = 2048;