# Programs: the entry and settings

The specification of [Programs: the entry and settings](https://github.com/Spite-Language/spite_documentation/blob/master/programs.md): every rule of this part of the language, with
its edge cases and its exact error texts. The page teaches; this one decides, and where they disagree, the page
is the bug.

## Constructors and the entrypoint

There is no `main`: a program is a folder, its entry file is the file named after the folder (`kal/kal.spite`,
class `Kal`), and the program is run by constructing that class; when the constructor returns, the program
ends with exit code `0`. A program is its folder, the one the compiler runs in, and a folder with no entry file named after it is an error naming
the file looked for. No other file may reopen the entry class
([Packages, namespaces and loading](packages.md#packages-namespaces-and-loading)).

The entry constructor takes **no arguments**: `'Kal' is the entry constructor, and it takes no arguments:
a program reads its run-time settings through Environment() (declared in its environment.spite, given on the
command line beside the compiler's flags), and what its build decided through Build(), anywhere`
(`diagnostics/entry_constructor_arguments`).

```gdscript kal.spite
var console = Console()
var environment = Environment()

func Kal() {
    console.print(environment.some_key)
}
```

**Every value on a command line is a named setting** (D1775): `--name=value`, or a bare `--name` for a
`Boolean`, reaching the program only as the `Environment` or `Build` attribute it sets (D1760). Nothing is given
by position and nothing is read by its text: `Program` has no `arguments()`, `argument_named` or `has_flag`, and
a word, a bare `--` or a flag the program declares no setting for is not read
(`tests/program_flags_and_settings`, `tests/programs_arguments_beside_settings`).

**Nothing starts a program behind its back.** The program is the entry point: there is no launcher. The compiler
reads the entry file, and the leading `load` lines of its entry function name the standard library, very explicitly
(the folder for the system the program is compiled for, which loads `common` itself), then follows every `load` in
order. The compiler looks nowhere for a library and no environment variable names one; a name that resolves to
nothing is an error. See [The standard library is a folder loaded by its path](packages.md#packages-namespaces-and-loading).

- A `load` names its folder with literal text: `a program's 'load' names its folder with text, like 'load "engine"':
  the compiler reads every loaded folder before it compiles, so the folder cannot be computed`; it may sit under an
  `if` on a `Build` field ([`load` is a bundle boundary](https://github.com/Spite-Language/spite_documentation/blob/master/packages.md#load-is-a-bundle-boundary)).
  A `load` compiles to nothing at run time.
- The program's own folder is the root its entry file sits in, and constructing the entry class is what runs it;
  with `--repl` a loop runs on it when asked, and it is released afterwards.
- The executable keeps only the floor: it leaves the command line to `Environment()`, constructs the entry class, and
  afterwards releases the singletons and class objects and prints the `--debug-memory` balance.
- The exit code is the value the entry constructor answers; a constructor that answers nothing leaves it at 0.

**Configuration is a class.** There is no configuration file format: a configuration is an ordinary class whose
constructor sets its state, or a field of `Environment` or `Build`.

## Program settings: `Environment`

`Environment` is a standard library singleton, and a program
**reopens** it ([Packages, namespaces and loading](packages.md#packages-namespaces-and-loading)) with a
file of its own named `environment.spite` to declare the settings it has, one `var` per setting. `Environment()`
works anywhere. Only the declared fields are read: nothing the program does not name is loaded, which makes it a
deterministic replacement for packages like Node's dotenv.

What follows is how it is compiled:

- **Where a value comes from.** Each field is read once, when the singleton is first made, from the first of:
  the program's own command line, `--serve=true` or, for a `Boolean`, bare `--serve` (what the program
  receives, see [Command line](compiler.md#command-line)); the process environment variable named by the field
  in upper case, `SERVE`; a `.env` file the program loaded (below); the declared default. The command line wins
  because it is the more deliberate, and the process environment wins over a file, as dotenv does. An argument
  that names no field stops the program as `Environment` is first made, with exit code 1 after `error: '--colour'
  names no setting of Environment: declare it in environment.spite, like 'var colour = ""', and the program reads it
  as Environment().colour` (`tests/environment_undeclared_flag`), and so does a word given by position, `error:
  'red' is given by position: every value on the command line is a named setting, like --name=red, declared in
  environment.spite as 'var name = ""'` (`tests/environment_word_by_position`): every value is a declared setting
  (D1812). There is no `unread_settings()`. The compiler is the one program that reads the flags its `Environment`
  does not declare, as the `Build` flags of the program it compiles ([compiler.md](compiler.md#flags-and-settings)):
  its `environment.spite` declares the private `_flags`, which the compiler fills with every flag given in its
  `_read_settings`, and then no flag is refused (D1813, D1815).
- **A `.env` file.** `environment.load_from_file(path): Boolean` reads a file of `NAME=value` lines (spaces
  around both trimmed, blank lines and lines starting with `#` skipped) and reads every setting again, so a
  setting neither the command line nor the process environment gives takes the file's value
  (`tests/environment_file_settings`). A name the file gives that no setting declares is reported on the error
  stream, `warning: 'UNKNOWN_THING' in .env names no setting of Environment, so it was not read`, and the rest
  is read. A file that cannot be read answers `false` and changes nothing (`tests/environment_file_missing`).
  The value a file gives is checked like any other (`tests/environment_file_malformed`), and a process
  environment variable is read the same way (`tests/environment_variable_setting`).
- **A setting is written like a compiler flag**: kebab-case on the command line, snake_case in code,
  `--worker-count=4` for `var worker_count = 1` (`conformance/stage6/kebab_setting`). The compiler refuses the
  snake_case spelling before anything is read (`error: '--worker_count' is written '--worker-count': flags and
  settings are kebab-case on the command line, and the field they set is 'worker_count'`), and so does the built
  program run directly: `--worker_count=4` for a declared `worker_count` stops it as it reads its settings,
  before anything else runs, with `error: '--worker_count' is written '--worker-count': a program's setting is
  kebab-case on the command line, like the compiler's flags, and it sets Environment.worker_count`, exit code 1.
  The check is one pass over the arguments when `Environment` is first made, and only in a program that
  declares a setting.
- **One set of names.** A program's `Build` field is given when it is compiled and its `Environment` field when
  it runs (D1813), both written the same way and both read from the same variable, so an `Environment` field
  named like a field the program's `build.spite` declares is a compile error naming both
  (`diagnostics/setting_named_like_flag`): rename one. The compiler's options are flags of the compiler's own
  `Environment`, so a program's setting may share their names.
- **The default's literal is the type.** A setting is declared with nothing but a literal default: `false`/`true`
  is a `Boolean`, a whole number (negative too) an `Integer`, `""` a `String`. A setting of an enum the
  `Environment` declares writes its type, `var mood: Mood = 'calm'`, and takes only that enum's value names
  (`tests/environment_enum_setting`); any other text stops the program with `error: the setting Environment.mood
  was given 'happy', which is not one of its values: calm, angry: write --mood=calm on the command line, or
  MOOD=calm in the environment or a .env file` (`tests/environment_enum_refused`, D1812). Any other type
  annotation, or any other default, is a compile error naming the forms: `'Environment.workers' is a setting, read from the
  program's command line (--workers=value) or its environment (WORKERS), so it is declared with only a literal
  default, and the literal is its type: 'var workers = false' (Boolean), 'var workers = 0' (Integer) or 'var
  workers = ""' (String), or one value of an enum the Environment declares, its type written: 'var workers: Kind
  = ...'` (`diagnostics/environment_setting`). Text that is not a value of the setting's type
  (`--serve=maybe`, `--workers=many`, a bare `--name` for a `String`) is a compile error when it is given to the
  compiler (`diagnostics/setting_value`). In a program run directly, wherever the value came from (the command
  line, the process environment or a `.env` file), it stops the program with exit code 1 after an error naming
  the setting, the value and the form it takes, since there is no line of the program to point to (D1760):
  `error: the setting Environment.workers was given 'many', which is not an Integer, a whole number: write
  --workers=4 on the command line, or WORKERS=4 in the environment or a .env file` (`tests/programs_setting_value_run`,
  `tests/environment_bare_text_setting`).
- **How the fields get filled.** The compiler writes one assignment per declared field into `Environment`'s
  `_read_settings`, which the constructor and `load_from_file` call, `serve = boolean_setting("serve", serve)`, where `boolean_setting`, `integer_setting` and
  `text_setting` are ordinary Spite functions of `Environment`. The reading itself is Spite; the
  compiler only writes the calls, the way it writes a template
  (`conformance/stage6/environment_settings`).
- **The compiler passes no setting on.** `spite --serve=true` is refused, since `serve` is no flag of the
  compiler's `Environment` and no field of the program's `Build`: a program's settings are given to its
  executable, and the program the compiler runs reads them from the environment (`SERVE=true`) or a `.env` file
  (D1812, D1813). There is no `--` separator (`diagnostics/separator_flag`).
- **Run-time cost**: one pass over the command line and one environment lookup per declared field, once,
  when `Environment()` is first made. The compiler makes `Environment()` as the entry's constructor starts, so a
  program that never makes it still reads and checks every flag: one no setting declares, a word given by
  position or a value not of its setting's type stops it as above (`tests/program_without_environment_refuses_flag`,
  `tests/program_without_environment_refuses_word`, `tests/program_unmade_environment_reads_flag`, D1820). A
  program that loads no standard library has no `Environment` and reads no argument.
- **A `--hot-reload` build reads the settings again after a reload**: a reload that changes
  `environment.spite` moves `Environment` to its new attributes and reads every setting again, as when the program
  starts, from the command line it was started with, then the environment, then the declared default. To read
  the declared default again, the compiled reading passes the default itself in such a build
  (`serve = boolean_setting("serve", false)`) rather than the field. See
  [repl.md](https://github.com/Spite-Language/spite_documentation/blob/master/repl.md#what-a-reload-can-change).

## Build settings: `Build`

`Environment` (above) is read when the program **runs**; `Build` (a singleton of the standard library) is decided
when it is **compiled**. Every compiler option is a `Build` field
with a literal default, and a program adds its own the same way it adds `Environment` settings, by reopening
`Build` in a file named `build.spite`. What follows is how it is compiled:

- **Every field is a constant.** A `--name=value` naming a `Build` field sets it; a field nobody
  sets keeps its declared default: the program's own `build.spite` if it reopens it, else a loaded package's,
  else the standard library's `Build`.
  Either way the value is written into the program: `build.serve` compiles to `true`, a condition on it is decided
  while compiling, the branch not taken is never generated, and nothing is read when the program runs. A default
  may be arithmetic on literals (`var seconds = 2 * 60`, `var offset = -3`), which is folded the same way, or
  another field of the same type (`var attempts = retries`), which is that field's constant, the command line's
  value included (`tests/build_default_reads_a_field`); a default that reads anything else is evaluated once, when `Build` is made (`tests/build_constant_defaults`). The field
  still exists on the `Build` singleton with that value, for reflection.
  The fields that say what one compile writes (`check`, `build`, `executable_path`, `final_classes`,
  `optimization_report`, `high_ir`, `low_ir`) are the exception: a flag sets what the compile does, and the program
  keeps the declared value, so a `--hot-reload` program's reload, which writes elsewhere, changes nothing of `Build`.
  **In a `--hot-reload` build**, a field the program or a package declares is read by the program's own code
  from the `Build` singleton while it runs instead, and both branches of a condition on it are compiled, so a
  reload that changes `build.spite` swaps the new value in; the standard library's reads of the fields its own
  `Build` declares (the compiler's options among them) stay constants, so `build.hot_reload` in its `Reload` is
  `true` and only the branch it takes is built (`tests/hot_reload_keeps_library_options`) ([repl.md](https://github.com/Spite-Language/spite_documentation/blob/master/repl.md#what-a-reload-can-change)).
- **The program decides its own `Build`**: a field the program's own `build.spite` declares keeps the
  program's default even when a loaded package's `build.spite` declares the same field, although the package is
  merged later; this is the one exception to "a later root replaces" ([packages.md](packages.md#packages-namespaces-and-loading)).
  The order is a flag, then the program, then the package, and a flag sets a field only a package declares as it
  sets any other; a field only one side declares is unchanged (`conformance/stage6/build_precedence`). A field
  that decides a `load` is still read before any package is loaded, so it must be the program's or the library's.
- **Flags are kebab-case**: `--repl-port=4000` sets the field `repl_port`, and a flag written with `_` is
  an error before anything is read, naming the hyphen form ([compiler.md](compiler.md#flags-and-settings),
  `diagnostics/underscore_flag`).
- **The compiler's options are fields**: `check`, `build`,
  `final_classes` and `optimization_report`, the path `executable_path`, and `optimized`, `development`, `repl`, `repl_port`,
  `hot_reload` and `debug_memory` ([Command line](compiler.md#command-line)). The compiler reads them from the
  program's resolved `Build`, after the whole program is read, so a program whose `build.spite` says
  `var optimized = true` is built optimized unless `--optimized=false` is given (`tests/build_settings_read`).
  `check` and `build` both on, from either place, is the error both flags on are
  (`conformance/build_check_and_build`), and `hot_reload` set there builds for live reload as the flag does. Since the compiler
  reads them before anything runs, each of its options is declared with only a literal (`true` or `false`, a
  whole number, or a text without holes); anything else is an error at the declaration:
  `build.spite:1: error: 'Build.optimization_report' is one of the compiler's options, which the compiler reads
  while compiling, before anything runs, so it is declared with only a literal: true or false, a whole number, or
  a text without holes, like 'var optimization_report = ...' in build.spite` (`conformance/build_option_literal`).
- **Formatting is not an option**: every compile formats first and nothing turns it off, so `--format`
  and a `format` field in a program's `build.spite` are compile errors (`diagnostics/format_flag`,
  `diagnostics/format_setting`).
- **`target_operating_system` comes from the flag alone**: it decides the system the program is compiled for before
  the rest is read. A `var target_operating_system = ...` (or `operating_system`) in
  a program's `build.spite` naming another system than the one the compile decided is an error naming the flag
  (one naming the same system, as `--final-classes` prints every `Build` field, is accepted): `'Build.target_operating_system' cannot be declared by a
  program: the standard library for the target system is loaded before the program, so the target is given to
  the compiler, --target-operating-system=linux (or windows, or mac), and is the system doing the compiling
  otherwise` (`diagnostics/target_in_build`). The system folders of the standard library
  still set it.
- **A `Boolean` may be given bare**: `--optimized` is `--optimized=true`. Any other bare flag is an error
  naming the value it needs.
- **Nothing passes silently.** A value that is not of the field's type is a compile error: `'--workers=many' was
  given to the compiler, but 'Build.workers' is an Integer: a whole number, like --workers=3`
  (`diagnostics/build_setting_type`); a flag naming an `Environment` field is the error above; a flag naming no
  field is an error listing the fields `Build` has: `'--serve' is not a compiler option, and this program's Build
  declares no setting 'serve' (it declares run, executable, ...)` (`diagnostics/unknown_compiler_flag`).
- **Two operating systems**: `operating_system` is the system doing the compiling (the compiler supplies
  it, and giving it is an error), and `target_operating_system` is the one the program is compiled for, which
  defaults to it. `--target-operating-system=linux` is the system the program is compiled for, which the program loads
  by its path ([above](#constructors-and-the-entrypoint)), and it folds, so `if build.target_operating_system == "windows" { }`
  keeps one branch. Each system's folder reopens `Build` with `var target_operating_system = "linux"`, which is
  only the default for a program that is read without the flag. The compiler
  learns which system it runs on from its own `build.target_operating_system`, folded when it was built.
- **`program`** is the full path of the folder the compiler built, the one it runs in (`D:\games\loaded_program`
  for `cd D:\games\loaded_program` then `spite`); the compiler supplies it and giving it is an error
  (`tests/loaded_program`).
- **`--final-classes` prints `Build` with its declared defaults**, not the values one build folded, so running the
  printed program takes its flags again instead of, say, printing its classes forever.
- **Run-time cost**: none. `Build` holds nothing at run time, since every field is folded, so in a
  production build it is one static object with no code behind it, and in an inspectable build an ordinary
  singleton ([Singletons](classes_and_files.md#singleton-rules)); choosing and writing the outputs is the
  compiler's work, not the program's.

---

Next: [Values and types](values_and_types.md).
