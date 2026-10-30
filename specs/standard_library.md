# Standard library

The specification of [Standard library](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md): every rule of this part of the language, with
its edge cases and its exact error texts. The page teaches; this one decides, and where they disagree, the page
is the bug.

## Standard library

Every value below is reference counted, exactly like a user class: a `String`/`List<T>`/`Dictionary<Key, Value>` is freed
the moment its last reference goes, not by a single-owner convention. See [Memory](https://github.com/Spite-Language/spite_documentation/blob/master/memory.md#memory) for the
retain/release rules and the cycle caveat.

**What no company owns is standard; what is built on a branded product is a package.** A protocol, a file format,
an algorithm or maths that no company owns belongs in the standard library, and the library grows to cover it when a program
needs it (HTTP, TLS, SHA-256/HMAC, Argon2, secure random bytes, base64, gzip, and the maths games need). A client
for a branded product (a database such as MongoDB or Postgres, a vendor's API or service) is never added to
the standard library: it is a package, maintained by whoever owns it, loaded pinned to a commit. An operating system's
folder is how a standard class is written for that system, not a brand package. Nothing in the standard library breaks the
rule.

Every class here is Spite, in the standard library's folder, which a program loads by its path, compiled with the program, and tree-shaken like the program's own code,
so a program carries only what it reaches ([Pure Spite](#pure-spite-dissolving-the-runtime) says what is still the
compiler's).

### The String class

An immutable value with a length (not a bare `char*`), sixteen bytes wherever it is kept. A literal is static and
never allocates; other text of up to 15 bytes is kept in those sixteen bytes and never allocates either; longer
text is one block on the heap, counted and freed with its last holder. **Its storage is Spite**:
`String` declares a `Buffer` for the characters (with a 0 after the last) and a length, and every member but a few is a
Spite function reading them ([memory.md](https://github.com/Spite-Language/spite_documentation/blob/master/memory.md#where-string-and-integer-keep-their-memory)). The ones the compiler supplies are the ones
that depend on where the characters are: `byte_at` (reading one), `sum` (`+`), and the making of a `String` from
bytes, which is how `slice`, `terminated_text()` and anything that fills a buffer itself make text. The right side of `+` casts toward
`String` (every numeric type, `Boolean`, and enum all format to text; see
[Numeric types](values_and_types.md#numeric-types) for `Float`/`Double`'s
shortest-round-trip printing); assigning a `String` to any numeric variable parses it, defaulting to `0`/`0.0` on
failure. `==`/`!=`/`<`/`>` compare by content, and
([Operators](functions_and_operators.md#operators)'s table) also work as
`equals(other)`/`less_than(other)`/`greater_than(other)`; `+` also works as `sum(other)`.

**Building text in a loop is linear.** `text = text +
piece`, `text = "{text}{piece}"` and any longer join that starts with the variable it is stored back into append
to that text in place when nothing else holds it, and otherwise copy it once with room to grow, so a `String`
shared by two names never changes under the other one. Short text grows inside its sixteen bytes and moves into a
block once it passes 15 bytes. When it applies and what it saves is
[optimizations.md](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md#text-appended-to-itself-grows-in-place)'s; `conformance/stage6/text_building` pins the
allocation counts.

| Method | Result | Notes |
|---|---|---|
| `length()` | `Integer` | |
| `is_empty()` | `Boolean` | |
| `slice(start, end)` | `String?` | the bytes from `start` up to `end`; `start == end` is `""`; `null` for a `start` below 0 or after `end`, or an `end` past the length, never a shorter text that would read like the right one; the program chooses `crash` or `fallback` (D1721, `tests/string_slice_edges`) |
| `get_at(index)`, written `text[index]` | `String?` | the whole UTF-8 character that starts at byte `index`; `null` below 0, past the end, or inside a character of several bytes (`tests/string_get_at`) |
| `byte_at(index)` | `Integer` | below 0 or past the end halts, naming the index and the length (`tests/code_past_the_end`) |
| `contains(text)` / `starts_with(text)` / `ends_with(text)` | `Boolean` | |
| `index_of(text)` | `Integer?` | `null` when absent |
| `replace(from, to)` | `String` | replaces every occurrence |
| `trim()` / `upper_case()` / `lower_case()` | `String` | |
| `split(separator)` | `List<String>` | an empty separator splits into single characters |
| `lines()` | `List<String>` | splits on `\n` |
| `pluralize()` / `singularize()` | `String` | the English plural or singular of the last word, by the rules below |
| `to_tiny()` / `to_short()` / `to_integer()` / `to_long()` | `Tiny?` / `Short?` / `Integer?` / `Long?` | digits with an optional `+` or `-`, spaces around them allowed; `null` for anything else (`"12abc"`, `""`) or a number the type cannot hold |
| `to_byte()` / `to_unsigned_short()` / `to_unsigned_integer()` / `to_unsigned_long()` | `Byte?` / `UnsignedShort?` / `UnsignedInteger?` / `UnsignedLong?` | the same, and `null` for a negative number |
| `to_float()` / `to_double()` | `Float?` / `Double?` | digits with an optional sign, `.` and exponent (`-1.5e2`, `.5`), spaces around them allowed; `null` for anything else (`"1e"`, `"nan"`) |
| `sum(other)` / `equals(other)` / `less_than(other)` / `greater_than(other)` | `String` / `Boolean` | what `+`/`==`/`<`/`>` call; they are only called through the operator |
| `to_string()` / `to_debug()` | `String` | the text itself / the text quoted, with `"`, `\` and a line feed escaped ([`Console.debug`](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md#console)) |
| `to_buffer()` | `Buffer` | the text's bytes, one per byte of its UTF-8 ([Bytes are a Buffer](#bytes-are-a-buffer)), for [hashing, encoding and compressing](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md#bytes-base64-compression-hashes-and-passwords) |

**`pluralize()` and `singularize()` inflect English the way Rails' ActiveSupport does**, so a
name and its collection read as a pair: `"active_quest".pluralize()` is `"active_quests"`, and
`"map_names".singularize()` is `"map_name"`. Only the last word changes: the text after the last `_` or space, or
from the last capital that follows a lower-case letter (`"ActiveQuest"` → `"ActiveQuests"`). The word is inflected
in lower case and handed back in the case it came in: `"Person"` → `"People"`, `"ACTIVE_QUEST"` →
`"ACTIVE_QUESTS"`. In order, a whole last word that is **uncountable** stays as it is; an **irregular** one
swaps with its pair; otherwise the first of ActiveSupport's ending rules that matches applies (`box` → `boxes`,
`enemy` → `enemies`, `wolf` → `wolves`, `knife` → `knives`, `analysis` → `analyses`, `medium` → `media`, `index` →
`indices`, `matrix` → `matrices`, `vertex` → `vertices`, `mouse` → `mice`, `ox` → `oxen`, `octopus` → `octopi`, `quiz`
→ `quizzes`, `status` → `statuses`), and a word no rule names takes or loses an `s`. A word already in the asked
number is left alone (`"quests".pluralize()` is `"quests"`, `"people".pluralize()` is `"people"`), and `""` answers
`""`. The rules are ActiveSupport's own; the words they would get wrong are in the table.

The uncountable and irregular words are attributes of `String` itself, beside `pluralize()`: `irregulars`, a dictionary from each singular to its plural (`person` →
`people`, `man` → `men`, `woman` → `women`, `child` → `children`, `foot` → `feet`, `tooth` → `teeth`, `goose` →
`geese`, `cactus` → `cacti`, `fungus` → `fungi`, `radius` → `radii`, `criterion` → `criteria`, `phenomenon` →
`phenomena`, `appendix` → `appendices`, `leaf` → `leaves`, `hero` → `heroes`, `potato` → `potatoes` and the rest),
and `uncountables`, a list (`data`, `equipment`, `health`, `information`, `money`, `news`, `series`, `sheep`,
`software` and the rest). A program adds a word by reopening `String`: a file `string.spite` in the program
that declares `irregulars` or `uncountables` replaces that list whole, so it restates the library's words it keeps:

```gdscript
var irregulars = {"person": "people", "sheaf": "sheaves"}
```

`pluralize()` reads the program's table at run time, and the compiler reads the same table where it inflects a
member's name ([metaprogramming.md](https://github.com/Spite-Language/spite_documentation/blob/master/metaprogramming.md)), so `map_sheaves()` collects a `sheaf` attribute once
`sheaf` is in it (`conformance/stage6/inflection_table`). Called on a literal (`"cactus".pluralize()`), the answer is worked out while compiling
([optimizations.md](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md#calls-on-constants-are-run-while-compiling)). `pluralize()` and `singularize()` are functions, like `upper_case()`.

### Maths

The maths of a 3D engine (skinning, animation, lighting) as members of the number classes, so it adds no
syntax and binds no singleton ([values_and_types.md](https://github.com/Spite-Language/spite_documentation/blob/master/values_and_types.md#maths-functions) teaches them). On `Float` and `Double`,
each answering the receiver's type unless it says otherwise, with every other operand cast to that type like any
argument:

| Member | `Float` / `Double` in C | Answers |
|---|---|---|
| `square_root()` | `sqrtf` / `sqrt` | not-a-number below zero; `-0` for `-0` |
| `sine()`, `cosine()`, `tangent()` | `sinf`, `cosf`, `tanf` / `sin`, `cos`, `tan` | of an angle in radians |
| `arc_sine()`, `arc_cosine()` | `asinf`, `acosf` / `asin`, `acos` | not-a-number outside -1 to 1 |
| `arc_tangent()` | `atanf` / `atan` | between -pi/2 and pi/2 |
| `rise.arc_tangent_over(run)` | `atan2f` / `atan2` | the angle of the point `(run, rise)`, between -pi and pi, in the right quadrant; `0` for `(0, 0)` |
| `power(exponent)` | `powf` / `pow` | |
| `exponential()` | `expf` / `exp` | infinity once it overflows |
| `logarithm()`, `logarithm_base_2()`, `logarithm_base_10()` | `logf`, `log2f`, `log10f` / `log`, `log2`, `log10` | the natural logarithm and the other two; minus infinity at `0`, not-a-number below it |
| `floor()`, `ceiling()`, `truncate()` | `floorf`, `ceilf`, `truncf` / `floor`, `ceil`, `trunc` | a whole value, still in the receiver's type: `-2.5` gives `-3`, `-2` and `-2` |
| `round()` | `roundf` / `round` | half away from zero: `2.5` is `3`, `-2.5` is `-3` |
| `absolute()` | `fabsf` / `fabs` | |
| `minimum(other)`, `maximum(other)` | `fminf`, `fmaxf` / `fmin`, `fmax`, after an `isnan` test of each operand | not-a-number passes on: `nan.minimum(0.0)` and `(0.0).minimum(nan)` are `nan`, never a real number that hides it |
| `clamp(low, high)` | `maximum(low)` then `minimum(high)` | not-a-number in any operand gives `nan`, and `high` wins when `low` is above it (`conformance/stage6/nan_passes`) |
| `is_finite()`, `is_infinite()`, `is_not_a_number()` | `isfinite`, `isinf`, `isnan` | a `Boolean` |

On every whole number, `Tiny` to `UnsignedLong`, answering the receiver's type:

| Member | Answers |
|---|---|
| `absolute()` | the value without its sign, and an unsigned value is itself; the smallest signed value has no positive counterpart, so its `absolute()` halts as its negation does, naming the class (`'lowest.absolute()' does not fit in an Integer (0 - -2147483648)`) |
| `minimum(other)`, `maximum(other)` | the smaller or larger, `other` cast to the receiver's type first like any argument: a `Byte`'s `minimum(300)` is a compile error for the literal and halts for a value that does not fit, as every narrowing that does not fit does |
| `clamp(low, high)` | `value` held between `low` and `high`, with `high` winning when `low` is above it, as on a float |

**A number class's special values are presets** (D1776): `Float.Pi()`, `Tau()`, `EulerNumber()`, `Infinity()`,
`NotANumber()`, `Largest()` (the largest finite value) and `Smallest()` (the most negative finite one) on `Float` and
`Double`, and `Largest()` and `Smallest()` on each whole number (`UnsignedLong.Largest()` is 18446744073709551615,
its `Smallest()` 0). Each builds its value on the class, and the compiler folds it. The old spelling names the
preset: `Float.pi()` is "'Float.pi' is the preset 'Float.Pi()': a special value of a class is a preset, named in
PascalCase and called on the class", and a call is never assigned (`Float.Pi() = 3.0` is "only a local, an attribute
or an index is assigned with '='", `diagnostics/maths_constant_assigned`). On a value, `angle.pi` is "'Pi' is a preset
of the class Float, not of a value: write 'Float.Pi()'"; on the class, a function of a value is "'Float.square_root()'
calls a function of a value on the class: the class Float answers only its presets, ...", and a name that is neither
is "the class Integer answers only its presets, Largest() and Smallest(), and 'Pi' is not one of them"
(`diagnostics/maths_constant_on_value`).

**Nothing here halts.** Floats keep infinity and not-a-number, so every edge answers the IEEE 754 value the
C library gives (`(-1.0).square_root()` is not-a-number) and `errno` is never read. Not-a-number is never
dropped on the way: `minimum`, `maximum` and `clamp` answer it when an operand is it, where C's `fmin` and
`fmax` would answer the other operand as if nothing were wrong. The whole-number
division checks are untouched: none of these divides.

**Each is a primitive of the language, lowered by the backend** (named in Spite, lowered in one
place, so no Spite changes with the backend). `--final-classes` prints them as bodiless declarations in each number
class, and the compiler is the one place that says what each becomes:
a call of the C library's function written where it is called, so `angle.sine()` is `sinf(angle)`, with no function of Spite's own
around it, and a whole number's `clamp` is two comparisons in one statement. None has hand-written C in a `.spite`
file of the standard library. A call whose operands are all constants, `(0.5).sine()` or `Float.Pi().cosine()`, is worked out
while compiling with the same C library function, so its answer is bit for bit the one the program would have
computed ([optimizations.md](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md#maths-on-constants-is-worked-out-while-compiling);
`conformance/stage6/maths_folding`).

**Tree-shaken, and nothing at run time.** A member a program never calls is not in its C; `<math.h>` is
included only when a member that calls the C library, or a `%` on a `Float` or `Double` (`fmodf`, `fmod`),
survives tree shaking, and only then does a build on Linux or
macOS link it (`-lm`; Windows has it in the C runtime). A program that uses none of it, `examples/hello`, carries
no `#include <math.h>`. There is no `fused_multiply_add`: without a processor flag telling the C compiler the
machine has the instruction, `fmaf` is a slow exact routine in the C library, a cost a reader would not expect
from one multiply and add.

Other languages' short names are errors naming the Spite one, on the class that has it: `side.sqrt()` is "Float
has no function 'sqrt': Spite spells it 'square_root', since no name is abbreviated", and likewise `sin`, `cos`,
`tan`, `asin`, `acos`, `atan`, `atan2`, `pow`, `exp`, `log`, `ln`, `log2`, `log10`, `ceil`, `trunc`, `abs`,
`fabs`, `min`, `max`, `isnan`, `isinf` and `isfinite` (`diagnostics/maths_other_spellings`).
`conformance/stage6/maths_functions` pins the values and the edges, and `maths_precision` what `Float` loses
against `Double`.

### System classes

Ordinary classes of the standard library, each written once for every system, with the few functions that call the system
in the `windows`, `linux` and `mac` folders, which reopen the class and reach the system's own
library through `DynamicLibrary`. What each member answers is in the section of [the page](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md) that teaches it.

| Class | Members |
|---|---|
| `Path` | made from text by its conversion, `func Path(from: String): Path` (D1776, D1785, D1788), so a program passes text where a `Path` is wanted; reading tidies it: `.` and empty pieces go, `..` takes back the piece before it (never above the root of a rooted path), `\` becomes `/`, and a trailing separator goes. `name`, `extension` (empty for none and for a leading-dot name), `parent` (`.` when there is none), `is_rooted`, `joined(other)` (a rooted `other` is itself), `written` (the text as given), `==` and `to_string()` (`tests/paths`). `File` and the rest hand the system the written text and keep it as their `path` (D1788). Every library function that takes a path takes a `Path`: `File`, `Directory`, `move_to`, `FileSystemWatcher.watch_folder` and `watch_file`, `Environment.load_from_file` and `Process.working_directory` |
| `File(path)` | `path`, `file_name` (the last piece of the path), `to_string(): String` (the path), `read_as_string(): String?` (`null` when the file cannot be read or its bytes are not UTF-8, so bytes never ride inside a `String`; `tests/reads_say_nothing`), `write_string(text): Boolean`, `append_string(text): Boolean`, `exists(): Boolean`, `remove(): Boolean`, `move_to(path): Boolean`; bytes: `byte_count(): Long?`, `last_written(): Instant?`, `read_as_buffer(): Buffer?` (`null` when the file cannot be read whole, a read comes short, or it is over 2 GiB, which no `Buffer` holds), `read_buffer_at(position, count): Buffer?` (halts on a negative position or count, the program's mistake), `write_buffer(buffer: Buffer): Boolean`, `append_buffer(buffer: Buffer): Long?`, `lock(): LockedFile?` ([Bytes are a Buffer](#bytes-are-a-buffer)). Nothing about a file halts (D1721): a write, `remove()` and `move_to(path)` answer whether they happened, and the program must use the answer, at least `fallback file.write_string(text)` or `crash file.write_string(text)` ([An answer is used](failure.md#an-answer-is-used), `conformance/dropped_boolean_answer`); see [Read and write a file](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md#read-and-write-a-file). `lock()` is a lock between processes: it waits (a point to return from in a `Concurrent`) until no other process holds a lock on this file, creating the file empty when it is missing, and answers a `LockedFile`, or `null` when the path cannot be opened or the system refuses the lock. The lock is let go by `unlock()`, by `drop()` and by the system when the process ends, so a crash never leaves it held; `LockFileEx` on Windows and `fcntl` on Linux and macOS. A `File` is a path with no open handle: each call opens and closes the file, positions replace seeking (`_fseeki64`/`_ftelli64` on Windows, so a position past 2 GB works), and `last_written()` reads `GetFileAttributesExA`'s `ftLastWriteTime` on Windows and `stat`'s modification time on Linux and macOS. `move_to(path)`, on a `File` and on a `Directory`, answers `false` and changes nothing when a file or folder is already at `path`, and otherwise moves with `rename` on Linux and macOS and `MoveFileExA` on Windows (a file may be copied across drives there; a folder never is); on `true` the value names `path`. On Windows every path goes to the system, and every name, folder, executable path, environment value and command line comes back, through the `W` calls as UTF-16, converted from and to UTF-8, so a name outside ASCII is its own text (`tests/names_outside_ascii`) |
| `Directory(path)` | `path`, `folder_name` (the last piece of the path), `entries(): List<Directory.Entry>?` (below; `null` for a folder that does not exist or cannot be opened, or that holds a name that is not valid UTF-16, an empty list for an empty one; `tests/directory_missing`), `exists(): Boolean`, `create(): Boolean`, `move_to(path): Boolean`; see [List a directory](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md#list-a-directory) |
| `Process(command, arguments)` | `working_directory`, `environment_variables`, `with_errors` (false; true reads the child's standard error into the output too, as the shell's `2>&1`), `run(): Integer?` (the exit code, `null` when the command could not be started), `read_output_as_string(): String?` (`null` before a run, after one that could not start, or when the output is not UTF-8), `read_output_as_buffer(): Buffer?` (the output's bytes as they came, `null` before a run or after one that could not start; `tests/reads_say_nothing`), `run_attached(): Integer?` (the exit code, `null` when the system could not start its command interpreter, so a command that cannot start is never read as one that exited with `-1`; `tests/process_attached_code`); see [Run a process](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md#run-a-process). Each argument reaches the child whole: single-quoted for the shell on Linux and macOS, and quoted by the `CommandLineToArgvW` rules on Windows, a `key=value` argument quoting only its value (`-script="a b"`). `run()` reads the child's standard output through `_popen`/`popen`; Windows opens the pipe in binary mode so `0x1A` is ordinary output data. `run_attached()` is the C library's `_wsystem` on Windows, `errno` telling a refused start from an exit code of `-1` |
| `Program()` | a singleton: `exit(code)`, `sleep(duration: Duration)` (waits the duration's whole milliseconds, rounded down; a duration under a millisecond, zero or negative only gives up the thread's turn and returns), `arguments(): List<String>` (what it was started with, [programs.md](programs.md#constructors-and-the-entrypoint)), `environment(name): String?` (`null` when the variable is not set, a value of any length read whole), `executable_path(): String?` and `working_directory(): String?` (`null` when the system refuses or the path is not UTF-8, a path of any length read whole), `live_allocations(): Long` (allocations not yet freed) and `live_bytes(): Long` (the bytes those asked for), which make the program count its allocations and frees only when it calls one of them; see [Program](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md#program). `exit` flushes `Console` first, since the C library's `exit` would drop what the program's own standard output still buffers |
| `Clock()` | a singleton: `elapsed(): Duration`, `now(): Instant`; `elapsed()` is the monotonic clock, a `Duration` from an arbitrary start to the nanosecond, so two readings subtract (`clock.elapsed() - started`) into the time between them; see [Clock](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md#clock), [Time](time.md#time-one-stored-instant-zones-for-presentation) |
| `Benchmark(work)` | `answer`, what one call of `work` answered, and `duration: Duration`, the monotonic time the call took; see [Time](time.md#time-one-stored-instant-zones-for-presentation) |
| `Console()` | a singleton: `print(...values)`, `write(...values)`, `error(...values)`, `debug(...values)`, `read_line(): String?`; see [Console](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md#console), and below |
| `FileSystemWatcher()` | `watch_for_changes(target: FileSystemWatcher.Target): Boolean`, `changes(): List<String>`, `wait_for_changes()`; see [Watch files and folders](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md#watch-files-and-folders), which is the rule. `HotReload` is built on it ([REPL and live reload](https://github.com/Spite-Language/spite_documentation/blob/master/repl.md#repl-and-live-reload)) |
| `Tcp.Server()`, `Tcp.Client()`, `Tcp.Connection` | TCP over IPv4 and IPv6 on every system. A server listens (`listen_locally(port)`, `listen_everywhere(port)`, `listen_at(address: IpAddress, port)`, each `Boolean`) and answers `accept(): Tcp.Connection?`; a client connects (`connect(address: IpAddress, port)`, `connect_locally(port)`, each `Boolean`); a client and a connection talk alike: `read_line(): String?` (`null` when the connection ended before a whole line, and for a line whose bytes are not UTF-8, the next call reading the line after it), `read_as_string(): String?`, `read_as_buffer(): Buffer?`, `write_line(text): Boolean`, `write_string(text): Boolean`, `write_buffer(buffer): Boolean`, `closed: Boolean`, `close()`. Every read waits ([Tcp](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md#tcp), and below); `--repl-port` and `spite connect` use `listen_locally` and `connect_locally` ([REPL and live reload](https://github.com/Spite-Language/spite_documentation/blob/master/repl.md#repl-and-live-reload)) |
| `IpAddress` | an IPv4 or IPv6 address and nothing else (a port is a number beside it): `is_ipv4()`, `is_ipv6()`, `==` (its `equals(other)`), `to_string()` (RFC 5952). It is read from text, a literal or a host name resolved to its first address, with `IpAddress(from: String): IpAddress?`, and every address of a name is `String.resolve_ip_addresses(): List<IpAddress>`; `IpAddress()` is `0.0.0.0`, and no constructor takes text (D1718; below) |
| `Concurrent(function)`, `Parallel(function)` | the handle stands in for what the function returned, and reading it is the wait; `finished: Boolean` never waits; dropping the handle waits for it; there is no `wait()` and no `join()`; see [concurrency.md](https://github.com/Spite-Language/spite_documentation/blob/master/concurrency.md) |
| `DynamicLibrary(file_name, naming, header)` | every foreign function, constant and type of a native library; see [Foreign libraries](foreign_libraries.md#foreign-libraries). It holds its `file_name` and `handle`, its constructor and `drop()`; opening, closing and finding a symbol are the compiler's reopening |
| `ForeignBytes(address, count)` | memory a foreign library hands out (an address and a size it answers): `count(): Long`, `get_at(position): Byte?` (`foreign[position]`), `set_at(position, value)`, `read_<number>_at(position): <number>?` and `write_<number>_at(position, value)`, big-endian forms included, as a `Buffer` has them, `read_buffer_at(position, count): Buffer?` and `write_buffer_at(position, buffer)`; see [Bytes are a Buffer](#bytes-are-a-buffer) |

**`Console` is a singleton:** `Console()` is the same instance everywhere, `Console` is an ordinary class
name rather than a reserved word, and `console.class.name` is `"Console"` like any other class. Calling a member
on `Console()` directly is an error: `'Console' is a singleton: bind it once beside the attributes, 'var console =
Console()', and use 'console.print'`. `read_line()` answers `null` at the end of the input with nothing read
and for a line whose bytes are not UTF-8 (the next call reads the line after it), reads a line of any length
whole, and drops a `\r` before the line feed (`tests/console_long_lines`). The entry constructor returning normally is exit code `0`.

**Printing is `to_string()`.** `print`, `write` and `error` are Spite in
`Console`, taking `...values: List<Printable>`, where

```gdscript
type Printable {
    to_string(): String
}
```

Every number, `Boolean`, `String` (whose `to_string()` answers itself), enum value, `Spite.Class` (its
`.name`) and `Spite.Namespace` (its `.name_with_namespaces`) answers it, so everything that printed before prints
the same. A class of your own prints once it declares `func to_string(): String`, and passing one that does not is
the ordinary shape error, naming the function: `'Pet' does not fit type 'Printable': it has no function
'to_string'`. A `List` or a `Dictionary` has no `to_string()` either (`'List<Integer>' does not fit type
'Printable'`), so it is printed with `debug`, or joined first (`diagnostics/print_without_to_string`,
`conformance/stage6/printable_values`). A `type` writes a required function as `to_string(): String`. What stays
the compiler's is only the floor: `_write_output(text)`, `_write_error(text)`, `_write_held()` and `_flush()` are
private, declared nowhere in Spite, and the compiler supplies them.

**A printed line is written out at once.**
`print`, `error` and `debug` flush after their line break, so output redirected to a file or a pipe
shows each line when it is printed instead of when the C library's buffer fills or the program exits; `write`
does not, so a line built from pieces goes out with the next line end. `read_line()` flushes first, so a prompt
written with `write` is on the screen before the program waits for the answer; `error` flushes the output first, so
the two streams keep the order they were written in when they reach one file; and `Program.exit(code)` and
`Process.run_attached()` flush first too. A line the system refuses to take (a closed or full pipe, a closed
terminal) is lost without a word: it is the one write the library does not answer for, since the output it would
be reported on is the output that failed, and a `print` that answered would make every program narrow every line
(D1734). In a program that starts a thread, each thread gathers what one `print`,
`write`, `error` or `debug` writes and hands it to the operating system in one piece, so two threads printing at
once never split each other's lines; a program with one thread writes
each piece as it comes. The C library already writes
a terminal's output promptly, so nothing changes there. Chosen as the cheapest way that shows every line: one
`WriteFile` or `write` per line. The alternatives cost the same or do not show
every line: line buffering (`setvbuf` with `_IOLBF`) is one system call per line too, and the Windows C library
treats it as full buffering; flushing on a timer, or only where the program waits, leaves the last line of a
program that computes without waiting in the buffer. A program that prints a great deal to a file and does not
need to be watched builds its text and prints it in fewer, longer lines.

Printing a value costs what the call says: the list of values
is a `List` like any variadic call's, a number is held in the list with its class and goes through its `to_string()`, and the text is written
with its length rather than up to its first zero byte. `conformance/stage6/text_building` and
`fused_chain_allocations` pin those allocations. A `crash` writes its operands itself, through each value's
`to_string()`, because it reports on the way out of a program that is stopping, after flushing what the program
printed before it.

**`Console.debug` and `to_debug()`.** Each class has an automatic `to_debug(): String` that returns something like
`Class {attribute: value, other: value}` by nesting `to_debug()`s. `debug(...values: List<Debuggable>)`, where `type Debuggable { to_debug():
String }`, writes each value's `to_debug()` separated by a space, then a line break. Every value answers it:

```text
Player { name: "hero", scores: [3, 7], bag: {"gold": 2}, partner: null, mood: 'calm' }
```

In detail:

- **A class shows its name and its attributes**, in declaration order, as `Name { attribute: value }`, and
  `Name {}` when it has none to show. An attribute that is a class is shown by *its* `to_debug()`, so a class that
  declares its own is shown by it wherever it appears. A `List` is `[a, b]`, a `Dictionary` is `{"key": value}`,
  text is quoted with `\"`, `\\` and `\n` escaped, an enum value is written the way Spite writes it
  (`'calm'`), a number and a `Boolean` as they print, and an absent `T?` is `null`. A `Spite.Class` is its name.
- **Private attributes are left out.** The walk is `value.attributes.each(debug_attribute)` run from
  `Spite.DebugInstance` ([reflection.md](https://github.com/Spite-Language/spite_documentation/blob/master/reflection.md#a-class-and-an-instance-of-it)), and a walk sees every
  attribute, private ones included, so `Spite.DebugInstance` skips each one whose name starts with `_`: it is the
  class's own business. The test is on a name known while compiling, so it folds, and a private attribute costs
  nothing in the text. The JSON and binary writers and readers skip them the same way, and attributes holding a
  singleton too ([json.md](https://github.com/Spite-Language/spite_documentation/blob/master/json.md#what-each-type-becomes)); `to_debug()` shows those.
- **A cycle ends at an object already being shown**: it is written `Name {...}`, so `first.next.next` pointing
  back at `first` shows `Node { value: 1, next: Node { value: 2, next: Node {...} } }`. Each class keeps the
  objects it is in the middle of showing, compared with `==` (identity, unless the class defines `equals`), and
  a tree of distinct objects is shown whole however deep it goes.
- **It is Spite, and it costs nothing unused.** `Spite.Debug<$value_type>` turns any
  value into its text, and `Spite.DebugInstance<$value_type>` walks a class
  instance. The generator's whole part is that asking a class for a `to_debug()` it does not declare answers one
  that calls them, created only when something asks, so a program that never debugs a class compiles nothing for
  it. A `type` or union that does not require `to_debug()` still answers it, dispatched to the class the value is.
- **The REPL keeps its own display** (`Player { name: hero, ... }`: text unquoted, nested objects as `Name {...}`,
  lists as `List<String>(...)`), which it builds from `Spite.Attribute`'s `.value` through `value.to_string()`,
  and the documented sessions depend on it.
- **What it costs**: in a program that calls `debug`, one generated `to_debug()` per class it shows, which
  builds the text as `String`s; the walk is compiled once per attribute, so each `attribute.value` is a typed read
  of the field and nothing is boxed. Every attribute it shows counts as read.

`conformance/stage6/debug_values`.

**A directory is navigated through its entries.** `Directory` has a `path` exactly as `File` does, and `entries()` answers a `List<Directory.Entry>?`, `null` when the folder cannot be opened, where
`Directory.Entry` is the union of `Directory` and `File` declared in the standard library. Each entry's `path`
is its parent's joined with its name, so a `switch` tells the two apart and a folder is walked by calling the same
function on it again ([Walk a directory tree](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md#walk-a-directory-tree); `conformance/stage4/directory_entries`).

In detail:

- **The name is `Entry`**, namespaced as `Directory.Entry`, because it is what a directory listing calls each of
  its items and it says nothing the class does not: `DirectoryEntry` would repeat the class it already lives in,
  and `Path` would claim a text value it is not.
- **Folders come first, then files, each sorted by name**; `.` and `..` are never listed. Each kind's names
  are sorted before its values are made.
- Each operating system's folder lists a directory its own way, through `entry_names(want_folders)`, since
  `entries()` is the public listing and Spite has no overloading.
- `--final-classes` prints `Directory` back out with its `union Entry`, which compiles because a union declared
  again replaces the earlier one ([Packages, namespaces and loading](packages.md#packages-namespaces-and-loading)).

**`Tcp` and `Udp` do what a game server needs, on every system** (D1717). Each protocol is a namespace of a
`Server`, a `Client` and, where a server accepts, a `Connection`. The classes hold everything but the calls into
each system's library, which the `windows`, `linux` and `mac` folders reopen them with:

- **One talking code.** `Tcp.Connection` holds the handle, the bytes read past a line and `closed`, and does all the
  reading and writing. `Tcp.Server.accept()` hands one out, and a `Tcp.Client` holds one, as a `WebSocket.Client`
  holds a `WebSocket.Connection`: its reads and writes are the connection's. `Http` and `WebSocket` are written on
  `Tcp.Client` and `Tcp.Connection`. A `Udp.Client` holds a `Udp.Server` opened on a port the system picks, so the
  datagram code is written once too (D1726).
- **Addresses are `IpAddress` values.** `IpAddress` holds only the address, four numbers for IPv4 or eight 16-bit
  groups and an IPv6 zone index (`fe80::1%4`), and the port is always a number beside it. It is the one place that
  reads an address from text, writes it as RFC 5952 gives it (lowercase, the longest run of two or more zero groups
  as `::`, the first when two are as long, the zone after a `%`), and turns it into the system's address and back.
  `IpAddress(from: String): IpAddress?` reads dotted IPv4 (each part 0 to 255, no leading zero) and IPv6 (one `::` at most, a
  dotted IPv4 ending, a numeric zone), or, for text that is a host name (labels of letters, digits and hyphens, the last not all digits, so `"1.2.3"` is a malformed address and never looked up), resolves it and answers its first address, IPv4 first, keeping the name; `null` when the text is neither or the name names nothing (`tests/ip_addresses`). An IPv4 address
  inside `::ffff:` is that IPv4 address, from text or from the system, so a datagram a dual-stack socket receives
  from `127.0.0.1` comes from `127.0.0.1`, and the two compare equal (`tests/ip_addresses`). `IpAddress()` is
  `0.0.0.0`.
- **Every network call takes an `IpAddress`, and only one** (Spite has no overloading, the cast fills the gap;
  D1727). `listen_at`, `connect`, `open_at`, `send_to`, `Http.Client.send` and `WebSocket.Client.connect` take an
  `IpAddress`; text reaches them through `IpAddress(from: String): IpAddress?`, which resolves a host name through
  `String.resolve_ip_addresses()`: the system's `getaddrinfo`, IPv4 answers first in the order the system gives
  them, then the IPv6 ones, empty when the name names nothing. Resolving waits in place, like connecting. An address
  resolved from a name keeps the name, and `Http.Client.send` writes it as the `host` header and
  `WebSocket.Client.connect` in its handshake; an address read from a literal is written as itself, an IPv6 one in
  brackets (`tests/http_exchange`). Until the cast of spite_compiler#228 and the folding of #229 land, a program
  writes `var read: IpAddress? = "127.0.0.1"` and narrows it.
- **Every interface.** `listen_locally`, `connect_locally` and `open_locally` use `127.0.0.1`; `listen_everywhere`
  and `open_everywhere` use `::` on a socket that takes IPv4 too (`IPV6_V6ONLY` off), or `0.0.0.0` where the system
  has no IPv6. A dual-stack socket sends to an IPv4 address as `::ffff:` and the address; an IPv4 socket cannot
  send to an IPv6 address, and `send_to` answers `false`. The listening queue is 64 connections deep.
- **Every read waits** (D1725). There is one read per kind and no `_now` twin: `accept`, `read_line`,
  `read_as_string`, `read_as_buffer` and `receive` wait for what they answer, or for the end. A frame loop keeps
  running because the compiler parks the wait (spite_compiler#218, #219). The REPL's own loop (`--repl-port`)
  polls through the private `Tcp.Server._accept_now` and `Tcp.Connection._read_line_now`, and `WebSocket` reads
  what waits unread before it closes through `Tcp.Connection._read_as_buffer_now`; no program can call them.
- **Waits the compiler knows.** `accept`, `read_line`, `read_as_string` and `read_as_buffer` reach
  `Tcp.Server._accept_handle` and `Tcp.Connection._receive_into`, and `Http.Server.next_request` reaches
  `Tcp.Server._first_readable` (the system's `poll`, or `WSAPoll`, over the listener and every kept connection, for
  at most `idle_limit`, a `Duration` (5 seconds unless set), or one second), which are the compiler's waits
  ([concurrency.md](concurrency.md#concurrency-concurrent-parallel-and-hidden-waiting)): in a `Concurrent` they
  return to the event loop while a helper thread makes the call. `write_line`, `write_string` and `write_buffer`
  send until everything is sent, in place.
- **`closed`** is an attribute: `false` once a connection is accepted or connected, `true` once a read sees the
  end of the connection or a failure, once a write fails, or after `close()`; a connection never connected is
  closed. A read on a closed connection answers `null` without calling the system, a write sends nothing, and
  nothing crashes.
- **Lines and bytes mix.** Text `read_line` read past its line waits in the connection, and `read_as_string` and
  `read_as_buffer` hand those bytes out first.
- **Text from a stream.** `read_as_string()` reads what `read_as_buffer()` would, and when that run ends inside a
  character of several bytes it reads up to three more bytes, one at a time, to finish it: a character the network
  cut in two is never `null`. It answers `null` once the connection is closed, or when the bytes are not UTF-8.
- **What it costs**: a `Boolean` per connection (and on Windows one more for the mode), and the functions a
  program calls: a system function only an unused call reaches (`getaddrinfo`, `ioctlsocket`, ...) is never looked
  up. `tests/socket_bytes` (a server and a client in one program, bytes both ways and a line, then the close),
  `socket_waits` (the waiting calls, inside a `Concurrent`), `ipv6_addresses`, `datagrams` and `ip_addresses` are
  the proof.

### Bytes are a Buffer

Bytes a program reads or writes are a `Buffer`, a class of the standard library; no program-facing function takes
or answers a `List<Byte>` for them ([Bytes are a Buffer](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md#bytes-are-a-buffer)).
Everything that hands bytes over answers a `Buffer`, and everything that takes bytes takes one: `File`, `Tcp.Connection`,
`Udp.Datagram`, `WebSocket.Message`, `Http.Request` and `Http.Response` bodies, `Process` output,
`String.to_buffer()`, `Base64`, `Deflate`, `Zlib`, `Gzip`, `Sha256`, `SecureRandom` (`random_buffer(count): Buffer?`, `null` when the system gives no random bytes, halting on a negative count), `Argon2.derive` (and `Argon2.hash(password): String?`, `null` with no random bytes for the salt),
`BinaryWriter`, `BinaryReader` and `ForeignBytes.read_buffer_at`.

**Bytes that may be text are one holder with two reads** (D1708), as a `File` is: `read_as_string(): String?`,
`null` when the bytes are not UTF-8, and `read_as_buffer()`, the bytes as they are. So a `WebSocket.Message`
(`is_text()` beside the two reads), a `Tcp.Connection`, a datagram (`Udp.Server.receive(): Udp.Datagram?` and
`Udp.Client.receive()`, read with the two reads and carrying its `sender_address: IpAddress` and `sender_port`;
`null` only when nothing arrives, so a datagram is never lost by reading it the
wrong way; D1716), an HTTP body (`read_body_as_string()`, `read_body_as_buffer()`, `write_body_string(text)`,
`write_body_buffer(buffer)`) and a `Process`'s output (`read_output_as_string()`, `read_output_as_buffer()`):
nothing that carries bytes forces them through a `String`.

- **There is no `List<Byte>`.** A run of bytes has one class. The type written anywhere (a variable, a parameter,
  a result, an attribute, a generic argument, `List<Byte>()`) is the error `'List<Byte>' is not a type a program
  writes: a run of bytes is a 'Buffer', which reads and writes the numbers in it: write 'Buffer(size)'`, at the
  line that writes it. A list the compiler would make of bytes without the type being written (a list literal
  whose items are `Byte`, a `map` or a passed function that answers a `Byte` per item, a generic class whose
  `List<$element_type>` is bound to `Byte`) is the error `this makes a 'List<Byte>', and a run of bytes is a
  'Buffer': make one with 'Buffer(size)' and 'append' each byte`, at the line that makes it; for a generic class it
  is the line that binds the type, not the line inside the class. Compile time only
  (`diagnostics/list_of_bytes`).
- **The class.** `Buffer(size)` is an empty buffer with room for `size` bytes. The size is always written: `Buffer(0)` when nothing about it is known. A `Buffer` keeps its bytes in one block it owns, at the
  library's memory floor as `String` and `List` do: the buffer is one object, a read is one load from its block, and
  there is no `List` inside it. A full block is resized to twice its size. How much room it keeps is the library's
  and the compiler's (its capacity and its `_reserve` are private), with one exception: the room a foreign function
  writes into. A program passes `Buffer(4096)` to the function, then `take_written(count)` says how many bytes it
  wrote, halting when `count` is negative or past the room (decided by Mortaro on issue 23).
- **The size is a suggestion to the compiler, never a limit.** Appending past it grows the buffer as always. The
  compiler may keep less room when it proves fewer bytes are ever written, and may keep a buffer of a few bytes whose
  every read and write is at a proven position, and which never leaves its function or its object, in plain variables
  with no block at all. None of this changes what a program reads; it shows only in the allocations `--debug-memory`
  counts.
- **A number's storage is a buffer.** A number class states its size as `var _memory = Buffer(4)` (`Integer`),
  `Buffer(1)` (`Byte`) and so on: a `Byte` is a buffer of one byte, which the compiler keeps as the value itself
  ([memory.md](memory.md)).
- **It is still a run of bytes.** `count(): Integer`, `is_empty(): Boolean`, `buffer[index]` (`get_at(index):
  Byte?`, `null` out of range), `buffer[index] = byte` (`set_at`, halting out of range), `append(byte)`,
  `truncate(count)`, `clear()` (which keeps the block) and `copy(): Buffer` mean what they mean on a `List`
  ([collections.md](collections.md#listt-additions)), and a buffer is walked with the same templates and passed
  functions a list of `Byte` is, so code that treats bytes as a list of them keeps working.
- **Byte order is part of each read and write, not of the buffer, and always named.** Every number of two bytes or
  more is read and written through a function that names its order after the number: `_little_endian` (the order of
  every machine Spite compiles for, and of Spite's own binary format) or `_big_endian` (the most significant byte
  first: PNG, network order). So `read_integer_little_endian_at(position)`, `read_integer_big_endian_at(position)`,
  `append_short_little_endian(value)`, `write_long_big_endian_at(position, value)`; a `byte` and a `tiny` have one
  byte and no order. No order is unnamed, so a call says what it reads. The library writes each operation once, a
  template over the enum `Buffer.ByteOrder` (`'little_endian'`, `'big_endian'`) whose value is a word of the
  function's name. One buffer may hold both orders, and the order of every read is known while compiling.
- **No position.** A buffer has no reading position: every read names where it reads, and a walk keeps its
  position in a variable of its own (`var at = 0`, then `at = at + 4`). There is one way to read a number, so there
  is no second one to choose between.
- **Numbers.** `<number>` and its width: `byte` and `tiny` 1, `short` and `unsigned_short` 2, `integer`,
  `unsigned_integer` and `float` 4, `long`, `unsigned_long` and `double` 8. Below, `<number>` stands for the number
  with its order for every number of two bytes or more (`integer_little_endian`, `double_big_endian`), and for the
  number alone for `byte` and `tiny`.
  - `read_<number>_at(position): <number>?` reads the number whose bytes start at `position`;
    `null` unless `position >= 0` and `position + width <= count()`.
  - `append_<number>(value)` adds the number's bytes at the end.
  - `write_<number>_at(position, value)` replaces the bytes at `position` and halts unless all of them are in the
    buffer, since the position is the program's.
  - `byte` has none of these, since the list forms already say them once: one byte anywhere is
    `buffer[position]`, one more at the end `append(byte)`, and one replaced `buffer[position] = byte`.

  A float is written and read as its IEEE 754 bits, so a `not a number` keeps its bits. The forms that took a
  position before the value are errors naming the form: `'read_integer' reads at the buffer's position and takes
  nothing: write 'read_integer_at(position)' to read anywhere else`, and `Buffer has no method 'write_integer':
  write 'write_integer_at(position, value)', or 'append_integer(value)' to add at the end`.
- **Runs of bytes.** `read_buffer_at(position, count): Buffer?` answers a new buffer of the `count` bytes at
  `position`, `null` unless every byte is in the buffer, and a negative `count` halts. `append_buffer(other, start, count)` copies a run
  of another buffer (or the same one) to the end and halts unless `start >= 0`, `count >= 0` and `start + count <=
  other.count()`.
- **Text.** `read_string_at(position, count): String?` is the text of the `count` bytes at `position` (`null` when they are not valid UTF-8, a halt outside the buffer), and `read_as_string(): String?` is the text all the bytes spell, `null` when they are not valid UTF-8 (a stray
  continuation byte, a cut sequence, an overlong form, a surrogate or a value past U+10FFFF); `text.to_buffer()`
  answers a `Buffer` of the text's UTF-8.
- **Values are a serializer's job, numbers the buffer's.** `BinaryWriter<T>` and `BinaryReader<T>` turn a whole
  value of type `T` into bytes in a `Buffer` and back, the counterpart of `JsonWriter<T>` and `JsonReader<T>` for
  text ([json.md](json.md#json-is-reflection-not-a-library)); reading and writing single numbers at positions is the buffer's own (and
  `ForeignBytes`', with the same names), never a serializer's. Many values are one value of a list:
  `BinaryWriter<List<T>>` and `BinaryReader<List<T>>`, never a cursor through the bytes.
- **A `List` has none of these.** Every function above that a `List` does not have is an error on a list of any
  element type, naming the class that has it: `List has no method 'read_integer_little_endian_at': bytes are a
  Buffer, which has it: make them with 'Buffer()'` (the same for `read_as_string`, `append_buffer` and the rest;
  `conformance/list_byte_functions`).
- **Files.** `read_as_buffer()` answers the whole file, `null` when it cannot be opened, when its size cannot be read, or
  when fewer bytes arrive than its size said; an empty file is an empty buffer. Whether the bytes are read or the
  file is mapped is the compiler's choice, never the program's (D1706). A file larger than a `Buffer` holds
  (more than 2 147 483 647 bytes) halts, as any `Long` cut to an `Integer` does, so read it in pieces with
  `read_buffer_at`. `read_buffer_at(position, count)` answers up to `count` bytes from `position`: fewer when the file ends
  first, an empty buffer at or past the end, `null` when it cannot be opened. A negative `position` or `count` halts, since it is
  the program's own mistake. `write_buffer(buffer)` replaces the content and answers whether every byte was written;
  `append_buffer(buffer)` answers the position the bytes start at, `null` when the file cannot be opened or not every
  byte was written. Each call opens and closes the file (`tests/file_as_buffer`).
- **Sockets.** `read_as_buffer()` waits until at least one byte has arrived and answers a fresh buffer of what arrived
  (up to 65 536 bytes at once), `null` once the connection is closed. Bytes `read_line` read past its line are handed
  out first. `write_buffer(buffer)` sends all of it, waiting. A `Tcp.Connection` keeps one 64 KB block it receives into, made on the first read
  and freed by `close()`, and each read copies what arrived into its own buffer: one allocation per read. A
  `Udp.Datagram` and a `WebSocket.Message`, ping or pong are a `Buffer` the same way.
- **A read the compiler proves** is a `read_<number>_at(position)`, and it answers the number itself, not a `T?`:
  under a bound that covers its last byte
  ([proofs.md](https://github.com/Spite-Language/spite_documentation/blob/master/proofs.md#a-proven-count-or-bound-proves-a-read-of-a-width)), or inside a counted loop whose
  window covers it, or at `row * stride + column` under one guard
  ([proofs.md](https://github.com/Spite-Language/spite_documentation/blob/master/proofs.md#a-proven-count-or-bound-proves-a-read)). Inside a counted loop it
  is one load (and one byte swap for a big-endian order) with no test. Elsewhere the buffer could have changed size
  by another name since the bound was proven, so the read keeps one compare the C compiler is told is never taken
  (two when nothing proves the position is not negative), and when it fails the program halts with `spite:
  'bytes.read_integer_little_endian_at(body + 4)' is outside its buffer: a bound proves only the top of an index, and this one is
  below 0 or the buffer changed, at file:line`. A proven `write_<number>_at` in a counted loop is one store; elsewhere
  it is the library's checked write. The byte order is in the function's name, so a read never tests it:
  a big-endian read is the same load with its bytes swapped.
- **ForeignBytes.** `ForeignBytes(address, count)` halts when `count` is negative or `address` is `0` with a
  `count` above `0`. It has the same `read_<number>_at(position)` and
  `write_<number>_at(position, value)`, big-endian forms included: its reads answer `null` and its writes halt outside `0` to `count`, exactly
  as a `Buffer`'s do; `read_buffer_at(position, count)` answers a fresh `Buffer`, and `write_buffer_at(position, buffer)`
  halts unless every byte fits. It has no position of its own, since the memory is the library's. It never frees the
  memory, and reading after the library has taken the memory back is the library's fault, reported as a native fault.
- **Bytes go through a `Buffer`, never an address.** A `File`, a `Tcp.Connection` and a `BinaryReader` read into and write
  from a `Buffer`; none of them takes an address and a count, and a `List<Byte>` anywhere is the error above.
  `conformance/list_byte_functions`, `tests/list_byte_numbers`, `tests/binary_files`, `tests/socket_bytes` and
  `tests/socket_waits`.
- **What it costs.** Each function is an ordinary member of `Buffer`, `File`, `Tcp.Connection` or `ForeignBytes`, so a
  program that reads no bytes carries none of them, and `Buffer` and `ForeignBytes` are not in a program that makes
  none. A `Buffer` costs a `List`'s fields.

**`WebSocket` is RFC 6455 written in Spite over `Tcp.Client` and `Tcp.Connection`**, a namespace of four classes (`common/web_socket/`):
`WebSocket.Client` connects, `WebSocket.Server` upgrades the requests an `Http.Server` hands out, both sides
talk through a `WebSocket.Connection` (the client holds its own), and a message received is a
`WebSocket.Message`:

- **The server is built on an `Http.Server`.** `WebSocket.Server(http)` takes the server the program already
  serves HTTP with, so one port serves both, as an Express app does: the program reads `http.next_request()`,
  asks `sockets.asks_to_upgrade(request)` (its `upgrade` header is `websocket`, any case), and calls
  `sockets.accept(request)` only then; any other request is the program's to answer with `http.respond`, which answers `Boolean`, whether the response was written whole; a client that goes before it is accepted does not end `next_request()`, which waits for the next. `accept` on
  a request that does not ask to upgrade halts, since answering it is the program's. `accept` answers a
  `WebSocket.Connection?`: `null` when the upgrade is malformed, after it has answered the request itself through
  the `Http.Server` (`400`, or `426` with `sec-websocket-version: 13`), so a request is never answered twice.
- **The handshake.** `accept` takes a `GET` over HTTP/1.1 whose `upgrade` is `websocket`, whose `connection` lists
  `upgrade` (any case, among other tokens), whose `sec-websocket-version` is `13` and whose `sec-websocket-key` is
  16 bytes in base64; it answers with `sec-websocket-accept`, the key and the protocol's own suffix hashed with
  SHA-1 and written in base64. SHA-1 is private to the connection: it is the protocol's checksum, not a hash to store
  anything with. A server picks no subprotocol and no extension. `Http.Server` has already let go of the
  connection: `next_request()` keeps a connection again only when `respond` is called. `WebSocket.Client.connect(address,
  port, request)` sends a fresh key of 16 bytes from `SecureRandom` (`false` when the system gives none; a masked frame with no random bytes for its mask is not sent and answers `false`), `host` (the address's name, or the address) with the
  port when it is not 80, and every header of the request; a request that is not a `GET`, has a body, or sets
  `host`, `upgrade`, `connection`, `content-length` or a `sec-websocket-` header other than
  `sec-websocket-protocol` halts, since the handshake writes those itself. The answer must be
  `HTTP/1.1 101` with the right `sec-websocket-accept`, no extension, and a subprotocol only if it is one the
  request offered. Calling `connect` a second time on one `WebSocket.Client` halts.
- **Talking.** The client and a connection have the same members: `send_string(text)`, `send_buffer(buffer)`,
  `send_ping(buffer)` (125 bytes at most), `receive(): WebSocket.Message?` (`null` once closed), `close()`,
  `closed`, `close_code`, `close_reason` and `last_pong`. A `WebSocket.Message` is one holder with two reads
  ([Bytes are a Buffer](#bytes-are-a-buffer)): `is_text()`, `read_as_string(): String?` (`null` for a binary
  message) and `read_as_buffer()`.
- **Frames.** A message goes out as one frame, its length in 7, 16 or 64 bits as the protocol asks. A client masks
  every frame with 4 bytes from `SecureRandom`; a server masks none. Reading, a frame is refused (`1002`) when a
  reserved bit is set, its opcode is not continuation, text, binary, close, ping or pong, it is masked the wrong
  way for its side, its length is not written in the fewest bytes or sets the top bit of 64, or it is a control
  frame longer than 125 bytes or in pieces; a continuation with no message started, or a new message before the
  last one ended, is `1002` too. A message whose pieces would pass the connection's `largest_message` is `1009` before its payload is
  read. Text is checked as UTF-8 once whole (`1007`).
- **Pings, pongs and closes.** A ping is answered with a pong carrying the same bytes, unless this side has sent
  its close. A pong is kept in `last_pong`. A close's code must be 1000 to 1003, 1007 to 1014 or 3000 to 4999, and
  its reason UTF-8; a close of one byte is `1002`. Answering a close sends the same code back (or a close with no
  code for a close with none) and closes the connection. Before closing the connection, either way, the connection
  reads away what has already arrived, so the system does not reset the connection and throw away the close
  frame it just sent.
- **`drop()`** of a connection still open sends a close with `1001` and closes the connection.
- **What it costs**: nothing for a program that makes no `WebSocket` class; for one that does, a frame header and a copy
  of each message to send, one read of the system per frame piece, and 4 bytes from the operating system's
  secure source per frame a client sends. `tests/web_socket_exchange` (a server and a client in one
  program: text, binary of each length size, an empty message, a ping, a close, a plain request the program answers, and a
  hand-made client sending a message in pieces with a ping between them) and `tests/web_socket_failures` (every close
  code above, a refused version and a client of a server that does not speak WebSocket) are the proof.

### Pure Spite: dissolving the runtime

**The standard library is Spite, and what the compiler supplies below it is a short list of operations, not a file
someone maintains.** `Spite.Class` is an ordinary standard library class with an ordinary declaration. Nothing ties
Spite to another language: everything the compiler supplies is either a primitive operation it writes where it is
called, like `+`, or explicit Spite.

- **Purity costs no performance**: a Spite-written `trim()` goes through the same optimiser a hand-written one
  would, and is the same machine code from a better source.
- **Library classes stop being special**: they are reopenable like any class, and an inspectable build shows
  every one of them to the REPL, while a production build carries only those the program reaches.

**What is Spite**, in the standard library:

| Part | How |
|---|---|
| `String`, `List<T>`, `Dictionary<Key, Value>`, number to text | over `Buffer`. The syntax stays the compiler's (`[]`, list literals, `List<T>()`), mapped onto these classes' functions; the member templates are Spite ([collections.md](collections.md#standard-library-metaprogramming)) |
| `File`, `Directory`, `Process`, `Program`, `Console.read_line`, `Clock`, `IpAddress`, `Tcp`, `Udp`, `FileSystemWatcher`, the time-zone database | written once; each operating system's folder reopens the class with the few functions that call its own library (`ucrtbase.dll`/`kernel32.dll`, `libc.so.6`, `libSystem.dylib`) through `DynamicLibrary` |
| printing, `to_debug()`, JSON and bytes | `Console`, `Spite.Debug<T>`, `Spite.DebugInstance<T>`, `JsonWriter<T>`, `JsonReader<T>`, `BinaryWriter<T>` and `BinaryReader<T>` ([json.md](https://github.com/Spite-Language/spite_documentation/blob/master/json.md)) |

**What is the compiler's**:

1. **Memory.** Where a value lives, how it is counted and freed, the table `--debug-memory` prints, and the threads,
   scheduler, handles and locks under `Concurrent`, `Parallel` and singletons are written by the compiler for
   the program that needs them; no class of the library stands for them and a program cannot name them
   ([memory.md](https://github.com/Spite-Language/spite_documentation/blob/master/memory.md#memory-belongs-to-the-compiler)).
2. **Supplied bodies**: functions declared without a body that the compiler writes: `Console`'s `_write_output`,
   `_write_error`, `_write_held` and `_flush`, `Program`'s and `Process`'s `_flush_output`, a `Buffer`'s reads,
   writes and copies, `String.sum`, `String.byte_at` and the making of a `String` from bytes (which depend on
   whether text fits inside the `String`), the number classes' bit operations, `DynamicLibrary`'s opening, closing
   and symbol lookup, and `Spite.Attribute`'s and `Spite.Function`'s dispatch.
3. **The object header**: retain, release and the class id are code the compiler emits, not functions anyone
   calls. For `String` that includes the two decisions only the header can make: a `'constant'` text is never
   counted, and `text = text + piece` grows the text in place only when its block's count is one, or inside the
   `String` itself while it is 15 bytes or fewer.
4. **Entry and exit**: the start of the program, and text literals, which are static data in the `'constant'`
   section, as symbols are.

**WebAssembly.** There, memory grows linear memory and the system library is the JavaScript host's. **The web shim is
the one honest exception, and its target is zero hand-written lines.** A file that runs in the JavaScript virtual
machine cannot be Spite by construction: it is on the far side of a boundary Spite does not own. What is achievable
is that nobody writes it: the imports come from `external js` declarations and the command-buffer drain loop is a
switch over an opcode table, both data the compiler can emit, so the source of truth stays Spite and nothing is
written in a second language ([targets.md](https://github.com/Spite-Language/spite_documentation/blob/master/targets.md)).

### examples/calculator

A recursive-descent arithmetic interpreter (`token.spite`, `lexer.spite`, `expression.spite` + one file per union
member, `parser.spite`, `evaluator.spite`, `calculator.spite`), reading its program from `program.txt` next to it.
Supports `+ - * /`, unary `-`, parentheses, variable assignment (`x = 1 + 2`) and reference, one statement per
line. The lexer scans `source.split("")` with a `while` loop that passes each character and its index on (so it
is not a loop a member template replaces), keeping the current token's start as an attribute and slicing
it out on a token boundary; the parser is recursive, including precedence climbing written as tail recursion
(`parse_expression_rest`/`parse_term_rest`). The parser makes each `BinaryExpression(...)` on a line
of its own and hands it on by name, since no constructor is an argument:
`left_expression`/`right_expression` are ordinary by-reference parameters, stored directly into the
union-typed attribute.

---

Next: [Lists, dictionaries and member templates](collections.md).
