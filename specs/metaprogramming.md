# Metaprogramming

The specification of [Metaprogramming](https://github.com/Spite-Language/spite_documentation/blob/master/metaprogramming.md): every rule of this part of the language, with
its edge cases and its exact error texts. The page teaches; this one decides, and where they disagree, the page
is the bug.

## Templates

**A parameter whose name is a word of its function's name makes the function a template**, and the parameter's type
says what the template ranges over. There is no `Symbol<...>` parameter: the name is the signal, the type says the
kind of member, and the body says which class.

| Parameter type | Ranges over | Inside an instance, the parameter is |
|---|---|---|
| `Spite.AttributeDeclaration` | the attributes of a class, and its functions that take no arguments | the constant declaration of that member |
| `Spite.FunctionDeclaration` | the functions of a class whose names fit a pattern with the parameter's word as its hole | the constant declaration of that function |
| `Spite.Argument` | the arguments of a function | the constant argument |

`Spite.Attribute` (an attribute bound to one object: `declaration`, `value`, `owner`) and `Spite.Function` do not make
a template. A parameter of those types is an ordinary parameter, so a function that takes one is an ordinary
function, called by a walk over an instance's members or handed a value. A template over the members of an object is
written over the declaration with the object as a parameter of its own. A parameter typed `Spite.Namespace` or
`Spite.Class` is an instance of one, never a template.

`show_attribute(attribute: Spite.AttributeDeclaration, monster: Monster)` answers `show_name(troll)` and
`show_health(troll)`. A template is instantiated only for the names a program calls, in every build; the template
itself emits nothing, and each instance is an ordinary function that costs what its body costs. `--final-classes`
prints each instance.

### Templates over attributes

- **The class is the class the body reads the declaration on.** Every `x[attribute]` and `x.attributes[attribute]` in the
  body names it (`x[attribute]` is short for `x.attributes[attribute]`; `attributes[attribute]` alone is the template's
  own class). `x` is known from its declaration: a parameter, an attribute, a local declared with its type or made by a
  constructor, an own function's answer, or an element of a list so known. Reads on parameters count before the
  others, so the declaration (and `attribute.class`) is that of the first parameter read. Reads on several classes
  narrow the walk to the attributes they all have, in the first class's order. A body that reads the parameter on
  nothing takes the class of the function it passes the parameter to, and with no such function it is an error, even
  when the template has a parameter of a class: nothing is guessed from it (D1783, superseding D810's default to the
  first such parameter). A nullable class is walked as nothing.
- **Errors.** A body that reads the declaration on no value: "'show_attribute' is a template over the attributes of a
  class, because 'attribute' is a word of its name, but its body reads 'attribute' on no value, so nothing says
  which class: read it as 'value[attribute]' on a value of the class it walks"
  (`conformance/template_unused_attribute`); with a parameter of a class it names that parameter, "read it as
  'shown[attribute]'" (`conformance/template_reads_no_parameter`). A value whose class its declaration does not say: "'show_attribute' uses
  'attribute' on 'shown', whose class is not known from its declaration, so nothing says which class it walks: use it
  on a parameter, an attribute or a local declared with its type, as in 'var shown: Monster = ...'"
  (`conformance/template_unknown_value`). Two classes with no attribute in common: "'copy_attribute' reads
  'attribute' on a 'Lamp' and on a 'Door', which have no attribute in common, so it would walk nothing: read it only on
  values whose classes share the attributes it walks" (`conformance/template_no_common_attribute`). The class written
  on the parameter, `attribute: Spite.AttributeDeclaration<Monster>` or `Spite.Attribute<Monster>`: "'attribute:
  Spite.Attribute<Monster>' names its class, but a template's class comes from its body: write 'attribute:
  Spite.AttributeDeclaration' and read it as 'value[attribute]' on a value of 'Monster'"
  (`conformance/template_attribute_spelling`). A bound attribute handed to a `Spite.AttributeDeclaration` parameter:
  "'declared' takes a 'Spite.AttributeDeclaration', the declaration of an attribute, and 'health' here is an attribute
  bound to an instance of 'Monster': declare the parameter 'Spite.Attribute', or walk the class's attributes"
  (`conformance/reflection_attribute_to_declaration`).
- **Inside an instance, the parameter is the constant declaration of that member**, a
  `Spite.AttributeDeclaration`; the bound attribute is `owner[attribute]`, which is `owner.attributes[attribute]`.
  Written as a type (`value: attribute.class`, `): attribute.class`) it is the member's real type; written as an
  expression, `attribute.class` is its `Spite.Class`, which prints as the type's name in `console.print` and in a
  text's `{}`. `attribute.name`, `attribute.camel_case_name` and `attribute.pascal_case_name` are text constants, and
  `attribute.index` is the member's place in declaration order. `attribute.value` is an error, since a declaration has
  no value: "'attribute.value' reads an attribute of an instance, and 'attribute' describes a declaration: walk the
  instance's own attributes, 'monster.attributes', or read 'monster.attributes[attribute]'"
  (`conformance/reflection_declaration_value`).
- **`owner[attribute]` is that member of the `owner` passed in**: the field, read or written, or a call to
  the function. A class reads its own members by name, `attributes[attribute]`, never `this.attributes[attribute]`,
  and that is the field itself, read or written without its getter or setter, so a `get_` or `set_` template over
  its own class never calls itself.
- **The class may be a codegen value**: `member: Spite.AttributeDeclaration` read as `item[member]` on a
  `$element_type` is how `library/list.spite` writes its member templates. When that value is not a class or a `type`
  (a number, a `T?`, a `List`), the template answers nothing.
- **The class may be a `type`**: the template ranges over the attributes the type names and the functions it requires
  with no arguments, and `row[attribute]` is the shape's own read, write or call, answered from the
  value's class at run time like any read through a `type`.
- **An exact function always wins over a template, for that one name and direction**: an exact `set_name` answers a
  write of `name` while its read still goes through `get_attribute`. A generated `get_<attribute>()` of an owning
  attribute (a `String`, `List<T>` or `Dictionary<Key, Value>`) returns a retained, independent value, exactly as an explicit
  getter does.
- **A template that makes a function the class already has is an error** at that function's line: "the template
  'store_attribute' makes 'store_row' for the attribute 'row' of 'RowView', which is already a function of 'Row':
  rename the function or the template". An error raised in code the compiler wrote for a template names the
  template's line (`conformance/template_function_clash`).
- **Reading a member counts as using it**: `x[attribute]` counts as a read of the member it reaches,
  while `attribute.name` and `attribute.class` describe it without reading it, so an attribute only they look at is
  the unread-attribute error ([Unused is an error](style.md#unused-is-an-error)).
- **A walk sees private attributes**, and may read and write them through the walked attribute; naming `_x` outside
  its class stays the private error.
- **A reflection object known while compiling has no run-time value.** A function a specialisation was compiled for
  cannot pass its parameter on as a value: "'attribute' is a reflection object known while compiling, so this
  function is compiled once for it, and it has no run-time value to pass on here", given at the statement that passes
  it (`conformance/reflection_specialised_kept`). A class held in an attribute typed `Spite.Class` and named as a type
  is "'kind' is not a class known while compiling, so it cannot be a type here"
  (`conformance/reflection_unknown_class_type`).

### Templates over functions

A parameter `phase: Spite.FunctionDeclaration` named by a word of its function's name ranges over the functions of a
class whose names fit a pattern with `phase` as its hole. The class and the pattern come from the first of:

- a call `x.f(...)` in the body whose name has the parameter as a word (the class is `x`'s, the pattern is `f`):
  `system.phase_each(amount)` ranges over the functions of the system's class named `<hole>_each`;
- a pattern written as text on a class, `$t.argument_count("<phase>_each")`;
- the parameter passed to an own function that does one of these.

The hole is any part of the name between the pattern's fixed words, with no enum behind it, so `phase_all` walks a
system's `update_all`, `render_all` and also a `sum_all`. The template is called as the plural of its name,
`run_phases_each(...)` for every function that fits (in the order they were made), or with the hole written,
`run_render_each(...)` for one. Inside an instance, `phase.name` is the function's name (`update_each`), `phase.hole` the
text in place of `phase` (`update`, a `String` known while compiling), `phase.index` its place, and `phase.argument_count()` the number of its
arguments. `$t.argument_count("<pattern>")` refuses a pattern whose functions take different numbers of arguments:
"... has no one answer: the functions of 'Painter' that fit it ...". `argument_count()` asked with no name of
anything but a walk over functions, such as the `argument` of a walk over arguments, is "'argument.argument_count()'
is how many arguments the function a walk over functions stands for takes, so it is asked with no name in a template
over functions, such as 'run_phase_each(phase: Spite.FunctionDeclaration)'" (`conformance/argument_count_outside_walk`).

- **A name whose word fits no function** is an error listing the functions that fit: "'run_updat_all' is
  'run_phase_all' with 'updat' for 'phase', but 'Move' has no function 'updat_all': the functions there that fit
  'phase_all' are 'update_all', 'step_all'" (`conformance/hole_values`).
- **A body that calls no function with the hole in its name**: "'run_phase_each' is a template over functions, because
  'phase' is a word of its name, but its body calls no function with 'phase' in its name, so nothing says which:
  call one as 'value.phase_each(...)', where 'phase' stands for the part of the name that varies"
  (`conformance/template_unused_function`).
- **The enum spelling is gone.** `phase: Symbol<$system_type.phase_each>` is "... write 'phase:
  Spite.FunctionDeclaration' and call the function as 'value.phase_each(...)' on a value of '$system_type'"
  (`conformance/template_symbol_function`).

### Templates over arguments

A parameter `argument: Spite.Argument` named by a word of its function's name ranges over the arguments of a function.
The function is the call its plural is spread into (`system.update_each(made_arguments(row))`), or, for a plural
called alone inside a template over `Spite.FunctionDeclaration`, the function that template stands for. The plural of
the name is the argument list of the call: one answer for each argument, in order, each typed by its argument when the
function's result is typed `argument.class`. Inside an instance, `argument.name`, `argument.class` and
`argument.index` are constants.

- **A plural called anywhere else** is "'show_arguments' walks the arguments of a function, but nothing here says
  which: spread it into a call, as in 'value.update_each(show_arguments())', or call it inside a template over
  'Spite.FunctionDeclaration', where it walks the arguments of that function"
  (`conformance/argument_walk_without_function`).
- **A plain walker over arguments** (`function.arguments.each(describe_taken)`) takes a name without the word, since a
  parameter named by a word of the name is a template.
- **The enum spelling is gone.** `argument: Symbol<$system_type.update_each>` is "... write 'argument:
  Spite.Argument' and spread its plural into the call, as in 'value.update_each(made_arguments())'"
  (`conformance/template_symbol_argument`).
- **A function whose result is typed through a reflection parameter** (`func made(argument: Spite.Argument):
  argument.class`) has no result type of its own. A call that hands it the parameter of a specialisation is compiled
  once for that member, and any other call is "'made' answers a type read from a reflection object, so it is compiled
  once for each object the compiler can identify, and this call hands it one known only at run time: pass a class,
  attribute, function or argument named in the code, or walked from one". The parameter may be a `Spite.Argument`,
  `Spite.AttributeDeclaration`, `Spite.Class` or `Spite.FunctionDeclaration`.

### Walks over folders and enums

A walk over the classes of a namespace or the values of an enum is ordinary code on the reflection objects, with no
parameter type of its own:

- `Namespace.classes.each(walker)`, `walker(part: Spite.Class)`, makes one copy of `walker` for each class of the folder
  (in the order of the dotted names), in which `part` is the class: a type (`Runner<part>()`), a constant for
  `part.name`, `part.has_function(...)` and `part.function_waits(...)` folded, and the class object where a value is
  wanted. `class.namespace.classes` and `Spite.Namespace.instances` are the other lists to walk.
- `Enum.values.each(walker)` calls `walker` once for each value, in the enum's order; `$t.values`, with `$t` bound to an
  enum, is the constant list of its values.
- A walker takes one parameter, and the state it needs goes in attributes. A namespace that names no folder is an
  unknown name.
- The walker is any function the compiler can name: one of the walking class (`each(add_system)`) or one of another
  object (`Part.classes.each(registry.add)`, `Registry.Phase.values.each(registry.note)`), whose copies are made in
  that object's class and called on it, the object read once (`tests/walker_of_another_object`).
- `Symbol<System>`, `Symbol<Phase>` and the other enum spellings are errors that name the walk to write: "'system:
  Symbol<System>' is not a type: ... walk the classes of 'System' as ordinary code, 'System.classes.each(add_system)',
  with 'system: Spite.Class'" (`conformance/template_symbol_namespace`), and "... walk the values of 'Phase' as ordinary
  code, 'Phase.values.each(run_phase)', with 'phase: Phase'" (`conformance/template_symbol_enum`). A `Symbol<$element_type>`
  on a list's member template names the form to write (`conformance/plain_symbol_on_list`).

### Calls built before they run

`Spite.Call(declaration, instance)` is a call of a function on an instance of the class that declares it, built
before it runs. `call.arguments["name"] = value` fills one argument, and `call.call()` runs it.

- **A declaration named in the code** (`Monster.functions["hurt"]`, or a `var` initialised with one and never
  assigned) makes the call compile away: the instance and each argument become hidden locals and `call.call()` is the
  direct call. An instance of another class is "'rock' is a 'Rock', and 'hurt' is declared by 'Monster': a call runs a
  declaration on an instance of its own class" (`conformance/reflection_call_other_class`); a missing argument is
  "'call' leaves the argument 'velocity' of 'hurt' unfilled: set 'call.arguments["velocity"]' before 'call()'"
  (`conformance/reflection_call_unfilled`); filling one twice is "'call.arguments["amount"]' is already set: a call
  fills each argument once, before 'call.call()' runs it"; a name the function lacks is "'hurt' has no argument
  'speed', so 'call.arguments["speed"]' fills nothing"; and the call used as a value is "'call' is a call known
  while compiling, which becomes the plain call itself and has no object to pass on".
- **A declaration known only at run time** is the library's `Spite.Call`, and `call()` tests that the instance's class
  declares the function and halts "call() runs 'f', declared by 'C', on an instance of another class" when it does not.
  The thunk that does so is made only when some code reaches `call()` at run time, so a program without one carries
  none. It passes the arguments `.arguments` holds, testing each against its parameter's type: a number, an enum, an
  object, a nullable one of them, a union (any of its classes) or a `type` (any class that fills it); a missing one
  halts "call() leaves the argument 'word' of 'C.f' unfilled: ...", one of another type "call() gives 'C.f' an
  argument 'word' that is not a String, the type it takes", and one beyond the parameters, named by the first such key in
  the order it was filled, "call() fills an argument 'speed' that 'C.f' does not have: it takes 'times', 'word'" (or
  "it takes no arguments") (`tests/run_time_call_arguments`, `tests/run_time_call_open_parameters`,
  `tests/run_time_call_extra_argument`).
  A function-value parameter takes the function bound to its object, `object.functions["doubled"]`, a
  `Spite.Function` (a bare function value fits no `Anything`, D1751): one that is not a `Spite.Function` halts
  "call() gives 'C.f' an argument 'change' that is not a Spite.Function: ...", one whose declaration has another
  signature halts "call() gives 'C.f' an argument 'change' whose function is not a Spite.Function<Integer, Integer>,
  the type it takes", and one that fits is called on its object (`tests/run_time_call_function_argument`). Filling
  an argument a second time halts as the known call refuses it: "spite: 'call.arguments["times"]' is already set: a
  call fills each argument once, before 'call.call()' runs it" (`tests/run_time_call_filled_twice`).
- **Calling a `Spite.Class` known only at run time with no arguments** makes an object of it, typed `Anything`,
  wherever the class object is held: a local or parameter (`part()`), an attribute (`kind()`, `shelf.kind()`), or
  a member read (`thing.class()` on an `Anything`, `argument.class()` on a `Spite.Argument` known only at run
  time) (`tests/run_time_class_member`). It goes through one function the compiler writes that compares the class object with each class of the
  program's own folder, each other class whose class object the program holds (a library class it names, as
  `kinds.append(Http.Response)`), and the number classes, `Boolean` and `String` (each its default), and makes the one
  it is. Only a class whose object the program holds can be known at run time, so the list is complete once the
  program is built (`tests/run_time_library_class`, `tests/run_time_number_class`). The entry class and a class
  whose constructor takes arguments halt, naming why; a singleton, a generic class, an enum or a union halts "spite:
  a class known only at run time makes an object only of a class that is not a singleton, a generic class, an enum
  or a union".
- **A singleton made with arguments has one object for each argument list** written (`Channel(1)` and `Channel(2)`
  are two objects, `Channel(1)` twice one); see [classes_and_files.md](classes_and_files.md#singletons).
- **A function of the calling object passed where a `Spite.Function` is wanted** is described in place, bound to the
  object, with its arguments and answer; so is a function value of another object held in a local that is never
  assigned again (`var roar = troll.roar`, then `run(roar)`): it is bound to that object, so `call_function()` runs
  on `troll` (`tests/function_value_of_another_object`). The description is complete, `reads` and `writes` included, however
  late in the program the question that needs it is written (`tests/run_time_accesses`).

**Plural and singular.** A template whose result is a collection of the member's values (`List<member.class>`)
takes the member in the plural, through `String.pluralize()`; every other template takes it singular. The template
declares which by its own name: `func map_members(member: Spite.AttributeDeclaration): List<member.class>`. The
compiler finds the member by inflecting every member name of the element's class and picking the one whose plural
is the word called, never by guessing from the call:

- one match is the member: `map_names` is `name`, `map_people` is `person`, `map_health` is `health`;
- the singular where the plural differs is "'map_name' answers every 'name', so it is written in the plural:
  'map_names'";
- two members with one plural is an error naming both and asking to rename one;
- a name `pluralize()` cannot inflect so that `singularize()` gives it back is an error naming the member and the
  fix: add the word to the irregulars table by reopening `String` and declaring `irregulars` or `uncountables`, or rename the member;
- a name `pluralize()` leaves as it is, already plural or uncountable, is its own plural: `map_name_with_namespaces`;
- a template that answers `List<member.class>` but names its member in the singular is an error naming the plural:
  "'map_member' answers every 'member' of the element, a 'List<member.class>', so its name holds the member in
  the plural: 'map_members'";
- a member that is a question (`is_alive`, `has_target`, `can_fly`) is never collected: `map_is_alive()` is an error
  naming `filter_is_alive()`, `count_is_alive()`, `any_is_alive()` and `all_is_alive()`.

**A member template chains through a member's own function.** `filter_<member>_<function>(arguments)` keeps the
items whose `member` answers `function(arguments)`: `players.filter_name_starts_with("a")`,
`functions.filter_name_ends_with("_each")`. It works on any list of any class, and a reflection object does not
answer its name's text functions itself.

**A list of a union** takes a member class in the plural as a filter that keeps that class and narrows the list to
it (`entries.filter_files()` answers a `List<File>`), and every other template takes it the same way, in the plural:
`count_files()` answers how many items are `File`s, `any_directories()` whether one is, `all_files()` whether every
one is (`true` for an empty list), and `remove_where_directories()` removes the `Directory` items in place; on a
`Dictionary`'s values, `remove_where_` by class is an error naming `keys()` and `remove(key)`
(`conformance/stage6/union_member_templates`). It templates an attribute every member class has as on any
list, the result typed from that common attribute. A name that matches both a member class and an attribute is a
compile error naming the fix.

**A `Dictionary` takes the same member templates as a `List`, over its values**: `Monster.attributes.map_names()`.

**A question member is asked without its `is_`.** The templates that ask a question of each element (`filter_`,
`count_`, `any_`, `all_`, `remove_where_`) name a member `is_<word>` as `<word>` when the element has no member
`<word>` of its own: `arguments.filter_mutated()` keeps the arguments whose `is_mutated` is true, as
`monsters.filter_alive()` would for an `is_alive`. Naming it in full, `filter_is_mutated()`, is the same template.

**There is no plain `map(function)`, and a class has no function values.** A member's values are collected with
`map_<members>()`; a value computed from an element becomes a get-only attribute of the element's class and is
collected the same way. `Monster.is_alive` names nothing, since `Monster` is a `Spite.Class` object; a function value
belongs to an instance ([functions_and_operators.md](https://github.com/Spite-Language/spite_documentation/blob/master/functions_and_operators.md#functions-are-values)).

## Walks

A walk is a call on a list of reflection objects, under [reflection.md's rules for constants](reflection.md#known-while-compiling):

- **Names are selected, never built.** `[]` takes a name written in the code and answers a `T?`; anything else is a
  filter over the names that exist. A text with holes is never a member name.
- **`each` over a known list of functions or arguments is unrolled when its walker needs the item known.** The list is
  `C.functions`, a local made from `C.functions` or from `C.functions.filter(test)` that nothing assigns again, or
  `function.arguments` of a function known this way; the walker needs the item known when it hands it to a function whose
  answer is typed by it (`made(argument): argument.class`) or walks its arguments with such a walker. One copy of the
  walker is made for each function (in `C.functions`' order) or argument, and the one whose name matches the item the
  list holds is called. The filter's test still runs when the program does; a list that holds an item the compiler did not
  list stops the program: `spite: 'walked.each' holds an item the compiler did not list for it, at <file>:<line> in
  <Class>.<function>`. Inside a copy, a call handing the known argument to a function typed by it (`made(argument):
  argument.class`) is compiled once for that argument.
- **A walk over a constant list is unrolled**, one call per element in the list's order (declaration order for
  attributes and arguments, the order made for functions, an enum's order for its values, dotted-name order for
  classes and namespaces), and the function it calls is compiled once per element. A walk over an empty list calls
  nothing.
- **A question on a constant folds**: `.is_resumable`, `.is_mutated`, `.is_stateful`, `.is_fixed_size`,
  `.is_singleton`, the kind questions, `.count()` of a constant list, `[]` with a literal name, and `==` between
  constant classes. An attribute set from one of a codegen type's yes-or-no questions needs no written type: `var
  inline = $component_type.is_fixed_size` is a `Boolean` (`.is_singleton`, `.is_optional`, `.is_enum`, `.is_stateful`,
  `.is_fixed_size`, `.is_list`, `.is_dictionary`; `tests/fits_vector_doc`). **`.is_resumable` cannot be asked of a function by itself**: its own answer would decide whether
  it waits, and that is an error.
- **`.is_mutated` follows every call**: an attribute set on what the argument was given, an item written or a list
  grown, a function called on it that changes its own object, or the argument handed to another function that does
  any of these, however deep, recursion included. Reading does not count, and neither does giving the argument's
  name a new object. A function whose body the compiler supplies counts as changing what it is handed unless it is
  known only to read, since an answer the study cannot prove is `true`. **`.is_stateful`** is the same study asked of the object: some function besides the constructor
  and `drop()` writes the class's own attributes or anything reached through them, or a singleton, or a singleton the
  class binds has state.
- **A `type` has no attributes to walk**: `$row_type.attributes.each(find)` with `$row_type` bound to a `type` is
  "'$row_type.attributes' walks the attributes of a class, and '$row_type' is 'WalkOverAType.Moving', a type, whose members are not
  attributes: walk a class, or read each of the type's members by its name" (`conformance/walk_over_a_type`), and so is
  reading the list itself, `var listed = $row_type.attributes`, with the same words (`conformance/attributes_of_a_type`).
- **An empty walk is loud when it must not be**: `crash Runner.functions.filter_name_ends_with("_each").count() > 0`
  folds, and is the compile error below when it is false.

## Codegen values (`$`)

`$name` means "replaced at code generation". That is its only meaning, everywhere it appears.

**A class declares each codegen value on a `generic` line of its own, at the top of the file**:

```gdscript weapon.spite
generic $damage_type
generic $is_magic

var damage: $damage_type = 0

func Weapon(new_damage: $damage_type) {
    damage = new_damage
}

func hit(): Integer {
    if $is_magic {
        return 1
    }
    return 2
}
```

```gdscript
var sword = Weapon<Integer, true>(10)
```

- **Every value is declared, even a single one**, so skimming the top of a file is how a human sees what a class
  accepts. The lines come after a `singleton` line and before everything else; one line declares one value,
  and a comma is an error naming the line to write: "a 'generic' line declares one codegen value: write 'generic
  $left_type' and put the next one on its own line".
- **Call sites are positional, always, in the order of the lines.** There is no named form: `List<Integer>()`,
  `Weapon<Integer, true>(10)`.
- **`$` is for generics only.** Every `$name` a class uses has a `generic` line and is supplied by the caller, so
  the lines are exactly "what a caller must pass". A `$name` with no line is a compile error. Used as a value it
  points at [`Environment`](https://github.com/Spite-Language/spite_documentation/blob/master/programs.md#run-time-settings-environment), which is where a program's settings
  live: "'Game' has no 'generic $verbose' line, and '$' is for generics only: a program's settings are fields of
  Environment, so ..."; used as a type it is "this class has no 'generic $other_type' line, so '$other_type' cannot
  be used as a type" (`diagnostics/codegen_values`).
- **Only a class has codegen values; there are no generic functions.** A function reads the `generic` lines of its
  class and declares none of its own; a function that must accept any class takes a `type` (the built-in empty
  `Anything` accepts every class), and `value.class` is the real class
  ([reflection.md](https://github.com/Spite-Language/spite_documentation/blob/master/reflection.md#known-only-at-run-time)). Listing values on a function is the error below.
- **A class needs no constructor to take codegen values.** `library/list.spite` is `generic $element_type` and its
  functions, and `library/dictionary.spite` is `generic $value_type`; an empty constructor is still an error.
- **Every hole must be filled.** There are no defaults. A call supplying the wrong number is a compile error
  naming the class's codegen values, in order: "'Box' takes 1 codegen value(s), in this order: $held_type,but 2 were given", so the mistake is corrected from the message rather than by opening the class. A `$name`
  that is not declared is an error too, which is what a typo like `$is_magik` produces.
- Reordering the `generic` lines changes what every existing positional call site means. Where the values
  have different kinds (a class versus a `Boolean`) the compiler catches it immediately; where they are the same
  kind (`Pair<Integer, String>` swapped) it compiles and means something else, and the tests are what catch it. This
  is a deliberate, accepted trade.
- **Each set of values is its own class**, made only where a program names it, with its own copy of every
  function surviving code calls. Nothing chooses between them at run time; what a generic costs is that code,
  once per set of values used, and a generic nobody makes costs nothing. `--final-classes` prints each
  class with its codegen values already bound, which is where `true` reads as `is_magic` again.
- Writing `generics` is a parse error naming this form, and so is listing codegen values on a function,
  `func Weapon<$damage_type, $is_magic>(...)`, on a constructor or any other function: "a function no longer
  lists codegen values between '<' and '>': declare each at the top of the file on its own line ('generic
  $damage_type', 'generic $is_magic'), and write 'func Weapon(' here" (`diagnostics/constructor_codegen_list`).
  A `generic` line inside a function is an error too.
- **The values can be left out when the constructor's arguments say them**: each parameter whose declared type
  mentions a `$name` is
  matched against its argument's type (`$name` itself, `$name?` (a `T` or a `T?` both give `T`), `List<$name>`,
  `Dictionary<$key, $value>`, and the arguments and return of a `Spite.Function<...>`). A `null` argument says nothing.
  Only when every `$name` is found; otherwise the error asks for them between `<` and `>`: "'Box' takes 1 codegen
  value(s), in this order: $held_type. Write them between < and > before the arguments". `Pair("Hero", 7)`,
  `Concurrent(file.read)`.
- `$name()` makes the default of what `$name` is bound to: a class through
  its constructor with no arguments, an empty `List` or `Dictionary`, `0`, `""`, and for a `type` whose members
  are all attributes a real object. `var held: $held_type = null` is an error, since `null` belongs to `T?`
  alone ([values_and_types.md](values_and_types.md#variables-and-values)), except on an attribute the
  constructor assigns and a local a walk fills. **Bound to a `type` that requires a function, `= null` is a
  compile error naming the attribute**, unless the class's constructor assigns the attribute: no object
  can supply the function, so there is no default to make. `'held' is a Weapon, a type that requires the function 'strike', so
  '= null' has no default to make: no object can supply a function it does not have. Make its type nullable, with
  a '?', and narrow it before use, or give it a real object in the constructor` (`diagnostics/function_shape_default`).

**A `generic` line may name a constraint.**
`generic $item_type: Printable`, with the class's own `type Printable { to_string(): String }`, accepts only types that
fit it, and a use that does not
(`Shelf<Pet>`) is an error where the class is named, naming the constraint, the class and what it lacks:
"'Pet' does not fit type 'Printable', which 'Shelf' requires of $item_type: it has no function 'to_string'",
instead of an error deep inside the generic's body. The constraint is optional and names a `type` the
class declares itself: another class's `type` is never named outside it (D1779), so a class that needs one declares
its own copy ([classes_and_files.md](classes_and_files.md#files-are-classes)). In detail:

- **Fitting is what [Types](values_and_types.md#types) already means by it**: the class a `type` would admit
  fits: a class with the listed functions and attributes, `String`, a number, an enum, a `List<T>` or
  `Dictionary<Key, Value>` with what the `type` needs, and the `type` itself. Each member is compared by its type
  as well as its name, as a value given to a `type` is: "'Stone' does not fit type 'Weighed': its 'weight' returns
  a String, but the type needs an Integer" (`conformance/generic_constraint_mistyped`). A `T?` fits only a `type` that `null` answers itself:
  `null` answers `to_string()` and `to_debug()` as `null`, so a `T?` fits `Printable` and `Debuggable`. For any other
  `type` it does not fit ("'Integer?' does not fit type 'Sized', which 'Shelf' requires of $item_type: it may be null,
  and null has none of what the type needs"), and neither does a function value.
- **A constraint names a `type`, not a class, a union or a number type**: anything else is an error on the
  `generic` line: "'$count_type' is constrained by 'Integer', which is not a 'type': a constraint names a
  'type' the class given must fit, like 'generic $count_type: Printable'".
- **It is checked once per class given**, where the compiler first makes that instance, written out or read
  from the constructor's arguments. After the error the generic is compiled as if it had been given the `type`
  itself, so its body adds no errors of its own.
- **It costs nothing at run time and tree-shakes as before**: the check is the compiler's alone, and the
  instance made is the one an unconstrained line would make.

`conformance/stage6/generic_constraints`, `diagnostics/generic_constraints`,
`diagnostics/generic_constraint_not_a_type`.

**Conditions on codegen values fold.** They are decided at compile time and the untaken branch is removed (tree
shaking), in every build, `--development`, `--hot-reload` and the REPL builds included: a codegen value is part of
which class this is (`Weapon<Integer, true>` and `Weapon<Integer, false>` are two classes), so there is no
run-time value for live reload to change. To change one, change the call site.

**A `fallback` or `crash` on such a condition folds the same way.** When the condition of a `fallback` or `crash`
asks only what is decided while compiling (a `$flag`, `$slot_type == Entity` or any other test of a codegen type,
a question asked of a reflection constant (`$T.functions["run_each"]`, `.is_resumable`, `.is_mutated`,
`.is_fixed_size`, `.arguments.count()`, of a codegen type or of a walked member's class), `argument.class == $row_type`,
a class test the value's type already answers (`item == $wanted_type`), a field of
[`Build`](https://github.com/Spite-Language/spite_documentation/blob/master/programs.md#compile-time-settings-build), and `not`, `and` and `or` over them), it is decided
for each instance, with no test at run time: when it holds,
nothing is written for it; when it does not, the `fallback` answers "nothing" (recording its trace line), and the
statements after it in the same block are not compiled for that instance, exactly as after an `if` whose taken
branch returns. A folded `fallback` is held to the same rule as any other: it is allowed only in a function whose
result can say "nothing" (a `T?`, `Nothing`, or a collection), so `fallback $slot_type == Entity` followed by
`return value.id` is written in a function returning `Integer?`; in one returning `Integer`, the answer for the
other instances is written down instead, `if $slot_type != Entity { return 0 }`, which folds the same way
(`conformance/stage6/folded_checks`, [failure.md](https://github.com/Spite-Language/spite_documentation/blob/master/failure.md#a-fallback-returns-what-its-answer-type-builds-with-no-arguments)).

**A `crash` that folds to false is a compile error where the program can reach it.** A `crash` whose condition is
decided while compiling and is false would halt every time its function runs, so it is a developer's mistake the
compiler can prove, and it is reported while compiling, at the `crash`, naming the instance: `'crash
$slot_type.is_fixed_size' always halts in Slot<List<String>>: its condition is decided while compiling and is
false, so the program would stop here every time this function runs: call it only where the condition holds, or
change what the condition asks` (`diagnostics/folded_crash`, and `diagnostics/folded_build_crash` for a `Build`
field and a class test). Only a function the program reaches counts (the
same reach tree shaking keeps in a production build, worked out for an inspectable build too, where nothing is
shaken), so an instance whose function nobody calls compiles (`conformance/stage6/folded_crash_uncalled`). This
is how a library turns its rules into compile errors: a `crash $system_type.functions["update_each"]` in the
function that runs a system fails the build for the class that breaks the rule, not the run. The sides of an `and`
are checked one at a time, as two `crash` lines would be, so in `crash $system_type.functions["update_each"] and
ready` the first side folds on its own and is this compile error when it is false, whatever `ready` holds; only a
condition that mixes a run-time value in through `or` (or `not` over an `and`) is tested at run time as a whole.

**A class test that can never be true for one instantiation folds to `false`.** Inside
a generic class, `if item.class == Health { }` where `item`'s type comes from a codegen value that is not `Health` in
this copy is `false`, and its branch is removed from that copy only, since generic code that a particular copy does
not use is what tree shaking removes, so it is on purpose not an error. Outside generics the never-true test stays
an error ([control_flow.md](control_flow.md#control-flow-in-full)). A codegen value bound to a class is itself
a class test on the right of `==`, `item == $wanted_type`
([values_and_types.md](values_and_types.md#unions-in-full)).

**Asking what type a generic was given.** When a codegen value is a type, `$value_type == String` is decided at
compile time like any other condition on a codegen value, and only the branch taken is compiled, so each branch
may use what only that type has, which is what lets one generic class treat text, numbers, lists and classes
differently. It is the class test ([control_flow.md](https://github.com/Spite-Language/spite_documentation/blob/master/control_flow.md#valueclass--class)) asked of a type instead of a
value. A type name asks for exactly that type; four names ask for a kind, since the type has arguments the test
does not want to spell:

| Test | True when the type is |
|---|---|
| `$value_type == List` | any `List<T>` |
| `$value_type == Dictionary` | any `Dictionary<Key, Value>` |
| `$value_type == Null` | any `T?` (`Null` is a member of the union a `T?` is) |
| `$value_type == Enum` | an enum (`JsonReader` and `BinaryReader` read one through its reading from text) |
| `$value_type.is_number` | any number class, `Tiny` to `Double`, never `Boolean` |

A union name is true for any of its members, and a `type` name for any type that fits it. `.is_number` asks
whether the type is a number class, answered from the class and never from a name a program writes
([values_and_types.md](https://github.com/Spite-Language/spite_documentation/blob/master/values_and_types.md#a-number-library-declares-the-type-it-needs)).
Such a test always folds, `--development` included, because the
branch it rules out would not compile.

**Only what survives folding is compiled.** A function of a generic class is
compiled, and so type-checked, for one instantiation only when code already compiled for the program names it (a
call that survived folding, a function value, a reflection table, or a call through a `type` the class is a
member of), so a helper reached only from a branch the instantiation rules out is never checked against that
type. When a folded `if` (or `else if` chain) takes a branch that ends in `return`, the statements after it in the
same block are not compiled either, and the names they read count as used, as for the untaken branch. This holds
in every build, inspectable ones included. A class that is not generic still compiles every function, so a
mistake in an uncalled one is still reported. `conformance/stage6/folded_helpers`.

**The types a type was built from are read by their codegen names**: `$value_type.element_type` for a
`List<$element_type>`, `$value_type.key_type` and `$value_type.value_type` for a `Dictionary<$key_type, $value_type>`, `$value_type.value_type` for a `$value_type?`, and a generic
class's own names for one of its instances, the names this section already gives the containers. Reading a name
the type does not have is an error listing them: "a List<Integer> has no codegen value named '$value_type': List<$element_type>, Dictionary<$key_type, $value_type> and $value_type? name theirs, and a generic class names its own"
(`diagnostics/every_attribute`). `conformance/stage6/every_attribute`.
Compared in a condition, a name read this way folds like `$value_type` itself: `if $list_type.element_type ==
Float`, `else if $map_type.key_type == String`, `$holder_type.held_type != Item`, to any depth and in every
branch of an `else if` chain, so a branch that does not fit the instantiation is not compiled
(`conformance/stage6/codegen_member_fold`). An `and` whose left side folds to
`false`, or an `or` whose left folds to `true`, folds without its right side, which may then ask what the type does
not have: `$list_type.element_type == List and $list_type.element_type.element_type == Float`.

**A codegen value that is a type reads as its class**: a member
read through it, `$component_type.name` or `$component_type.attributes`, is read from the bound class's
`Spite.Class`, exactly as `Health.name` is, and no value of the type is made. `$component_type` alone in an
expression is still the "is a type here" error (`conformance/stage6/codegen_class_name`).

The `generic` line is a declaration like `var`, and follows a header pattern shared with `singleton`.

---

Next: [Reflection](reflection.md).
