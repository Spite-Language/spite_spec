/* The library tests/foreign_callback_reentered_site calls: check.sh builds it next to the program. */
#ifdef _WIN32
#define EXPORTED __declspec(dllexport)
#else
#define EXPORTED
#endif

EXPORTED int call_with(int value, int (*function)(int)) { return function(value); }
