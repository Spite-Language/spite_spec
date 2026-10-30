# Retired tests

These programs describe classes that the language now hides (D662): memory and allocators, and threads and
synchronisation (Lock, Atomic, Scheduler, ThreadPool, ThreadLocal, ThreadSlot) are not classes a program can name
or inspect. They are kept so that the behaviour they checked can be rebuilt as language constructs.

- atomic_values: Atomic counters, compare_and_swap, exchange and wrapping across Parallel handles; retired because Atomic is not a class under D662
- finished_values: finished_value on Concurrent and Parallel handles driven frame by frame; retired because Scheduler is not a class under D662
- finished_workers: workers draining items while frames resume other work; retired because Scheduler is not a class under D662
- frames_between_waits: systems that wait across frames while stages run Parallel work; retired because Scheduler is not a class under D662
- template_argument_wait: a templated driver waiting across frames; retired because Scheduler is not a class under D662
- thread_locals: ThreadLocal values separate per thread and empty on a new thread; retired because ThreadLocal and ThreadPool are not classes under D662
- thread_local_growth: ThreadLocal slots created while the pool runs; retired because ThreadLocal and ThreadPool are not classes under D662
- thread_pool_reuse: pool threads reused across jobs; retired because ThreadPool is not a class under D662
- parallel_stages: per-thread buffers and a Lock-guarded id counter across Parallel stages; retired because ThreadLocal and Lock are not classes under D662
- allocator_after_use: an object's allocator cannot change after use (conformance); retired because allocators are not a class under D662
- allocator_reads: reading an object's memory.allocator (conformance); retired because allocators are not a class under D662
- allocator_resizes: a borrow invalidated by an allocator that grows another list (conformance); retired because allocators are not a class under D662
- atomic_misuse: Atomic of Float or a non-number (conformance); retired because Atomic is not a class under D662
- address_forms: reading and writing bytes at a Memory.Address (conformance); retired because Memory.Address is not a class under D662
- memory_access: reading and writing memory at an address (conformance); retired because Memory.Address and Memory.Heap are not classes under D662
- number_memory: a number declaring a wrong byte size with Memory.Bytes (conformance); retired because Memory is not a class under D662
- number_memory_spelling: a number's memory spelled with a heap allocation (conformance); retired because Memory.Heap is not a class under D662
- foreign_callbacks_voices: C calling a callback table kept in heap memory; retired because Memory.Heap and Memory.Address are not classes under D662 (the rest of foreign_callbacks stays in tests)

These programs used `Concurrent` or `Parallel`, which left the language under D1715 (spite_compiler#224): what they printed came from the handles themselves, so plain code that runs in order cannot print it. They are kept so the behaviour can be rebuilt as work the compiler overlaps itself (spite_compiler#218, #219, #220).

- default_handles: function_waits on functions whose parameters are Concurrent or Parallel handles (joining a handle waits, reading its finished_value does not)
- handle_debug_running: a running Concurrent's debug text prints `running` before its join
- finished_polling: polling a Parallel's and a Concurrent's `finished` until the work completes
- finished_value_running: finished_value() answers null while a Concurrent runs and its value after the join
- dropped_handle_waits: reassigning or dropping a Concurrent handle (in a local, an object, an attribute, a list) waits for its work, seen in the log order
- concurrent_unfinished_at_exit: a Concurrent a singleton holds runs to its end after the entry function returns
- poll_without_step_halts: polling `finished` on a Concurrent that nothing steps halts the program
- concurrent_wait_cycle: joining a Concurrent that waits for the joiner stops the program with a wait-cycle message
- concurrent_wait_cycle_in_place: every Concurrent waiting for another while none sleeps stops the program
- function_waits_dropped: letting go of a Concurrent makes a function wait, and a dropped task's `finished`
- state_machines: Concurrent functions resume at each wait, so their log lines interleave, plus a dropped handle's `finished`
- waits_never_hang: every kind of wait point (and/or, if, ==, function values, unions, types, constructors, dropped handles) is a place a Concurrent returns from
- concurrent_waits: two sleeping Concurrent calls interleave their log lines, and a dropped handle is joined
- waiting_systems: a system whose update_each can wait is started with Concurrent, and the path is chosen at run time by function_waits
- pooled_loads_wait: a pool that starts loads with Parallel and collects them with finished_value() does not wait, only the direct file read does
- singleton_stateless_calls: a singleton function that starts a Parallel on a helper calling back into the same singleton, then polls the handle's finished_value without deadlocking
- type_function_values: a type's function held as a value is a point a Concurrent returns from, and still answers
- web_socket_exchange: a WebSocket.Client and a WebSocket.Server conversing (text, binary, ping/pong, close) plus raw fragmented and unmasked frames, which needs both ends running at once
- web_socket_failures: a WebSocket.Client refused by a plain HTTP server and the server's failure close codes (1007, 1009, 1002, 4000, 1006, the 426 refusal), which needs both ends running at once
