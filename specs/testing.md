# Testing

The specification of [Testing](https://github.com/Spite-Language/spite_documentation/blob/master/testing.md): every rule of this part of the language, with
its edge cases and its exact error texts. The page teaches; this one decides, and where they disagree, the page
is the bug.

## How testing works

- **A test is a function that crashes, not a framework.** Its name is `test` or starts with `test_`, it takes no
  arguments, and an expectation is `crash <condition>`, which halts on a false condition and reports the
  condition's source text with every operand's value ([What a crash reports](https://github.com/Spite-Language/spite_documentation/blob/master/failure.md#what-a-crash-reports)).
  There is no `expect` and no matcher vocabulary. A value a test returns is dropped.
- **A project keeps one entry point; its tests are a separate program** that `load`s it (D1768), so tests are
  never part of the program a hot reload rebuilds. The library's `Test` class runs them:
  `var tester = Test(namespace)` takes a namespace (`Test(Checks)`, a folder's namespace named alone or by its
  dotted path), and `tester.run()` runs every test in it (`tests/test_class_passing`).
- **Discovery is reflection**: `run()` walks the namespace's `.classes`, then each of its `.namespaces` the same
  way, and in each class its `.functions` in the order they are declared, keeping those named `test` or
  `test_...` (`testing_is_not_a_test` is not one). Nothing is registered.
- **The constructor is the set-up**: each test runs on an instance made by calling the walked class with no
  arguments, which runs its constructor and its attributes' defaults, so a class sets up what its tests share
  there (`tests/test_class_setup`). The test is called through `Spite.Call(declaration, instance)` and
  `call.call()`.
- **Each test runs on its own**: `run()` starts the program again (`Program().executable_path()`) once per test,
  with the environment variable `SPITE_TEST` holding the test's dotted name (`Bank.AccountTests.test_withdraws`).
  That run builds the class and calls only that test, in place, printing nothing of its own, and its status is the
  test's: 0, or the crash's. So no test sees the state another left, and a crash fails only its test.
- **The report**: for each test, `pass <name>` or `fail <name>` (a run that ended with any status other than 0),
  then every non-empty line its run printed, both streams, trimmed and indented by four spaces; then
  `<count> tests, <passed> passed, <failed> failed`. The program then exits with status 1 when any test failed,
  0 otherwise (`tests/test_class_failing`, whose failing test is followed by one that still runs).
- **The program's entry class has no stand-in instance**, so its `.functions` is empty
  ([reflection.md](https://github.com/Spite-Language/spite_documentation/blob/master/reflection.md)): tests go in
  classes of a namespace of their own. Loading another program to test it does not run that program's entry.

---

Back to [the specification](README.md).
