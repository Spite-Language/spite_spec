# REPL and live reload

The specification of [REPL and live reload](https://github.com/Spite-Language/spite_documentation/blob/master/repl.md): every rule of this part of the language, with
its edge cases and its exact error texts. The page teaches; this one decides, and where they disagree, the page
is the bug.

## Nothing needs a restart

No change to a program's code needs a restart. Every refusal in [What a reload can change](https://github.com/Spite-Language/spite_documentation/blob/master/repl.md#what-a-reload-can-change)
is a gap to close, not a rule. The steps, in the order a game meets them:
1. **A class's attributes change**, [Changing a class's attributes](https://github.com/Spite-Language/spite_documentation/blob/master/repl.md#changing-a-classs-attributes): the reload moves
   every live object to the new layout, copying kept attributes by name, giving new ones their defaults and
   releasing removed ones. A rename is given at the prompt, never in the
   program's code: `reload {Hero.attributes["level"]: "rank"}`. The rules:
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
     the map that would keep the values. `reload` with a map (`{}` included) compiles again with it; the map
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
   values and the REPL's tables are compiled again; an attribute of an enum type keeps its value, since a class's
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

All of it lives only in a `--hot-reload` build: a normal build carries none of it.

A REPL that inspects and drives the *running* program, local (`--repl`) and remote (`--repl-port`), and live
reload (`--hot-reload`). The REPL is Spite the compiler adds to a build that asks for it (the class `Spite.Prompt`), walking the program through
reflection; nothing of it is in a build that did not ask for it. It answers paths, singletons by name, assignment of
a literal, calls with literal arguments and paths through what they answer, `attributes`, `functions`, `classes`,
`describe`, `enums`, `memory`, `help`, `exit`, `reload`, `last_reload` and `wait_reload`, assignment into elements,
`T?`s and of whole instances, and walking into a union. `--repl-port` runs it over `spite connect` and `Tcp`,
answered where the program waits and at each loop's check point. Live reload runs on Windows. Code typed at the
prompt is compiled and run by `eval` and `run`.

### Reflection in a REPL build

The emitted `main` hands the loop the entry instance as a `Spite.Attribute` named `program` before every command. In
a `--repl` or `--repl-port` build a `Spite.Attribute` stays linked to the live value it describes, through four
functions the compiler supplies: `value_attributes()` and `value_functions()` read the value's own attributes and
functions (a list's attributes are its elements, named `0`, `1`, ...; a dictionary's are its entries, named by their
keys), `assign(text)` writes a number, `Boolean`, text or enum value into it (through the class's
`set_<attribute>(value)` when it declares one) and answers whether it could, and a `Spite.Function`'s
`call_with_text(arguments)` calls it with literal arguments and answers its result as a `Spite.Attribute?`, `null`
when an argument is not one the loop can write. Outside those builds they answer an empty list, `false` and `null`,
and the reflection they walk is not emitted. A function is callable from the prompt when every parameter is a
number, `Boolean`, text or an enum; an instantiated template function (`set_age`) is one like any other, and a
template nothing calls is still never instantiated, so tree shaking of templates is unaffected. A REPL build is
inspectable ([compiler.md](compiler.md#inspectable-and-production-builds)): nothing else is tree-shaken either.

### Command language

A small subset of Spite expressions, rooted at the entry instance under the fixed name `program` (regardless of
the entry class's own Spite name):

- **paths**: `program`, `program.monsters`, `program.monsters[0].health`, `program.settings.volume`,
  `program.target`. A path may leave out the leading `program.`, so `player.health` and `program.player.health`
  name the same value. Walking through a non-null `T?` is transparent; a dictionary's entries are walked by key.
- **calls**: `program.monsters.count()`, `program.monsters[0].roar()`, `program.player.set_age(3)` (any
  callable function, including an instantiated template one, and the public functions of a standard library
  class a path reaches, `label.upper_case()` or `program.console.read_line()`, `tests/interactive_library_calls`); `List<T>`'s and `Dictionary<Key, Value>`'s `count()`,
  and `Dictionary<Key, Value>`'s `keys()` (the keys, one a line, or `scores holds no keys`) and `has(key)`, each of which
  must end the expression. A call that answers a value is walked on like any path,
  `World().position_of(1).across`, and it runs once; a call that answers `Nothing` ends the expression, and a
  path after one is refused after it ran: `World().spawn() ran, and it answers nothing, so nothing can follow it`.
- **singletons by name**. A call whose name starts with a capital letter, alone or after a namespace (`World()`,
  `Ui.Panel()`, `Column<Position>()`), names a singleton class and binds the one instance the program holds, as the
  program's own `var world = World()` does; a path, call or assignment continues from it
  (`Tick().step_milliseconds = 50`). It never makes an instance: a singleton the program has not asked for yet is
  refused, `Tick has not been made yet: nothing in the program has asked for it, and the prompt never makes one`, and
  so is a class that is not a singleton (`'Position' is a class, not a singleton: the prompt never makes an object,
  it reads the ones the program holds, and 'classes' lists the singletons`) and a name no singleton has (`no
  singleton 'Wrld' in the program: 'classes' lists them`). A singleton made with arguments is named with them,
  literals as the program writes them, apart with commas: `Channel(1)` binds the instance the program's
  `Channel(1)` made, and an argument list the program never asks for has none (`the program holds no Channel(3): a
  singleton made with arguments has one instance for each argument list the program asks for, Channel(1),
  Channel(2)`), nor does the name without arguments; arguments given to one made without them are refused (`'World'
  is a singleton made without arguments, so it takes none: World()`). `classes` lists each such instance by its
  arguments (`Channel(1)`). The whole name is the class's qualified name with its generic arguments; the name without its namespace is
  accepted when one singleton has it, and a name two singletons share is refused naming both. A generic class that is
  not a singleton, such as `Lookup<Position>()`, is refused like any class.
- **How the loop finds them.** In a REPL build the compiler writes two functions of `ReadEvaluatePrintLoop`:
  `singletons()`, a `Spite.Attribute` for each singleton the program binds, named by its qualified name and linked
  to the instance when it has been made (read from the singleton's static slot with an acquiring load, never through
  its constructor), without a value when it has not; and `class_names()`, the program's own classes that are not
  singletons. Of the standard library's singletons only `Environment` and `Build` are listed: the rest (`Console`,
  and the rest) are the language's own machinery, and describing them would drag most of the
  library's classes into the build's reflection, which a reload that compiles only the changed classes would then
  have to reproduce. Any other build writes them as an empty list and empty text, and nothing calls them, so they
  are shaken out with the rest of the loop: a normal build carries none of it.
- **Breakpoints.** `break <file>:<line>`, `breaks`, `clear` and `clear <file>:<line>` answer in a program built with
  `--hot-reload` and a REPL (`--repl` or `--repl-port`), and everywhere else with `'break ...' answers in a program
  built with --hot-reload and --repl-port: ...`. The file is a path of the program ending in what is written
  (`ticker.spite`, `game/ticker.spite`); none such file is refused (`no file of the program ends in 'nowhere.spite':
  a breakpoint names one of the program's own files, as in 'break hero.spite:12'`), two are refused naming them
  (`'ticker.spite' names more than one file of the program (...): write more of the path`), a site without a line is
  `'ticker.spite' names no line of a file: a breakpoint is set as 'break hero.spite:12'`, and a line no statement of
  a block starts on is `no statement starts on 'ticker.spite:3': a breakpoint stops before a statement, so name the
  line a statement starts on`; each refusal is a reload's, `the reload was refused, and the program keeps all of its
  code: ...`, and the breakpoints stay as they were. `break` answers `the program stops before the statement on
  ticker.spite:16`, a site already set `'ticker.spite:16' already holds a breakpoint: 'breaks' lists them`; `breaks`
  answers the sites a line each, or `no breakpoint is set: 'break hero.spite:12' sets one`; `clear` answers `every
  breakpoint is cleared`, `clear <site>` answers `the breakpoint on <site> is cleared`, or `no breakpoint is set on
  '<site>': 'breaks' lists them`. How it is built: the running program writes its breakpoints beside its executable
  (`<program>.reload_breaks`, a site a line, removed when the program starts) and runs a reload, whose answer it
  keeps to itself; the compiler reads the file in a reload compile and, before the statement starting on each line,
  writes `var spite_breakpoint_on_line_<line> = Spite.Breakpoint()` and `spite_breakpoint_on_line_<line>.pause_at(
  "<site>")`, both on that line, after the file's rules are checked. `Spite.Breakpoint` is a class the compiler
  writes into a `--hot-reload` REPL build; its `pause_at` hands the site to `ReadEvaluatePrintLoop.pause_at`, so a
  build carries nothing for breakpoints but that class until one is set, and only the functions of the files holding
  one change. `pause_at` waits until `continue`, answering the prompt meanwhile: the console REPL prints `stopped at
  <site>` and reads commands; over `--repl-port`, a stop inside a command the REPL is answering (a call that reaches
  the breakpoint) answers that command `stopped at <site>` at once, and the command's own answer is the answer to
  `continue`; a stop the program's own code reaches answers `continue` with `continued from <site>`, as the console
  prints it. While stopped, every command answers as usual, `where` answers the site, a breakpoint another thread reaches
  waits its turn (that thread stops there once `continue` lets the first one go, and the program's thread answers
  no command at its check points or waits meanwhile, since the stop answers them), a breakpoint reached again by a
  call made at the stop is passed with `spite: the program passed the breakpoint at '<site>'
  while it is stopped at '<other>', so it did not stop there` on the error output, and a save the watcher finds is
  reloaded before the next command the stop reads (on the program's thread; a stop on another thread reloads only
  once it goes on). A line a `switch` case's statement starts on holds a breakpoint like any other; one reached before the REPL has
  the program is passed the same way (`... before the REPL had started ...`). `where`, `locals` and `continue`
  answer `the program is not stopped at a breakpoint: 'break hero.spite:12' sets one` when it is not. `locals` lists
  `self` and every local and parameter in scope at the stop, a line each, `<name>: <type> = <value>`, the value
  written as the loop writes a nested value (a number, a `Boolean`, an enum value or a text in full, `null`, a
  collection as `List<Integer>(3)`, an object as `Hero {...}`) and `self` as `self: Counter = Counter { steps: 0,
  history: List<Integer>(1) }`; the compiler writes those lines before the breakpoint's call, from the types it
  knows there, so a stop costs nothing more than its lines (`tests/hot_reload_lists_locals`). Only the program's own classes
  can hold a breakpoint, since only their code is swapped.
- **`bytes`: native memory.** `bytes <path>` answers one JSON object: for a value the path holds an object of (an
  instance, a list, a dictionary, a singleton), `class`, `address` (hexadecimal text), `bytes` (the object's size),
  `hex` (its bytes, at most 4096 shown) or `"unreadable":true`, then for an instance `fields`, each with `name`,
  `class`, `offset` from the object's start, `bytes` and `value` (printed as the loop prints a nested value), and
  for a list `buffer` with the item buffer's `address`, `bytes`, `shown` and `hex`; for a number, `Boolean`, enum or
  text held by an attribute or an element, `class`, `address`, `bytes`, `hex` and `value` of its own slot. A value
  reached only through what a call answers, with no slot of its own, is refused naming what to ask instead.
  `bytes 0x1f2a40 64` reads a range: an address in hexadecimal and 1 to 4096 bytes, anything else refused with the
  form. **A read never crashes the inspected program**: the bytes are copied by a function the compiler writes, which asks
  the operating system (`ReadProcessMemory` on the program's own process on Windows, a write into a pipe on Linux
  and macOS, which fails with `EFAULT` rather than faulting) and a range it cannot copy whole answers
  `unreadable: 0x10 for 8 bytes`. Reads only.
- **What `bytes` is built from.** Inspection is library Spite, and the REPL is one caller of it. Three members of
  `Spite.Attribute`, supplied by the compiler: `held_memory()`, the object a reflected value links to (its address
  and `sizeof` its C struct); `stored_memory()`, the slot an attribute or element is stored in, from which an
  attribute's offset is its address less its owner's; and `buffer_memory()`, a list's item buffer. Each answers
  `null` where there is none, and they answer only for a `Spite.Attribute` linked to a live value, which is a REPL
  build's (outside one every `Spite.Attribute` answers `null`). The copying is done by the compiler's own
  code for the system. A program that never inspects carries none of it; the attribute's five hidden
  numbers are set only where a REPL build links it.
- **Generic functions beyond the instances the program holds.** The prompt calls what the build compiled: a generic
  class's functions on an instance some path reaches, or on a generic singleton (`Column<Position>().values[0]`). An
  expression the build never compiled, `Lookup<Position>().of(12)`, which makes a `Lookup`, needs code the program
  lacks, which `eval` supplies. In a `--hot-reload` build the reload machinery already compiles the whole program and
  writes a library of what changed; `eval` hands it the typed expression as the body of a function of a class of its
  own, compiles that library, loads it and calls the function once, answering the value made text.
  The program must allow the construction it asks for, so that path is refused for a singleton's constructor exactly
  as above.
- **assignment** of a number, `Boolean`, text or enum literal: `program.player.age = 5`,
  `program.monsters[1].name = "rat"`, `program.player.job = 'knight'`, through `set_<attribute>` when the class
  declares one, the same rule ordinary compiled Spite code follows. It answers the value read back afterwards,
  so a `set_<attribute>` that refused it shows the old one. A text literal, as an assigned value, a call's argument
  or a dictionary's key, is read as a program reads one, with the same escapes (`\n`, `\t`, `\r`, `\\`, `\"`, `\{`,
  and any other character after a backslash is that character); a hole is refused, since the prompt reads literals
  (`"the {hour} show" has a hole, and the prompt reads a text literal as it is written: \{ writes a brace, and
  'eval' makes text from values`).
- `attributes` (the entry instance's attributes, `name: Class = value` a line), `functions` (its functions,
  `name(argument: Class, ...): Returns` a line), both also of any path (`functions World()`, `attributes
  Tick()`); an empty list is never an empty answer: `program is a ReplWorld, which has no function the
  prompt can call: 'classes' lists the singletons, whose functions the prompt can call, such as
  World().entity_count()`, and `... which has no attributes`. A function whose name starts with `_` is private to
  its class and `drop()` runs only when a value is released, so the prompt neither lists nor calls either:
  `'_swap' is private to values, and the prompt calls what code outside the class may call`. `classes` answers the
  singletons the prompt can bind, `World()` a line with `not made yet` after one the program has not made, then the
  program's other classes a line each. Then `help`, `exit`, and `reload`, `last_reload` and `wait_reload`
  ([Live reload in detail](#live-reload-in-detail)).
- Printing a class instance shows `ClassName { attribute: value, ... }` one level deep: a nested class attribute
  prints as `ClassName {...}`; a list or dictionary prints as `List<T>(count)`/`Dictionary<Key, Value>(count)` regardless
  of depth; a `T?` prints `null` or its held value at the *same* depth; a union prints its active member through
  its `to_string()` (`Circle { radius: 2 }`) at any depth; text prints without quotes (`name: hero`); a call that
  returns `Nothing` prints nothing.
- Every error is one line and never a crash: an unknown attribute names the ones that do exist (`no attribute
  'monstrs' in program: console, player_name, player_age, monsters`), an index out of range says so (`index 5 is
  out of range: names holds 2`), a literal of the wrong kind is a type mismatch (`type mismatch: target.health is
  an Integer, and "x" is not one`), calling an attribute is `target.name is not callable`, and an unknown function
  is `no function 'nope' in program: 'functions' lists them`.
- **`eval` and `run`** answer in a program built with `--hot-reload` and a REPL (`--repl` or `--repl-port`), and
  everywhere else with `'eval ...' answers in a program built with --hot-reload and --repl-port: ...`. The running
  program writes the code beside its executable (`<program>.reload_prompt`: `eval` or `run` on its first line, the
  code on the next) and runs a reload, whose answer it keeps to itself. In a reload compile the compiler writes the
  class `Spite.PromptCode` from that file: one function, `answer(program: <Entry>): String`, where `program` names
  the entry instance (`_program` when the code does not mention it), whose body for `eval <expression>` is `var
  value = <expression>` and `return "{value}"`, and for `run <statement>` is the statement and `return ""`. A build
  writes it with the body `return ""`, so the REPL calls the same function before and after: a reload swaps the new
  body in, and the REPL calls it once; `eval` answers its text and `run` answers nothing. Code that does not compile
  answers the compiler's error, the program keeps its code, and the file goes back to the code compiled last, so a
  later reload does not fail on it; the code stays loaded until the next `eval` or `run`. The class is written to
  `.spite/build/spite/prompt_code.spite` and its digest kept in the manifest, so a reload whose code is unchanged
  rebuilds nothing. A program that starts removes the file a run before it left.
- **`describe <class>`** answers the class's qualified name (`, a singleton` after one), its attributes
  (`name: Class` a line; a private `_` one is left out) and its public functions with their signatures
  (`grow(by: Integer): Integer`), or `none`. The name is the qualified name, or the name without its namespace when
  one class has it; two classes sharing it are refused naming both, and a name no class has is `no class 'Nope' in
  the program: 'classes' lists them`. It covers the classes `classes` lists (the program's and every loaded
  package's, not the standard library's) from `class_descriptions()`, text the compiler writes into a REPL build
  like `class_names()`, so describing a class runs nothing.
- **`enums`** answers each of the program's enums, `Owner.Name: 'value', ...` a line, from `enum_names()`, which the
  compiler writes into a REPL build like `class_names()`; a program with none answers `the program declares no enum`.
- **`memory`** answers `<n> live allocations holding <b> bytes`, from `Program().live_allocations()` and
  `live_bytes()` ([memory.md](https://github.com/Spite-Language/spite_documentation/blob/master/memory.md)), which any program can call.
- **Walking into a union**: a union value is walked as its active member, `shape.radius`, and prints with the
  member's class name. The reflected attribute keeps the union as its `.class`; `held_class()` answers the class of
  the object it holds when that differs (a union's active member), and `null` otherwise.
- **Assignment**: `null` goes into a `T?` (`nickname = null`) and is refused elsewhere (`mood is a Mood, which is
  never null: only a T? holds null`); a literal goes into a list or dictionary element through the element's own
  slot; and a right side that is not a literal is a path, whose instance an attribute or element of a class (or of
  its `T?`) then holds, through `set_<attribute>` when the class declares one, and only when the classes match:
  `follower is a Circle?, and spare is a Square: the loop assigns an instance of the attribute's own class`. A right
  side that is one construction of a class `classes` lists that is not a singleton (`follower = Circle(3)`,
  `Ui.Panel(2)`, `Lookup<Position>()`) makes its object: in a program built with `--hot-reload` and a REPL (`--repl` or `--repl-port`)
  it is compiled as `run`'s code is, the statement `<target> = <construction>` (the target with `program.` in front
  when it starts at neither `program` nor a singleton), which the REPL runs once and then answers the target read
  back, as an assignment does; code that does not compile (a class that does not match) answers the compiler's
  error, and the program keeps its code; elsewhere it is
  refused (`'follower = Circle(3)' makes an object, which answers in a program built with --hot-reload and
  --repl-port: ...`). The
  compiler-supplied members are `Spite.Attribute`'s `assign_null()` and `assign_attribute(source)`, beside
  `assign(text)`; each answers whether it could.

### `--repl`

Runs the entry constructor normally; when it returns, instead of dropping the entry instance and
exiting, reads commands from stdin with a `spite> ` prompt until `exit` or end of input, then drops and
exits normally (memory balanced).

### `--repl-port=<port>`

`repl_port` is a `Build` field ([Build settings: `Build`](programs.md#build-settings-build)), so the
port is part of the build: there is no run-time override, and changing the port means rebuilding. Before the entry
constructor runs, the program starts a background thread with a TCP server bound to `127.0.0.1:<port>` **only**.
It never listens on any other interface, and there is **no authentication**: anyone who can reach that port on
that machine can read and mutate the running program. This is a local debugging tool, not something to expose
past `127.0.0.1`.

The line protocol is designed for an AI client: each request is one line of the command language; each
response is one line of JSON, `{"ok":true,"value":"...","type":"Integer"}` on success or
`{"ok":false,"error":"..."}` on failure (values and errors are JSON-escaped strings; a multi-line value, such as
the answer to `attributes`, uses `\n` inside that one JSON string, never a real newline in the wire bytes).
Clients connect and disconnect one at a time while the program keeps running; when the entry constructor returns,
the process stays alive serving the REPL until a client sends `exit`.

The design:

- **Spite, over `Tcp`.** The server is `ReadEvaluatePrintLoop.serve`, written by the compiler
  over `Tcp.Server` and `Tcp.Connection` ([System classes](standard_library.md#system-classes)), whose operating-system
  members each system's folder of the standard library supplies (`ws2_32.dll` on Windows, `libc.so.6` on Linux,
  `libSystem.dylib` on macOS). The REPL listens with
  `listen_locally`, which binds `127.0.0.1` and nothing else, so "loopback only" holds by construction.
  `ws2_32.dll` is opened when the first `Tcp.Server` is made, so a program without a REPL never loads it.
- **The thread.** The one piece of C this adds is written by the compiler, only in a `--repl-port` build: a
  two-line thread entry, `spite_remote_loop_thread`, that calls `ReadEvaluatePrintLoop.serve` with the entry
  instance. Spite starts it and waits for it through the operating system's folder (`CreateThread` and
  `WaitForSingleObject` from `kernel32.dll`; `pthread_create` and `pthread_join` elsewhere). No `Thread` class is
  added to the library: [concurrency.md](https://github.com/Spite-Language/spite_documentation/blob/master/concurrency.md) says how a program is concurrent, and this thread belongs
  to the REPL.
- **Answered where the program waits.** The socket thread runs no Spite code: it waits on the socket the REPL
  reads (the listening socket until a client connects, then the client's) and, the moment a command can be read,
  raises the check points' flag and wakes the program's wait, then waits until the program's thread has taken it
  before it watches the socket again; the program's thread reads the command, answers it and writes the answer, so
  no command waits for a timer ([concurrency.md](https://github.com/Spite-Language/spite_documentation/blob/master/concurrency.md#the-repl-answers-at-the-waits)). The
  program's thread answers it the next time the program waits (`program.sleep`,
  `Console.read_line()`, a `File` read or write, a `Tcp` accept or read), so a command sees the program between two steps, never in the middle of one, and
  the program's state is never read or written by two threads at once. After the constructor returns, the program
  waits for nothing but commands. A program that never waits is answered at its loops' check points (below);
  [concurrency.md](https://github.com/Spite-Language/spite_documentation/blob/master/concurrency.md#the-repl-answers-at-the-waits) shows a frame loop served between frames and a
  busy loop served between passes. The standard library's own loops have no check points, so a command waits for a
  library call to return.
- **Every loop is a check point in a REPL build.** In a `--repl-port` or `--hot-reload` build, the generator ends
  every pass of every `while` in the program's own code (not the standard library's) with a check point: one
  relaxed load of a flag that the scheduler raises, which the REPL's thread and the file watcher call when
  they hand something over. Only when it is raised does the pass call the scheduler's check point, which lowers it
  and, on the scheduler's thread, answers a pending command (the handover flag `answer_pending` already reads) and
  runs a pending reload. On any other thread (the thread pool's, a helper) it does nothing. A loop that never waits is
  answered between two passes. A build without those flags has no check point: its C is byte for byte what it
  would otherwise be. `--repl` alone answers after the constructor returns, from the console, so it needs none. The
  cost where it exists is one load per pass: 400 000 000 passes took 106 ms, against 1 566 ms when each pass called
  `check_point()` ([optimizations.md](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md)).
- **The port is part of the build**, like any flag the compiler folds: `main` listens on it, on the main
  thread, before the constructor runs, so a client that connects any time after the program starts is served. A
  port another program holds stops the program before its constructor with `error: the REPL could not listen on
  127.0.0.1:<port>` and exit code 1.
- **The answer is shared with `--repl`.** `answer(command, program)` returns a `ReadEvaluatePrintLoop.Answer`
  (`succeeded`, `text`, `class_name`); the console loop prints `text`, and the socket loop writes it as JSON:
  `"type"` is the class of the value, empty for `help`, `attributes` and `functions`, and `Nothing` for a call
  that returns nothing. Every message the console loop prints as a complaint is `"ok":false` on the wire.
  JSON escapes are `\"`, `\\`, `\n`, `\r`, `\t` and `\u00XX` for any other control character.
- **`exit`** answers `{"ok":true,"value":"","type":""}`, closes the connection and calls `program.exit(0)`,
  which flushes what the program printed. It is handed over like any command, and the program ends at the next
  wait or pass of a loop it reaches after the one that answered it, so what the command before `exit` changed
  still runs (a loop told to stop prints its last line): it ends at once only when nothing would wake the wait it
  answered at (no sleep or helper thread running), or when the constructor has already returned. When the constructor returns first, `main` serves commands until then. With `--repl` too, the console
  loop runs first, and after its `exit` the process keeps serving.
- `spite program --repl-port=4000` is the form, as for every `Build` field; a port outside 1 to 65535 is
  `error: --repl-port takes a port number from 1 to 65535: spite program --repl-port=4000`.

### `spite connect <port>`

A tiny client built into the compiler binary itself, over the same `Tcp` classes (a `Tcp.Client`): with no `--command`, an
interactive prompt that sends each typed line and pretty-prints the JSON response (`value (type)`, just `value`
when the type is empty or `Nothing`, or `error: message`); with `--command="..."`, sends that one command, prints
the raw JSON response line, and exits, which is the form tests and AI clients use. Nothing listening on the port
is `error: nothing is listening on 127.0.0.1:<port>` and exit code 1.

### Live reload in detail

A change is seen through the operating system and only what changed is rebuilt, and `--hot-reload` is a flag of its
own. [Live reload](https://github.com/Spite-Language/spite_documentation/blob/master/repl.md#live-reload---hot-reload) teaches it; this section holds its rules.

- **The flag.** `hot_reload` is a `Build` field (default `false`). It **implies** `--development` rather than
  requiring it, since a new version of a class may call a function nothing called before. It works without a REPL:
  the watcher still swaps, and each swap or refusal is one line on the program's error output (`spite: rebuilt
  Monster`); an explicit rebuild needs `--repl` or `--repl-port`, whose `reload` swaps in what changed and answers
  what it rebuilt, whose `last_reload` answers what the last swap (the watcher's included) did, and whose
  `wait_reload` answers the same once the watcher is done (below). A build without
  it has no slots, no watcher and no reload code: `reload` answers `'reload' answers in a program built with
  --hot-reload, which swaps its code while it runs, and this one was not`, from a branch folded on the `Build`
  constant. As with `--repl-port`, a `--hot-reload` build swaps where the program waits and at each loop's check
  point. `spite program --hot-reload` runs the program attached to the terminal, like a REPL build.
- **A `--hot-reload` build compiles like an inspectable build.** It keeps every run-time check of one, and it
  inlines no call and evaluates no call while compiling, so every call of a function a reload can replace reaches the
  function's slot, also from a loop that is running when the reload swaps (a loop that called an inlined copy would
  keep the old code for as long as it runs). A reload compiles with exactly the flags the program was built with.
- **`wait_reload` waits for the watcher.** It answers once the watcher is not compiling, and every file the
  running code was compiled from has the size and modification time it had when the last reload (the watcher's or
  `reload`'s) started compiling; then it swaps in what that compile made, if nothing has yet, and answers what
  `last_reload` would. Over `--repl-port` the waiting is done by the connection's thread, so the program keeps
  running meanwhile. A file new since the last compile is seen only through the watcher's event. It never hangs:
  after ten minutes it answers `the watcher has not swapped in the saved files after 10 minutes: 'reload'
  compiles them now`.
- **A reload waits for the waits of what it swaps.** A program function that waits inside work the compiler
  started ([concurrency.md](concurrency.md#a-wait-in-a-frame-does-not-hold-the-frame)) is a state machine in a
  `--hot-reload` build too, so such work overlaps as in any other build. A reload that would swap a function while
  such work is stopped at a wait inside it (the function's own wait, or a call below it that has not returned) is
  held until no such work is stopped inside any function it swaps; the program runs on
  meanwhile, and the reload is swapped in at the first check point after that. So a wait always finishes on the
  code that started it, and the code after it is the new code the next time the function is called. A reload held
  for more than 10 seconds says once which function it waits for: `spite: the reload waits for
  'Server.accept_next', where started work is stopped at a wait, and is swapped in once it returns`. `reload` and
  `wait_reload` answer once it is swapped in, as for any reload. Each such function's first frame (its machine's
  entry, `<name>___begin`) goes through a slot like every function, and each frame counts itself in a table kept
  by name while it lives, only in a `--hot-reload` build.
- **The swap mechanism: one slot per function.** In a `--hot-reload` build every function (the program's own, the
  standard library's, and every one the compiler writes) starts with a jump through its slot, eight bytes of the
  executable's data that start holding the address of the function's body. Every call site, function value, thread
  entry and REPL thunk still names the function's start, so all of them follow a swap, and a reload writes the new
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
  a change it sees, and the REPL's thread compiles before it hands `reload` over, one compile at a time; the
  finished code waits until the program's first wait or check point after it, where the program swaps it in, so
  the program never waits for the compiler. `reload` answers what its own compile produced (swapped in by then).
  While the swap runs, the scheduler is paused and a wait blocks where it is, so nothing else runs until it is done.
  New code is never unloaded: values it made, such as its text literals, may still be referenced.
- **What reloads.** A function body (the next call runs it; a call already running finishes the old one). A new
  function or a new class (the new code calls it). **Reflection follows the reload**: each program class's list of
  functions, `<Class>___functions`, has a slot like a function, and a rebuilt
  class's library swaps in its new list, so the REPL's `functions` lists a function a reload added and calls it at
  the prompt (a reload that adds `growl()` to `monster.spite` lets `monster.growl()` be called). The cost is one slot
  per class, only in a `--hot-reload` build. A changed parameter list or return type (a new function: the rebuilt
  class and its callers are rebuilt). A deleted function keeps its last code for whatever still holds it, a
  function value or the REPL, and the answer names it. An attribute's default value (through the `_init` slot, for
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

Next: [Testing](testing.md).
