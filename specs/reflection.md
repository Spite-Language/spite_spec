# Reflection

The specification of [Reflection](https://github.com/Spite-Language/spite_documentation/blob/master/reflection.md): every rule of this part of the language, with
its edge cases and its exact error texts. The page teaches; this one decides, and where they disagree, the page
is the bug.

## Reflection objects

**Every class is an instance of `Spite.Class`**, every namespace of `Spite.Namespace`, every function declared in a
class of `Spite.FunctionDeclaration` (and bound to an instance, of `Spite.Function`), every attribute declared in a
class of `Spite.AttributeDeclaration` (and bound to an instance, of `Spite.Attribute`, which holds its `declaration`, its
`value` and its `owner`), and every argument of `Spite.Argument`. A class's `.attributes` is a `Dictionary<String, Spite.AttributeDeclaration>` and an instance's a `Dictionary<String, Spite.Attribute>`, keyed by name; `Whisper.attributes["to"]` names an attribute as a value, and a literal key on a class named in the code that declares no attribute of that name is a compile error naming the class (`tests/attribute_named_as_value`, `conformance/attribute_named_unknown`). A function's `.reads` and `.writes` are the `Spite.AttributeDeclaration`s of its class its code reads and writes, following every call to its own class (`tests/function_accesses`). A function's and an attribute's declaration answer `.source_file`, the `File` it is written in (a reopening's file for a member the reopening declares; `tests/declaration_source_file`), filled only in a program that asks it, and a bound function reads it through its declaration. They live in the `Spite` namespace (`Spite.Class`, not `SpiteClass`), so nothing in it can be
mistaken for a class of a program. `Spite` is a reserved root namespace: a program's `spite/` folder may only
reopen its classes, and `load "spite"` is an error ([packages.md](https://github.com/Spite-Language/spite_documentation/blob/master/packages.md#the-spite-namespace-is-reserved)).
Their members are the tables of [The `Spite` classes](https://github.com/Spite-Language/spite_documentation/blob/master/reflection.md#the-spite-classes).

**Every member is a get-only attribute named for what it answers**, read without `()`: a question is `is_...`, a
collection is plural. A question that takes something is a collection indexed or filtered instead
(`Monster.functions["alive"]`, `hurt.arguments[1].is_mutated`). What acts rather than answers stays a function:
`call_function()`, a `Spite.Call`'s `call()`, and a list's own `count()`. A name is a `String`, constant text the compiler writes once, so passing it around allocates nothing.

**Every member is a getter with no setter**: the data sits in private fields and `get_name()` and the rest answer
it, so `described.name = ...` is "'name' is read-only". **A name starting with `_` is private** (everywhere but
on a parameter, where `_` means unused on purpose; [Lexical structure](classes_and_files.md#lexical-structure)): it
is read, written or called only inside its own class (a reopening is inside), and from anywhere else it is an error
naming the getter when there is one: "'_name' is private to 'Class': a name starting with '_' is used only inside
its own class, and 'name' reads it from outside". The members only the compiler can write (`call_function()`, the
value of a bound attribute) are the compiler's reopening of those classes, printed by `--final-classes` like any
member.

**What `.functions` contains**: the functions a class declares, plus the template instances generated for it,
because those are functions of the class in the program as built; reflection describes the program that exists,
not the source as written. **The list holds every function the class has once compiling ends**, in the order they
were made, so it does not depend on the order classes were compiled in. **A constructor is not one of them**: it does
not answer on an instance, it makes one. A class of the standard library describes its members too:
`Console.functions` lists its functions like any program class's.

**`.class` is a real `Spite.Class`, not the class's name as text**, and so are `Spite.Attribute.class` and
`Spite.Argument.class`: `function.arguments[0].class == ServerContext` is an identity comparison the compiler
checks, where a test on the name's text quietly turns false when a class is renamed. Printing a `Spite.Class`
prints its `.name`, and printing a `Spite.Namespace` its dotted name. **An enum's reflection object is a
`Spite.Enum`, not a `Spite.Class`**: an enum named as a value and `.class` on one of its values answer it, and its
`.values` are its `Spite.EnumValue` declarations keyed by name ([Enums in full](values_and_types.md#enums-in-full),
`tests/enum_reflection`). A `Spite.Class` describing an enum type (an attribute's `.class`) answers only `.is_enum`.

**A namespace is an instance of `Spite.Namespace`**: the tree of folders made readable, `.parent` walking up,
`.classes` and `.namespaces` walking down. `.namespace` and `.parent` are `Spite.Namespace?`, and the chain ends
at `null`; there is no root object. `Spite.Namespace.instances` is every namespace of the program's own classes,
parents first, in the order the classes were found. A namespace lists no enums, unions or types: each belongs to
the class that declares it (D1745), never to a folder. **A class lists what it declares**: `.enums`, `.unions` and `.types` are
`Dictionary<String, Spite.Enum>`, `Dictionary<String, Spite.Union>` and `Dictionary<String, Spite.Type>`, keyed by name in
declaration order; a `Spite.Union` and a `Spite.Type` answer `.name`, `.name_with_namespaces` and `.owner`, and a union's
`.members` is a `Dictionary<String, Spite.Class>` keyed by each member's name with namespaces. Since a union or a
`type` is never named outside its class (D1779), this is the only way another class reaches one
(`tests/class_declarations`). **A class and a namespace with the same dotted name is a compile
error.**

**`.source_files` and `.source_directories` are where a class and a namespace come from**: a `File` for every file
that declares or reopens the class, and a `Directory` for every directory a namespace's classes come from, each in
load order. A question about a path is asked of those `File` and `Directory` values; reflection has no other path
member but a declaration's `.source_file`, the one file a function or an attribute is written in. They are build-machine paths: a shipped program that needs the files beside a package copies them into its
output ([packages.md](https://github.com/Spite-Language/spite_documentation/blob/master/packages.md#files-beside-a-packages-source)).

## A class and an instance

- **`Monster.attributes` and `Monster.functions` are declarations, `Spite.AttributeDeclaration` and
  `Spite.FunctionDeclaration`; `troll.attributes` and `troll.functions` are the same members bound to `troll`,
  `Spite.Attribute` and `Spite.Function`.** A class's members are information and an instance's are data: a
  declaration never gives a value. A PascalCase receiver is the class and a lowercase one an instance, the
  convention [Foreign libraries](foreign_libraries.md#foreign-libraries) uses for `user32.Input` against
  `user32.input_mouse`.
- **A function walked over, or handed, one kind takes that kind.** A declaration given to a `Spite.Attribute`
  parameter is "'describe' takes a 'Spite.Attribute', an attribute bound to an instance, and 'health' here is a
  declaration of 'Monster', which has no value: declare the parameter 'Spite.AttributeDeclaration', or walk an
  instance's attributes", and a bound attribute given to a `Spite.AttributeDeclaration` parameter is the error
  naming `Spite.Attribute` the other way round.
- **A bound attribute's `.value`** reads and writes the instance's field, typed as the field when the attribute is a
  constant. `.value` of an attribute that describes a declaration is "'attribute.value' reads an attribute of an
  instance, and 'attribute' describes a declaration: walk the instance's own attributes, 'monster.attributes', or
  read 'monster.attributes[attribute]'".
- **`value.attributes[attribute]`**, with a declaration, is that field of `value`, read or written: a plain typed
  field access when `attribute` is a constant, and a call to the getter where the class declares one.
- **The bound forms are ordinary parameter types.** A parameter typed `Spite.Attribute` or `Spite.Function` makes no
  template ([Templates](metaprogramming.md#templates)): a function that takes one is called by a walk over an instance's
  members, and a template is written over the declaration with the object as a parameter of its own.
- **A bound function** is a function value of that instance ([functions_and_operators.md](https://github.com/Spite-Language/spite_documentation/blob/master/functions_and_operators.md#functions-are-values)),
  so a function named on an instance and its reflection are the same object. Calling it always calls. Its `.owner` is
  the instance it is bound to, and its `.declaration` the `Spite.FunctionDeclaration` it is bound from; its
  `.name`, `.arguments`, `.returns`, `.is_resumable`, `.returned_literal`, `.reads` and `.writes` are read through
  the declaration. A class's declarations have no `call_function()`: one runs only on an object, through a bound
  function or a `Spite.Call` (`tests/function_declarations`). A function value given where a
  `Spite.FunctionDeclaration` is wanted is its declaration.
- **`Spite.Call(declaration, instance)` builds a call without running it**, the only way to build one: the
  declaration is a `Spite.FunctionDeclaration`, and `instance` must be of the class that declares it.
  `.arguments["name"] = value`
  fills each argument once, and `call()` runs it. With the function and the arguments known while compiling it is
  the plain direct call, and no object exists. An argument left unfilled is "'call' leaves the argument 'velocity'
  of 'run_each' unfilled: set 'call.arguments["velocity"]' before 'call()'" where the compiler sees it; otherwise
  `call()` halts naming it. An instance of another class is "'rock' is a 'Rock', and 'hurt' is declared by
  'Monster': a call runs a declaration on an instance of its own class". The rules and every error text are
  [Calls built before they run](metaprogramming.md#calls-built-before-they-run).
- **A walk over another class's attributes sees its private ones**, since a walk that skips some silently builds an
  incomplete copy, column or layout: `.attributes` lists them in declaration order with the rest, and
  `value.attributes[attribute]` and a bound attribute's `.value` read and write them. Naming `_x` outside its class
  stays the private error. A serializer and `to_debug()` leave them out by testing `attribute.name`, which folds.
- **A number's, a `Boolean`'s and a `String`'s `.attributes` is empty**: the compiler stores those values itself,
  so the fields their library classes declare to say so are not attributes a walk could read or write.
- **A `List<T>`'s or `Dictionary<Key, Value>`'s attributes are its entries** (named by index or by key), not the fields of the
  class that stores them, so a class object's `.attributes` is empty for a collection.

## Reaching a class

- **A class name read as a value is its `Spite.Class`**, member by member, with no instance anywhere:
  `Weapon.name` is `Weapon`, and `Weapon.namespace` is the same namespace `weapon.class.namespace` gives. A function
  of the class described called on its name (`Weapon.debug()`) is "'Weapon' is a class, not a value: write
  'Weapon()' to make one, and give it a name with 'var ... = '", since there are no static functions.
- **A class name where a `Spite.Class` is wanted is its class object**: an argument, `var kind: Spite.Class =
  Health`, an assignment to such a name and a `return` from a function returning one; a codegen value bound to a
  class passes the same way. Anywhere else (`var kind = Health`, an argument whose parameter is a `type`) a bare
  class name is "'Health' is a class, not a value", naming the `Spite.Class` form.
- **On the right of `==` a class name is a class test, except when the left side is itself a `Spite.Class`**: then
  the two class objects are compared, so `component.class == component_class` and `kind == Health` both mean "the
  same class".
- **`class` is an inherited attribute of every instance, not a keyword**: inside a function it is the class the
  function answers on. A local or an attribute named `class` takes precedence (that is how `Spite.Attribute.class`
  reads its own field), and `class ClassKeyword {` at file scope fails at 1:1 with "there is no 'class' keyword",
  because a file is a class named after the file. **`namespace`** is the same kind of name for the class's
  namespace.
- **`.class` through a `type` or a union answers the real class**, from the object's own tag at run time; an
  object literal answers `Object`.
- **Describing a class makes no object of it.** A class object's `.attributes` are its declarations, and its
  `.functions` are read from the class as the compiler built it. The program's entry class's `.functions` is empty.
  A class object and a namespace object are each made once, whichever thread asks first.
- **`Spite.Class.instances` is every class in the program**, in dotted-name order: the classes of the program's own
  folder and of every package it loads, not the standard library's, not the anonymous
  classes behind object literals, and not a generic class before its codegen values are supplied.

## Known while compiling

- **A constant** is a reflection object the compiler can identify: a class, enum or namespace named in the code (a
  name that is both an enum and a class is the enum), `$T`, `class` and `namespace` inside a function; `value.class`,
  `value.attributes` and `value.functions` when `value` is a local, parameter or attribute whose declared type is a
  class (not a `type`, a union, an optional or a reflection class); any member of a constant; `[]` on a constant
  list with a literal name or number; `count()`, `is_empty()`, `first()`, `last()`, `filter(function)` and the member
  templates on a constant list; a text function on a constant name; `==`, `!=`, `and`, `or` and `not` of constants;
  and a `var` initialised with a constant and never assigned again, when the function never needs its value at run
  time, `null` from a `[]` that finds nothing included. `filter(function)` asks a function whose body is one `return`, with its parameter bound to each element.
- **What folds.** A question on a constant is `true` or `false` in the generated code, a count is a number and a
  name is a literal. An `if`, `fallback` or `crash` on a constant keeps only what runs, and one that always holds is
  not an error. `[]` with a literal name answers a constant `T?`: present, a narrowing `if`, `fallback` or `crash` on it
  folds to nothing and still narrows that path for what follows; absent, its branch is never compiled.
- **`each(function)` over a constant list is unrolled** when the function is one of the class's own, takes one
  parameter, and every element fits it: one call per element, and no list. This holds for the attributes of a class,
  the classes of a namespace (`Shop.Tools.classes.each(make)` makes one copy of `make` for each class, in which the
  parameter is the class), the values of an enum (`Phase.values.each(run)` calls `run` once for each value) and the
  known functions and arguments of [Walks](metaprogramming.md#walks).
- **`Enum.values[n]` answers `Enum?`** like every `[ ]`, also with a literal `n`, so a `crash` narrows it;
  `Enum.values.find_by_name(text)` is the text read as the enum, an `Enum?`.
- **A function called with a constant for a reflection parameter is compiled once for it**, whether it is the
  calling class's own or one called on another object (`helper.describe(attribute)`), a
  *specialisation* named `<function>_for_<member>` in `--final-classes` (a number is added when two would share a
  name). Inside it the parameter is the constant: `attribute.class` and `argument.class` are types, `part()` makes
  an instance of a constant class `part`, `monster.attributes[attribute]` reads and writes the field, and a function
  whose result is typed by its reflection parameter (`func made(argument: Spite.Argument): argument.class`) answers
  that type. A specialisation is not one of the class's `.functions`; the function itself is compiled only when
  something calls it with a run-time object or uses it as a value.
- **A run-time object** (a value typed `Anything` or a union, a reflection object kept in an attribute, anything
  typed at the REPL) is answered from the run-time tables, built only for the classes such a read can reach, and for
  every class in a `--repl`, `--repl-port` or `--development` build. A run-time function's `.reads` and `.writes` come
  from a table of every function the program makes an object for, emitted only when some code reads them at run
  time. A standard library function, a function value taken through a `type` and every function of a `--hot-reload`
  build are not in it, and asking them halts. A run-time object answers the questions a constant does
  (`.is_stateful`, `.is_list`, `.index`, `argument.is_mutated` and the rest); the answers to the kind questions,
  `.is_mutated` and `.returned_literal` are written into the tables only when some code asks one of them at run
  time, and `.returned_literal` of a function whose body is not one `return` of a text literal halts. A run-time
  `attribute.value` is `Anything?`, a number
  held with its class's tag and text boxed. Handing one to a function whose result is typed by its reflection parameter is the error shown
  [on the page](https://github.com/Spite-Language/spite_documentation/blob/master/reflection.md#known-only-at-run-time); naming a type through a reflection object that is not a constant is "'<path>'
  is not a class known while compiling, so it cannot be a type here"; and using a specialisation's parameter as a
  run-time value is "'<name>' is a reflection object known while compiling, so this function is compiled once for
  it, and it has no run-time value to pass on here".
- **Reflection may be as detailed as it likes, because what is not used is not emitted.** Which parts of it a
  program touches is decidable while compiling, so tree shaking is exact: a program keeps the tables only for what
  it reads at run time, and a program that only asks constants keeps none.

## The names reflection gives every object

**`class`, `attributes` and `functions` cannot name an attribute or a function of a class.**
Neither can a `get_` or `set_` function whose member would be one of them (`get_attributes`). A class member of the
same name would be read in place of reflection's, silently: a class with `var attributes = Table.Attributes()` would
make every `JsonReader` of it read each key as one the class does not have. The error is

`the attribute 'attributes' has a name reflection gives every object: 'value.attributes' lists its attributes, and
JsonReader, JsonWriter and every attribute walk read through it, and one of its own would hide it.
Name it something else, like 'table_attributes'`

with the suggestion made from the class's own name, and for an accessor `the function 'get_attributes' is read as
'attributes', a name reflection gives every object: ...`. The classes of the `Spite` namespace are exempt: they are
reflection. Locals and parameters may use the names. A class of the standard library never declares one.

## Functions of `Spite.Class`, and why there are no static functions

**Spite has no static class functions.** A class is an instance of `Spite.Class`, an ordinary standard library
class with an ordinary declaration, so a member that belongs to the class rather than to its instances is a member
of that object, and `Spite.Class` is where it is declared, with its default. `library/spite/class.spite` declares
`.is_singleton` that way:

```gdscript
var _singleton = false

func get_is_singleton(): Boolean {
    return _singleton
}
```

**A class file cannot replace a function of its class object.** A class's name is a constant naming an instance
of `Spite.Class`, as any constant names a value, so `Gadget.has_function(name)` is a call on that instance and runs
`Spite.Class`'s function. Every function a class file declares belongs to the class's instances, whatever its name:
a `to_string()` in `gadget.spite` is how a gadget prints, and `Gadget.to_string()` still answers what
`Spite.Class` says. A singleton says what it is with its `singleton` line
([classes_and_files.md](https://github.com/Spite-Language/spite_documentation/blob/master/classes_and_files.md#singletons)), and declaring `is_singleton` in a class is "a class says
it is a singleton with a 'singleton' line at the top of its file, not with a function: delete 'is_singleton()' and
write 'singleton' as the file's first line".

- **`Spite.Class` is reopenable, like any other standard library class, and that is the only way to change what a
  class object answers.** Reopening it changes a function or a default of every class object for the **whole
  program**, and adding a member gives every class object that member. Nothing about this is silent: a changed
  default shows per class in `--final-classes`, with the root it came from. The compiler has no warnings
  ([Style](style.md#style)), so visible generated output is the whole mitigation.

**Why a member and not a declaration**: it is the same philosophy as a configuration file being a class that gets
reopened, rather than a static file. A static line can only state a value; a function can compute one, and it costs
the language nothing because functions already exist.

---

Next: [Packages, namespaces, and mods](packages.md).
