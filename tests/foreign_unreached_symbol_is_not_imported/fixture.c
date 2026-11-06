/* The library tests/foreign_unreached_symbol_is_not_imported calls: it exports add_numbers and nothing else. */
#ifdef _WIN32
#define EXPORTED __declspec(dllexport)
#else
#define EXPORTED
#endif

EXPORTED int add_numbers(int left, int right) { return left + right; }
