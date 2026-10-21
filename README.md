# SpiteSpec

The tests of the new Spite compiler, kept out of the compiler's own repository so that they ship as a
repository of their own, with the tests up to date, instead of mixed in with the compiler.

The compiler is in `.`. This folder holds the programs it is run on.

## The shape

Two flat folders. Each subfolder is one test with everything it needs, and it runs on its own in a moment, so a
change is checked by the tests it touches and not by one long run of everything.

    tests/<name>/<name>.spite                 a program that must compile and run
    tests/<name>/expected_exit_code.txt       the exit code it must leave
    conformance/<name>/<name>.spite           a program that must fail to compile
    conformance/<name>/expected_errors.txt    the errors it must fail with, word for word

A test shares no file with another, so it can be read, changed or moved without touching the rest. A
conformance program ported from the old repository keeps the old error text exactly: the new compiler must
write the same words, and a conformance test that fails is a part of the port still to do.

## The format

A program names the standard library in its first line, because there is no launcher any more and the
compiler does not go looking for a library:

    func ExitCode(): Integer {
        load "../../../SpiteCompiler/.spite/library/common"
        ...

The path is relative to the folder of the program that holds the load. The junction
`.spite/library` points at the new standard library, so a program that loads it reads the new one. The
old library is still there for the old compiler, and `whole_library` is the one program that still
names it.

## Running them

The runner is a Spite program, `runner/runner.spite`. Run from this folder:

    ../SpiteCompiler/.spite/spite-0.0.6-windows/spite.exe runner
    ../SpiteCompiler/.spite/spite-0.0.6-windows/spite.exe runner tests/sum conformance/duplicate_function

With no names it runs every test; with names it runs only those. It compiles each one with the new compiler,
`../SpiteCompiler/.spite/spite.exe`, prints `FAILED <folder>: <why>` for each that fails, then how many passed
and failed, and leaves 1 when any failed. The old compiler builds the runner until the new one can.

One program by hand:

    ../SpiteCompiler/.spite/spite.exe tests/exit_code
    tests/exit_code/.spite/build/exit_code.exe
