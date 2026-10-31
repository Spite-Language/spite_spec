# Memory

The specification of [Memory](https://github.com/Spite-Language/spite_documentation/blob/master/memory.md): every rule of this part of the language, with
its edge cases and its exact error texts. The page teaches; this one decides, and where they disagree, the page
is the bug.

## The memory model

**Reference counting is the memory model**, JavaScript-like. Every non-scalar value (a
class instance, `List<T>`, `Dictionary<Key, Value>`, `String`, a union of classes, an object literal) is a reference:
passing it, assigning it, storing it in a field/list/dictionary, and returning it all share the exact same object.
Scalars (every numeric type, `Boolean`, an enum value) are plain values, copied. There is no reference syntax
(no `&`): a parameter of a class type is written `Type`. A copy is always explicit: `copy()`/`deep_copy()` (below).

- **Retain and release.** Every reference is counted. Binding one to a `var`/parameter, or storing it into a
  field/list element/dictionary value, retains it (bumps the count); a scope ending, a value being overwritten, or
  an element being removed from a container releases it (drops the count). When a release brings the count to
  zero: the class's own `drop()` function runs first, if it declared one, then every attribute that itself needs
  releasing is released, then the object is freed. A singleton is never counted: fetching and letting go of one
  does nothing, and it is destroyed at exit ([classes_and_files.md](https://github.com/Spite-Language/spite_documentation/blob/master/classes_and_files.md#singletons)).
- **Run-time cost.** A count in every object's header and one addition or subtraction per retain and
  release. The counts are atomic only in a program that can share an object between threads (one in which the
  compiler starts work on another thread, or that is built with `--repl-port` or `--hot-reload`),
  and in a production build only for the classes whose objects code on another thread can retain or release;
  every other program, and every other class, counts with plain arithmetic
  ([optimizations.md](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md#plain-counts-where-no-other-thread-counts-a-class),
  [plain counts](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md#plain-counts-where-no-other-thread-counts-a-class)). A loop over a
  list of different classes that may run them at once ([concurrency](concurrency.md#what-runs-at-once))
  is followed class by class: a class that only one of the calls able to run together retains, releases, makes or
  frees, and no other code on another thread does, keeps plain counts and its pool; a class two of them count is
  counted atomically while the loop runs them at once and plainly otherwise, and its pool of objects is kept per
  thread ([counts per group](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md#plain-counts-where-no-other-thread-counts-a-class)).
- **`drop()`.** A class may define `func drop() { ... }` to run cleanup the moment its last reference goes (closing
  a file handle, logging, clearing a back-reference to help break a cycle by hand, see below). It takes no
  parameters and returns nothing; the compiler calls it automatically, never by name.
- **Identity vs equality.** `==` on two class instances calls `equals` if the class defines one ([Operators](functions_and_operators.md#operators));
  otherwise it compares **identity**: are these two references the same object.
  A union compares identity the same way. A comparison releases whatever operand it produced itself (an item
  read with `[]`, a call's result), so `kept[index] == shape` holds nothing afterwards.
  `String` always compares by content, never by identity (sharing a `String`'s buffer is unobservable, since it is
  immutable, and so is its absence: text of up to 15 bytes has no buffer to share, each holder keeping it in its
  own sixteen bytes).
- **`copy()`/`deep_copy()`**. Every non-scalar value has both:
  `copy()` is shallow: a fresh object, its own attributes/elements the exact same references the source had
  (retained, not duplicated; a `String` field needs no special handling either way, since it is immutable).
  `deep_copy()` recurses: every reference-kind attribute/element gets its own `deep_copy()`/independent buffer
  instead of being shared. An attribute or element typed as a union or a `type` shape is copied through the class
  of the value it holds at run time, so a `Cargo` holding a `Crate` gets a new `Crate`; a `String`, a number held
  in one, an enum value and a singleton are shared, since none of them can be changed through the copy, even
  inside a class copied through its table (`tests/deep_copy_shape_values`). A union or a shape value
  answers `deep_copy()` itself (`var copied = cargo.deep_copy()`, a `Cargo`). A class that declares its own
  `deep_copy()` is copied through it wherever it is reached. A self-referring structure (a tree, a linked list, an
  expression whose operands are expressions) is copied node by node (`conformance/stage6/deep_copy_unions`).
  **`deep_copy()` copies a graph with its shape**: a structure whose references lead back to an object already being
  copied (a parent holding its children, each holding its parent) gets one copy of each object, and the copied
  references point at the copies, so the copy has the same cycles as the original. A back reference's copy holds the
  copy of its object when the same `deep_copy()` copies that object, before or after reaching the back reference, and
  the original object otherwise. The compiler writes each class's deep copy: one whose attributes can reach itself (through an
  object attribute, or any class of a union attribute), reach a `type` attribute (whatever fills it, since its
  fillers are known only once the program is built: `tests/deep_copy_type_cycles`), or reach a back reference, remembers the objects copied during the call in a table made for that call; every
  other class is copied plainly, with no table (`conformance/deep_copy_graphs`). A `List` or a `Dictionary`
  whose items reach such a class copies all its items through one table, so items that hold each other (a list of
  a parent and its child) are copied once each and keep their links (`tests/deep_copy_list_graph`). An object reached twice
  through classes that cannot reach themselves (two attributes holding the same leaf) is copied twice.
- **There is no `Weak<T>`: the compiler picks a cycle's back reference.** A program writes plain references, and the
  compiler decides which reference of a cycle (a child's `parent`, a node's `previous`) holds no count, so a cycle is
  freed with its owner. It decides from how the program links its objects, never from the class graph alone. An
  attribute `a: T?` of a class `C` holds no count when one function both sets `c.a = t` (directly, or through a
  constructor that stores its argument in `a`, as `Child(name, this)`) and stores `c` into `t`'s attribute `f`
  (`t.f = c`, `t.f.append(c)`, `t.f[k] = c`), `T` declares that `f`, and `f` holds a `C`: through a collection
  (`children: List<Child>`), or directly when `c` was made in that function and `t` was not. A call of a program
  function whose name only one class declares counts as the stores that function makes, with `this` and its
  parameters read as the call's receiver and arguments, so `team.recruit(player)` (which appends its argument to
  `players`) beside `player.play_for(team)` (which sets `team`) links them as the two stores written out would
  (`tests/back_reference_two_functions`). The stores a function makes include those of the program functions it
  calls the same way, three calls deep, so `team.sign(player)` calling `recruit(player)` and `player.play_for(this)`
  links them too (`tests/back_reference_two_levels`). Neither `C` nor `T` is a singleton or generic. The class graph alone cannot tell a `parent` from an owning `answer: Type?` beside
  `members: List<Type>`, and taking an owning reference for a back one would free an object a program still holds,
  so a cycle the code does not link this way keeps its counts and leaks: break it by hand by clearing the `T?` field
  that closes the loop.
- **A back reference reads `null` once its object is freed**, and the freed object's memory stays until the last back
  reference to it goes, so no read is ever dangling. An object some back reference can point at carries one more
  word: a count of the back references to it plus one for all its counted references together. When its counted
  references reach zero it runs `drop()` and lets its attributes go, and its memory is freed when that word reaches
  zero. Reading a back reference checks the object's count and answers `null` for a freed one; writing one counts the
  new object's word up and the old one's down. The read holds its object while the value read is used, as a call's
  result is held, so a call on it (`child.parent.leave()`) whose body lets the object's owner go still runs on a live
  object, which is freed when the call returns; on two threads the test and the count are one step, so a read never
  takes back an object whose last release has begun. A back reference proven present may lose its object at any later
  call that lets the owner go, so a read that relies on the proof halts on a freed object, naming the attribute,
  rather than use `null` as an object. A program with no back
  reference carries nothing: no slot, no helper, no check.
- Parameters follow the same rule as everything else in [Functions](functions_and_operators.md#functions): a scalar is passed by value (copied); anything
  else is passed by reference (the same object, retained for the callee's own binding and released when the
  callee's scope ends); nothing is written at the call site. A name the caller already holds for the whole call
  (its own parameter, or a local it owns) is passed without that count when the callee never assigns the
  parameter: nothing the call runs can let the caller's name go, so the
  count would change nothing a program can see ([optimizations.md](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md#an-argument-its-caller-holds-is-passed-without-counting)).

Every function/method return retains its result, so a getter's returned value is always a fresh, independent
reference, including through attribute read interception ([Operators](functions_and_operators.md#operators)): a `person.age`-shaped read answered by a
getter, used directly as a call/print argument rather than stored, is released like any other temporary
(`ExpressionResult.is_owning` in the generator is how a read that compiles to a call, but still
parses as a `.member`/`.index` expression (an intercepted attribute read, the
`attributes[attribute]` template form), tells every caller whether it is independently owned regardless of what
the plain AST shape alone would suggest).

### Assigning a name read from a list changes only the name

A local declared from an item of a `List` or a `Dictionary` names that item: `var layout = layouts[index]` and
`layout.width = 3` write the item the list holds. **Assigning the local names something else**: `layout = made`
changes `layout` and leaves the list holding the old item. When nothing reads the local after that assignment, the
assignment can only be a write meant for the list that never reaches it, so it is a compile error at the
assignment:

- **Read directly**: `'layout' was read from 'layouts[index]', so assigning it here changes only 'layout', and
  nothing reads 'layout' after: 'layouts' still holds the old item. Write 'layouts[index] = made' to replace the
  item, or give the new value a 'var' of its own` (`diagnostics/replaced_items`).
- **Answered by a function**: a call to a function of the program whose every `return` answers an item of a `List`
  or `Dictionary` attribute (or `null`), directly or through another such function, up to four calls deep. The
  error names the item and its class, and the function of that class that writes the list when it has one:
  `'layout' holds 'values[row]' of 'Column', answered by 'lookup.of(entity)', so assigning it here changes only
  'layout', and nothing reads 'layout' after: 'Column' still holds the old item. Replace it through
  'Column.write_at', which writes 'values', or give the new value a 'var' of its own`; with no such function, `Replace it through a
  function of 'Column' that writes 'values[row]'`.

The rule is judged in the block that declares the local, and in the branches of an `if` or a `switch` that is the
last statement naming it: the assignment must be the last place the local is named, and no earlier statement may
assign it (an earlier assignment means the old value is no longer the item). An assignment inside a `while` loop
whose local is declared outside it may be read by the next pass and is left alone, as is a local declared from
anything that is not an item read: a constructor, a `copy()`, a library function such as `first()`, or a function
that may answer something else or reads its list through a local, a getter, a function value or a `type`. A
library class is not checked.

### `--debug-memory`

`--debug-memory` reports total allocations and frees right before the program exits (`allocations: N frees: N`),
and they must balance exactly, not just be bounded, for every example, every documentation program,
`tests/references` and the compiler's own runs. When they do not, it also prints a **leaked-object summary by class
name**, naming which classes' instances are still live. The table keeps each live allocation's class beside it and
forgets it on free, so the summary names exactly the objects still alive, the same ones on every run; memory that
is not an object (a list's elements, a string's bytes) is counted but not named. An object a program leaked is
named even when it points at a singleton, which is still destroyed at exit. The compiler's own bookkeeping
(the list of singletons to destroy) is allocated outside the table and not counted. The counts are exact and the same on
every run, so a test pins them, and a change in where the compiler places memory shows as a changed count: objects
placed in the frame allocate nothing (`tests/memory_debug_memory_placed`), a buffer made in a loop allocates its
storage once a pass (`tests/memory_counts_buffers`), and a program that keeps nothing frees all it allocated
(`tests/memory_counts_nothing_leaked`).

**Fresh memory is filled with a fixed pattern.** Every byte an allocation or a growing `resize` hands out that the program has not written is `0xA5`, so a
`Long` read from it is -6510615555426900571 and an address read from it points nowhere: reading bytes nobody wrote
fails the same way on every run, instead of depending on what the heap held, which the environment decides. A `resize` keeps the bytes it moves and fills only the new ones; the table keeps each allocation's size
for that. Only a `--debug-memory` build does this.

**Run-time cost.** The table and the summary exist only in a `--debug-memory` build: no other build carries them.
Allocations are counted only there.

### Memory is not observable

**Memory and threads belong to the compiler, and a program cannot observe them.** No class stands for them: there is
no memory address, allocator, allocation count, object placement or list of live instances a program can read or
name, so no optimisation is ever blocked by something a program might be watching. Only `--debug-memory` reports
leaks, outside the program. Reading the declared shape of a
class (its attributes and functions) stays, since it is fixed while compiling. A class that needs room for bytes
holds a `Buffer`, and the compiler places and optimises it from the proofs it has.

### Placement: the compiler decides where memory lives

Memory is an abstraction: the compiler places a value in a register, in the frame of a function or on the heap, from
what it proves, and the program's text is the same whichever it chooses. A program has no way to ask for raw memory
or for a place, and none to observe the choice ([Memory is not observable](#memory-is-not-observable)):

- **Register:** a number's own value. Its width is not a name the compiler knows and not the size of its room
  (`var _buffer = Buffer(4)` in `Integer`): it is the range its class's `set_this` checks
  ([values_and_types.md](values_and_types.md#a-number-states-its-range), D1757, D1780). The wrapper is flattened: a
  number is its machine scalar, and `this` is the value.
- **A buffer's size is a starting size** (D1757): `Buffer(size)` suggests how much room to start with, never a
  promise; the compiler may start it smaller, or size it from what it proves is written, and never starts it
  larger than asked; nothing a program sees changes. A local buffer that is only appended to a known number of
  bytes outside every loop, and otherwise only read, starts with those bytes
  ([optimizations.md](https://github.com/Spite-Language/spite_compiler/blob/master/OPTIMIZATIONS.md#a-buffer-starts-with-the-room-it-is-proven-to-use),
  `tests/trimmed_buffer`). Room handed to a foreign call is the exception: there the size is the least the compiler
  gives (until spite_compiler#226 sizes that room itself, D1766).
- **In the value:** text of up to 15 bytes is kept in the sixteen bytes of the `String` itself
  ([optimizations.md](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md#short-text-lives-inside-the-string)).
- **Constant:** a `String` literal's characters are part of the program.
- **Registers or the frame, for objects:** an object that never leaves the function that made it is placed after
  inlining, in registers or in the frame, and so is the fresh answer a function writes into its caller's slot and a
  temporary inside an expression. Never leaving means: read and written only through its attributes, handed only to
  functions proven to keep nothing (the same proof for each parameter and for the object a function is called on),
  compared, given a new fresh object, and returned only from a function that answers its class (copied to the heap
  there). Not in a function that waits
  ([optimizations.md](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md#objects-that-never-leave-their-function-live-in-registers-or-the-frame)). A local made
  by its constructor lives in the frame on the same terms when its class also holds text, lists, dictionaries or
  other objects: what it holds is let go where the local's scope ends, and returning it moves it to the heap,
  attributes and all.
- **Heap:** everything else. The compiler may keep the objects of one class side by side
  ([optimizations.md](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md#objects-of-one-class-sit-together)).

There is no way to ask for the stack by name: it would be a second way to allocate, and one a program could get wrong
by keeping an address past the return. What makes a data structure's layout efficient stays its author's: one
`Buffer` holding many values at offsets they choose. Placement runs entirely while compiling and costs nothing at run
time; a frame slot is cheaper than the heap call it replaces.

### Rows walked from lists

**An item read from a `List<T>` is the item itself**, a counted reference like any other value: it may be kept,
returned, put in another list, given a second name and read after the list changes size (`tests/list_shared_items`).
Whether the compiler keeps a list's items inline, when no item is held anywhere else, is its choice and nothing a
program can see (spite_compiler#205). An object literal of a `type` whose attributes are items read from lists
(`var row: Moving = {position: positions[index], velocity: velocities[index]}`) is an ordinary value of that type
(`tests/list_rows`, `tests/lent_to_calls`). The rules for building such rows from columns:

- **A row filled by a walk.** A local declared as a `type` with `= null` (`var row: $row_type = null`) and filled on the very next
  line by a plural walk of its own class (`fill_attributes(row, index)`, whose template ranges over that `type`
  with `attribute: Spite.AttributeDeclaration`) is a row when the walk writes out to one: the template's body is the
  statement `row.attributes[attribute] = <value>`, after any number of `crash` lines and `var` lines naming a
  value (`var stored_row = rows[attribute.index]`, each written out where its name is read, so a hoisted
  `values[stored_row]` walks exactly as `values[rows[attribute.index]]` does) or naming an object the template
  goes on to change (below), the value built from `<object>.attributes[attribute]` (read as
  `<object>.<the attribute's name>`), member reads, `[ ]`, the template's other parameters and whole numbers, and
  the call's arguments are the row and names, attribute paths (`matcher.rows`) or number or `Boolean`
  literals. The compiler then writes, in place of the two lines, each attribute's `crash` lines walked as the
  value is (`crash columns.position[index]`), then the literal, one attribute for each attribute of the `type` in
  its order (`{position: columns.position[index], velocity: columns.velocity[index]}`).
  **A walked read is narrowed by the template's `crash` lines**
  (every `[]` answers `T?`, and a row's attribute is an item, never a `T?`). The walk cannot prove its index: the
  places come from a list the runner filled, and nothing the compiler knows ties them to the columns' counts. So
  the template states each read it relies on, innermost first (`crash rows[attribute.index]`, then `var
  stored_row = rows[attribute.index]` and `crash Column<attribute.class>().values[stored_row]`); a read the lines do not narrow is the usual `this
  value may be null` error on the walk's line. The lines sit inside the branch they narrow, so a template that
  chooses per attribute keeps its shape (`if attribute.class == Entity { row.attributes[attribute] =
  Entity(entity) } else { crash ...; row.attributes[attribute] = Column<attribute.class>().values[...] }`). A
  `crash` line is walked like the fill line, and a generic
  singleton made with no arguments is a path root (`Column<Position>().values[found[1]]` narrows like a name's
  read), so each line narrows the very read the literal makes. Cost: a walked `crash` line
  reads the item once into a local and tests it, and the literal (or the lent argument) uses that local instead
  of reading again, so a row costs one compare per read (`benchmarks/a_walked_crash_lines_read_is_the_rows_read`).
  The template is not called, so it is not compiled for that walk. A walk of any other shape leaves the local an
  ordinary `type` value. Nothing runs for it: the rewrite is done while compiling (`tests/walked_rows`).
- **A walked row over sparse columns.** A walk that fills a
  row may also be written with:
  - **A generic singleton per attribute.** `Column<attribute.class>()` in the line is the singleton made with the
    walked attribute's class, written out as `Column<Position>()` for `position`. A singleton made from
    `attribute.class` is never an attribute, so it may be written where it is used, inline included (the usual
    errors for a singleton made inline leave it out).
  - **The place of the attribute.** `attribute.index` is the attribute's place among those walked, from 0, a
    constant written into the line (`found[attribute.index]` becomes `found[1]`), and it may be read in any
    function a walk of attributes calls. The places are read from a `List<Integer>`, whose numbers are values of their
    own, so the list may be any object's: the runner's own attribute, a walk argument such
    as `matcher.rows`, or `matcher.rows[attribute.index]` read in the line itself. It is read once, when the row is
    made, so a runner does not copy another object's places into its own list first. Read elsewhere, it is
    `'attribute.index' is the attribute's place among
    the attributes walked, so it is read in a function a walk of attributes calls, such as 'fill_attribute(attribute:
    Symbol<Row>, ...)'`. The line's parts are `<value>.attributes[attribute]`, attribute reads, `[ ]`, calls
    (constructions included), the walk's parameters, `attribute.index` and whole numbers; anything else in the line
    of a walked row is `'row' is a row filled by the walk on the next line, and its line for
    'position' reads 'attribute.index + 1', which a walked row cannot write out: ... so work out 'attribute.index
    + 1' before the walk and pass it in` (`tests/sparse_rows`).
  - **A local the template changes.** A `var` that later
    lines of the branch change, by assigning into it (`own.id = entity`, through `set_id` when the class has one) or
    by calling one of its functions (`own.raise(1)`), is made as a local before the row, those lines run where they
    stand among the `crash` lines, and the row's attribute is that local. Lines that change
    anything else make the walk ordinary calls (`tests/walked_row_locals`).
  - **A choice per attribute.** The template's statement may be an `if` whose conditions are decided while
    compiling for each attribute (`attribute.class == Entity`, `attribute.class.has_function(...)`), each branch
    holding one such line and its `crash` and `var` lines; the branch taken for an attribute is what is written out
    for it.
  - **What the row holds.** Each attribute is counted once when the row is made and let go at the end of the row's
    block, so a call that removes it from its column cannot free it under the row.
  - **A list column's element lent to the row.** The count above is left out when it cannot
    matter: the attribute's line calls a function of a singleton (`Column<Trail>().at(found[3])`) whose body is
    only `return <list>[<index>]` after `crash` of that same read (which is asked for, since the read is a `T?`),
    where `<list>` is an attribute of the singleton holding a `List` of a class that nothing assigns after the
    singleton is made and `<index>` is a whole-number parameter or literal, the function takes nothing counted, and
    the rest of the row's block is proven to let go of nothing: every call in it is one the compiler can name (a
    phase template's call named as the phase's own function), and none of them, nor anything they reach, lets go of
    an object, removes from or replaces into a list, assigns an attribute holding an object or calls through a
    function value, and no class whose objects can be let go in the meantime has a `drop()` reaching that
    singleton or its list. The row then holds the element as it lies in the list, never retained or released, read
    through a copy of the function, `<name>___lent_element`, which answers exactly what the function answers, crash
    included. Nothing is an error: every other caller of the function, and every line that
    keeps the element, counts it as before. **Cost**: none. `tests/lent_list_elements`,
    `tests/lent_list_elements_parallel`
    ([optimizations.md](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md#objects-that-never-leave-their-function-live-in-registers-or-the-frame)).
  - **Cost.** Nothing runs for any of it: the singleton is the one a program would reach anyway, `attribute.index`
    is a constant, and the choice is made while compiling.
    `benchmarks/a_walked_crash_lines_read_is_the_rows_read` walks rows over sparse columns and measures them
    against C.
- **Arguments a plural fills.** A call whose whole argument list is the plural of a template over that function's
  arguments, written as a statement of its own (`system.phase_each(made_arguments(found))`), stands for exactly
  one call. For each argument in order, the compiler folds the
  template's body for that argument (an `if` decided while compiling keeps only its branch, as in a walked row)
  and writes it out before the call when what is left is:
  - **one `return` of a walked line**, after any `crash` lines that narrow its reads (written out before the
    call, walked for each argument, as a walked row's are), built as a walked row's line is
    (`<value>.attributes[...]` aside):
    attribute reads, `[ ]`, calls and constructions, the template's other parameters (read as the plural's
    arguments), `argument.index` and whole numbers, with `Column<argument.class>()` the singleton made with the
    argument's class: `return Column<argument.class>().values[stored_row]` after `var stored_row =
    rows[argument.index]` (a `var` naming a walked value is written out where its name is read). It is a local of
    the call, counted for the call and let go after it;
  - **a walked row declared, filled and returned** (`var row: $row_type = null`, `fill_attributes(row, rows)`,
    `return row`, or `var row: argument.class = null`), which is written out as the walked row it is, under
    every rule above, and passed as a row. Folding `argument.class == $row_type` is what lets a generic
    `Stream<$system_type, $row_type>` fill the one argument of its row type this way
    (`tests/streamed_rows`).
  Any other body is the template's ordinary call. A parameter the function ignores, named `_position`, is filled
  and passed like the others, and a local written for it drops the underscore, since the call reads it
  (`tests/ignored_lent_argument`).
  **Cost.** The written-out arguments cost what the walked line costs; the template is not called, so it is not
  compiled for that call ([optimizations.md](https://github.com/Spite-Language/spite_documentation/blob/master/optimizations.md#objects-that-never-leave-their-function-live-in-registers-or-the-frame);
  `tests/lent_arguments`).
- **A walked line names the attribute's class wherever it was declared.** `Column<attribute.class>()` in a walked
  row or a plural's argument is written out with the class's full name, so a row type declared in another namespace
  (`mouse: Component.Mouse` in `Input`) names the same class from the runner's file.

`tests/list_shared_items`, `tests/list_rows`, `tests/list_columns`, `tests/walked_rows`, `tests/sparse_rows`,
`tests/walked_row_locals`, `tests/streamed_rows`, `tests/lent_arguments`, `tests/ignored_lent_argument`,
`tests/lent_list_elements`, `tests/lent_list_elements_parallel`, `tests/lent_to_calls`.

---

Next: [Metaprogramming](metaprogramming.md).
