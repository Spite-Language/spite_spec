# Control flow

The specification of [Control flow](https://github.com/Spite-Language/spite_documentation/blob/master/control_flow.md): every rule of this part of the language, with
its edge cases and its exact error texts. The page teaches; this one decides, and where they disagree, the page
is the bug.

## Control flow in full

```gdscript
if condition { } else { }
if nullable_value { } else { }     # runs with nullable_value unwrapped in place when it is not null; else when it is
while condition { }
switch enemy {
    Player: enemy.hurt()
    Monster: { enemy.die() }
}
```

An `if` on a `T?` narrows the value in place ([Null safety and `fallback` narrowing](https://github.com/Spite-Language/spite_documentation/blob/master/failure.md#narrowing)): the block runs with `value` as a plain `T`,
and the `else` runs exactly when it is null/absent: one rule for narrowing everywhere, `fallback`/`if`/`switch`
alike. `do` is not a keyword ([Lexical structure](classes_and_files.md#lexical-structure)). The terminal-`if` lint
([The last `if` of a function](https://github.com/Spite-Language/spite_documentation/blob/master/failure.md#the-last-if-of-a-function)) applies only when the `if` has no `else`:
one with an `else` already handles the missing case explicitly, so there is nothing left to rewrite with
`fallback`. `else if` is written on one line; the formatter joins an `else { if ... }` into it.

**A statement ends with its line.** Anything left on the line
after a statement is a parse error at the first word left over, so nothing written there is silently dropped or
read as a second statement: `return every attribute at its default` is `'attribute' is left over after the end of
the statement: a statement ends with its line, so remove it, or put it on a line of its own if it is a statement`
(`diagnostics/left_over_after_return`). An `if`, `while` or `switch` ends with the `}` that closes it (after its
`else` chain, for an `if`), and a word after that `}` on the same line is `'console' is left over after the '}'
that closes this statement: ...` (`diagnostics/left_over_after_block`).

**A local declared only to be returned is a compile error.** A `var name = value` or `var name: T = value`,
with `T` written as the function's result type, followed at once by `return name`, is `'name' is declared only to
be returned on the next line: write 'return value', since 'return' already makes the value the function's T`
(`diagnostics/returned_local`). A local declared with any other type is a conversion of its own and is not
refused.

**A loop states the condition it runs under: `while true` is a compile error** (D1746), like any condition the
compiler knows is always true: `'while true' is decided while compiling and is always true here: a loop ends by its
condition, so loop 'while' what its leaving paths change, a 'running' local they set to false or the loop's real exit
test` (`conformance/spinning_loop`). A loop that waits for a value loops `while not found` and assigns it; one that
runs until something happens loops `while running` and sets `running = false` where it ends
([failure.md](failure.md#nothing-fails-silently-the-rule)).

There is deliberately no `break`/`continue`: `while` is the only loop construct Spite has, full stop.
Neither is a keyword, but either written as a statement of its own is an error naming the form (`Spite has no
'break': a loop stops in its own condition, like 'while index < count and not found', ...`,
`diagnostics/old_break`), and both stay usable as names. An
early exit says so in the loop's own condition: a flag (`var stopped = false` ... `while not stopped { ... }`)
or the bound itself (`while index < words.count() and words[index] != "stop"`).

`while` is the only loop. There is no `for`, so that people favor metaprogramming: reach for `List<T>`/`Dictionary<Key, Value>` metaprogramming
([Member templates](https://github.com/Spite-Language/spite_documentation/blob/master/collections.md#member-templates-loops-you-do-not-write),
[Passing a function for each element](https://github.com/Spite-Language/spite_documentation/blob/master/collections.md#passing-a-function-for-each-element)) first, and index with
`while index < list.count() { }` when a loop is genuinely needed. Writing `for` is a parse error rather than
silently doing something else: "Spite only has 'while' loops; there is no 'for'. Use List<T>'s metaprogramming
helpers or 'while index < list.count() { ... }' instead." Loops and ifs are otherwise discouraged in application
code.

`while` costs nothing at run time beyond the loop itself, with one exception in debugging builds: in a
`--repl-port` or `--hot-reload` build, each pass of a `while` in the program's own code (not `library/` or
`launcher/`) ends with a check point that answers a waiting REPL command or reload
([repl.md](https://github.com/Spite-Language/spite_documentation/blob/master/repl.md#remote---repl-port)); every other build has none. A local `--repl` build gets none either: it
reads its commands from the console only after the entry constructor returns, so a check point there would have
nothing to answer and would cost a call per pass for nothing.

### Nested `if`/`else`

**An `if` with an `else`, directly inside a branch of another `if` with an `else`, is a compile error.**
The message names the fix: "this 'if'/'else' is inside a branch of another 'if'/'else': move it into a function
named for what it decides, or, when both test which member of a union a value is, use one 'switch'"
(`diagnostics/nested_if_else`). An `else if` link counts as a branch of its chain, so an `if`/`else` inside the
body of an `else if` is caught too. Not counted: a flat `else if` chain; an `if` without an `else` inside a
branch; and an `if`/`else` inside a `while` or `switch` inside the branch, since the loop or the switch is the
unit. The message names "a union or an enum", since `switch` takes an enum's values.
Compile time only.

### An `if` that only holds an `if`

**An `if` with no `else` whose body is exactly one `if` with no `else` is a compile error that names the `and`
form.** `if modified { if size { return "{modified}-{size}" } }` is `if modified and size { ... }`. The message:
"this 'if' only contains another 'if' and neither has an 'else': write one condition, 'if modified and size'".
Not counted: an `if` with other statements beside the inner one, and an `if` or an inner `if` with an `else` or
an `else if` (that is a decision tree, [above](#nested-ifelse)). Compile time only.

### One statement per case

A case of a `switch` is one statement: a call, an assignment, a `return` or a `crash`, on the case's line, or the same
one statement inside `{ }`. A case of two statements or more is an error: "this case runs 2 statements: a case is
one statement, so move them into a function named for what they do and call it here". Each case reads as one thing
that happens to its member or value, and a path that needs more is a function with a name. The rule holds for every
switch, over a union, a `T?`, an enum or a `Boolean`.

### `switch` over values

A `switch` whose subject is an enum value or a `Boolean` takes value cases: an enum literal (`'red':`), or `true`
and `false`. **It names every value, and there is no `_:`** (D1770): a value added to the enum later is an error at
every switch until it has a case.

- One that leaves a value out is "this switch over 'light' has no case for 'amber': a switch over an enum covers
  every value, so a value added later cannot be forgotten; add the cases".
- `_:` anywhere is "'_:' answers for what the switch does not name, so a value added later slips past it: name every
  value with a case of its own (cases may share a one-line body), or choose among values with no closed set with an
  'if'" (`conformance/switch_rest_refused`).
- **A switch over a number or a text is an error**: "'switch word' chooses among values with no closed set, so no case
  list is ever whole: switch on an enum's values or a union's members, or write the choice as 'if' tests"
  (`conformance/switch_on_text`). Code that switches on a thing's name switches on the thing (its class, an enum
  value) instead.
- Two cases may share a one-line body (D1762). A switch whose cases all share one body but one is an error writing
  the `if`/`else` (D1778): "every case of this switch but 'Fish' does the same thing: write 'if creature.class == Fish
  { ... } else { ... }'" (`conformance/switch_one_exception`), unless the shared body reads the switched value (D1791).
- A value named twice is "this switch has two cases for 'red': keep one"; a value outside the enum is the usual
  "'blue' is not a value of this enum"; a class as a case of such a switch, a name or a call as a case, a value
  case in a switch over a union, and a value case over a `T?` (narrow it first) are errors saying so.
- The subject is evaluated once, into a temporary; the cases are tested in order as an `if`/`else if` chain on
  it. Nothing is added at run time beyond those tests.
- A `switch` whose every case ends by leaving the function leaves too, so after `if not found { switch ... }`
  whose cases all return, `found` is narrowed ([failure.md](https://github.com/Spite-Language/spite_documentation/blob/master/failure.md#an-if-that-leaves-proves-the-rest)).

### A chain of `if`s on one enum value is a `switch`

Three or more `if`s in a row, each with no `else`, whose condition is `subject == constant` and whose whole block is
one `return`, with the same `subject` (a name or a member path, no call) and an enum literal as the constant, are a
compile error at the first `if`; so is an `if`/`else if` chain of three or more such links. The message writes the
cases out and asks for every value: "these 3 'if's each compare 'light' with one of its enum values and return,
which is what a 'switch' says: write 'switch light { 'red': return 30  'amber': return 5  ... }', naming every
value". A chain over numbers or text stays as it is, since no switch can list every number or text (D1770,
`diagnostics/switch_chain`). A link may compare its subject with several values joined by `or`; each becomes a case.

### A `while` that a member template already says

**A `while` that only walks every element of a list, doing what a member template does, is a compile error naming
the template**; `while` stays for loops over state. The shape is exact: the statement before the loop is `var counter = 0`; the condition is
`counter < list.count()` with `list` a name or a path of type `List<T>`; the last statement is
`counter = counter + 1`; the counter is read nowhere else in the body and not at all after the loop; and the
rest of the body reads `list[counter]` (directly, or through one `var item = list[counter]` first) and is
exactly one of these, with `m` and `n` members of `T` that are not private (an attribute or a function taking
nothing), which needs `T` to be a class, or with `f` a function that takes the element as its only argument,
which fits a list of anything, numbers and `String` included:

| Body | The error names |
|---|---|
| `item.m()` | `list.each_m()` |
| `result.append(item.m)`, or `var value = item.m` then `result.append(value)` | `var result = list.map_ms()` |
| `result.append(item)` | `var result = list.copy()` |
| `if item.m { result.append(item) }` | `var result = list.filter_m()` |
| `if item.m { result.append(item.n) }` | `var result = list.filter_m().map_ns()` |
| `if item.m { total = total + 1 }` | `var total = list.count_m()` |
| `total = total + item.m` | `var total = list.sum_m()` |
| `if item.m == value { return item }`, the loop followed by `return null` | `return list.find_by_m(value)` |
| `if item.m { return true }`, the loop followed by `return false` | `return list.any_m()` |
| `if not item.m { return false }`, the loop followed by `return true` | `return list.all_m()` |
| `f(item)` | `list.each(f)` |
| `if f(item) { result.append(item) }` | `var result = list.filter(f)` |
| `if f(item) { total = total + 1 }` | `var total = list.count(f)` |
| `total = total + f(item)` | `var total = list.sum(f)` |
| `if f(item) { return item }`, the loop followed by `return null` | `return list.find(f)` |
| `if f(item) { return true }`, the loop followed by `return false` | `return list.any(f)` |
| `if not f(item) { return false }`, the loop followed by `return true` | `return list.all(f)` |

`result` must be a local declared `List<...>()` earlier in the same block and `total` one declared `0`, neither
mentioned between its declaration and the loop, and `value` may not mention the counter or the element. The
message reads `this 'while' walks every element of 'items' only to add up 'price': write 'var total =
items.sum_price()'` (`diagnostics/template_walk`). `f` is a function of the class (`say_hello`), a function of an
attribute or a local (`evaluator.process_line`), or a local or attribute holding a function value; it
takes exactly one argument, and where the loop tests it (`filter`, `count`, `find`, `any`, `all`) it returns
`Boolean` (a function answering a `T?` there is a presence test, which no passed-function form says, so that loop
stays). The message names it: `this 'while' walks every element of 'names' only to call 'say_hello' with each:
write 'names.each(say_hello)'` (`diagnostics/walk_with_function`). Loops that pass extra arguments, need the index, walk
two lists, scan text, stop early any other way or walk state are not touched. Compile time only.

---

Next: [Style: the compiler is the formatter and the linter](style.md).
