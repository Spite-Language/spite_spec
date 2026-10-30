# JSON and binary

The specification of [JSON and binary](https://github.com/Spite-Language/spite_documentation/blob/master/json.md): every rule of this part of the language, with
its edge cases and its exact error texts. The page teaches; this one decides, and where they disagree, the page
is the bug.

## JSON is reflection, not a library

Writing JSON is walking a value's attributes and
reading it is a series of symbol keyed writes, so JSON needs no machinery of its own: a `type` shape is the JSON
shape, and the JSON pair is written with the language's own metaprogramming in `library/`, like the rest of the
standard library ([Pure Spite](standard_library.md#pure-spite-dissolving-the-runtime)). JSON is a format
for talking to foreign systems; it is not what Spite programs send each other
([The wire format](https://github.com/Spite-Language/spite_documentation/blob/master/targets.md#the-wire-format)).

- **Writing never fails at run time**: a class that cannot be serialised is a compile error, the same check
  isomorphic classes need. Only parsing can fail, and it follows the failure model: a parse that may not succeed
  returns a `T?`, and a variant that crashes exists for callers who want the report instead. The compile-time
  check: when a program's line makes a `JsonWriter<T>`,
  `JsonReader<T>`, `BinaryWriter<T>` or `BinaryReader<T>`, the compiler walks `T` (through `T?`, list
  elements, dictionary values and every attribute of every class it reaches) and a `union`, a `type`
  (`Anything` included) or a function value is an error at that line naming the attribute path; the rest of the
  four classes' files is then not reported on, so one mistake is one error (`diagnostics/json_unwritable`).
  Compile time only. The error names the class and what it does: `<class> cannot write '<T>'` for a writer,
  `cannot read` for a reader, and the reason ends `JSON does not say which member a value is` or `the bytes do not
  say which member a value is`, and `has no JSON form` or `has no binary form`.
- The one exception to "writing never fails" is below: a `Float` or `Double` JSON cannot hold.

**Four classes: `JsonWriter`, `JsonReader`, `BinaryWriter`, `BinaryReader`**, and no helper class beside them. Each names its type and is made once
and reused, and each call carries its own input. Each class is generic, and every conversion it makes is a function the compiler writes from it for
the types a program actually uses, so a program that never names one carries none of it: no table of
types, no reflection and no registry exists at run time. At run time a conversion costs what its Spite says:
`JsonWriter.write(value)` makes one `String` per member and joins them, `JsonReader.read(text)` walks the text once, and the binary pair writes and reads each number with one machine store or load.

```gdscript
var writer = JsonWriter<Order>()        # made once, reused for every order
var text = writer.write(order)          # String: never fails
var reader = JsonReader<Order>()        # made once, reused for every text
var read = reader.read(text)            # Order?: null on text that is not an Order
crash read                              # when the text is the program's own and a bad one is a bug

var packer = BinaryWriter<Order>()      # made once, reused for every order
var bytes = packer.write(order)         # Buffer: never fails
var unpacker = BinaryReader<Order>()    # made once, reused for every buffer
var unpacked = unpacker.read(bytes)     # Order?: null on bytes that are not exactly one Order
var many = BinaryWriter<List<Order>>()  # many values are one value: a list of them
var received = connection.read_as_buffer()       # Buffer?: what a connection has
crash received
var from_socket = unpacker.read(received)
```

The names: `write` and `read`, and nothing else: a reader answers a `T?`, which `crash` narrows when a
failure is a bug, and many values in one buffer are one `List<T>`. The old name `Json` is an error that names the new
pair: `there is no 'Json': it is two classes, 'JsonWriter<T>()' whose 'write(value)'
makes the text, and 'JsonReader<T>()' whose 'read(text)' answers a 'T?'` (`diagnostics/json_split`).

- **A JSON serializer is made with no arguments and reused**: `JsonWriter<T>()` and `JsonReader<T>()` name their
  type, and each call carries its own input: `write(value: T?): String`, which writes `null` for an empty `T?`,
  `read(text: String): T?`, reading the whole text as one value. Nothing
  is held between calls but the serializer's settings, so one writer or reader serves a whole program.
- **A binary serializer is made with no arguments and reused, as a JSON one is**, and it is a serializer and
  nothing more: it turns a value of its type into bytes in a `Buffer` and back, as the JSON pair turns one into
  text, while reading and writing single numbers at positions is the `Buffer`'s own
  ([standard_library.md](standard_library.md#bytes-are-a-buffer)). `BinaryWriter<T>()` has
  `write(value: T): Buffer`, and `BinaryReader<T>()` has `read(bytes: Buffer): T?`, which reads one value that
  must fill the bytes. The format's numbers are little-endian. There is no reading from a `Memory.Address`:
  bytes from a `Tcp.Connection` or a `File` are a `Buffer` already. Many values are one value, `BinaryWriter<List<T>>`
  and `BinaryReader<List<T>>`, and a part of a buffer is read by taking it first with `read_buffer_at`
  (`tests/binary_many_values`). The value a writer takes is a `T`, not a `T?`: an empty `T?` is narrowed by the caller, who then
  knows whether anything was written (a `T?` inside a value has its presence byte). Nothing is held between
  calls.
- **What it is written with** is reflection, a walk of the value's attributes with `each`
  ([reflection.md](https://github.com/Spite-Language/spite_documentation/blob/master/reflection.md#known-while-compiling)), and
  [Codegen values (`$`)](metaprogramming.md#codegen-values-) and nothing else, as
  [How it is written](https://github.com/Spite-Language/spite_documentation/blob/master/json.md#how-it-is-written) shows. The binary walk has no helper class: `BinaryWriter<T>` and `BinaryReader<T>` hold it themselves, and a nested value, list element or
  dictionary entry is walked by a `BinaryWriter<attribute.class>()` or `BinaryReader<attribute.class>()` of its own that
  continues in the same buffer at the same position.
- **An enum is written by walking its values**, which a generic may do for the enum it is given
  ([values_and_types.md](https://github.com/Spite-Language/spite_documentation/blob/master/values_and_types.md#enums)): that is how `BinaryWriter` and `BinaryReader` find an enum value's
  index and how many values the enum has, with no table at run time: writing one costs one comparison per value
  of the enum, and reading one a comparison per value and a text-to-enum cast.
- **What each type becomes** is [the table on the page](https://github.com/Spite-Language/spite_documentation/blob/master/json.md#what-each-type-becomes), applied again to whatever a value
  holds. JSON text escapes `"`, `\`, line feed, carriage return and tab as `\"`, `\\`, `\n`, `\r`, `\t` and every
  other control character as `\u00XX`, and reading turns any `\uXXXX` back into UTF-8, surrogate pairs included. A
  class's attributes are written in declaration order, leaving out private (`_`) ones as `to_debug()` does
  ([standard_library.md](standard_library.md#system-classes)) and ones holding a singleton, which are never read
  either; a `Dictionary`'s entries in the order they were set. Both questions fold where the walk is compiled, so a
  skipped attribute costs nothing.
- **A walk reads every attribute**, so an attribute a program only ever hands to a writer counts as read, and the
  unused-attribute error does not fire for it .

The details:

- **The binary format** is [The binary format](https://github.com/Spite-Language/spite_documentation/blob/master/json.md#the-binary-format) and [the table](https://github.com/Spite-Language/spite_documentation/blob/master/json.md#what-each-type-becomes):
  little-endian fixed-width numbers, `Boolean` as one byte, an enum as its index in the smallest of 1, 2 or 4
  bytes that holds every index, text as a varint length and UTF-8, `T?` as a presence byte, lists and dictionaries
  as a varint count and the items, nested objects inline, and no header. The varint
  is unsigned LEB128, and a count or length above what an `Integer` holds, or above the bytes left, fails the read.
- **`BinaryWriter<T>.schema()` and `BinaryReader<T>.schema()` answer a `Long` the compiler works out** (the
  name is provisional): the 64-bit FNV-1a hash, as a signed `Long`, of the
  walk's text: the class's qualified name, then `name:type` for each attribute the bytes carry (not `_...`, and
  not one holding a singleton), in
  order, joined by `;` inside `{ }`, a nested class written the same way, `List<T>`, `Dictionary<Key, Value>`, `T?`, an
  enum as its name and `(` its values joined by `,` `)`, a class already being written by its name alone, and any
  other type by its name. It is a bodiless declaration the compiler supplies, a C macro that is the constant,
  so it costs nothing at run time and a program that never asks carries none of it. A writer and a reader of the
  same `T` answer the same value (`conformance/stage6/binary_schema`).
- **The bytes are a `Buffer`**: one block, no count per byte, and a reader borrows nothing, since each
  call reads the block of the buffer it is given and keeps nothing of it. The library's binary classes grow the
  buffer and write into its block directly, which only `library/` may do.
- **`BinaryReader` answers `null` for every bad input** (bad input is a normal condition, never a crash):
  bytes that end inside a value, a `Boolean` byte other than 0 or 1, a presence byte other than 0 or 1, an enum
  index past the enum's last value, and a count past the bytes
  left, and bytes left over after the value.
- **The API is a class you make once and reuse**, with `write` and `read`. The generic is on
  the class because that is where Spite puts generics. `write` needs no crashing twin, since it cannot fail.
- **`JsonReader` is strict about what it cannot use and lenient about what it does not need**
  ([Reading input you did not write](https://github.com/Spite-Language/spite_documentation/blob/master/json.md#reading-input-you-did-not-write), and
  [Failure](failure.md#failure-three-outcomes-and-no-others)'s three outcomes). An unknown key is skipped,
  whatever it holds, because foreign systems send more than a program asks for; a missing attribute keeps the
  default its class declares (`tests/json_basics`), the value [Variables and values](values_and_types.md#variables-and-values)
  gives anything never set. Besides a value of the wrong kind, a missing brace and anything after the value
  (`{} x`) make `read` answer `null`: the object would be wrong, and a wrong object that looks right is a
  surprise.
- **A key in camelCase or PascalCase reads into the snake_case attribute it spells.** The exact name wins:
  `JsonReader` first compares the key with every attribute's name, as it always did, and only a key that matched
  none is compared with each attribute's name written in camelCase and in PascalCase (the words after the
  first, or every word, capitalised and the underscores dropped: `buy_price` is `buyPrice` and `BuyPrice`). A key
  that matches neither is skipped as before. A snake_case key never matches a spelling (those have no
  underscores), and only a snake_case name has one, so no key can match two attributes; when the text holds both
  `buy_price` and `buyPrice`, the later one wins, as a repeated key does. `JsonWriter` writes the attribute's own
  name. The spellings are compile-time constants a walked attribute answers, `attribute.camel_case_name` and
  `attribute.pascal_case_name` ([metaprogramming.md](metaprogramming.md#templates)),
  and `JsonReader` compares them in a second walk, `created.attributes.each(read_camel_attribute)`, that runs only
  for a key the first walk did not match: snake_case input runs the same comparisons it did before, and a program that reads
  no JSON carries none of it (`conformance/stage6/json_camel_case`).
- **A key that is not an attribute's name comes from the serializer's optional `keys` attribute, a map keyed by
  attribute objects and set before the serializer's first use**: `{MeshRecord.attributes['minimum_level_of_detail']: "min_lod"}`. `JsonWriter` writes the attribute
  under its key and `JsonReader` reads that key into it, so the text round-trips. **The key replaces the
  attribute's name**: with one, neither the name nor its camelCase and PascalCase spellings read into the
  attribute any more, so each attribute has exactly one key; attributes without one keep the camelCase and
  PascalCase reading. Every serializer takes the same kind of map. A constant map folds into the code the
  serializer is compiled to: the keys are literal text, and reading matches them as it matches a name. A map that is not written
  out (filled in a loop, answered by a function, or a literal with a computed key or text) is made at run time: it is
  copied when it is set, and the compiler fills a key table from the copy then, one slot per public attribute of
  the class, so each record then costs what the constant case costs: writing reads the attribute's slot, and
  reading compares each key with the slots as it compares a constant key (`tests/json_run_time_keys`). The table
  is the compiler's, never a class of the library or a name a program can write. The copy and its table belong to
  the serializer it is set on, and changing the map it was copied from later changes neither. A map made at run
  time that gives a key to anything but one of the class's public attributes holding a value halts where it is
  set, "spite: 'JsonWriter.keys' gives a key to an attribute that is not one of the public attributes of
  'ShelfItem' that hold a value, at <file>:<line> in <class>.<function>" (`tests/json_run_time_keys_unknown`).
  A map is folded only where the compiler proves it fixed: every serializer of the class is given the same
  written-out map, and nothing changes it, holds it in a local or passes it to a function. Anywhere else (one
  serializer given a written-out map and another a map made at run time, two different written-out maps, a key
  changed through an index, `set_at`, `remove_at` or `clear`, the map held in a local or passed to a function) the
  serializer reads its own map while the program runs, looking each key up where it writes or reads the
  attribute, so a change made through any name is seen; nothing is refused (D1781 under D1758; `tests/json_keys_mixed`,
  `tests/json_keys_passed`, `tests/json_keys_changed`). The compiler decides once every function but the
  serializers' is built, so the order the program is written in changes nothing. Naming an attribute the class
  does not have, a private one or one holding a singleton is an error. Nothing in a class names its own keys.
- **A function `json_key_<attribute>()`**, where `<attribute>` is one of the class's attributes, is the old way
  to name a key and an error at the function, so a key it gave is never silently lost: "'MeshRecord.json_key_minimum_level_of_detail'
  would name the JSON key of 'minimum_level_of_detail', and JsonReader and JsonWriter do not read a function for
  it: a key that is not an attribute's name is given to the serializer in a map keyed by the attribute,
  '{MeshRecord.attributes['minimum_level_of_detail']: "..."}'" (`diagnostics/json_key`).
- **What a reader makes** (D1784). A class made without arguments (no constructor, or one without parameters) is
  made so and its attributes are filled. A class whose constructor takes arguments is read through its first
  conversion, `func Version(from: String): Version?` (D1776): `JsonReader` reads the value the conversion takes, any
  type it reads itself, and converts it, and a conversion that answers `null` makes the read `null`
  (`tests/json_converted`). A class with neither is refused at the line that makes the reader: "JsonReader cannot
  read 'Order': 'Order.items' is Item, whose constructor takes arguments, so the reader cannot make one to fill, and
  Item has no conversion from what JSON holds: give Item a constructor without parameters, or a conversion like
  'func Item(from: String): Item?'. ..." (`conformance/json_read_needs_a_maker`). `BinaryReader` makes a class only
  the first way, since its bytes hold the attributes and never what a conversion takes: "... so the reader cannot
  make one to fill: give Item a constructor without parameters" (`conformance/binary_read_needs_a_maker`).
- **A number reads into whatever number type the attribute has** through that class's conversion from text, with
  no rule of the reader's own, so `3.7` read into an `Integer` makes the read `null`, since `Integer(from: String)`
  answers `null` for it (`tests/json_number_into_integer`, D1784); JSON has one number type and the class already
  says which one it wants.
- **Infinity and not-a-number crash `JsonWriter`**: JSON
  (RFC 8259) holds neither, and a float became one through a division by zero or an overflow the program did not
  guard, which is the moron's mistake ([failure.md](https://github.com/Spite-Language/spite_documentation/blob/master/failure.md)); floats themselves keep them,
  and `BinaryWriter` writes their bits like any other. The crash shows the value and the attribute's path,
  `Class.attribute`, with `[index]` or `["key"]` for an element: `shown=inf	path=Order.price` (and the
  same for negative infinity and not-a-number; `conformance/stage6/json_infinity`). A program that wants `null`
  there checks the number first.
- `--final-classes` does not print the functions `JsonWriter<Order>` generated, because a generic class's file
  is shared by all its instances.
- **An enum attribute** is written as its value's name and read back through the enum's reading from text, so a
  name the enum does not have is a value of the wrong kind: `read` answers `null` (`tests/json_enum_text`).

---

Next: [Time](time.md).
