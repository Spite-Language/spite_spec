/* The library tests/foreign_symbols_resolved_when_the_library_opens calls: it exports touch and nothing else. */
#include <stdio.h>
#ifdef _WIN32
#define EXPORTED __declspec(dllexport)
#else
#define EXPORTED
#endif

EXPORTED int touch(void) {
    FILE* written = fopen(".spite/foreign_symbols_resolved_touched.txt", "w");
    if (written == 0) { return 1; }
    fputs("touched", written);
    fclose(written);
    return 0;
}
