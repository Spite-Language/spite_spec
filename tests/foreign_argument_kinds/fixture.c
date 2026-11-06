/* The library tests/foreign_argument_kinds calls: check.sh builds it next to the program. */
#include <stdint.h>
#ifdef _WIN32
#define EXPORTED __declspec(dllexport)
#else
#define EXPORTED
#endif

EXPORTED int fill_bytes(unsigned char* bytes, int count) { for (int index = 0; index < count; index = index + 1) bytes[index] = (unsigned char)(index * 3 + 1); return count; }
EXPORTED int choose(int flag) { return flag ? 10 : 20; }
EXPORTED int mode_number(int mode) { return mode * 100; }
EXPORTED int whole_of_float(float value) { return (int)(value * 4.0f); }
EXPORTED int whole_of_double(double value) { return (int)(value * 4.0); }
EXPORTED long long WidthsAdded(signed char tiny, short small, unsigned char byte, unsigned short port, unsigned int big, unsigned long long huge) {
    return (long long)tiny + (long long)small + (long long)byte + (long long)port + (long long)big + (long long)huge;
}
EXPORTED void Forget(int value) { (void)value; }
