/* The foreign library for the foreign callback tests of tests/foreign_*: built with zig cc -shared next to the program. */
#include <windows.h>
#define EXPORTED __declspec(dllexport)

/* Kept without user data and called later, the way Windows keeps a window procedure. */
static void (*kept_click)(int count) = 0;
EXPORTED void keep_click(void (*click)(int)) { kept_click = click; }
EXPORTED void click(int count) { if (kept_click) kept_click(count); }
EXPORTED void forget_click(void) { kept_click = 0; }

/* A COM-style table of two functions, kept on the C side and called from a thread the library makes. */
static void (*table_started)(int voice) = 0;
static void (*table_ended)(int voice, long long buffer) = 0;
EXPORTED void keep_table(void (*started)(int), void (*ended)(int, long long)) {
    table_started = started;
    table_ended = ended;
}
static DWORD WINAPI play_main(LPVOID argument) {
    int voice = (int)(long long)argument;
    table_started(voice);
    table_ended(voice, voice * 100LL);
    return 0;
}
EXPORTED void play_on_a_thread(int voice) {
    HANDLE thread = CreateThread(0, 0, play_main, (LPVOID)(long long)voice, 0, 0);
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
}

/* Several threads of the library call one function with no user data, many times each. */
typedef struct { int calls; void (*hit)(int); } HitJob;
static DWORD WINAPI hit_main(LPVOID argument) {
    HitJob* job = (HitJob*)argument;
    for (int call = 0; call < job->calls; call = call + 1) job->hit(1);
    return 0;
}
EXPORTED void hit_from_threads(int threads, int calls, void (*hit)(int)) {
    HANDLE handles[16];
    HitJob job = { calls, hit };
    for (int index = 0; index < threads; index = index + 1) handles[index] = CreateThread(0, 0, hit_main, &job, 0, 0);
    WaitForMultipleObjects(threads, handles, TRUE, INFINITE);
    for (int index = 0; index < threads; index = index + 1) CloseHandle(handles[index]);
}

/* The same with user data first, as XAudio2 passes it. */
typedef struct { int calls; void (*report)(void* context, int value); void* context; } ReportJob;
static DWORD WINAPI report_main(LPVOID argument) {
    ReportJob* job = (ReportJob*)argument;
    for (int call = 0; call < job->calls; call = call + 1) job->report(job->context, 1);
    return 0;
}
EXPORTED void report_from_threads(int threads, int calls, void (*report)(void*, int), void* context) {
    HANDLE handles[16];
    ReportJob job = { calls, report, context };
    for (int index = 0; index < threads; index = index + 1) handles[index] = CreateThread(0, 0, report_main, &job, 0, 0);
    WaitForMultipleObjects(threads, handles, TRUE, INFINITE);
    for (int index = 0; index < threads; index = index + 1) CloseHandle(handles[index]);
}

EXPORTED int add_numbers(int left, int right) { return left + right; }
