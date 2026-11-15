/* The foreign library tests/foreign_fault_under_a_function_ending_in_a_stop calls: it makes the program's own
   ExitProcess fault, so the fault sits under a Spite function whose last instruction is the call to it, and says
   how the system's frame walk reads that function's frame. */
#include <windows.h>
#include <intrin.h>
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

static void say(const char* text) {
    DWORD written;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), text, (DWORD)strlen(text), &written, 0);
}

/* The walk reads a frame by the unwind entry its return address falls in: inside the calling function's own entry,
   or on the first byte of the next function, whose prologue has not run. */
static void say_how_the_walk_reads(ULONG64 returned_to) {
    ULONG64 image = 0;
    PRUNTIME_FUNCTION entry = RtlLookupFunctionEntry(returned_to, &image, 0);
    ULONG64 offset = returned_to - image;
    if (entry && entry->BeginAddress < offset && offset < entry->EndAddress) {
        say("the frame under the fault is read as its own function's\n");
    } else {
        say("the frame under the fault is read as the start of another function\n");
    }
}

/* Faults once: the report the program prints then ends through the real ExitProcess again. */
static VOID WINAPI faulting_exit(UINT code) {
    (void)code;
    put_back(kept_function);
    say_how_the_walk_reads((ULONG64)_ReturnAddress());
    *(int volatile*)0 = 0;
}

static void break_the_exit(void) {
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
            if (strcmp((const char*)named->Name, "ExitProcess") != 0) continue;
            broken_slot = &slots->u1.Function;
            kept_function = *broken_slot;
            put_back((ULONGLONG)faulting_exit);
            return;
        }
    }
}

EXPORTED int largest_integer(void) {
    return 2147483647;
}

/* Breaks ExitProcess, then calls back into the program inside a handler that takes every fault. */
EXPORTED int call_back_guarded(void (*callback)(void)) {
    break_the_exit();
    __try {
        callback();
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return -1;
    }
    return 0;
}
