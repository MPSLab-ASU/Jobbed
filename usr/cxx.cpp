extern "C" {
#include <drivers/uart.h>
};

class TestClass {
	public:
		unsigned long value;
		TestClass(unsigned long v) {
			value = v;
		}
		void increment() {
			value++;
		}
		void increment(unsigned long v) {
			value += v;
		}
};

extern "C" void cpp_demo(unsigned long v)
{
	TestClass tc = TestClass(v);
	tc.increment();
	uart_hexn(tc.value);
}
