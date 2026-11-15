/* The foreign library tests/native_fault_in_a_routine_from_a_resumable_function calls: it makes the program's
   own HeapAlloc fault, the way a broken heap faults inside the system's heap functions. */
#include <windows.h>
#include <string.h>
#define EXPORTED __declspec(dllexport)

static ULONGLONG* broken_slot = 0;
static ULONGLONG kept_function = 0;

static void put_back(ULONGLONG function) {
    DWORD old;
    VirtualProtect(broken_slot, sizeof(*broken_slot), PAGE_READWRITE, &old);
    *broken_slot = function;
    VirtualProtect(broken_slot, sizeof(*broken_slot), old, &old);
}

/* Faults once: the report the program prints then allocates through the real HeapAlloc again. */
static LPVOID WINAPI faulting_alloc(HANDLE heap, DWORD flags, SIZE_T size) {
    (void)heap;
    (void)flags;
    (void)size;
    put_back(kept_function);
    return *(LPVOID volatile*)0;
}

static void break_the_heap_functions(void) {
    BYTE* image = (BYTE*)GetModuleHandleA(0);
    IMAGE_NT_HEADERS64* headers = (IMAGE_NT_HEADERS64*)(image + ((IMAGE_DOS_HEADER*)image)->e_lfanew);
    IMAGE_DATA_DIRECTORY imports = headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    IMAGE_IMPORT_DESCRIPTOR* library = (IMAGE_IMPORT_DESCRIPTOR*)(image + imports.VirtualAddress);
    for (; library->Name; library = library + 1) {
        IMAGE_THUNK_DATA64* names = (IMAGE_THUNK_DATA64*)(image + library->OriginalFirstThunk);
        IMAGE_THUNK_DATA64* slots = (IMAGE_THUNK_DATA64*)(image + library->FirstThunk);
        if (!library->OriginalFirstThunk) names = slots;
        for (; names->u1.AddressOfData; names = names + 1, slots = slots + 1) {
            if (names->u1.Ordinal & IMAGE_ORDINAL_FLAG64) continue;
            IMAGE_IMPORT_BY_NAME* named = (IMAGE_IMPORT_BY_NAME*)(image + names->u1.AddressOfData);
            if (strcmp((const char*)named->Name, "HeapAlloc") != 0) continue;
            broken_slot = &slots->u1.Function;
            kept_function = *broken_slot;
            put_back((ULONGLONG)faulting_alloc);
            return;
        }
    }
}

/* Breaks the heap functions, then calls back into the program, which allocates from a function that waits. */
EXPORTED void call_back_broken(void (*callback)(void)) {
    break_the_heap_functions();
    callback();
}

static DWORD WINAPI later_main(LPVOID argument) {
    Sleep(50);
    break_the_heap_functions();
    ((void (*)(void))argument)();
    return 0;
}

/* Breaks the heap functions on a thread of its own, then blocks there in a call. */
EXPORTED void start_later(void (*callback)(void)) {
    HANDLE thread = CreateThread(0, 0, later_main, (LPVOID)callback, 0, 0);
    CloseHandle(thread);
}

EXPORTED void block_forever(void) {
    Sleep(INFINITE);
}
