# Lists, dictionaries and member templates

The specification of [Lists, dictionaries and member templates](https://github.com/Spite-Language/spite_documentation/blob/master/collections.md): every rule of this part of the language, with
its edge cases and its exact error texts. The page teaches; this one decides, and where they disagree, the page
is the bug.

## Standard library metaprogramming

**Every standard library template names a `<member>`, and a member is a field or a zero-argument function without
distinction.** `map_age` and `map_get_age` are the same call, because reading an attribute already goes through
`get_<attribute>()` ([Operators](functions_and_operators.md#operators)). The compiler may answer the
field case with a direct read, but that is an optimisation, not something the writer thinks about. What a template
requires of a member is its **arity and its return type**, never whether it is stored or computed.

The templates, what each answers and what it requires of the member are [the table on the page](https://github.com/Spite-Language/spite_documentation/blob/master/collections.md#member-templates-loops-you-do-not-write),
which is normative; a `List<T>` of a class and a `Dictionary` whose values are of a class answer all of them. These, together
with `while` and the passed functions below, cover the cases a `for` loop covers elsewhere. This is why a list of
components renders with nothing new in the language: `todos.map_renders()` calls `render()` on every element and
collects the results, exactly as `todos.map_titles()` collects a field.

A member that does not fit is a compile error naming the member, what it is, and what the template needs. Two
of them name a fix: `count()` is only ever a collection's own size, so `count_<member>()` on a numeric member says
`but 'count_' needs it to return Boolean (to add up a numeric member use 'sum_stars')`; and `each_<member>()` on an
attribute says `'each_size': the member 'size' of 'Thing' is an Integer, but 'each_' needs it to be a function:
reading an attribute and discarding it does nothing`. `sort_by_` is stable (equal keys keep their order) and
is a merge sort, so its time grows as `n log n`: it reads each key once, then merges runs of an index list, and
makes the key list, two index lists and the result (`sort_by(f)` is the same template, and `Directory`'s
`entries()` sorts the names through it; `conformance/stage6/sorting_many`).

**How the templates are written.** They are templates in `library/list.spite`, each a `while` over the list's
`Memory` buffer: `func filter_member(member: Spite.AttributeDeclaration): List<$element_type>` answers every
`filter_<member>` call. The parameter names a member of the *element*, not of the list (whose own attributes are
its buffer), because its body says so: it reads the parameter on an item of the element class, `item[member]`, which is how every
[template](metaprogramming.md#templates) finds its class, one mechanism for all. `item[member]` is the field, or the field, or a call to the
zero-argument function, which is "the value held in that field" applied to members. A `String` element's
members are `String`'s own zero-argument functions (`upper_case`, `length`, `is_empty`, ...). The generator binds the template to the
element's member and checks [the table](https://github.com/Spite-Language/spite_documentation/blob/master/collections.md#member-templates-loops-you-do-not-write) before it compiles the body, so a member that does not fit is still
the error naming the member, its type and what the template needs; it writes none of the templates' C. Only the
names a program calls are compiled. A `--repl`/`--repl-port` build compiles every template that fits every
element class of a list the loop can reach, and lists them as that list's functions, so `monsters.sum_health()`
works at the prompt. A `Dictionary` answers the same names through its values (`inventory.sum_price()` is
`inventory.values().sum_price()`), and a program's own `list.spite` reopens `List` to add a template of its own,
as its `dictionary.spite` reopens `Dictionary` (over `Spite.AttributeDeclaration`, read on a `$value_type`).
A template there reads its member on an item (`item[member]`), which is what makes it a template over the element
class; one that read it on `List`'s own attributes would range over its buffer and answer nothing. `member:
Symbol<$element_type>` is an error naming the form to write: "'member: Symbol<$element_type>' is not a type: a parameter
named by a word of its function's name makes the function a template, its type says what it ranges over, and the body
says over which class: write 'member: Spite.AttributeDeclaration' and read the attribute as 'value[member]' on a value
of '$element_type'" (`conformance/plain_symbol_on_list`).

**Chains are one loop.** Each template takes what the previous one returns, and a chain means exactly its steps written out one by one.
The compiler runs a chain as one loop over the first list with no list in between: when a template is called
directly on a `map_`/`filter_` call (on a `map_`/`filter_` call, and so on) of a `List` or `Dictionary`, the
generator writes one Spite function on the first list's class (a `while` over its buffer, an `if` per
`filter_`, a `var` per `map_`, and the last template's step) and calls that instead. A `map_` in the middle
must reach a class; anything the rule cannot write (a nullable member, a union) is compiled step by step, which
means the same. The difference a program can see is only order: a member function in a fused chain runs element
by element. `conformance/stage6/fused_chain_allocations` pins what it saves: four chains run a thousand times
allocate nothing, so the whole program makes 15 allocations (its objects, the list and printing the total)
against more than 16 000 step by step.

**Passing a function for each element.** An iterator sees only the
element and the list, never the class the call is written in: `people.each_say_hello()` names a member
`say_hello` of each `Person`, and a function of the caller never answers
it. When the calling class has a function of that name, the error says to pass it
(`'String' has no attribute or zero argument function 'say_hello' for 'each_say_hello': a template reads a member
of each element, never a function of this class, so pass this class's 'say_hello' instead: 'each(say_hello)'`).
The caller's function is passed as a bound function value, owned by whoever it is bound to:

- **The forms.** `each(f)`, `filter(f)`, `any(f)`, `all(f)`, `count(f)`, `find(f)`, `sort_by(f)` and
  `sum(f)` on a `List` or a `Dictionary` (through its values), for an element of any type; `remove_where(f)` on a
  `List`, never on a `Dictionary`. `f` takes the element
  as its only argument, with exactly the element's type; [the table of templates](https://github.com/Spite-Language/spite_documentation/blob/master/collections.md#member-templates-loops-you-do-not-write) applies to what it returns (`filter`, `any`,
  `all`, `count` and `find` want `Boolean`, `sum` a number, `sort_by` a number or a `String`, `each`
  anything). `find(f)` answers the first element `f` is true for, or `null`, which is the `find_by_` template with
  `true` as its value. `count` with no argument stays the collection's size.
- **No `map(f)`.** A list has no `map` taking a function: `names.map(measure)` is "there is no 'map(function)':
  collect a member of each element with 'map_<member>()', and a value computed from each element by giving the
  element's class a read-only attribute (a 'get_<name>()' with no setter) and writing 'map_<name>()'". The value
  becomes a named member of the element's class, which every list of that class can collect and a chain fuses.
  A value that needs more than the element (a function of another instance, or of the caller's state) is
  collected by a `while`, which the loop rule leaves alone.
- **`map_` on a test.** `map_<member>()` collects values, and its member must not be a `Boolean`: the plural
  rule for collections applies to nouns, and a test (`is_alive`, `has_target`) is a question about each element.
  `monsters.map_is_alive()` is "'map_is_alive': the member 'is_alive' of 'Monster' is a Boolean, but 'map_'
  needs it to be a value to collect, not a test: write 'filter_is_alive()' to keep the elements it is true for,
  'count_is_alive()' to count them, or 'any_is_alive()' or 'all_is_alive()' to ask whether any or all of them
  pass".
- **The owner.** `say_hello` alone is bound to this instance; `greeter.greet` to `greeter`; a variable holding a
  `Spite.Function<T, R>` is called through the value. `people.filter_active().each(greeter.greet)` mixes a
  member template with another instance's function.
  A list's own function is bound to the list the same way (`numbers.each(found.append)`); a chain that passes one
  is not fused, and runs step by step. So is every library class's: a `Dictionary`'s (`keys.filter(counts.has)`, `keys.count(counts.has)`), and a
  `String`'s or a number's (`words.filter(greeting.contains)`). Any of them may also be held as a value: `var
  lookup = counts.get_at` is bound to that dictionary, and `var check = greeting.contains` to the text `greeting`
  held then. A text or a number is a value, not an object, so the function value keeps its own copy of it, in a
  small box the value lets go with itself: assigning `greeting` anew afterwards does not change what `check` asks
  (`conformance/stage6/held_value_functions`). What the function answers is what the form sees, so `counts.get_at`
  answers `Integer?`, and `keys.sort_by(counts.get_at)` is an error naming the fix: a function of your own
  that narrows it (`conformance/stage6/library_functions_passed`, `diagnostics/library_function_mistakes`).
- **How it is written.** No template changes: the same `library/list.spite` template (`each_member(member:
  Spite.AttributeDeclaration)`) is instantiated once per function and owner class, with a last hidden parameter holding
  the owner (the function's instance, passed at the call site), and `item[member]` reads as
  `owner.say_hello(item)`, or `owner(item)` when the owner is a held function value. A function written by name
  therefore costs no allocation and no indirect call; only a held value is called through `Spite.Function`. Only
  the forms a program calls are instantiated, and these instances are not listed among the list's `functions`
  nor offered at a `--repl` prompt.
- **Chains.** A passed function chains and fuses with the member templates: `filter(f)` may sit in the middle
  of a chain and any form may end one, so `people.filter_active().each(greeter.greet)` and
  `names.filter(is_short).sum(doubled_length)` are each one loop; the fused function takes each owner as a
  parameter. The element's own members still come only from the element.
- **Mistakes** name the form: `'filter(say_hello)': 'say_hello' returns nothing, but 'filter' needs it to return
  Boolean`, `'each' calls 'greet_twice' with each 'String' as
  its only argument, but 'greet_twice' takes 2`, `'each' calls 'count_to' with each 'String', but 'count_to' takes
  'Integer'`, and an argument that is no function at all: `'each' takes a function to call with each element, such
  as 'each(say_hello)' or 'each(greeter.say_hello)', and 'engine' is not one` (`conformance/each_of_a_value`).
- **The same checks hold for a member template of a program's own collection**: a generic class of the program
  with an `$element_type` whose `sum_member` is called as `bag.sum_name()` on a `String` member is "'sum_name': the
  member 'name' of 'Repository' is a String, but 'sum_' needs it to return a numeric type", as on a `List`
  (`conformance/own_member_template_mistakes`).
- **The loop rule.** A `while` that only does what `each(f)` does is under the same rule as the member templates
  ([control_flow.md](control_flow.md#a-while-that-a-member-template-already-says)). A function that needs more than the element (`print_statement(statement, depth)`) keeps its `while`, and so
  does a `while` that collects what a passed function answers for each element, since no form collects one.

`conformance/stage6/passed_functions`, `diagnostics/passed_functions`.

## Member templates over an enum value

**The templates that ask a yes-or-no question of each element also work on a named enum's values.**

- **Which templates.** `filter_`, `count_`, `any_`, `all_` and `remove_where_`, the ones whose member must
  "take nothing, return `Boolean`" in [the table](https://github.com/Spite-Language/spite_documentation/blob/master/collections.md#member-templates-loops-you-do-not-write). The others already
  take a member and compare or read it (`find_by_kind('files')`, `map_kind()`, `sort_by_kind()`), so an enum value
  adds nothing to them, and `sum_<value>` or `each_<value>` is the ordinary "no member" error.
- **How the name is read.** For `list.filter_<name>()` over elements of class `T`: when `T` has a member named
  `<name>`, it is that member, exactly as for any template. Otherwise the compiler looks at `T`'s members typed with a named
  enum (attributes and functions taking no arguments, non-nullable) and at the values each enum lists. Exactly
  one such member whose enum lists `<name>` makes the template read `element.<member> == '<name>'`, and nothing
  else changes: same result type, same fusion, same order, same cost as a `Boolean` member, one comparison of two
  small integers. A reopened enum's values are the reopening's list ([packages.md](https://github.com/Spite-Language/spite_documentation/blob/master/packages.md#reopening-an-enum-replaces-it)).
- **A union element** is read through a member every class of the union answers, with the same enum type; a
  member only some of them answer is not a candidate.
- **Errors, never a guess.** No member and no enum value: the usual error, which then also says no enum of
  `T`'s members lists `<name>`: "'Post' has no attribute or zero argument function 'deleted' for 'any_deleted', and
  no enum of its members lists 'deleted'". Two or more members whose enums list `<name>`: "'filter_<name>' could
  read '<a>' or '<b>', whose enums both list '<name>': write the comparison as a Boolean member of <T>, or pass a
  function". A member named `<name>` that is also a value of such an enum: an error naming both, since which one is
  read must not depend on what else the class declares: "'count_published' could read the member 'published' of
  'Post' or compare 'stage' with 'published', a value of its enum: rename the member 'published'".
- **A member a union's classes answer differently** (an attribute in one, a getter `get_<member>()` in another) is
  read through each class's own, for these templates as for every other.
- **No helper answers one kind of a listing.** A function that answers the part of a listing of one kind, such as
  `Directory.files()` or `Directory.folders()`, would be a second way to write `entries().filter_files()`, so there
  is none.
- **Tree shaking and run time.** Resolved while compiling; nothing exists at run time that a `Boolean` member's
  template would not have, and an enum no template reads costs nothing.

## List<T> additions

A list's members, their results and their edge cases are [the table under `List<T>`](https://github.com/Spite-Language/spite_documentation/blob/master/collections.md#listt), which is normative:
`list[index]` and `get_at(index)` are one function and answer `T?`, `null` out of range (`[]` is only
a shortcut for `get_at`, [functions_and_operators.md](functions_and_operators.md#operators); an out of
range read answers `null` and never a default that would read as a real element); `first`, `last`, `remove_first`
and `remove_last` answer `null` on an empty
list (like `[]`), `insert` clamps to the nearest end, and `set_at`, `remove_at` and `remove_swapping` halt,
naming the line. How much room a list keeps is the library's and the compiler's (its `_reserve` is private). A `List` of numbers,
`Boolean`, enums or `Memory.Address` holds the values themselves in its buffer. There is
no `for`: a list is walked with a template, a passed function ([above](#standard-library-metaprogramming)),
or a `while` that does more than they do.

**A list has no functions for bytes, and there is no list of bytes.** Every `List<T>` has the same members whatever
`T` is, and `T` is never `Byte` (the error is in
[Bytes are a Buffer](standard_library.md#bytes-are-a-buffer)): the numbers laid out in bytes, their byte order and the text bytes spell are a
`Buffer`'s ([Bytes are a Buffer](standard_library.md#bytes-are-a-buffer)). Calling one of a `Buffer`'s byte
functions on a list of any element type is the error `List has no method 'read_integer_little_endian_at': bytes are a
Buffer, which has it: make them with 'Buffer()'`, with the function's own name (`read_<number>_at`,
`append_<number>`, `write_<number>_at`, the old `write_<number>` and `_big_endian` names, `append_buffer`,
`read_buffer_at`, `read_as_string`, `read_string_at`) (`conformance/list_byte_functions`).

`contains(value)` compares with `==`, so it is there only for elements that are numbers, `Boolean`, `String` or an
enum; on a list of a class the call is `List has no method 'contains'`: ask with `any(f)` or `find_by_<member>`.
`index_of(value)` is the same comparison answering where: the first element's index, or `null` when no element is
equal, so a missing value is never read as a place. On a list of a class, `find_index_by_<member>(value)` answers
the index of the first element whose member equals `value`, the place `find_by_<member>` would have found.

Names say where: `add` does not, so it is `append` (and `prepend`); `pop()` is `remove_last()`, beside
`remove_first()`. Writing `add` or `pop` is a compile error naming the replacement: `List has no method 'add', which
does not say where: write 'append' to add at the end, or 'prepend' at the start`, and `List has no method 'pop':
write 'remove_last()', or 'remove_first()' to take from the start` (`diagnostics/old_list_names`).

## Dictionary\<Key, Value\>

Insertion-ordered, keyed by its `Key` type; its members are [the table under
`Dictionary<Key, Value>`](https://github.com/Spite-Language/spite_documentation/blob/master/collections.md#dictionarykey-value), which is normative. `dictionary[key]` is
`get_at(key)`, a `Value?` that is `null` for an absent key, and `dictionary[key] = value` is `set_at(key, value)`,
the same functions as every other `[]`. `keys()` answers a fresh `List<Key>` and `values()` a fresh `List<Value>`,
in insertion order; `copy()` and `deep_copy()` answer a `Dictionary<Key, Value>`, and `deep_copy()` copies the
values and keeps the same keys, so an object key stays the one object.

**The type names both.** `Dictionary` takes exactly two type arguments, the key type then the value type, in a
written type and in `Dictionary<Key, Value>()` alike. Any other count is `a Dictionary names two types, its keys'
and then its values', as in 'Dictionary<String, Integer>', and this one names 1`
(`diagnostics/dictionary_type_arguments`). Nothing about the key is inferred from how the dictionary is used: a
key given to `[]`, `[] =`, `get_at`, `set_at`, `has` or `remove` is an argument of type `Key`, cast to it as any
argument is ([casting](values_and_types.md#casting)), and a key that does not fit is the ordinary mismatch error:
`dictionary[key]` is the call `get_at(key)` and `dictionary[key] = value` the call `set_at(key, value)`, so a number
key meets a `String` key type through the number's `to_string()` as any argument does, with no rule for keys alone
(`tests/dictionary_keys_cast_as_arguments`, D1784).
Two dictionary types are the same type when both their key types and their value types are.

**What can be a key**, and how it is found (one key per value the type tells apart):

- a whole number (`Tiny`, `Byte`, `Short`, `UnsignedShort`, `Integer`, `UnsignedInteger`, `Long`, `UnsignedLong`)
  by its value: stored and hashed as the number, with no `String` made for it, in a lookup or in the table;
- a `Float` or a `Double` by its value: `0.0` and `-0.0` are one key, and every `not_a_number` is one key, the first
  one stored being the key `keys()` answers;
- a `String` by its bytes, and a `Boolean` by its value;
- an enum value as that value, whatever text its name is;
- an object of a class by its identity: one object is one key, and an equal-looking object is another key. The
  dictionary holds each key it stores (it counts as one more reference), so an object cannot be freed, and its
  memory reused by another, while it is a key.

Anything else is `a Dictionary cannot be keyed by 'Integer?': a key is a number, a Boolean, a String, an enum value
or an object of a class` (a nullable, a union, a shape, a function value), and a value class is `a
Dictionary cannot be keyed by '<class>': it is a value, copied wherever it goes, so it has no identity to key by;
key the dictionary by a number or a String made from it`. **A class that declares `equals`** (in any file that
declares or reopens it) cannot be a key, since finding keys by identity would miss an object `equals` calls equal:
`a Dictionary keyed by 'Date' finds each key by identity, one object one key, but 'Date' declares 'equals', so an
equal 'Date' would not be found: key the dictionary by what 'equals' compares, a number or a String`
(`diagnostics/equals_keys`).

The key and value types are the program's intention, not its storage
([write it plainly](write_it_plainly.md#a-class-is-what-you-mean-not-how-it-is-stored)): the compiler may hash,
number, sort or fold a dictionary away by any heuristic, as long as every operation answers what the rules above
say.

**A dictionary literal** is `{` then entries apart with commas or newlines (a last comma allowed), each a key, `:`
and a value, then `}`. A key is a text literal or a whole-number literal: a brace that opens on a name is an object
literal instead. The entries are set in the order written. Stored where a `Dictionary<Key, Value>` is declared, the
literal has that type and each key and value is cast to it; otherwise its key type is `String` when its first key
is text and `Integer` when it is a number, and its value type is the first value's. The same key twice is `"key" is
written twice in this dictionary: keep one entry for each key`. `{}` is an empty object literal, never a
dictionary: an empty dictionary is `Dictionary<Key, Value>()`.

**A key the literal lacks, asked for by a `crash`, is a compile error.** A local dictionary written out and read only
by keys written out is known while compiling, so `crash points["platinum"]` on a key it does not hold would halt every
time: "'crash points["platinum"]' always halts: its condition is decided while compiling and is false, so the program
would stop here every time this function runs: call it only where the condition holds, or change what the condition
asks; the dictionary written out as 'points' holds no key 'platinum'", in every build (`conformance/dictionary_literal_missing_key`).
A read of a missing key with no `crash` answers `null`, as any `[ ]` read does.

**Written out.** As JSON, a key is text: a `String` key is the text, a whole-number key is the number in quotes
(`{"7":"SEVEN"}`), an enum key is its name, and reading JSON reads each key's text back as the key type. A JSON
writer or reader of a dictionary keyed by anything else is an error at the `JsonWriter` or `JsonReader`: `its
dictionary is keyed by 'Float', and a JSON key is text: key it by a String, a whole number or an enum value`.
Binary bytes carry each key as its type is written, and `schema()` counts the key's type in, so dictionaries with
different key types have different schemas. No format writes an object key, which is an identity:
`its dictionary is keyed by 'Player' objects, each key one object's identity, which has no binary form`.

It is a hash table over two ordered lists:
`get`, `has`, `set` and `[]` take the same time however many keys there are, and `remove` takes time in
proportion to the dictionary's size. An empty dictionary allocates no table.

The member templates, declared in `library/dictionary.spite`, and the passed-function forms work on a
`Dictionary` through a copy of its values, as they do on a `List<T>`, all but `remove_where`: removing from
that copy would change nothing, so `remove_where_<member>()` and `remove_where(f)` on a dictionary are an error
naming `remove_at(key)`. To walk keys and values together, use `while` over `dictionary.keys()` and read
`dictionary[key]`.

`deep_copy()` works for classes, lists, dictionaries, unions and `type` shapes, and follows them all the way
down. A `String` is shared rather than duplicated because it is immutable; the rest of what it does, cycles
included, is [Memory's rule](memory.md#the-memory-model).

## One list

**`List<T>` is the only list** (D1707): there is no `Vector<T>` and no `Items<T>`. A program says what it keeps,
and how the items are stored is the compiler's choice, never the program's: numbers, `Boolean`s, enums and
`Memory.Address` are kept flat in the list's buffer, and a list whose items are never held anywhere else may keep
them inline (spite_compiler#205), with nothing a program can see changing. So an item read from a list is the
item itself: a change made through one name shows through every other, and the item may be kept, passed, returned
and read after the list changes size (`tests/list_shared_items`, `tests/list_items_run`). A program's own template
goes in a `list.spite` of its folder ([above](#standard-library-metaprogramming), `tests/own_collection_templates`).

## Removing many at once

A `List` removes many elements in one pass:

- **`remove_where(test)` and `remove_where_<member>()`.** One template, `remove_where_member(member:
  Spite.AttributeDeclaration)` in `library/list.spite`, so it answers both a
  member (`creatures.remove_where_dead()`: the member returns `Boolean`) and a passed function
  (`numbers.remove_where(is_odd)`, on a `List` of anything). It walks the collection once; an element that stays is
  exchanged with the first place not yet kept, and when the walk ends everything past the kept ones is released.
  The survivors keep their order, and the test sees every element once, first to last.
- **Why exchange, not move.** At every moment of the walk the collection holds each of its original elements
  exactly once, some of them already moved down: no slot is ever empty, a copy, or released while the collection
  still counts it. So whatever the test does to the collection (read it, append to it, even remove from it),
  nothing can be released twice or read after it was released; it only sees the elements in a different order.
  Moving an element down would leave its old slot holding a second copy that a test could release, or a blank that
  is no valid element. The exchange costs one more copy per element that stays
  (`benchmarks/removing_many_at_once` measures `remove_where` against C, which moves).
- **`truncate(count)`** keeps the first `count` elements and releases the rest; a `count` below zero or not below
  the size does nothing. **`swap(first, second)`** exchanges two elements, doing nothing when either index is out
  of range; with `truncate` it is how a pass of the program's own removes by something other than a function of
  the element, such as a mask of rows.
- **Chains.** `remove_where` changes its receiver, so it is not a step of a fused chain, and calling it on
  a chain is an error: `'remove_where' removes from the collection it is called on, and 'names.filter(is_short)'
  makes a new one that nothing keeps: call it on the collection itself, with one test that says everything to
  remove`. A test returning anything but `Boolean` is `'remove_where(measure)': 'measure' is an Integer, but
  'remove_where' needs it to return Boolean`.
- **Proofs.** All three count as shrinking wherever `remove_at` does: on the collection itself, and
  through the call effects of functions (a function that calls one of them records `shrink:` on the collection). So
  a proof about an element is undone by one. `swap` changes no size but changes which element an index names, so it
  counts too.
- **Cost.** Only what a program calls is compiled. `swap` is three copies of the element through a temporary on the
  C stack (`TypedMemory.swap_values`, `InlineMemory.swap_items`, written per element type by the generator), with
  no allocation. Measured at 200 000 inline `Velocity` items and a `List<Integer>` beside them: half removed,
  340 µs against 370 µs for a `remove_swapping` per removed row while walking the rows and 530 µs for one per
  entity of a despawn list; nine in ten removed, 290 µs against 490 µs and 930 µs.

`conformance/stage6/bulk_removal`, `diagnostics/bulk_removal`, `benchmarks/removing_many_at_once`.

## There is no `Heap<T>`

`Heap<T>`, a box for a value, does not exist (it is not `Memory.Heap`, the allocator). References are the default
([Memory](memory.md#the-memory-model)), so a class or union containing itself recursively just declares an
ordinary attribute of type `T`: no indirection is needed, and there is no struct-layout cycle to guard against,
since a class is always a heap object referred to by pointer. A `union Expression` of `NumberExpression` and
`BinaryExpression`, with `var left: Expression? = null` in `binary_expression.spite`, is the whole of it;
[memory.md](https://github.com/Spite-Language/spite_documentation/blob/master/memory.md#self-referential-classes-and-unions-just-work) has a tree that runs.

Writing `Heap<T>`/`Heap<T>(value)` is a compile error naming this fix: `there is no 'Heap<T>': a class, a list and
a text are references already, so a class that holds one of its own kind declares an ordinary attribute, like
'var left: Expression? = null' ('Memory.Heap' is the allocator, for containers of your own)`
(`diagnostics/old_list_names`).

---

Next: [JSON and binary](json.md).
