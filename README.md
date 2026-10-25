# SpiteSpec

The tests of the new Spite compiler, kept out of the compiler's own repository so that they ship as a
repository of their own, with the tests up to date, instead of mixed in with the compiler.

The compiler is [spite_compiler](https://github.com/Spite-Language/spite_compiler). This repository holds the programs it is run on, and [`specs/`](specs/README.md), the rules those programs check, written out as Markdown.

## The shape

Two flat folders. Each subfolder is one test with everything it needs, and it runs on its own in a moment, so a
change is checked by the tests it touches and not by one long run of everything.

    tests/<name>/<name>.spite                 a program that must compile and run
    tests/<name>/expected_exit_code.txt       the exit code it must leave
    tests/<name>/expected_output.txt          what it must print, both streams together (optional)
    tests/<name>/input.txt                    what it reads from the console (optional)
    tests/<name>/flags.txt                    the compiler's flags after the folder (optional)
    tests/<name>/arguments.txt                what the program is run with (optional)
    conformance/<name>/<name>.spite           a program that must fail to compile
    conformance/<name>/expected_errors.txt    the errors it must fail with, word for word

A test shares no file with another, so it can be read, changed or moved without touching the rest. A
conformance program ported from the old repository keeps the old error text exactly: the new compiler must
write the same words, and a conformance test that fails is a part of the port still to do.

## The format

A program names the standard library in its first line, because there is no launcher any more and the
compiler does not go looking for a library:

    func ExitCode(): Integer {
        load "../../../spite_standard_library/common"
        ...

The path is relative to the folder of the program that holds the load. The repositories sit side by side
under one folder, as `gh repo clone` names them (`spite_compiler`, `spite_standard_library`, `spite_spec`), so
`../../../spite_standard_library` is the library from any `tests/<name>` or `conformance/<name>`, with no link to
make.

## Running them

The runner is a Spite program, `runner/runner.spite`, built by the compiler it runs the tests with (build
the compiler first with `bash scripts/bootstrap.sh --install` in `../spite_compiler`). Run from this folder:

    ../spite_compiler/.spite/spite.exe runner --build
    runner/.spite/build/runner.exe
    runner/.spite/build/runner.exe tests/sum conformance/duplicate_function

With no names it runs every test; with names it runs only those. A name ending in `*` runs every folder that
starts with it (`tests/float*`), and `--failed` runs again only the tests that failed in the last run, so the
loop while fixing something is the tests it touches, not the whole corpus. It compiles each one with the new compiler,
`../spite_compiler/.spite/spite.exe`, prints `FAILED <folder>: <why>` for each that fails, then how many passed
and failed, and leaves 1 when any failed.

One program by hand:

    ../spite_compiler/.spite/spite.exe tests/exit_code
    tests/exit_code/.spite/build/exit_code.exe
