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
