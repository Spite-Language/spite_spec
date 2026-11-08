# SpiteSpec

The tests of the new Spite compiler, kept out of the compiler's own repository so that they ship as a
repository of their own, with the tests up to date, instead of mixed in with the compiler.

The compiler is [spite_compiler](https://github.com/Spite-Language/spite_compiler). This repository holds the programs it is run on, and those programs are the specification of Spite: a rule is
stated by the program that fails when it breaks, not by prose.

## Where a rule lives

- **A behaviour** (what a program prints, answers or halts with) is a `tests/<name>` program.
- **A refusal** (a program the compiler must reject) is a `conformance/<name>` program, with the exact words of
  its error in `expected_errors.txt`. One fault per program, since a compile reports its first error.
- **How to write Spite**, the rules and limits a person needs and what an error means, is taught by
  [spite_documentation](https://github.com/Spite-Language/spite_documentation/blob/master/README.md), one page per
  part of the language; its titled code blocks run as tests too.
- **What an agent needs to write Spite** is in the documentation's
  [skill](https://github.com/Spite-Language/spite_documentation/blob/master/skills/spite/SKILL.md) (`SKILL.md`,
  `reference.md`).
- **Why** a rule is what it is, is its row in
  [DECISIONS.md](https://github.com/Spite-Language/mortaros_spite/blob/master/DECISIONS.md).

A change to a rule changes its program here and its docs page, the skill included, in the same push. Where a page
and a program disagree, the program decides and the page is the bug. There is no Markdown specification here any
more: it was retired once every rule it stated had its program or its docs page (spite_compiler#24).

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
        load "../../../spite_standard_library/windows"
        ...

The path is relative to the folder of the program that holds the load. The repositories sit side by side
under one folder, as `gh repo clone` names them (`spite_compiler`, `spite_standard_library`, `spite_spec`), so
`../../../spite_standard_library` is the library from any `tests/<name>` or `conformance/<name>`, with no link to
make.

## Running them

The runner is a Spite program, `runner/runner.spite`, built by the compiler it runs the tests with (build
the compiler first with `bash scripts/bootstrap.sh --install` in `../spite_compiler`). Run from this folder:

    (cd runner && ../../spite_compiler/.spite/spite.exe --build)
    runner/.spite/build/runner.exe
    runner/.spite/build/runner.exe --tests=tests/sum,conformance/duplicate_function

With no `--tests` it runs every test; `--tests` names the ones to run, separated by commas. A name ending in `*`
runs every folder that starts with it (`--tests=tests/float*`), and `--failed` runs again only the tests that failed in the last run, so the
loop while fixing something is the tests it touches, not the whole corpus. It compiles each one with the new compiler,
`../spite_compiler/.spite/spite.exe`, run inside a copy of its folder, prints `FAILED <folder>: <why>` for each that fails, then how many passed
and failed, and leaves 1 when any failed.

The tests run in parallel, as many at once as the machine has processors; `--jobs=4` sets how many. Each has
300 seconds to compile and run, and `--time-limit=60` sets them: a test still running then is stopped, with
every process it started, and fails with `stopped after 300 seconds, the program did not finish`, so a hang
fails one test and never stops the run. Each test runs in a process of its own, the runner itself started with
`--one-test=<folder>`, which is also how to run one test with nothing else around it.

One program by hand (the compiler builds it and runs it, or only builds it with `--build`):

    cd tests/exit_code
    ../../../spite_compiler/.spite/spite.exe
    ../../../spite_compiler/.spite/spite.exe --build
    .spite/build/exit_code.exe
