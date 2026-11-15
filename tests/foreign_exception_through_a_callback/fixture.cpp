/* The foreign library tests/foreign_exception_through_a_callback calls: it calls back into the program inside a
   C++ try, and the program's callback calls throw_now, whose exception would unwind through the callback. */
#define EXPORTED extern "C" __declspec(dllexport)

EXPORTED int call_and_catch(void (*callback)(int)) {
    try {
        callback(1);
    } catch (int) {
        return -5;
    }
    return 0;
}

EXPORTED void throw_now(int value) {
    throw value;
}
