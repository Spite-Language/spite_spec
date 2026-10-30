# Concurrency: waiting without colouring

The specification of [Concurrency: waiting without colouring](https://github.com/Spite-Language/spite_documentation/blob/master/concurrency.md): every rule of this part of the language, with
its edge cases and its exact error texts. The page teaches; this one decides, and where they disagree, the page
is the bug.

## What runs at once

No function is coloured, and a program never says what runs at once: the compiler decides. It overlaps work it
proves independent, wherever the work stands and where it pays, and a program shows exactly what running it in
order shows: the same output, the same answers, the same values read. The one exception is a crash: the first
crash stops the program (below). Wherever the compiler cannot yet prove work independent, the program runs it in
order, slower and with the same output; spite_compiler#218, #219 and #220 widen what it overlaps. IO never blocks
the program by the program's own hand, and the compiler puts the REPL's drain points where the program already
waits.

**`Parallel(...)` and `Concurrent(...)` are not part of Spite.** Writing either is an error at the call:
`'Parallel' is not part of Spite: the compiler decides what runs at once, so call the function itself`
(`conformance/parallel_is_gone`), and the same with `'Concurrent'` (`conformance/concurrent_is_gone`). There is
no handle type to name and no parallel form of a walk: `list.parallel_each_update()` is an unknown member
(`conformance/parallel_each_is_gone`).

**Threads.** Work the compiler overlaps (calls in a row, a loop over different classes, bands) runs on a thread of
the program's thread pool, and nothing else starts one but a blocking call's helper (below). The pool is made
empty at the first such start and grows: the work goes to a thread that finished earlier work and waits for more,
and when none is waiting the pool starts a new thread for it, which then stays in the pool. So no work ever waits
for a thread to come free, and a program in which nothing is overlapped has no pool and starts no thread. Each
start hands the work to a waiting thread (a lock, a queue and a wake) and pays for starting a thread only when none
is waiting; everything started is joined before the statement after the one that started it.

**Locks and per-thread values are not the program's to write.** Work the compiler overlaps never shares anything
one of them writes, so it needs no lock, and no singleton gets one. There is no lock, atomic or thread-local class
for a program to name, and naming `Spite.Internal` anything in a program is a compile error
([packages.md](packages.md)). `conformance/stage6/singleton_guard`, `singleton_forms`, `singleton_lock_calls`,
`singleton_reads`, `unshared_locks`, `coarse_locks`, `locked_waits` and `parallel_handover` call singletons and
objects from many pieces of work and print what running them in order prints.

**The mechanism: compile-time state machines, and a helper thread per blocking call.** The aim is no runtime, only
compile time. The machine has no coroutines, so the compiler writes them. Every function that can reach a wait
from inside work the compiler starts as a state machine of its own (a call a frame loop starts, a read it
overlaps, below) is compiled twice: the plain function, for every other call, and a resumable version:

- **Which functions.** A function *waits* when it is one of the waits below or calls a function that waits: by
  name, through a constructor, through a union's or a `type`'s function that waits for one of its classes, or
  through a function value of a signature some function made into a value with that signature waits for (found
  over the calls the plain bodies make, to a fixpoint). The function of work the compiler starts gets a state
  machine when it waits, and so does every function a state machine calls that waits, constructors included; a
  singleton's constructor gets none. In a `--hot-reload` build a program class's state machine starts through a
  slot a reload swaps, and a reload waits while such work is stopped inside a function it swaps
  ([repl.md](repl.md), "A reload waits for the waits of what it swaps").
- **The frame** is a block on the heap: a header (the step function, the wait it stopped at, whether it runs or
  has finished), the result, `self`, the parameters, every local and temporary of the body (a name declared twice
  in nested blocks gets two fields), and one slot per wait for the frame it waits on.
- **The step function** runs the body with every local read from the frame, starts with a jump to the wait it
  stopped at, and answers `true` when the body returns and `false` at a wait that is not over. Jumping into a
  `while` or an `if` needs nothing, since no local lives on the stack.
- **A wait** written as a call to a function that waits, anywhere an unconditional value is computed (a
  statement, a `var`, an argument, an operand, a `{...}` in text, a condition), becomes: hold the receiver, make
  the callee's frame with the arguments, step it until it answers `true` (returning `false` from this step while it
  does not), take its result and free it. It runs before the rest of the statement it is written in. A `while`
  whose condition waits becomes a loop that waits at the top of every pass and leaves when the condition is false.
  A call inside a template is a call by name like any other.
- **Waits in a condition, a comparison, or behind a choice made while running** are points the state machine
  returns from too, so two waits never hold each other on the stack: the right side of `and`/`or`, and every
  condition of an `if` after the first, is computed only if what came before lets it, with its waits inside that
  test; `==` and `!=` on a nullable left side whose right side waits keep the left side, wait for the right side
  once, then compare the two kept values; a call through a function value makes the frame of whichever function
  the value holds (a small function per signature compares the value's function with each function made into a
  value with that signature that has a state machine, and makes its frame, or answers none, and the value is then
  called plainly), and a union's or a `type`'s function does the same by class; a constructor that waits makes the
  object, then steps the constructor's own state machine on it. A wait through a `String`'s or a number's function
  held as a value runs the event loop where it is.
- **A wait inside an expression keeps the written order.** What the expression computes before the wait (the
  left operand of an operator, the earlier pieces of a text, the earlier arguments of a call, the receiver of a
  call, the left side of an `and`/`or` or a comparison) is computed before it, into a temporary in the frame, and
  what comes after it is computed after it. A reference read that way is held for the wait and let go once the
  expression is done, so a change other work makes during the wait neither frees it nor is seen by it. A local, a
  parameter, a constant, `self` or another wait's result is not copied, since nothing can change it while the
  state machine waits. An assignment works out its value before its target. `conformance/stage6/wait_order`.
- **The waits at the bottom** are small state machines the compiler writes: `Program.sleep` registers a deadline
  (the `Duration`'s whole milliseconds from now) and is over when the clock passes it; `Console.read_line_into`,
  `File.read_into`, `File.write_text`, `File.write_from`, the wait for a file's lock (`File.lock()`),
  `Tcp.Server._accept_handle`, `Tcp.Connection._receive_into` and `Tcp.Server._first_readable` start their one
  system call on a helper thread and are over when it flags that it returned; waiting for work the compiler
  started is over when that work's frame has finished.

Files cannot be waited on without blocking on any of the three systems, so a blocking call is handed to a
helper thread of the pool (what libuv does for files). The alternative, each system's own readiness (IOCP, epoll,
kqueue), is not used: an ordinary file is always "ready" to epoll and kqueue, the Windows console cannot be read
through IOCP, and one mechanism keeps each system's folder small.

**What the compiler writes for code outside a state machine.** In a program that uses the scheduler (one in which
the compiler starts waiting work, or built with `--repl-port` or `--hot-reload`) each blocking call outside a state
machine is wrapped: a sleep runs the event loop until its time, and a blocking call runs on a helper thread while
the event loop runs. Every other program gets none of it: no wrapper, no scheduler, no state machine.

**Blocking is what the compiler picks when it is faster.** The wrapper asks the scheduler first: when no work it
started on the caller's thread is alive and no REPL is listening there, nothing else could run on that thread
meanwhile, so the wrapper makes the plain blocking call.

**The scheduler** is the compiler's own, and a program cannot name it. It keeps one loop per thread, made the first
time a thread waits (`conformance/stage6/concurrent_in_parallel`). The REPL's commands and reloads are answered
only on the program's own thread. Each loop keeps the frames of the work that has not finished, the deadlines of
the sleeps in them and a count of helper threads in flight. Its loop answers the REPL's pending command and a
pending reload, if any, steps every frame that is not already running further down the stack, and, when none
finished, waits until a helper thread or the REPL's socket thread signals, with the nearest deadline as its
timeout. Waiting forever on nothing is a deadlock, and a crash.

**Soundness.** A program that starts a thread (overlapped work, a blocking call's helper, or `--repl-port`), or
hands C a `ForeignCallback` it may call from one, counts references with atomic operations, and the
`--debug-memory` counts are kept safely. Every other program keeps the plain counts. Spite code on the program's
thread only ever changes hands at a wait, so state machines need nothing more. A singleton first used from two
threads at once is made once: the fetch takes the singleton's own lock only while its slot is empty
(`conformance/stage6/singleton_race`). A cycle's back reference (the compiler's choice, [memory.md](memory.md))
needs no care across threads: a parent and its child built on another thread are both freed there. One smaller
gap: a REPL client's `exit` while a helper thread is blocked reading the console may wait for that read when the
process exits.

**Reads in a row overlap.** All the IO classes use it, and a `File` or `Tcp.Connection` is not changed: the compiler does it
at the call site. When two or more statements in a row are each `var name = receiver.read_as_string()` on a `File` (or
`receiver.read_line()` on a `Tcp.Connection` or a `Tcp.Client`), with no written type, the receiver a name or an attribute path, no statement
naming a variable an earlier one declared, and the name never assigned again in the function, the compiler starts
every one but the last as a state machine of its own, and after the last waits for each before the next statement
runs. Only reads, only side by side, and all finished before anything else runs, so nothing the program does next
(writing one of those files, say) can see a difference; reads followed by other work, or a read into a name that
is assigned later, stay where they are. Such a program uses the scheduler and counts references atomically; a
program without two reads in a row compiles as before. A `Directory` listing is not a waiting call (it has no
helper thread), so starting it early would not overlap anything, and it is left alone.
`conformance/stage6/overlapped_reads`.

**Calls in a row run at once.** Two or more declarations in a row, each `var name = receiver.function(arguments)`
where `receiver` is `this` or an attribute of `this` and the arguments and the answer are numbers, run at once when
every call is quiet work and no argument reads a name an earlier one of them declares: every call but the last runs
on a thread of the pool, the last on the calling thread, and each started one is joined right before the first
statement that reads its name, or that holds a `return` or a `crash`; one that may halt is joined right after the
run, so nothing after it shows before its crash. A call is quiet work when its function,
through every function it reaches, belongs to a class of the program that is not a singleton (the entry class
included), takes and answers only numbers, reads only its parameters and locals, writes only its locals, calls only
functions of `this`, and holds no object, text, list, attribute, fallback, foreign call, function value or recursion;
it may halt, by a `crash`, a whole-number result that does not fit, a division by zero or a number narrowed into
one it does not fit. Each call must weigh at least a million (each statement 1, a loop's body times its bound, a
call what its function weighs). Nothing is overlapped in the standard library or in a `--hot-reload`, `--repl` or
`--development` build. The compiler finds independent work wherever it can prove it, not where the program says
so; overlapping by the data each piece of work touches (inside one function, across classes) is
spite_compiler#242. `tests/overlapped_work`, `tests/overlapped_work_in_the_entry`.

**The first crash stops the program.** When work that runs at once crashes, the first crash that happens stops the
program at once: its report is printed and nothing else runs or prints, and a thread that crashes after it never
reports. So when two pieces of work would both crash, either one's report may be the one printed, and exactly one
is. The report is what any crash prints (D1783): the line and values, and the frames of the work that crashed down
to its first function, then one frame line more, marked `started`, naming where the program wrote the call that
ran apart, `spite.frame	tests/overlapped_work_crashes/overlapped_work_crashes.spite:6	OverlappedWorkCrashes	OverlappedWorkCrashes	started`,
since the program text never says that work ran apart; the caller's further frames are left out. A program in which nothing runs at once halts as it
always did. `tests/overlapped_work_crashes`, `tests/overlapped_work_halts`.

**Objects made for their owner count apart.** An attribute is made for its owner when every assignment to it in
the program (its declaration and constructor included) gives it an object constructed right there, or nothing, and
its owner is never copied with it (a `copy()` or a struct copy of the owner). What a call reads and writes of the
object such an attribute holds, and of the lists and objects that object's own attributes made for it hold, counts
apart from the object any other such attribute holds: two attributes of different classes or names never hold one
object. An object passed to a function counts, inside it, as the argument it was passed as, when the function does
nothing with that parameter but read and write through it, pass it to a list's or dictionary's own functions or to
another such function, or compare it. An attribute made for its owner that is, besides, only ever read through,
passed to such functions, compared or let go, is kept nowhere else, and then a call that reaches an object of its
class through any other name (a local, a parameter of unknown origin) does not touch it. An item read out of a list
attribute of a singleton counts apart from an item read out of another singleton's list attribute only when the
two lists hold no object in common: for a loop over different classes, the loop compares the two lists' items when
it starts, for each two classes that meet and need it, and runs in order when one object is in both; neither call
may write either list. Anything else read through an object of a class counts as every object of the class, as
before. `conformance/stage6/owned_objects`.

**A loop over a list of different classes runs them at once.** A statement `list.each_function()`, or
`list.each(own_function)` where `own_function` takes one parameter and its whole body is `parameter.function()`,
where `list` is a name or a path of attributes holding a `List` of a `type` and `function` is one the `type`
requires, with no arguments, answering nothing, follows the rule above with the elements in the place of the row:
for every two classes the `type`'s call can reach, the compiler decides while compiling whether their `function`s
are independent (the same reckoning as calls in a row, and neither writes the list's items), and which of them reach a loop. When the statement runs, it reads each element's
class: if the list holds from two up to as many elements as there are such classes, no two of one class, every
two independent and at least two reaching a loop, every element that reaches a loop but the last of them runs on
a thread of its own and the rest run on the calling thread, in the list's order, and all are joined before the next
statement; otherwise the loop runs in order, as written. Two objects of one class are never run at once. When no
two classes could ever run together, the statement is the plain loop and the program carries nothing of this. The
same builds and places are left out as for calls in a row, and a crash halts the same way. `conformance/stage6/row_of_classes`.

**A loop whose passes write only their own item runs in bands.** The same two statements over a `List`
of a class of the program (not a singleton), where `function` belongs to that class, takes no arguments
and answers nothing, run in bands on threads, one band for each processor with the first on the calling thread
and all joined before the next statement, when every pass writes only its own element: taken through every function
it reaches, the pass writes attributes only of the object it runs on (reached as that object, not through another
name of its class), or the items of a list or dictionary attribute of it that holds one made for it and never
handed anywhere else, and reads nothing a pass writes, read through any other name; and it counts no reference,
prints, reads or writes a `File`, a `Directory` or a network class, waits, calls anything outside the program but plain arithmetic and the clock, or calls through a function value. The compiler weighs the pass while compiling (each statement 1, a loop's body as many times as its bound
when the bound is a number, 8 times otherwise, a call what its function weighs) and runs in bands only when the
pass weighs at least 256 and the list's count, read when the statement runs, times that weight reaches a million (see
[optimizations.md](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md#a-loop-whose-passes-write-only-their-own-item-runs-in-bands)), and,
for a `List`, only when every element is held by the list alone, so that no object is two elements; otherwise the
loop runs in order, as written. When no pass qualifies the statement is the plain loop. The same builds and places
are left out as for calls in a row, and a crash halts the same way. `conformance/stage6/band_passes`.

## A wait in a frame does not hold the frame

A call that waits, made by the work a frame loop runs, is started by the compiler as a state machine of its own,
so the frame goes on while it waits.

- **A frame loop** is a `while` in a function of a class of the program (not the standard library's, not a generic class's
  template) whose body calls `sleep` on a `Program` (an attribute whose type is `Program`, or `Program()` itself),
  directly or through calls by name to functions its own class declares (not its constructor), followed to any
  depth. The calls are found in every statement and condition of the body, nested blocks included, but not inside
  a `{...}` of a text.
- **The frame's code** is the body of each frame loop, and every function of the loop's own class that a frame's
  code calls by name, followed to any depth. A function of the frame's code is that code wherever it is called from.
- **A started call** is a statement of the frame's code that is a call, and nothing else: `receiver.function()`,
  with no arguments, its answer unused, where `receiver` is an attribute of the frame's class, a path of
  attributes from it, or an item of a list such an attribute or path holds, read by a name or a whole-number
  literal (`systems[index]`); where the receiver's
  type is a class of the program that is not a singleton or a value class, whose `function` (not its constructor)
  answers nothing, or a `type` that requires `function` answering nothing; and where that function can wait, as
  `is_resumable` answers ([metaprogramming.md](metaprogramming.md#asking-a-question-while-compiling)): for a `type`,
  when any of its classes' functions can. The compiler starts it as a state machine of its own, which runs the
  work until its first wait, and keeps it, with the site that started it, instead of waiting for it. Every other
  statement, a call whose function never waits included, is compiled as before.
- **Where it moves on.** Started work moves on at every wait on the program's thread, which in a frame loop is the
  frame's sleep.
- **In flight.** Each unfinished piece of work is kept with the site (one per started statement) that started
  it. Each time one is started, the site first lets go of every one that has finished, then keeps the new one when
  it has not finished; while more than eight from the same site have not finished, it waits in place for the
  oldest of them. So no site has more than eight in flight once its statement is done, and nothing is ever
  dropped unfinished.
- **Its end.** When the entry function returns, every started call that has not finished runs to its end before
  standard output is flushed and before any singleton is destroyed ([classes_and_files.md](classes_and_files.md),
  teardown order), so its work, its writes and its crash, if it crashes, all happen before the program ends.
- **A failure.** A crash inside a started call halts the program with the report of that crash, naming its own line
  and the stack of the work that ran it; nothing is kept back for a later frame to report. A `fallback` that fails
  inside it returns from its function, as anywhere.
- **Where not.** Not in the standard library, not inside a state machine (it waits there as before), not for a
  call whose answer is used (`var text = file.read_as_string()` waits where it is written), and not for a call on
  the loop's own object (`tick()`, `wait_for_next_tick()`), which is the frame's pace.
- **Answered once every function is compiled.** Whether the function can wait is known once every function it
  reaches has been compiled, so each function of the frame's code holding a statement of that shape is compiled
  after the others. If a function compiled later still changes the answer, the build fails at the statement:
  `whether 'report.fetch' can wait was answered false here, before every function it reaches was compiled, and it
  is true once they are, so this frame would wait where it should go on: call it from a function the frame loop's
  own class declares`.
- **What it costs.** A program in which no statement is started compiles as before. One in which some are uses the
  scheduler, plus two lists of started work, scanned each time a call is started. `conformance/stage6/frame_waits`.
- **No write is lost.** The started work and the frame run in turns, each step from one wait to the next, so a
  value one of them reads before a wait may be stale after it. A call is started only when neither side can write
  back such a value. The compiler reads the plain (not resumable) code of every function the started work reaches
  that can wait, and of every function the frame loop reaches that can wait, in order and twice over (so a value
  carried around a loop is seen): a local or parameter holding a number, a `Boolean`, an enum, text or a value
  class, computed from an attribute of such a kind (or, for a parameter, from anything), is *carried* once a call
  that can wait follows it. The call is not started, and waits where it is written exactly as before, when a
  carried value is written into an attribute of the same name of an object the function did not make, is passed
  to a function of the program that writes an attribute of that name, is returned and written so by the caller, or
  when one statement reads an attribute, waits and writes the same attribute. A carried object reference is not a
  stale value: what is read through it is read when it is read. Each call kept in place is listed in
  `--optimization-report` under `Waits that hold the frame`, `` `<call>()` waits in place: <Class>.<function>
  writes `<attribute>` from a value it read before a wait, and the frame could write it while the call waits ``
  for the started side, or `` the frame loop writes `<attribute>` from a value it read before a wait, and the call
  could write it while the frame waits `` for the frame's side, which keeps every call that loop's class starts in
  place. A program whose every such call is kept in place still carries the scheduler it would have used.
  `conformance/stage6/frame_waits_kept`.

---

Next: [Standard library](standard_library.md).
