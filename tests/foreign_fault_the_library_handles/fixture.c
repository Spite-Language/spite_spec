/* The foreign library tests/foreign_fault_the_library_handles calls: it catches its own fault. */
#include <stdint.h>
#include <windows.h>

__declspec(dllexport) int read_or_answer(int64_t address) {
    __try {
        return *(volatile int*)(intptr_t)address;
    } __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        return -1;
    }
}
