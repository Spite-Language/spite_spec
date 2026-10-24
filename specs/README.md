# Spite specification

This folder is the formal specification of Spite: every rule of the language, with its edge cases and the exact
text of every error. [The documentation](https://github.com/Spite-Language/spite_documentation/blob/master/README.md) is the tutorial, which teaches the language with
programs that run; each page of the specification states in full the part of the language one page of the
documentation teaches, and has that page's name.

**The specification is normative.** Where a page of the documentation and the specification disagree, the
specification decides, and the documentation is the bug. A rule is stated once, here, on the page for its part of
the language; the documentation teaches it and links here for the edge cases and the error texts.

Read the documentation first. Come here to know exactly what the compiler accepts, what it refuses and with what
words, and how a rule is compiled.

## Pages

In the documentation's reading order. Four pages of the documentation have none here:
[getting_started.md](https://github.com/Spite-Language/spite_documentation/blob/master/getting_started.md) is a guide, [optimizations.md](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md) and
[proofs.md](https://github.com/Spite-Language/spite_documentation/blob/master/proofs.md) are catalogues that summarise and link the rules stated on these pages, and
[targets.md](https://github.com/Spite-Language/spite_documentation/blob/master/targets.md) states its few rules where it teaches them.

1. [Write it plainly: the compiler decides how it runs](write_it_plainly.md), specifying [write_it_plainly.md](https://github.com/Spite-Language/spite_documentation/blob/master/write_it_plainly.md).
2. [Classes and files](classes_and_files.md), specifying [classes_and_files.md](https://github.com/Spite-Language/spite_documentation/blob/master/classes_and_files.md).
3. [Programs: the entry, the launcher, and settings](programs.md), specifying [programs.md](https://github.com/Spite-Language/spite_documentation/blob/master/programs.md).
4. [Values and types](values_and_types.md), specifying [values_and_types.md](https://github.com/Spite-Language/spite_documentation/blob/master/values_and_types.md).
5. [Nullable values and failure](failure.md), specifying [failure.md](https://github.com/Spite-Language/spite_documentation/blob/master/failure.md).
6. [Functions and operators](functions_and_operators.md), specifying [functions_and_operators.md](https://github.com/Spite-Language/spite_documentation/blob/master/functions_and_operators.md).
7. [Control flow](control_flow.md), specifying [control_flow.md](https://github.com/Spite-Language/spite_documentation/blob/master/control_flow.md).
8. [Style: the compiler is the formatter and the linter](style.md), specifying [style.md](https://github.com/Spite-Language/spite_documentation/blob/master/style.md).
9. [Memory](memory.md), specifying [memory.md](https://github.com/Spite-Language/spite_documentation/blob/master/memory.md).
10. [Metaprogramming](metaprogramming.md), specifying [metaprogramming.md](https://github.com/Spite-Language/spite_documentation/blob/master/metaprogramming.md).
11. [Reflection](reflection.md), specifying [reflection.md](https://github.com/Spite-Language/spite_documentation/blob/master/reflection.md).
12. [Packages, namespaces, and mods](packages.md), specifying [packages.md](https://github.com/Spite-Language/spite_documentation/blob/master/packages.md).
13. [Concurrency: waiting without colouring](concurrency.md), specifying [concurrency.md](https://github.com/Spite-Language/spite_documentation/blob/master/concurrency.md).
14. [Standard library](standard_library.md), specifying [standard_library.md](https://github.com/Spite-Language/spite_documentation/blob/master/standard_library.md).
15. [Lists, dictionaries and member templates](collections.md), specifying [collections.md](https://github.com/Spite-Language/spite_documentation/blob/master/collections.md).
16. [JSON and binary](json.md), specifying [json.md](https://github.com/Spite-Language/spite_documentation/blob/master/json.md).
17. [Time](time.md), specifying [time.md](https://github.com/Spite-Language/spite_documentation/blob/master/time.md).
18. [Game maths](game_maths.md), specifying [game_maths.md](https://github.com/Spite-Language/spite_documentation/blob/master/game_maths.md).
19. [Foreign libraries and operating systems](foreign_libraries.md), specifying [foreign_libraries.md](https://github.com/Spite-Language/spite_documentation/blob/master/foreign_libraries.md).
20. [Compiler command line](compiler.md), specifying [compiler.md](https://github.com/Spite-Language/spite_documentation/blob/master/compiler.md).
21. [REPL and live reload](repl.md), specifying [repl.md](https://github.com/Spite-Language/spite_documentation/blob/master/repl.md).
22. [Testing](testing.md), specifying [testing.md](https://github.com/Spite-Language/spite_documentation/blob/master/testing.md).

---

Start with [Write it plainly: the compiler decides how it runs](write_it_plainly.md).
