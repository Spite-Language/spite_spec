/* The foreign library tests/foreign_site_is_the_faulting_threads calls: a thread of its own blocks in a call. */
#include <windows.h>
#define EXPORTED __declspec(dllexport)

static DWORD WINAPI later_main(LPVOID argument) {
    Sleep(50);
    ((void (*)(void))argument)();
    return 0;
}

EXPORTED void start_later(void (*callback)(void)) {
    HANDLE thread = CreateThread(0, 0, later_main, (LPVOID)callback, 0, 0);
    CloseHandle(thread);
}

EXPORTED void block_forever(void) {
    Sleep(INFINITE);
}
