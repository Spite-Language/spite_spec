/* The foreign library of tests/foreign_callback_kept_by_its_owner_*: built with zig cc -shared next to the program. */
#define EXPORTED __declspec(dllexport)

/* Kept with user data last and called later, the way Vulkan keeps a debug messenger. */
static void (*kept_tick)(int count, void* context) = 0;
static void* kept_context = 0;
EXPORTED void keep_tick(void (*tick)(int, void*), void* context) { kept_tick = tick; kept_context = context; }
EXPORTED void tick(int count) { if (kept_tick) kept_tick(count, kept_context); }
EXPORTED void forget_tick(void) { kept_tick = 0; kept_context = 0; }
