extern "C" {
#include <graphics/lfb.h>
#include <usr/string.h>
};

class Substation {
	public:
		unsigned long temperature;
		unsigned long critical_temperature;
		unsigned long current_production;
		unsigned long max_production;
		unsigned long current_load;
		Substation(unsigned long ct, unsigned long mp) {
			temperature = 0;
			current_production = 0;
			current_load = 0;
			critical_temperature = ct;
			max_production = mp;
		}
};

extern "C" void cpp_demo(unsigned long v)
{
	Substation s = Substation(120000+10*v, 95000000 - 23*v);
	char str[14];
	char* start = ulong_to_string(s.max_production, str);
	draw_string(0, 15, start);
}
