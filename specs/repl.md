# REPL and live reload

The specification of [REPL and live reload](https://github.com/Spite-Language/spite_documentation/blob/master/repl.md): every rule of this part of the language, with
its edge cases and its exact error texts. The page teaches; this one decides, and where they disagree, the page
is the bug.

## Nothing needs a restart

No change to a program's code needs a restart. Every refusal in [What a reload can change](https://github.com/Spite-Language/spite_documentation/blob/master/repl.md#what-a-reload-can-change)
is a gap to close, not a rule. The steps, in the order a game meets them:
1. **A class's attributes change**, [Changing a class's attributes](https://github.com/Spite-Language/spite_documentation/blob/master/repl.md#changing-a-classs-attributes): the reload moves
   every live object to the new layout, copying kept attributes by name, giving new ones their defaults and
   releasing removed ones. A rename is given with the reload, never in the program's code: at the REPL as
   `load "hero.spite", {Hero.attributes["level"]: "rank"}` ([Loading a changed file](#loading-a-changed-file)),
   and over the MCP as `reload`'s `renames` ([mcp.md](mcp.md)). The rules:
   - A `--hot-reload` build gives each object of a program class two hidden words after its header, `spite_moved`
     (its attributes' block once they moved, else 0) and `spite_live` (its place in the class's list of live
     objects), reads every attribute through `<Class>___fields(object)`, and describes each class's layout --
     every attribute's name, type, offset, size and release, in a table the reload reads (`<Class>___layout`).
   - A class's layout is its attributes' names and types in order. A
     reload whose layout for a class differs from what the running program holds compiles the whole program. It
     compiles every function that reads that class's attributes or its size again and swaps it in; one it could
     not swap is refused by name (`the change reaches ...`), and the program keeps all of its code.
   - The library installs every slot it replaces or none, then moves the objects: each live object of a changed
     class gets a new block, made with the new defaults, into which each attribute of the same name and type is
     moved, or the attribute the reload's map pairs with it, first. Every attribute of the old layout nothing
     took is released after every object of every changed class has moved, so a release that reaches another
     moved object finds it moved. Objects made while moving (a new attribute's default) are made in the new layout
     and not moved again.
   - Nothing else runs while it installs and moves: the scheduler's tasks are paused, and every task on the thread
     pool (work the compiler runs at once) stops first at the next pass of one of its loops, where it
     holds no attribute half read, and goes on once the swap is done; a task that starts meanwhile waits to start.
     A task that reaches no such point within 10 seconds (it waits on a lock, sleeps, or reads a socket) refuses
     the reload: `a task on another thread did not reach a loop's check point within 10 seconds, so nothing was
     swapped in and no object moved under it: reload again once that work has finished`. This is a
     `--hot-reload` build's alone: each task counts itself in and out of a gate, and each check point a task
     passes looks whether a swap is waiting.
   - An attribute of a new type is a new attribute: it holds its default. The reload names every class it moved,
     and for each what is new, gone, renamed or retyped. When, in one class, an attribute is gone while another is
     new and the reload was given no map, the reload is held: nothing is swapped in, and the answer names both and
     the map that would keep the values. A reload given a map (`{}` included) compiles again with it; the map
     applies to that compile only, and one naming an attribute the running class lacks, of another type, or one
     the reload does not rename is refused. The map is a map literal as a program writes one: entries apart with
     commas (a last comma allowed), each a key, a colon and a value, at any spacing; a key is
     `<Class>.attributes[<name>]`, the name a symbol (`'level'`) or a text, and a value is a text literal, escapes
     included, holding an attribute's name. Anything else is refused before anything is compiled, naming the entry
     and the form (`'Hero.attributes["level"] "rank"' is not an entry of the map: a key, a colon and a value, as
     Hero.attributes["level"]: "rank"`).
     The moves are named in the answer and on the error output (`moved every Hero to its new attributes: health is
     new and holds its default`).
   - Once a class has moved, its code reads through the function for as long as the program runs.
2. **Dependents are rebuilt**: every function of a `--hot-reload` build has a slot, the standard library's, a
   number's (a function a program adds to `Integer`) and the helpers the compiler writes included, so a change
   whose dependents cannot be swapped alone compiles the whole program and swaps in every function whose C
   changed, keeping the heap. A helper that keeps state of its own (a static variable: a singleton's cache, a list
   of live objects, a foreign function loaded on first use) has no slot, since a new copy would start with its own
   empty state; a change that reaches one is refused by name. The running program's command line, its check point
   flag and its fallback ring are shared with every reload library, so reloaded code reads the arguments the program
   started with. The functions the scheduler turns into waits (`Program.sleep`, `Console.read_line_into` and the
   like) keep their direct calls. A `--hot-reload` build is slower for it: every call of the standard library is
   an indirect call the C compiler cannot inline. It exists to give information while the program runs, and
   speed is measured on release builds.
3. **Enums change**: in a `--hot-reload` build every enum value's number is fixed for as long as the program runs, so
   nothing held has to be re-mapped. The build numbers each enum's values in order and writes the numbers into its C
   (`Mood_calm = 0`); a reload gives every value the running program knows its number again and a new value the
   next free one, so a value moved up the list or added before the others keeps what every object, list, local
   and key already holds. A removed value keeps its number and its name too: something may still hold it, and it
   still prints and compares as itself. A `switch` over an enum with no `_:` halts when it meets a value outside its
   cases (in a `--hot-reload` build, only a removed one can) with `spite: a switch over LiveEnum.Mood met the
   value 'angry', which a reload removed from it: restart the program, or put the value back`, rather than
   running no case. An enum whose values changed compiles the whole program, since the code naming its
   values is compiled again; an attribute of an enum type keeps its value, since a class's
   layout names an enum by its name, not its values. The number an enum value compiles to is a representation
   detail ([values_and_types.md](https://github.com/Spite-Language/spite_documentation/blob/master/values_and_types.md)), so nothing else observes this.
4. **`environment.spite` and `build.spite`**: in a `--hot-reload` build `Environment` and `Build` move to new
   attributes like a class of the program, and a `Build` field that the program or a package declares is read by
   the program's own code from the `Build` singleton while it runs, instead of being folded into it (the standard
   library's own reads of the compiler's options stay folded). A change to either file, or a deleted file,
   compiles the whole program. When a reload swaps in `Environment`'s reading of its settings or `Build`'s values,
   the running singleton reads them again: `Build` takes the values the build gives, and every setting of
   `Environment` is read as when the program starts, from the command line it was started with, then the
   environment, then the declared default, so a setting added while the program runs finds a `--volume=7` given
   at start. A new setting's answer says so: `volume is new and is read like every setting, from the command
   line, the environment or its default`.

All of it lives only in a build with the swap machinery: `--hot-reload`, `--repl` and `--mcp-port` each carry it,
and on this page "a `--hot-reload` build" names any of them. Only `--hot-reload` and `--mcp-port` also carry the file
watcher. A normal build, `--development` included, carries none of it.

The REPL (`--repl`) is a console where a person types Spite into the running program, or into an empty one, and
sees what it answers. It has no commands: every line is Spite. An AI drives a running program through the
language's MCP instead ([mcp.md](mcp.md)). Both compile what they are given into the running program through the
swap machinery of [Live reload in detail](#live-reload-in-detail), so both run on Windows for now; Linux and macOS
are spite_compiler#297.

### `--repl`

- **The flag.** `repl` is a `Build` field (default `false`). It implies the swap machinery of a `--hot-reload` build
  (one slot per function, objects that can move, the reload compile), since every line is compiled and swapped in
  as a reload, and so a REPL build is inspectable ([compiler.md](compiler.md#inspectable-and-production-builds)). It
  does not imply the file watcher: a saved file reaches the session only through `load`
  ([Loading a changed file](#loading-a-changed-file)), so nothing changes under the person typing unless they ask.
  `--repl --hot-reload` adds the watcher, which swaps every save in as it does without a REPL. A `--development`
  build implies neither.
- **What runs.** `spite --repl` in a program's folder starts the program and the console together. The console
  prints `spite> ` and reads lines on a thread of its own from before the entry constructor runs; each line runs on
  the program's thread the next time the program waits or ends a pass of one of its loops
  ([Answered where the program waits](#answered-where-the-program-waits)), and, once the constructor has returned,
  as soon as it is compiled. In a folder with no entry the REPL is a scratchpad: an empty program that loads the
  standard library of the system it runs on and whose constructor returns at once.
- **A line is Spite.** A line that parses as an expression is evaluated and its value printed as `"{value}"` makes
  it text; an expression whose type is `Nothing` prints nothing. Any other line is a statement (an assignment, a
  call, an `if`, a `while`) and prints only what it prints. `program` names the entry instance, and the entry's
  attributes and functions are also named bare (`player.health`, `greet()`), as inside the entry class. A line is
  ordinary code: it makes objects, calls any function, a variadic one included (`console.print("a", 1, true)`), and
  names any class; `World()` is the program's singleton, made by the line if nothing has made it yet, as anywhere
  in code. Reflection answers questions about the program: `Spite.Class.instances`, `Mood.values`,
  `Hero.attributes`, `Hero.functions`, `program.attributes` ([reflection.md](reflection.md)), and
  `Program().live_allocations()` ([memory.md](https://github.com/Spite-Language/spite_documentation/blob/master/memory.md)).
- **What a line declares lives on.** The session is one object of `Spite.Session`, a class the compiler writes into
  a REPL build. `var name = value` at the prompt declares an attribute of it, which every later line names bare; a
  `var` of a name the session already holds declares it again, with the new value and its type, the session moving
  to its new attributes as any class does ([Nothing needs a restart](#nothing-needs-a-restart)). `func` at the
  prompt declares a function of the session, which later lines call by name; declaring it again replaces it. A line
  that opens a block (`{`) without closing it continues on the next, under a `...> ` prompt, until its braces
  balance; then the whole is one line.
- **How a line runs.** The compiler writes the session to `.spite/build/spite/session.spite`: its attributes and
  functions, and the line as the body of `run_line(program: <Entry>): String`, which answers `"{value}"` for an
  expression and `""` for a statement. It compiles that as a reload of the running program, which compiles the
  session class alone unless the line changes something other classes were compiled against, swaps it in and calls
  `run_line` once. A line that does not compile prints the compiler's error, the session goes back to what it was
  before the line, and the program keeps all of its code. A crash in a line ends the program, as every crash does
  ([failure.md](failure.md)). The session file's digest is kept in the manifest, so a line that compiles to what
  the session already holds rebuilds nothing.
- **The console owns the input.** While the REPL is open every line typed is the REPL's: the program's own
  `Console.read_line()` answers `null`, as at the end of input, and never waits for the keyboard.
- **Ending.** The end of input (Ctrl+Z then Enter on Windows, Ctrl+D elsewhere) ends the process as the program's
  end does: the entry instance is dropped, memory is balanced and the exit code is 0. A line that calls
  `program.exit(code)` ends it the same way with that code. **Ctrl+C ends the process at once**, whatever is
  running, a line or the program's own code: nothing is dropped, nothing is waited for, and the exit code is 130.
- **The REPL says nothing of its own.** It prints the prompts, a line's value and the compiler's errors. Nothing
  else it prints is a message the REPL wrote.

### Loading a changed file

- **`load "<path>"` typed at the prompt** compiles the program again with that file as it is on disk now, swaps in
  what changed as any reload does ([Live reload in detail](#live-reload-in-detail)), and prints what the reload
  rebuilt, moved or removed. The path is read as a program's `load` reads it, from the entry's folder.
- **A loaded file keeps its place.** A file the program already loads is updated where it is in the load order, so
  what goes live is what a fresh start would choose ([A reload keeps load order](#live-reload-in-detail)). A file
  the program never loaded is loaded after every other, so its declarations win, as a `load` added as the last
  line of the entry's constructor would. A folder loads each of its files the same way.
- **Only what is named is read again.** Without the watcher, a file changed on disk and not loaded stays as the
  program was compiled from it, and a later `load` of another file still compiles that one as it was. With
  `--hot-reload`, the watcher swaps every save in, and `load` compiles at once.
- **A rename is given to `load`**: `load "hero.spite", {Hero.attributes["level"]: "rank"}`. The map is the one
  step 1 above describes. A load whose change is held, since an attribute is gone while another is new, prints the
  held message, which names the `load` with the map that keeps the values; `load "hero.spite", {}` lets them go.

### Answered where the program waits

- **The console's thread runs no Spite code.** Once a line is read and compiled, it raises the check points' flag,
  wakes the program's wait, and waits until the program's thread has run the line; an MCP connection's thread hands
  over the same way ([mcp.md](mcp.md)). The program's thread runs it the next time the program waits
  (`program.sleep`, `Console.read_line()`, a `File` read or write, a `Tcp` accept or read) or reaches a check point,
  so a line sees the program between two steps, never in the middle of one, and the program's state is never read
  or written by two threads at once ([concurrency.md](https://github.com/Spite-Language/spite_documentation/blob/master/concurrency.md#the-repl-answers-at-the-waits)).
  After the constructor returns, the program waits for nothing but lines. The standard library's own loops have no
  check points, so a line waits for a library call to return.
- **Every loop is a check point** in a `--repl`, `--mcp-port` or `--hot-reload` build: the generator ends every
  pass of every `while` in the program's own code (not the standard library's) with one relaxed load of a flag the
  scheduler raises, which the console's thread, an MCP connection's thread and the file watcher raise when they
  hand something over. Only when it is raised does the pass call the scheduler's check point, which lowers it and,
  on the scheduler's thread, runs a pending line and swaps in a pending reload. On any other thread (the thread
  pool's, a helper) it does nothing. A build without those flags has no check point: its C is byte for byte what
  it would otherwise be. The cost where it exists is one load per pass: 400 000 000 passes took 106 ms, against
  1 566 ms when each pass called `check_point()` ([optimizations.md](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md)).

### Native memory

Inspection is library Spite. Three members of `Spite.Attribute`, supplied by the compiler: `held_memory()`, the
object a reflected value links to (its address and `sizeof` its C struct); `stored_memory()`, the slot an attribute
or element is stored in, from which an attribute's offset is its address less its owner's; and `buffer_memory()`, a
list's item buffer. Each answers `null` where there is none, and only in a REPL or MCP build. **A read never crashes
the inspected program**: the bytes are copied by a function the compiler writes, which asks the operating system
(`ReadProcessMemory` on the program's own process on Windows, a write into a pipe on Linux and macOS, which fails
with `EFAULT` rather than faulting), and a range it cannot copy whole answers `unreadable: 0x10 for 8 bytes`. How a
line reaches a `Spite.Attribute` linked to a live value is still to settle (spite_compiler#296).

### Live reload in detail

A change is seen through the operating system and only what changed is rebuilt, and `--hot-reload` is a flag of its
own. [Live reload](https://github.com/Spite-Language/spite_documentation/blob/master/repl.md#live-reload---hot-reload) teaches it; this section holds its rules.

- **The flag.** `hot_reload` is a `Build` field (default `false`). It **implies** `--development` rather than
  requiring it, since a new version of a class may call a function nothing called before. It is the swap machinery
  and the file watcher together. It works without a REPL: the watcher swaps every save in, and each swap or
  refusal is one line on the program's error output (`spite: rebuilt Monster`). `--repl` carries the machinery
  without the watcher, and a person compiles a change with `load` ([Loading a changed file](#loading-a-changed-file));
  `--mcp-port` implies `--hot-reload`, and the MCP's `reload` compiles at once ([mcp.md](mcp.md)). A build with none
  of the three has no slots, no watcher and no reload code. Every one of them swaps where the program waits and at
  each loop's check point. `spite --hot-reload` runs the program attached to the terminal, like a REPL build.
  `--development` does not imply `--hot-reload`: the machinery costs every call a jump and every object two words,
  and a development build that wants it asks for it.
- **A `--hot-reload` build compiles like an inspectable build.** It keeps every run-time check of one, and it
  inlines no call and evaluates no call while compiling, so every call of a function a reload can replace reaches the
  function's slot, also from a loop that is running when the reload swaps (a loop that called an inlined copy would
  keep the old code for as long as it runs). A reload compiles with exactly the flags the program was built with.
- **Settled: what the MCP's `evaluate` waits for.** The saves are settled once the watcher is not compiling, and
  every file the running code was compiled from has the size and modification time it had when the last reload
  (the watcher's or one asked for) started compiling, and what that compile made is swapped in or refused. The
  waiting is done by the MCP connection's thread, so the program keeps running meanwhile. A file new since the last
  compile is seen only through the watcher's event. It never hangs: after ten minutes the wait ends with `the
  watcher has not swapped in the saved files after 10 minutes: 'reload' compiles them now` ([mcp.md](mcp.md)).
- **A reload waits for the waits of what it swaps.** A program function that waits inside work the compiler
  started ([concurrency.md](concurrency.md#a-wait-in-a-frame-does-not-hold-the-frame)) is a state machine in a
  `--hot-reload` build too, so such work overlaps as in any other build. A reload that would swap a function while
  such work is stopped at a wait inside it (the function's own wait, or a call below it that has not returned) is
  held until no such work is stopped inside any function it swaps; the program runs on
  meanwhile, and the reload is swapped in at the first check point after that. So a wait always finishes on the
  code that started it, and the code after it is the new code the next time the function is called. A reload held
  for more than 10 seconds says once which function it waits for: `spite: the reload waits for
  'Server.accept_next', where started work is stopped at a wait, and is swapped in once it returns`. A `load` at the
  REPL and the MCP's `evaluate` and `reload` answer once it is swapped in, as for any reload. Each such function's first frame (its machine's
  entry, `<name>___begin`) goes through a slot like every function, and each frame counts itself in a table kept
  by name while it lives, only in a `--hot-reload` build.
- **The swap mechanism: one slot per function.** In a `--hot-reload` build every function (the program's own, the
  standard library's, and every one the compiler writes) starts with a jump through its slot, eight bytes of the
  executable's data that start holding the address of the function's body. Every call site, function value, thread
  entry and session line still names the function's start, so all of them follow a swap, and a reload writes the new
  body's address into the slot. Functions are aligned to 32 bytes in such a build, so a function's code does not
  depend on where it lands. Any other build is unchanged: direct calls and tree shaking. The cost is one indirect
  jump per call.
- **What the build records.** Beside the executable, `<program>.reload_host` lists every function the executable
  defines with its start, its slot, the class it belongs to and a digest of its machine code (each reference that
  leaves the function counted as what it names: the function called, the state reached by name, the system function
  imported, the text a constant holds); every piece of state the code reaches, by the name the compiler gives it
  where it makes it (a singleton's record, a static object, the counts, the scheduler's state), with its address;
  every system function the executable imports, with the address of its entry; a digest of the routines every
  function shares; every class's id and layout; every enum's values; and a digest of each of the program's files.
  The executable holds the path of the compiler that built it, the folder it was started in and its flags, so a
  reload compiles with the same `Build` values the running program has.
- **Rebuilding: `spite <folder> <flags> --executable-path=<program> --reload-at=<address>`.** A reload runs that
  compiler with those flags, and it prints its answer and its errors to standard output, where the running program
  reads them. A compile given `--reload-at` that is not a `--hot-reload` build, by the command line or by a
  `hot_reload = true` in the program's `build.spite`, is refused: `'--reload-at' is given by a running --hot-reload
  program when it compiles a change, with the flags it was built with: build with --hot-reload, and the program
  reloads itself`. It reads the manifest and compares the program's files with it before lowering anything: nothing
  changed answers `unchanged`. Otherwise it compiles the program and compares each function's digest with the
  manifest's: every function whose digest changed is rebuilt and swapped through its slot, every function the
  running program lacks is added with a slot of its own, and every other function keeps the code the program runs.
  The class ids are seeded from the manifest by the class's name, so every class keeps the id its instances carry,
  and a new class takes the next one after the largest. It writes the rebuilt functions into `<program>.reload_<n>`,
  machine code laid out at the address the program gave, with every reference resolved against the running program:
  a call to a function the program has goes to its start, state the program holds is the program's, and the
  constants and new state the new code needs are copied in. It prints `rebuilt A, B` (the classes of the rebuilt
  functions), `removed A.f` when a function the running program has is gone, `library <path>` and `manifest
  <path>`, the next manifest, written as `<program>.reload_pending`. A reload is refused, and the program keeps all
  of its code, when a class the running program describes has another layout (the answer names the attributes that
  are new or gone), when an enum's values change other than by new ones added last, when the routines every
  function shares are compiled differently, when the new code calls a system function the program never imported,
  and when it reaches state the compiler did not name. A reload whose compile changes no code the program runs
  answers `no code the program runs changed, so nothing was rebuilt`.
- **Swapping at a drain point.** The first reload reserves room for new code near the program (256 MB at the
  image's base plus 1 GB on Windows, so every reference reaches it in 32 bits); each reload commits the next part of
  it, copies the new code in, flushes the instruction cache, writes the slots of the rebuilt functions, and makes
  the manifest it was given the running one. **The compile runs on a helper thread**: the watcher's thread compiles
  a change it sees, and the console's thread or an MCP connection's thread compiles a line, a `load` or a `reload`
  before it hands it over, one compile at a time; the finished code waits until the program's first wait or check
  point after it, where the program swaps it in, so the program never waits for the compiler. A `load` or a
  `reload` answers what its own compile produced (swapped in by then).
  While the swap runs, the scheduler is paused and a wait blocks where it is, so nothing else runs until it is done.
  New code is never unloaded: values it made, such as its text literals, may still be referenced.
- **What reloads.** A function body (the next call runs it; a call already running finishes the old one). A new
  function or a new class (the new code calls it). **Reflection follows the reload**: each program class's list of
  functions, `<Class>___functions`, has a slot like a function, and a rebuilt
  class's library swaps in its new list, so `Monster.functions` lists a function a reload added, and a line at the
  REPL or an MCP `evaluate` calls it (a reload that adds `growl()` to `monster.spite` lets `monster.growl()` be
  called). The cost is one slot
  per class, only in a `--hot-reload` build. A changed parameter list or return type (a new function: the rebuilt
  class and its callers are rebuilt). A deleted function keeps its last code for whatever still holds it, a
  function value or the session, and the answer names it. An attribute's default value (through the `_init` slot, for
  instances made afterwards).
- **A reload keeps load order.** Every reload, fast or whole, compiles a class from all the files that declare or
  reopen it, in load order, so the declaration that is live after it is the one a fresh start would choose: the
  last loaded, except a `Build` field the program's own `build.spite` declares, which stays the program's. Editing a
  declaration a later load replaces changes nothing; deleting the replacement swaps the earlier one back in. A
  reload whose compile changes no code the program runs answers `no code the program runs changed, so nothing was
  rebuilt`.
- **What a reload refuses.** A file that does not compile is refused, with the compiler's error, and
  the program keeps all of its code, so a save caught half-written is harmless.
- **Watching.** `HotReload` watches through the standard library's
  `FileSystemWatcher` ([System classes](standard_library.md#system-classes)), the one watcher any program uses:
  `ReadDirectoryChangesW` with overlapped I/O from `kernel32.dll` on Windows, `inotify` and `poll` from `libc.so.6`
  on Linux, and `kqueue`/`kevent` on each folder and file from `libSystem.dylib` on macOS, with no polling. On a
  thread of its own, it calls `wait_for_changes()`, which returns once 100 ms pass without a change, then compiles
  the change and wakes the scheduler. The program's own folder is watched with every folder below it, and so is
  **every folder the program loads**: the compiler writes their list into the build, `HotReload.loaded_folders()`.
- **Crashes and work on other threads.** A crash inside reloaded code reports the library's own fallback trace.
  Work on the thread pool running a function whose slot is re-pointed finishes the old code.

---

Next: [The language's MCP](mcp.md).
