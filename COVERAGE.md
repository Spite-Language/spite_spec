# Coverage

Under D799 SpiteSpec is the specification: a program tests every rule and corner of the language, a compiler that
passes them all is complete, and a Markdown page of `specs/` is retired once every rule it states has
its program. This page is the audit: for each spec page, every rule, edge case and exact error text it states, the
program that covers it, and whether it is covered.

A status is one of:

- **covered**: a program written before this audit covers it;
- **added**: a program written for this audit covers it;
- **(compiler fails: ...)**: the program follows the spec and the master compiler gets it wrong, for the cause given;
- **missing**: no program yet;
- **superseded by Dnnn**: a later decision in `DECISIONS.md` in the [mortaros_spite](https://github.com/Spite-Language/mortaros_spite/blob/master/DECISIONS.md) repository replaces the rule, so no program tests it;
- **not a program rule**: nothing a program can observe.

Decision numbers are those of `DECISIONS.md` in the [mortaros_spite](https://github.com/Spite-Language/mortaros_spite/blob/master/DECISIONS.md) repository.

## Summary

| Page | Rules | Covered before | Covered now |
|---|---|---|---|
| values_and_types.md | 148 | 90 | 144 |
| control_flow.md | 51 | 19 | 45 |
| functions_and_operators.md | 56 | 29 | 53 |
| classes_and_files.md | 50 | 32 | 49 |
| collections.md | 108 | 51 | 100 |
| failure.md | 147 | 95 | 133 |
| memory.md | 107 | 34 | 81 |
| standard_library.md | 100 | 50 | 86 |
| programs.md | 40 | 20 | 35 |
| testing.md | 8 | 1 | 5 |
| time.md | 19 | 9 | 19 |
| json.md | 41 | 14 | 35 |
| reflection.md | 64 | 32 | 52 |
| metaprogramming.md | 67 | 31 | 49 |
| packages.md | 52 | 36 | 47 |
| concurrency.md | 85 | 53 | 64 |
| foreign_libraries.md | 58 | 27 | 46 |
| game_maths.md | 24 | 21 | 24 |
| style.md | 79 | 38 | 67 |
| write_it_plainly.md | 11 | 4 | 4 |
| compiler.md | 51 | 22 | 35 |
| repl.md | 47 | 19 | 25 |
| all | 1413 | 727 | 1198 |

## values_and_types.md

Rules: 148. Covered before: 90. Covered now: 144.

| Rule | Program(s) | Status |
|---|---|---|
| Every variable has a default; type inferred or annotated | tests/arithmetic, tests/numeric_types | covered |
| Defaults: Integer 0, Float 0.0, Boolean false, String "", a class its attribute defaults | tests/generic_defaults | added |
| `[]` answers `T?`, narrowed like any other value | conformance/index_reads, tests/lists | covered |
| `var x: T = null` on a non-nullable attribute is an error naming both ways out | conformance/null_defaults | covered |
| The same for a local (`'var count = 0'` for a default) | conformance/null_default_locals | covered |
| The same for a codegen-typed attribute: `$name()` is its default | conformance/function_shape_default | covered |
| `$name()` makes the default of what `$name` is bound to (number, String, class, List) | tests/generic_defaults | added |
| `= null` kept on an attribute the constructor assigns at its top level | tests/null_default_assigned_in_constructor | added |
| `= null` kept on a local a walk over its type's attributes fills | tests/walked_rows, tests/walked_row_locals | covered |
| A `var` may shadow a name in the same scope, with another type | tests/rebinding | covered |
| A `var` may never shadow a function it can see | conformance/shadowed_function | covered |
| Rebinding: evaluate, then drop the old value, then bind | tests/shadow_drop_order | added (compiler fails: the old value is dropped at the end of the scope, after the new one is used) |
| Shadowing a name never read is the unused error | conformance/shadowed_unread | added (compiler fails: compiles) |
| Escapes `\n \t \r \\ \" \{`, alone, paired, after and inside holes | tests/text_escapes | covered |
| Any other character after a backslash is that character | tests/text_unknown_escape | added (compiler fails: refuses `\q` as "not something Spite reads in a text") |
| The formatter writes `\'` as `'` | none | missing (a formatting program rewrites itself; the runner needs to format a copy) |
| A text inside a hole is a text of its own, to any depth | tests/text_nested_holes, tests/interpolation | added |
| A hole reaching the end of the file: `this '{' inside text never closes: ...` | conformance/text_hole_never_closes | added (compiler fails: reports "this text never closes") |
| A text reaching the end of its line: `this text never closes: ...` | conformance/unclosed_text | covered |
| No cast syntax; the right side is cast toward the left (assignment, argument, return) | tests/cast_toward_left, tests/arithmetic | added |
| Wider right operand of `*` is an error: "write the Long first ('total * count')" | conformance/wider_multiplication_order; conformance/wider_right_operand | added (compiler fails: names the '-' fix and writes "a Integer"; the existing wider_right_operand expects the compiler's wording, not the spec's) |
| For `-`, `/`, `%` the fix is "store the left side in a Long first ('var wide: Long = count')" | conformance/wider_subtraction | added (compiler fails: writes `'var wide: Long = ...'` for the name) |
| A Float/Double right side under a whole number is wider | conformance/wider_decimal_right | added (compiler fails: names the '-' fix for '*') |
| Signedness alone is not wider | tests/signedness_not_wider | added (compiler fails: refuses `Integer + UnsignedInteger`) |
| An integer literal that fits the left type is not wider (`small + 1` is a Byte addition) | tests/byte_literal_addition | added |
| A comparison is held to the same rule, named turned around ("write the Float first ('-0.5 < progress')") | conformance/comparison_float_right; conformance/wider_comparison | added (compiler fails: words it as arithmetic, "is a comparison in ... since arithmetic takes"; wider_comparison expects the compiler's wording) |
| A float literal under a whole number never fits a comparison | conformance/wider_comparison | covered |
| A constant expression overflowing Integer is an error naming its value | conformance/constant_overflow | added (compiler fails: compiles) |
| Anything with `to_string()` casts to text where a String is wanted | tests/direct_text | covered |
| A class with no `to_string()`: `a Pet cannot be used where a String is needed` | conformance/text_without_to_string | added |
| One-hole text: assign the value directly | conformance/lone_hole | covered |
| One-hole text: declare, `'var shown: String = clicks'` for a non-text value | conformance/lone_hole_declare | added (compiler fails: drops the written type, `'var shown = clicks'`) |
| One-hole text: declare, `'var shown = name'` for text | conformance/lone_hole_declare_text | added |
| One-hole text: return, pass, key, join, elsewhere (`to_string()`) | conformance/lone_hole_return, lone_hole_pass, lone_hole_key, lone_hole_join, lone_hole_compare | added |
| A text with a written character or two holes is unaffected | tests/lone_hole_unaffected | added |
| Text read as an enum with `to_<enum>()`, answering `T?`; assigning to `Enum?` calls it | tests/text_to_enum | covered |
| Assigning text to a plain enum is an error | conformance/enum_from_text | covered |
| Old type spellings are errors: `'Int' is spelled 'Integer'` | conformance/old_type_spellings | covered |
| An integer literal defaults to Integer; one too large becomes a Long | tests/large_literal_long | added |
| A decimal literal defaults to Float | tests/float_precision | covered |
| Lists, dictionaries, `T?` and generics work with every numeric type | tests/numeric_types, tests/generic_vectors | covered |
| `to_<type>()` on text for each number type, `null` when it does not fit | tests/text_to_number, tests/text_to_each_number | added |
| Text assigned to a `T?` number calls `to_<type>()` | tests/text_to_number | covered |
| Text assigned to a plain number is an error | conformance/text_to_number | covered |
| An integer literal too large for a Long is a compile error | conformance/literal_too_large | added (compiler fails: the compiler crashes in whole_number_value) |
| `count()`, `length()`, `index_of()` answer Integer | tests/count_is_integer | added |
| Whole-number `+ - *` that does not fit halts, in every build | tests/integer_overflow | covered |
| An unsigned overflow prints its operands unsigned | tests/unsigned_overflow | covered |
| `-value` halts for the smallest signed value, printed as `(0 - value)` | tests/negate_overflow | added |
| `-value` on an unsigned value halts unless it is 0 | tests/negate_unsigned, tests/negate_overflow | added |
| The smallest value divided by -1 halts; its remainder is 0 | tests/smallest_divided | covered |
| A value put into a narrower name halts when it does not fit | tests/narrowing_overflow | covered |
| A decimal into a whole number: NaN, infinity, out of range halt | tests/decimal_narrowing | covered |
| A change of signedness is checked; unsigned into wider signed never is | tests/signedness_change | covered |
| A singleton attribute counted from many threads checks the atomic addition | tests/singleton_counts | covered |
| A literal that does not fit: `300 does not fit in a Byte, ...` | conformance/literal_does_not_fit | covered |
| `wrapping_sum`, `wrapping_subtract`, `wrapping_multiply` keep the low bits | tests/wrapping_functions | covered |
| The check is left out under a `<` or `>` on the counter, and on constants | tests/counter_room | covered |
| Division by zero halts, naming the operation and line | tests/division_by_zero | covered |
| A constant zero divisor is an error | conformance/division_by_constant_zero | covered |
| A folded zero divisor (`total % (2 - 2)`) is an error | conformance/remainder_by_folded_zero | added (compiler fails: compiles) |
| A proven divisor is not checked | tests/divisions | covered |
| Float division by 0.0 is infinity or NaN | tests/float_remainder | added |
| Float `%` truncates toward zero and keeps the left side's sign; `% 0.0` is NaN | tests/float_remainder, tests/number_parameter | added |
| Floats keep infinity and NaN with no check | tests/nan_passes | covered |
| Shortest-round-trip printing (`0.1` on a Float) | tests/float_printing, tests/printable_values | added |
| Exponent forms `%.17g` / `%.9g`; `inf` and `nan` | tests/float_printing, tests/half_precision | added |
| Ranges: literals, locals, parameters, assignments | tests/proven_ranges | covered |
| Ranges: narrower answers (`count()`, `%`, `/`, `minimum`, `clamp`) | tests/proven_ranges, tests/count_bound_proofs | covered |
| Conditions narrow ranges; `or`, `!=`, `else` narrow nothing | tests/proven_ranges | covered |
| Branches join; loops widen first; passes are counted; a total adds a term a pass | tests/proven_ranges, tests/strided_counted_loops | covered |
| A REPL or hot-reload build proves ranges like every build | tests/hot_reload_proves_ranges | added |
| `--optimization-report` lists every check kept | tests/optimization_report | covered (compiler fails: the output differs) |
| A value put into a narrower name keeps its check whatever its range | tests/narrowing_overflow | covered |
| Numbers are classes; `this` is the value inside a reopened number class | tests/live_settings | covered |
| Writing a number as text is its `to_string()`; `Boolean.to_string()` | tests/printable_values | covered |
| A conversion belongs to the value converted: `to_<type>()` casts | tests/numbers, tests/arithmetic | covered |
| Declaring a `from_` function is an error | conformance/from_conversion | covered |
| Calling `Float.from_integer()` is an error | conformance/cast_by_name | covered |
| `type` may name a parameter | tests/type_parameter_name | added (compiler fails: "a parameter is declared 'name: Type', and found 'type'") |
| Bitwise functions of the whole numbers, their widths and counts | tests/bitwise_functions | covered |
| `<<` and `>>` are errors naming the function | conformance/old_shift | covered |
| Lone `&` is an error naming `bits_and` | conformance/old_ampersand | covered |
| Lone `\|`, `^`, `~` are errors naming their function | conformance/old_bar, conformance/old_caret, conformance/old_tilde | added |
| `shifted_right` arithmetic on signed, logical on unsigned; left loses top bits | tests/bitwise_functions | covered |
| A count of the width or more shifts every bit out | tests/bitwise_functions | covered |
| A negative shift count halts | tests/negative_shift | covered |
| The other operand is cast to the receiver's type; a wider one is an error | tests/bitwise_functions, conformance/wider_bitwise_operand | covered |
| Bitwise functions on Float, Double, Boolean are errors | conformance/bitwise_on_float | covered |
| Bits read as another type in place (`bits()`, `bits_as_float()`, ...) | tests/half_precision | covered |
| `bits_as_unsigned()` and `bits_as_signed()` | tests/signedness_change | covered |
| Maths functions and constants are members of the number classes | tests/maths_functions | covered |
| `angle.pi`: "'pi' is a constant of the class Float, not of a value" | conformance/maths_constant_on_value | covered |
| `Float.pi()`: a constant is never called | conformance/maths_constant_called | added (compiler fails: "a Spite.Class has no attribute or function 'pi'") |
| `Float.pi = 3.0`: a constant is get-only | conformance/maths_constant_assigned | covered |
| A function of a value called on the class names the class's constants | conformance/maths_function_on_class | added (compiler fails: "a Spite.Class has no attribute or function 'square_root'") |
| Short names of other languages (`sqrt`) are errors | conformance/maths_other_spellings | covered |
| `this` passes or returns the object itself | tests/this_passed | added |
| `this.name` is an error | conformance/borrowed_this | covered |
| `this.to_string()`: "a class calls its own functions by name" | conformance/this_function_call | added (compiler fails: gives the attribute message for a function call) |
| A decimal literal is a decimal in what is emitted (`1.0 / 3.0`) | tests/decimal_literal_division | added |
| A decimal literal beside a Float is a Float; two literals keep Double precision | tests/float_precision | covered |
| The last bits of a decimal result in a counted loop are no guarantee | tests/float_precision | covered |
| Every number class fits `type Number`; Boolean does not | tests/number_type, conformance/number_type_mistakes | covered |
| A `Number` parameter takes the operators, compiled per class | tests/number_parameter | covered |
| An operator through a `type` that requires its function | tests/shape_operators, tests/shape_operator_cut, tests/shape_operator_unlike | covered |
| An operator through a `type` without the function is an error | conformance/shape_operator_without_function | added (compiler fails: "has no attribute or function 'sum'") |
| `Number` operators in a REPL or hot-reload build halt on a wider right side | none | missing |
| An enum takes no `=` and one value per line | conformance/enum_with_equals, conformance/declaration_one_per_line | added |
| An enum in `player.spite` is `Player.Job`, plain `Job` where unambiguous | tests/enum_qualified_names | added |
| An enum value resolved from where it is used | tests/enums | covered |
| A value two enums share, with no expected type, is an error | conformance/enum_value_ambiguous | added |
| A symbol not in the expected enum is an error listing its symbols | conformance/symbol_not_in_enum | added (compiler fails: does not list the symbols the enum accepts) |
| An untyped symbol literal is an error | conformance/symbol_without_context | added |
| A symbol where text is wanted is an error | conformance/symbol_for_text | covered |
| A `Symbol` reads as text; `symbol.class` is `Symbol` | tests/symbol_from_text, tests/json_symbols | added (compiler fails: `tag.class` prints String) |
| `Symbol(text)` answers `Symbol?`, null unless in the table | tests/symbol_from_text | added |
| Reopening replaces an enum whole | tests/enum_reopening, conformance/enum_reopening_replaces | covered |
| `Course.values` lists the values; `values[1]`; `find_by_name` | tests/enum_values_walk | added (compiler fails: `Course.values[1]` is not a `Course?`) |
| A generic walks `$value_type.values` | tests/enum_values_generic | added (compiler fails: the values are passed as String) |
| Enum numbers with `=`, counted on from the one before | tests/numbered_enums | covered |
| `to_integer()` answers a value's number | tests/enum_to_integer | covered |
| Two values with one number, or a number past Integer, are errors | conformance/enum_numbers, conformance/enum_number_too_large | covered |
| A hot reload cannot change a value's number | none | missing |
| A switch over a union covers every member and narrows | tests/unions, conformance/missing_switch_case | covered |
| A function every member has is called on the union | tests/union_common_function, tests/unions | added |
| Every member is a class of objects | conformance/union_value_members | covered |
| `_:` answers for the rest, narrowed to each | tests/rest_case | covered |
| A repeated case body is an error; a case doing what `_` does | conformance/switch_rest_case, conformance/switch_same_as_rest | covered |
| `_` not last is an error | conformance/switch_rest_not_last | covered |
| `_` after every member has a case is an error | conformance/switch_rest_after_all | added (compiler fails: compiles) |
| A switch over `T?` has `<Type>:` and `Null:` | tests/nullable_switch_null_case, tests/nullable_values | added |
| `value == Circle` is a class test and narrows inside the `if` | tests/class_test | covered |
| A null `T?` is no class: the test is false | tests/null_class_test_false, tests/class_test | added |
| A class not a member of the union is an error | conformance/class_test_not_member | added (compiler fails: compiles) |
| A generic class is named `Storage<$component_type>` only right of `==` | tests/generic_class_test, conformance/generic_class_in_comparison | covered |
| A codegen value bound to a class is a class test | tests/codegen_class_test | covered |
| A switch that is one early return is an error | conformance/switch_single_case, conformance/switch_one_case_if | covered |
| `Monster?` is the union of Monster and Null | tests/nullable_values | covered |
| A `type` is matched by shape, object literals included | tests/shapes, tests/shape_defaults | covered |
| `.class` through a type names the real class; an object literal answers Object | tests/class_through_shape | covered |
| A class test admits nothing | tests/tested_classes | covered |
| A type may require functions | tests/shape_functions, conformance/shape_function_mismatch | covered |
| The type's own name in a required signature stands for the class that fits | tests/number_type | covered |
| A type as the element of a variadic parameter | tests/variadic_arguments | covered |
| A function taking a type is compiled once per class that reaches it | tests/shape_copies | covered |
| A required function named without calling is bound to the value | tests/type_function_values | covered |
| A List of a type answers the member templates over its members | tests/shape_members | covered |

## control_flow.md

Rules: 51. Covered before: 19. Covered now: 45.

| Rule | Program(s) | Status |
|---|---|---|
| `if`/`else`, `while`, `and`/`or`/`not` run as written | tests/control_flow | covered |
| `else if` chain on one line | tests/control_else_if | added |
| `if` on a `T?` narrows in place; `else` runs when null | tests/nullable_narrowing, tests/control_nullable_else | added |
| `do` is not a keyword; writing it is an error | conformance/old_do | covered |
| Terminal-`if` lint only when the `if` has no `else` | conformance/terminal_if, tests/control_else_if | added |
| Formatter joins `else { if ... }` into `else if` | | missing (formatter rule, D800) |
| Words left after a statement: error at the first one | conformance/left_over_after_return | covered |
| A word after the `}` closing a statement is an error | conformance/left_over_after_block | covered |
| Local declared only to be returned is an error | conformance/returned_local | covered |
| Local declared with another type is not refused | | missing |
| `while true` with no return/assert/crash and no call is an error | conformance/spinning_loop | covered |
| `while true` that calls something is left alone | tests/control_while_true_returns | added |
| Spinning-loop exception in `Parallel` work | conformance/endless_parallel_work | covered |
| Spinning-loop exception in a locked singleton function | conformance/endless_locked_loop | covered |
| `break` as a statement is an error naming the form | conformance/old_break | covered |
| `continue` as a statement is an error naming the form | conformance/control_old_continue | added |
| `break` and `continue` stay usable as names | tests/control_break_as_name | added (compiler fails: refuses `var break` as a statement) |
| `for` is a parse error naming `while` and the templates | conformance/control_old_for | added |
| `while` costs a REPL check point only in `--repl-port`/`--hot-reload` builds | | missing (repl page) |
| Nested `if`/`else` inside an `if`/`else` branch is an error | conformance/nested_if_else | covered |
| `if`/`else` inside an `else if` body is caught too | conformance/control_nested_in_else_if | added |
| Not counted: flat chain, `if` without `else`, `if`/`else` inside `while`/`switch` | tests/control_nested_allowed | added |
| `if` whose body is only an `if`, neither with `else`: error naming `and` | conformance/control_if_only_if | added (compiler fails: rule not built) |
| A switch case is one statement; two is an error | conformance/control_case_two_statements | added (compiler fails: rule not built) |
| Value switch over an enum, number, text, negative literal | tests/value_switch, tests/control_switch_negative_and_text | added |
| Enum switch missing a value without `_:` | conformance/control_switch_enum_missing, conformance/missing_switch_case | added |
| `_:` after every enum value is an error | conformance/control_switch_enum_rest_unneeded | added (compiler fails: rule not built) |
| Switch over a number has no `_:` | conformance/control_switch_number_no_rest | added (compiler fails: reports the falling-end error instead) |
| Switch over a text has no `_:` | conformance/control_switch_text_no_rest | added (compiler fails: reports the falling-end error instead) |
| Value named twice | conformance/value_switch | covered |
| Value outside the enum | conformance/control_switch_value_outside_enum | added (compiler fails: points at the line before the case) |
| Class as a case of a value switch | conformance/control_switch_class_case_on_enum | added (compiler fails: accepted) |
| Name or call as a value case | | missing |
| Value case in a switch over a union | conformance/control_switch_value_case_on_union | added (compiler fails: says the union has no 'equals') |
| Value case over a `T?` (narrow it first) | conformance/control_switch_value_on_nullable | added |
| `_:` not last, same body as `_:`, two same bodies | conformance/switch_rest_not_last, switch_same_as_rest, switch_rest_case | covered |
| One case plus `_:` that only return: `return light == 'red'` | conformance/control_switch_value_single_case, conformance/switch_single_case | added |
| One case and `_:` is an `if` | conformance/switch_one_case_if | covered |
| Subject evaluated once | tests/control_switch_subject_once | added (compiler fails: subject evaluated once per case) |
| Switch whose cases all leave narrows after it | tests/control_switch_leaving_narrows | added (compiler fails: not narrowed after the switch) |
| Three `if`s on one value each returning: error writing the switch | conformance/switch_chain | covered |
| `if`/`else if` chain of three such links | conformance/switch_chain_else_if | covered |
| Chain with `or` | conformance/switch_chain_or | covered |
| Two such `if`s are left alone | tests/control_switch_chain_two_allowed | added |
| Chain covering every enum value gets no `_:` | | missing |
| Walk doing `item.m()`: `each_m()` | conformance/template_walk | covered |
| Walk on a `Vector`/`Items`: member rows only | none | superseded by D1707 (one List) |
| Walk calling `f(item)`: `each(f)` | conformance/walk_with_function | covered |
| Walk rows `map_ms`, `copy`, `filter_m`, `filter_m().map_ns`, `count_m`, `sum_m`, `find_by_m`, `any_m`, `all_m` | conformance/walk_map, walk_copy, walk_filter, walk_filter_map, walk_count_member, walk_sum_member, walk_find_by, walk_any_member, walk_all_member | added (compiler fails: only the `each` rows are detected) |
| Walk rows `filter(f)`, `count(f)`, `sum(f)`, `find(f)`, `any(f)`, `all(f)` | conformance/walk_filter_function, walk_count_function, walk_sum_function, walk_find_function, walk_any_function, walk_all_function | added (compiler fails: only the `each` rows are detected) |
| Loops with extra arguments, index use, two lists, `T?` tests stay | | missing |

## functions_and_operators.md

Rules: 56. Covered before: 29. Covered now: 53.

| Rule | Program(s) | Status |
|---|---|---|
| Functions, recursion, explicit `return` | tests/functions | covered |
| Return type written `(): Type`; `() Type` is a parse error | conformance/return_type_colon | covered |
| Compiler-called names (`drop`, `to_string`, `equals`...) have fixed signatures | conformance/reserved_signature, conformance/equals_signature, conformance/to_string_signature | covered |
| Falling off the end gets a default with no diagnostic | conformance/falling_end | covered (the suite refuses it, as D244 requires: the spec sentence is the bug) |
| A function with no return type returns nothing | tests/functions | covered |
| Scalars copied, everything else shared by reference | tests/functions_shared_parameters, tests/references_basic | added |
| `&Type` is a parse error | conformance/old_reference_type | covered |
| An argument casts toward its parameter | tests/functions_argument_cast | added |
| No overloading: a name declared twice in one file | conformance/duplicate_function | covered |
| `assert` only where the result can say nothing | conformance/default_answer, tests/assert_runs | covered |
| A named function is bound to its instance; `.owner` | tests/function_owner, tests/function_values | covered |
| Attribute named like a function | conformance/shadowed_function | covered |
| Local named like a function | conformance/functions_shadowed_local | added |
| Parameter named like a function | conformance/functions_shadowed_parameter | added |
| `Monster.is_alive` is not a function (exact text) | conformance/functions_class_function_value | added (compiler fails: says 'Monster' is a class, not a value) |
| A text's or number's function is bound to a copy of the value | tests/functions_text_function_value | added |
| Function passed by name to `each`/`filter` builds no value | tests/passed_functions, tests/library_functions_passed | covered |
| `Spite.Function<...>` typed, last is the return; `Nothing` | tests/typed_functions | covered |
| `printer(value)` is `printer.call_function(value)` | tests/typed_functions, tests/call_function_arguments | covered |
| Reopening `call_function` to trace every call | | missing |
| A typed value goes where a plain `Spite.Function` is expected | tests/functions_reflected_accepts_typed | added (compiler fails: crashes, 'has no Spite.Function to describe it') |
| A plain (reflected) function cannot go where a typed one is expected | conformance/functions_reflected_to_typed, conformance/function_value_mistakes | added |
| An attribute holds a function value set later | tests/typed_functions | covered |
| An attribute defaulting to its own class's function is a cycle that leaks | | missing |
| `.returns` reports `Nothing` for a function without a return type | tests/function_values | covered |
| `Anything` takes any object; `List<Anything>`; `==` narrows back | tests/functions_anything, tests/tested_classes | added |
| A number, `Boolean` or text passed as `Anything` arrives as itself | tests/functions_anything_parameter | added |
| Variadic `...name: List<T>` gathers values; zero values is empty | tests/variadic_arguments | covered |
| Variadic element type may be a `type` | tests/variadic_arguments | covered |
| `...` not on the last parameter | conformance/variadic_not_last | covered |
| `...words` not typed `List<Type>` | conformance/functions_variadic_untyped | added (compiler fails: generic 'write its type after the colon' message) |
| A value of a variadic call is never a call or construction | conformance/variadic_mistakes | covered |
| A whole list passed to `...` is an error | conformance/variadic_mistakes | covered |
| A whole list is fine when the element type is a list | tests/functions_variadic_lists | added |
| A function value keeps the spread | tests/variadic_arguments | covered |
| A number, `Boolean` or enum fits a `type`; enum answers `to_string()` | tests/printable_values, tests/functions_enum_to_string | added |
| `.class` through a shape names the value's own class | tests/class_through_shape | covered |
| Operators `+ - * == != < > <= >=`, unary `-`, `[]`, `[]=` map to functions | tests/operators | covered |
| `/` and `%` map to `divide` and `remainder` | tests/functions_operators_divide | added |
| A union duck-types an operator when every member defines it | tests/functions_union_operator | added |
| Missing `sum` names the operator | conformance/functions_operator_missing_sum | added (compiler fails: generic 'has no attribute or function' text) |
| Missing `negate` names `-` | conformance/functions_operator_missing_negate | added (compiler fails: generic text) |
| Missing `get_at` names indexing | conformance/functions_operator_missing_get_at | added (compiler fails: different text) |
| Missing `set_at` names assigning through an index | conformance/functions_operator_missing_set_at | added (compiler fails: names get_at instead) |
| Direct call of an operator function is an error (`a.sum(b)`) | conformance/functions_direct_sum | added (compiler fails: accepted) |
| Direct `set_<name>`/`get_<name>` call is an error | conformance/operator_function_call, conformance/functions_getter_by_name | added |
| `get_attribute('age')` / `set_attribute('age', 2)` name the shortcut | conformance/functions_direct_get_attribute, conformance/functions_direct_set_attribute | added (compiler fails: says Person has no get_attribute/set_attribute) |
| An operator function used as a value is allowed | conformance/operator_function_call | covered |
| Every `get_at` answers `T?`; a plain `T` is an error | conformance/plain_get_at | covered |
| A program's indexable class narrows by what it writes | tests/indexable_class, conformance/indexable_unproven | covered |
| Right operand wider than the left is an error | conformance/wider_right_operand, conformance/wider_comparison | covered |
| Getter/setter interception from outside the class | tests/accessors | covered |
| Compound assignment reads through getter and writes through setter | tests/functions_interception_compound | added |
| A setter answers a write with no attribute of its name | tests/functions_setter_without_attribute | added |
| A getter with no setter is read-only | conformance/read_only_attribute | covered |
| An exact function wins over a template for `set_<name>` | tests/accessors, tests/symbol_codegen | covered |

## classes_and_files.md

Rules: 50. Covered before: 32. Covered now: 49.

| Rule | Program(s) | Status |
|---|---|---|
| Newlines end statements; commas separate entries on one line | tests/lists, tests/objects | covered |
| Comments start with `#` | conformance/comment_inside_function, conformance/comment_prose_comment | covered |
| Strings use double quotes; enum values single quotes | tests/strings, tests/enums | covered |
| Names: snake_case values, PascalCase classes, `$name` codegen values | conformance/naming, conformance/function_name_case | covered |
| `_name` is private to its class; the error names the getter | conformance/classes_private_name | added (compiler fails: accepted) |
| A private function called from another class | conformance/classes_private_function | added (compiler fails: accepted) |
| A private name is used freely inside its own class | tests/classes_private_inside | added |
| `_name` on a parameter means ignored on purpose | conformance/read_underscore_name | covered |
| `load` cannot name a function, variable or parameter | conformance/load_function, conformance/load_parentheses | covered |
| `singleton` stays usable as a name | tests/classes_singleton_as_name | added (compiler fails: refuses 'singleton' as a variable name) |
| `generics` header list is an error | conformance/classes_generics_list | added (compiler fails: generic 'a file holds declarations' text) |
| No `do` | conformance/old_do | covered |
| No `let`/`const`; both usable as names | conformance/old_let, tests/classes_old_keywords_as_names | added |
| No `class` keyword | conformance/class_keyword | covered |
| `class`, `attributes`, `functions`, `instances`, `memory` cannot name a member | conformance/reflection_names, conformance/classes_attribute_named_class, conformance/classes_attribute_named_memory, conformance/classes_function_named_functions | added |
| A file is one class named in PascalCase after the file | tests/multiple_classes | covered |
| A file's declarations are in the enforced order | conformance/declaration_order | covered |
| No file-level statements | conformance/file_scope_statement | covered |
| The entry class may not be reopened | conformance/entry_class_reopened | covered |
| A class may not hide another (`Physics.Plugin` and `Plugin`) | conformance/hidden_class | covered |
| Side-by-side namespaces hide nothing | tests/classes_side_by_side_names | added |
| An enum, union or type inside a class hides nothing | tests/classes_enum_in_class_not_hidden | added |
| A class named like a bare library class is an error | conformance/named_like_library | covered |
| A class named like a namespaced library class is legal | tests/classes_namespaced_library_name | added |
| A file with the library class's dotted name reopens it | tests/reopen_library | covered |
| Three classes sharing a first word in one folder is an error | conformance/classes_prefix_namespace | added (compiler fails: rule not built) |
| Two such classes, or a word that is not whole (`Highway`), are legal | tests/classes_prefix_two_allowed | added |
| A reopening may not hand-copy a library operator | conformance/hand_copy_of_library | covered |
| The constructor is the function named like the class; no `new` | tests/constructor_params | covered |
| No constructor: an implicit one that gives the defaults | tests/classes_implicit_constructor | added |
| An empty written constructor is an error | conformance/empty_constructor | covered |
| `assert` is not allowed in a constructor | conformance/constructor_guard | covered |
| One file declares each name once | conformance/duplicate_function | covered |
| A constructed object dropped as a statement is an error | conformance/dropped_construction | covered |
| `singleton` line; `func is_singleton()` is an error | tests/singletons, conformance/singleton_function | covered |
| One instance per distinct constructor argument values | tests/classes_singleton_per_arguments | added (compiler fails: one instance for every argument list) |
| A singleton's arguments are literals | conformance/singleton_arguments | covered |
| A generic singleton has one instance per codegen values | tests/generic_singletons | covered |
| A singleton is bound as an attribute, never a local | conformance/local_singleton | covered |
| Value classes (`String`) bind a singleton as a local | tests/classes_text_binds_singleton_locally | added |
| A singleton is always bound before use (member read) | conformance/inline_singleton, conformance/inline_singleton_attribute | covered |
| Passing or returning a singleton call | conformance/classes_inline_singleton_argument, conformance/classes_inline_singleton_returned | added |
| An unread singleton binding is an error | conformance/classes_unread_singleton | added |
| Singletons are not counted | tests/singleton_counts | covered |
| Teardown newest first; drop() using a destroyed singleton halts | tests/singleton_teardown, tests/singleton_used_after_exit, tests/singleton_held_at_exit, tests/singleton_freed_after_teardown | covered |
| Singletons made in a circle: compile error, or run-time halt | conformance/singleton_circle, tests/singleton_circle | covered |
| Made once whichever thread asks first | tests/singleton_race | covered |
| Thread safety only in programs that use `Parallel` | tests/singleton_lock_calls, tests/unshared_locks | covered |
| A singleton that holds nothing is one static object | tests/singleton_stateless_calls | covered |
| A reopening may add the `singleton` line | | missing |

## collections.md

Rules: 108. Covered before: 51. Covered now: 100.

| Rule | Program(s) | Status |
|---|---|---|
| A member template names a field or a zero-argument function alike | tests/member_templates | covered |
| filter_, count_, any_, all_, sum_, map_, sort_by_, find_by_, each_ on a List of a class | tests/member_templates | covered |
| The same templates on a Dictionary through its values | tests/member_templates, tests/dictionary_templates | covered |
| find_index_by_<member> answers the index or null | tests/index_of | covered |
| sum_<member> on a String member is an error naming what sum_ needs | conformance/member_template_mistakes | covered |
| count_<member> on a number names sum_<member> | conformance/template_count_numeric | added |
| each_<attribute> says reading an attribute does nothing | conformance/template_each_attribute | added |
| sort_by_ is stable (equal keys keep their order) | tests/list_sort_stable | added |
| sort_by_ is n log n, many elements | tests/sorting_many | covered |
| A String element's members are String's own zero-argument functions | tests/text_member_templates | covered |
| A program's own list.spite or dictionary.spite adds a template | tests/own_collection_templates | covered |
| A template parameter written `Symbol<...>` is an error naming the form to write (D798) | conformance/plain_symbol_on_list, conformance/template_symbol_function, conformance/template_symbol_argument, conformance/template_symbol_namespace, conformance/template_symbol_enum | added |
| A template's parameter is `member: Spite.AttributeDeclaration`, its class inferred from `item[member]` (D798) | (library), tests/plural_templates, tests/own_collection_templates | covered |
| A --repl build lists the templates that fit | tests/interactive_templates | covered |
| A chain is one loop, no list in between (allocations) | tests/fused_chains, tests/fused_chain_allocations | covered |
| A member function in a fused chain runs element by element | tests/list_chain_order | added (compiler fails: chain runs step by step) |
| A template never answers with a function of the caller; error names each(f) | conformance/template_caller_function | added (compiler fails: generic "has no attribute or function") |
| each/filter/any/all/count/find/sort_by/sum with a passed function | tests/passed_functions | covered |
| find(f) answers null when nothing matches; any/all of an empty list | tests/list_find_none | added |
| count() with no argument stays the size | tests/list_find_none | added |
| No map(f): the error names map_<member> and a getter | conformance/passed_functions | covered |
| map_ on a Boolean test is an error naming filter_/count_/any_/all_ | conformance/template_map_test | added (compiler fails: plural-name error instead) |
| A bound function of another instance (greeter.greet) | tests/passed_functions | covered |
| A list's or a library class's own function passed (found.append, counts.has) | tests/plain_items, tests/library_functions_passed | covered |
| A held text or number function keeps its own copy | tests/held_value_functions | covered |
| counts.get_at answers Integer?, so sort_by(counts.get_at) is an error | conformance/library_function_mistakes | covered |
| filter(f) where f returns nothing | conformance/passed_returns_nothing | added (compiler fails: generic function type mismatch text) |
| each(f) where f takes two arguments | conformance/passed_argument_count | added (compiler fails: generic function type mismatch text) |
| each(f) where f takes another type | conformance/passed_argument_type | added (compiler fails: generic function type mismatch text) |
| An argument that is no function at all | conformance/passed_functions (later error, not first) | missing |
| A while that only does each(f) is refused | conformance/walk_with_function | covered |
| Templates over an enum value: filter_/count_/any_/all_/remove_where_ | tests/enum_value_templates | covered |
| Two members whose enums list the name: error | conformance/enum_value_templates | covered |
| No member and no enum lists the name: error says so | conformance/enum_template_no_member | added (compiler fails: generic "has no attribute or function") |
| A member named like an enum value: error naming both | conformance/enum_template_member_and_value | added (compiler fails: compiles) |
| A union element is read through a member every class answers | tests/union_member_templates | covered (compiler fails: unread attribute 'title') |
| No Directory.files() / folders() helper | none | missing |
| list[index] and get_at answer T?, null out of range and before 0 | tests/list_empty_ends, tests/lists | added |
| first, last, remove_first, remove_last answer null on an empty list | tests/list_empty_ends, tests/lists | added |
| insert clamps to the nearest end | tests/list_insert_clamps | added (compiler fails: insert halts on -5) |
| set_at out of range halts naming the line | tests/list_set_out_of_range | added |
| remove_at out of range halts | tests/list_remove_out_of_range | added |
| remove_swapping moves the last into the index; out of range halts | tests/list_remove_swapping | added |
| reserve(count) adds no element | tests/list_reserve | added |
| append, prepend, clear, reverse, join, copy | tests/lists, tests/deep_copies | covered |
| join turns numbers, Boolean, enum values into text; empty list joins to "" | tests/list_join_values | added |
| A List of numbers, Boolean or enums is flat | tests/plain_items | covered |
| A List has no Buffer byte functions: error naming Buffer | conformance/list_byte_functions | added (compiler fails: generic "has no attribute or function") |
| contains compares with ==, only numbers, Boolean, String, enum | tests/lists, tests/index_of | covered |
| contains on a list of a class: List has no method 'contains' | conformance/list_contains_class | added (compiler fails: compiles) |
| index_of answers the first index or null | tests/index_of | covered |
| add is an error naming append and prepend | conformance/old_list_names | covered |
| pop is an error naming remove_last | conformance/list_pop_name | added |
| Dictionary is insertion ordered; replacing a value keeps its place | tests/dictionary_insertion_order, tests/any_keys | added |
| dictionary[key] is null for an absent key | tests/dictionaries, tests/number_keys | covered |
| has(key) and remove(key) (spec names) | tests/dictionaries (contains, remove_at) | superseded by D667 |
| Removing an absent key does nothing | tests/dictionary_remove_absent, tests/number_keys | added |
| keys() and values() are fresh lists | tests/dictionary_fresh_lists | added |
| copy() is one level, deep_copy() copies values and keeps object keys | tests/dictionary_deep_copy_keys, tests/number_keys | added |
| Dictionary takes exactly two type arguments (1 named) | conformance/dictionary_type_arguments | covered |
| Dictionary with three type arguments | conformance/dictionary_three_types | added |
| A key is cast to Key like any argument | tests/number_keys | covered |
| Whole-number keys by value, no String made | tests/number_keys | covered |
| 0.0 and -0.0 are one key | tests/any_keys | covered |
| Every not_a_number is one key | tests/dictionary_not_a_number_key | added |
| String, Boolean, enum keys | tests/dictionaries, tests/any_keys | covered |
| An object key by identity; an equal object is another key | tests/any_keys | covered |
| A nullable key is an error | conformance/unkeyable_types | covered |
| A union key is the same error | conformance/dictionary_union_key | added (compiler fails: compiles) |
| A value class key is an error ("it is a value, copied wherever it goes") | none | missing |
| A class that declares equals cannot be a key | conformance/equals_keys | covered |
| Dictionary literal: commas or newlines, last comma, inferred key and value types, declared type casts | tests/dictionary_literals | added |
| The same key twice in a literal is an error | conformance/dictionary_literal_duplicate_key | added (compiler fails: compiles) |
| {} is an empty object literal, never a dictionary | none | missing |
| As JSON a whole-number key is quoted, an enum key is its name | tests/number_keys, tests/enum_key_json | covered |
| A JSON dictionary keyed by Float is an error | conformance/json_float_key | added (compiler fails: compiles) |
| Binary bytes of a dictionary keyed by objects is an error | conformance/binary_object_key | added (compiler fails: compiles) |
| remove_where on a Dictionary is an error naming remove | conformance/dictionary_remove_where | added (compiler fails: generic "has no attribute or function") |
| deep_copy follows classes, lists, dictionaries, unions, shapes, cycles | tests/deep_copies, tests/deep_copy_graphs, tests/deep_copy_unions, tests/shape_copies | covered |
| Vector item rules: a class with a List attribute is refused | none | superseded by D1707 (one List) |
| Vector<Integer> holds plain values: write List<Integer> | none | superseded by D1707 (one List) |
| Vector<List<Integer>> error text | none | superseded by D1707 (one List) |
| A class with drop() cannot be an item | none | superseded by D1707 (one List) |
| A class using this as a value cannot be an item | none | superseded by D1707 (one List) |
| list[index] answers T? null out of range; set_at out of range halts | tests/list_out_of_range | added |
| A read narrowed by a loop bound or crash count | tests/list_rows, tests/proven_ranges | covered |
| append copies; a constructor is not the argument | tests/list_find_sort | superseded by D1707 (one List) |
| remove_at moves later items down; out of range halts; clear | tests/list_remove_moves_down | added |
| Vector templates, find_by_, sort_by_ copies | tests/list_find_sort | superseded by D1707 (one List) |
| List is the only list: an item read from it is the item itself, kept, shared and read after the list changes size | tests/list_shared_items, tests/list_items_run, tests/list_by_reference_kept | added |
| Borrowed item not read after append or reserve | none | superseded by D1707 (one List) |
| A program's own vector.spite template is checked for borrows | none | superseded by D1707 (one List) |
| each(f) on a Vector of a class is an error | none | superseded by D1707 (one List) |
| each(f) on a Vector of String is an error | none | superseded by D1707 (one List) |
| Items chooses inline or references while compiling | tests/list_columns | superseded by D1707 (one List) |
| Items<Integer> is the error naming List<Integer> | none | superseded by D1707 (one List) |
| Items<T?> narrowed in place; a loop bound does not prove presence | tests/list_narrowed, conformance/list_nullable_unproven | superseded by D1707 (one List) |
| Items borrow errors name the choice first | none | superseded by D1707 (one List) |
| Items by reference: a read item is kept across growth and removal | tests/list_by_reference_kept | superseded by D1707 (one List) |
| Items remove_at and remove_swapping out of range halt | none | superseded by D1707 (one List) |
| remove_where and remove_where_<member> keep survivors in order | tests/bulk_removal | covered |
| remove_where's test sees every element once, first to last | tests/list_remove_where_order | added |
| truncate out of range does nothing | tests/list_truncate_edges, tests/bulk_removal | added (compiler fails: truncate(-1) halts) |
| swap out of range does nothing | tests/list_swap_out_of_range, tests/bulk_removal | added |
| remove_where on a chain is an error | conformance/bulk_removal | covered |
| remove_where with a test returning Integer is an error | conformance/remove_where_returns_number | added (compiler fails: generic function type mismatch text) |
| truncate and swap undo a borrow and a proof | none | missing |
| Heap<T> is an error naming an ordinary attribute | conformance/list_heap_box | added (compiler fails: "no generic class 'Heap' here") |
| A union holding itself as an ordinary attribute works | tests/deep_copy_unions | covered |

## failure.md

Rules: 147. Covered before: 95. Covered now: 133.

The crash report's stop line is written as D672 words it, `spite: the program stopped at <path>:<line> in <Class>.<function> (<name>=<value>, ...)`, instead of the page's `spite.crash<TAB>id...` line; the programs added keep the page's content (values in scope, missing links, calls, frames) in that line.

### Nothing fails silently (the table of what the language refuses)

| Rule | Program(s) | Status |
|---|---|---|
| A `T?` read as if present is refused; `value == null` is an error | tests/nullable_narrowing, conformance/maybe_in_arithmetic, conformance/null_comparison | covered |
| `[]`, `first()`, `last()`, `remove_first()` answer a `T?` | tests/lists, tests/present_zero, conformance/index_reads | covered |
| A loop-proven read with a counter below zero halts naming the read | tests/proven_read_outside | covered |
| A guard `assert` answering a default the caller takes for real is an error | conformance/default_answer | covered |
| A write to an object only this function holds, never read after, is an error | conformance/lost_writes | covered |
| Assigning a local read from a list as if it wrote the list is an error | conformance/replaced_items | covered |
| `false`, `0`, `""` are never taken for missing | tests/present_zero | covered |
| Text that is not a number answers a `T?`; text assigned to a number is an error | tests/text_to_number, conformance/text_to_number | covered |
| A function with a result falling off its end is an error | conformance/falling_end | covered |
| A proof gone stale after an assignment or call must be proven again | conformance/call_undoes_proof, conformance/path_narrow_assignment | covered |
| A failed `crash` names the missing link, index and count, or key | tests/crash_reports_missing_index, tests/crash_reports_missing_key, tests/crash_reports_null_link | added (compiler fails: the stop line names no missing link) |
| A native fault prints `spite.fault` with place, last foreign call and stack | tests/native_fault_foreign, tests/native_fault_stack, tests/native_fault_illegal, tests/native_fault_heap | covered |
| Whole-number division by zero halts; a literal zero divisor is an error | tests/division_by_zero, conformance/division_by_constant_zero | covered |
| Arithmetic that does not fit halts; wrapping only by `wrapping_*` | tests/integer_overflow, tests/unsigned_overflow, tests/wrapping_functions | covered |
| A value too big for a narrower name halts; a literal that does not fit is an error | tests/narrowing_overflow, conformance/literal_does_not_fit | covered |
| A wider operand cut to fit is an error | conformance/wider_right_operand, conformance/wider_comparison | covered |
| A constant that overflows its type is an error | conformance/literal_does_not_fit | covered |
| A `Float` gone to infinity written as JSON crashes naming the attribute | tests/json_infinity | covered |
| A folded-false `crash` in reached code is an error | conformance/folded_crash | covered |
| A value computed and never read, an attribute nothing reads, is an error | conformance/unused_names, conformance/unused_attributes | covered |
| An object made and dropped on one line is an error | conformance/dropped_construction | covered |
| `for`, `break`, `++`, `&&` and other habits are errors naming the Spite form | conformance/old_break, conformance/borrowed_syntax, conformance/borrowed_operators | covered |
| A borrowed `Vector` item kept or read after the vector moved is an error | none | superseded by D1707 (one List) |
| An item lent to the caller retained and released as owned | tests/narrowed_lends | superseded by D1707 (one List) |
| A C compile with no executable, or two builds sharing one C file, is an error | | missing |
| Two threads writing a singleton: the compiler makes it safe | tests/singleton_race | covered |
| A `while true` holding a singleton's lock forever is an error | conformance/endless_locked_loop | covered |
| Polling a `Concurrent`'s `finished` with nothing stepping it halts | tests/finished_polling, conformance/unstepped_poll | covered |
| `call_function()` skipping a function that takes arguments halts | tests/call_function_arguments | covered |
| C calling a dropped `ForeignCallback` halts naming it | tests/foreign_callback_dropped | covered |
| A `while true` that never leaves and calls nothing is an error | conformance/spinning_loop | covered |
| A `Parallel` that loops forever and never waits is an error | conformance/endless_parallel_work | covered |
| A peer that hung up: `socket.closed` turns true, reads answer 0 or null | tests/web_socket_failures, tests/concurrent_sockets | covered |
| An impossible date halts; `text.to_date()` answers null | tests/time_text_errors | covered |
| Every build writes `<program>.crashes`, one line per site | tests/crashes_map | added (compiler fails: no `.crashes` file is written) |

### Narrowing

| Rule | Program(s) | Status |
|---|---|---|
| `assert value` narrows a `T?` local, parameter or field for the rest of the block | tests/nullable_narrowing, tests/nullable_values | covered |
| `assert value and other` narrows; `other` sees it narrowed | tests/and_narrowing | covered |
| `value = null` after narrowing makes it a `T?` again | conformance/narrowed_name_reassigned | covered |
| A narrowed value-type `T?` assigned another stores presence and value | tests/narrowed_value_assignment | covered |
| Narrowing tests presence, never the value (zeros, `false`, `""`) | tests/present_zero | covered |
| `if value { } else { }` narrows the block; `else` runs when null | tests/nullable_narrowing, tests/narrowed_read_local | covered |
| A function's last statement `if value { }` with no `else` is an error | conformance/terminal_if | covered |
| A narrowing `if` not last, or nested in a loop, is no lint error | tests/terminal_if_not_last | added |
| An `if` whose body only returns the default is an error naming the `assert` | conformance/default_guard, conformance/empty_collection_guard, conformance/returning_guard | covered |
| The guard's condition is turned around, `or` by De Morgan | conformance/guard_de_morgan | added |
| The guard rule holds inside a `while` or a nested `if` | conformance/guard_inside_loop | added |
| An `else if` guard is covered too | conformance/default_guard | covered |
| A guard `if` in a constructor names `crash` instead | conformance/constructor_guard | covered |
| An `if` returning a value in a number or Boolean function is left alone | tests/leaving_if | covered |
| An `if` that leaves proves the opposite of its condition for the rest of the block | tests/leaving_if, tests/returning_branch_proof | covered |
| The leaving `if`'s own block sees none of that proof | | missing |
| A check the leaving `if` already proves is "proves nothing" | conformance/leaving_if_proves_nothing | added (compiler fails: refuses `crash found` as a String condition) |
| `assert` narrows every link of a chain and its prefixes | tests/path_narrowing | covered |
| A sibling path stays `T?` | conformance/sibling_path_unnarrowed | added |
| Every side of an `and` narrows, in `assert`, `crash` and `if` | tests/and_narrowing, tests/and_if | covered |
| The right side of an `and` in any expression sees the left's narrowing | tests/and_return_narrowing | added |
| A local copy is its own path | conformance/namespace_nullable | covered |
| A local that only copies a name to narrow it is an error (exact text) | conformance/copy_to_narrow, conformance/copy_to_narrow_conditions | covered |
| A copy whose source is assigned later (a snapshot) is not a copy for narrowing | tests/narrowing_snapshot_copy | added |
| A typed `var` in a generic class is not a copy for narrowing | | missing |
| A local holding a `[]` read is narrowed, not a copy | tests/narrowed_read_local | added |
| A second check of a narrowed name names the earlier check (exact text) | conformance/check_proves_nothing, conformance/known_fact_checked | covered |
| A check of a `[]` read proven by a loop names the loop condition (exact text) | conformance/proven_element, conformance/proven_index_check | covered |
| `assert` on a value that is never `T?` is "cannot be null here" (exact text) | conformance/assert_on_plain_value | added (compiler fails: says a condition must be a Boolean) |
| `crash` narrows a chain as `assert` does | tests/crash_missing_link, tests/crash_missing_value | covered |
| Reading through an unnarrowed link is an error | conformance/path_narrow_assignment, conformance/path_narrow_else | covered |
| Assigning to a narrowed path or what it reads through undoes it | conformance/path_narrow_assignment | covered |
| Assigning a non-null value to the narrowed path keeps it narrowed | tests/narrowed_path_kept | added |
| ...and undoes everything narrowed beneath it | conformance/narrowed_path_undone_below | added (compiler fails: compiles) |
| `while value` narrows its body; walks a chain with no `assert` | tests/while_narrowing_walk | added (compiler fails: `while current` does not narrow) |
| Undoing inside a `while` a narrowing made before it and read in it is an error | conformance/path_narrow_loop | covered |
| Proving it again inside the loop after what undid it is accepted | tests/proof_again_in_loop | covered |

### Reading with `[]`

| Rule | Program(s) | Status |
|---|---|---|
| `[]` answers `T?`; a call-free index makes the read a path | conformance/index_reads, tests/structural_index | covered |
| An index holding another `[]` read is an error (exact text) | conformance/nested_index_read | added (compiler fails: reports the inner read as unproven instead) |
| A proven count proves indices below it (`== 3`, `>= 3`, `> 2`, `!= 0`, `not is_empty()`) | tests/count_bound_proofs | covered |
| A count proves `List` indices only, not a `Dictionary`'s keys | conformance/count_proves_no_key | covered |
| `index < list.count()` proves `list[index]`, also `- 1`, the left of an `and`, every side of a `while`'s `and` | tests/bound_proofs, tests/counted_loops | covered |
| `assert`/`crash` on an already proven read is an error | conformance/proven_index_check | covered |
| `base + k < count()` proves `base` to `base + k`; `<=` proves to `base + k - 1` | tests/bound_proofs, tests/strided_counted_loops, conformance/bound_proofs_undone | covered |
| A width read is proven by the read of its last byte | tests/buffer_bytes, conformance/byte_reads | covered |
| A position known not negative needs no lower test (seen only in the generated code) | | missing |
| One guard `rows * stride <= count()` proves the cells of an area | tests/area_guard_cells | added |
| Assigning `rows`, `stride` or the list undoes the area guard | | missing |
| A count kept in a name (`var count = names.count()`, or `- k`) proves as the count does | tests/count_in_name_proves | added |
| A read proven before a loop may be proven again inside it | tests/proof_again_in_loop | covered |
| Assigning the list or index, `clear`, `remove_*` undoes a proof | conformance/count_undoes_proof, conformance/bound_proofs_undone | covered |
| Any call-free index is a path; assigning a name it reads undoes it | tests/structural_index, conformance/structural_index_undone | covered |
| A call in the index keeps the read a plain `T?` | conformance/call_in_index | added (compiler fails: compiles, taking `names[next_index()]` as proven) |
| A call undoes a proof only if it may change what the proof depends on | tests/receiver_call_effects, tests/same_name_call_effects, conformance/call_undoes_proof, conformance/receiver_call_undoes_proof, conformance/branch_undoes_proof | covered |
| Calling a function value undoes every proof that reads an attribute or a list | | missing |
| A list passed to a function that shrinks its parameter undoes proofs about it | | missing |
| A proven read still checks its bounds at run time and halts naming it (exact text) | tests/proven_read_outside | covered |
| A declared `get_at` must answer a `T?` (exact text) | conformance/plain_get_at | covered |
| A read through a program's own `get_at` is narrowed only by what it writes | conformance/indexable_unproven, tests/indexable_class | covered |
| `get_at(index)` called by name answers a `T?` that is not a path | conformance/get_at_by_name | added |
| A read used before narrowing names the lines that would narrow it | conformance/index_reads | covered |
| A `Boolean?` is never a condition in `if`, `while`, `not`, `or` | conformance/maybe_boolean_conditions, conformance/joined_maybe_boolean | covered |
| `assert flag` / `crash flag` on a `Boolean?` ask for `true` | tests/assert_maybe_boolean, tests/crash_maybe_boolean | covered |
| `flag == true` needs no narrowing | tests/present_zero, tests/hashes | covered |
| Assigning through a `T?` is an error naming the fix | conformance/store_through_nullable | covered |
| `first()`, `last()`, `remove_first()`, `remove_last()` answer `T?` | tests/lists, tests/present_zero | covered |
| `==`/`!=` take a `T?` on either side; null equals only null | tests/nullable_comparison | added |
| A chain through a `T?` inside `==`/`!=` needs no narrowing | tests/chain_comparison | added (compiler fails: asks to narrow `made`) |
| `<`, `<=`, `>`, `>=` take no `T?` | conformance/nullable_ordering | added |
| A `Spite.Namespace` compares with text by `equals(String)` | | missing |

### Failure: three outcomes, `assert`, `crash`

| Rule | Program(s) | Status |
|---|---|---|
| A failed `assert` answers nothing, `null`, or an empty collection | tests/assert_default, tests/assert_runs, tests/assert_answers_empty | added (compiler fails: refuses `assert` in a function returning a `List` or `Dictionary`) |
| An `assert` in a function returning a whole number is an error (exact text) | conformance/default_answer | covered (compiler wording leaves out the answer offered) |
| ...returning a `Boolean`: offers `false`, no `Boolean?` | conformance/default_answer_boolean | added (compiler fails: offers 'Boolean?', no answer, bare 'crash') |
| ...returning a `String`: offers `""` | conformance/default_answer_text | added (compiler fails: no answer offered, bare 'crash') |
| ...returning an enum: offers its first value | conformance/default_answer_enum | added (compiler fails: no answer offered, bare 'crash') |
| ...returning a `Float`: offers `-1.0` | conformance/default_answer_decimal | added (compiler fails: no answer offered, bare 'crash') |
| ...returning a class made with arguments or a singleton: no answer offered | conformance/lent_element_guard | covered |
| Checked per instance of a generic class; an `assert` decided while compiling too | | missing |
| Every path of a function with a result ends in a `return` (exact text) | conformance/falling_end | covered |
| Paths end at `return`, bare `crash`, both-branch `if`/`else`, a full `switch`, `while true` | tests/every_path_returns | added |
| A generic class's falling end is reported once, naming the instance | | missing |
| `assert` in a constructor is an error (exact text) | conformance/assert_in_constructor | added (compiler fails: compiles) |
| ...including the entry class's constructor | conformance/assert_in_entry_constructor | added (compiler fails: compiles) |
| `crash` is allowed in a constructor | tests/crash_receiver | covered |
| `crash` mirrors `assert`'s polarity | tests/crash | covered |
| A bare `crash` is the form for a branch that cannot happen | tests/crash, tests/every_path_returns | covered |
| `crash false` is formatted to a bare `crash` | | missing |
| `crash user` narrows a `T?` and never returns | tests/crash_missing_value | covered |
| `crash` and `assert` take no message | | missing |

### What a crash reports

| Rule | Program(s) | Status |
|---|---|---|
| A firing `crash` flushes stdout, writes to stderr, exits with 1 | tests/crash_report, tests/crash_compound | covered |
| The crash line carries the condition's operands | tests/crash_report | covered |
| ...then parameters, locals in scope and attributes; objects and lists left out; condition names left out; text cut at 80 bytes with `...(<length> bytes)` | tests/crash_report_values | added (compiler fails: only the condition's operands are written) |
| Every call in the condition is reported with its answer, and its arguments; never evaluated twice | tests/crash_reports_call_once | added (compiler fails: no values for a call) |
| A condition that is a call on a value reports the value through `to_string()` | tests/crash_reports_receiver | added (compiler fails: no value written) |
| Only what ran in an `and`/`or` is reported | tests/crash_compound | covered (the report's values are not checked) |
| A failed narrowing reports what is missing: `is null`, `is missing: index, count`, `is missing: key` | tests/crash_reports_null_link, tests/crash_reports_missing_index, tests/crash_reports_missing_key | added (compiler fails: nothing named) |
| The report lists one `spite.frame` line per Spite function, innermost first | tests/crash_reports_frames | added (compiler fails: no frames) |
| The report prints the failed asserts before it, oldest first, `answered=` and `repeated=` | tests/crash_report, tests/assert_runs | covered |
| `answered=empty` for a collection result | tests/assert_trace_empty | added (compiler fails: refuses the `assert`) |
| The ring keeps 32 lines and writes `earlier=<count>` | tests/assert_ring_earlier | added |
| A failed `assert` prints nothing; `--trace-asserts` streams it | tests/quiet_asserts, tests/trace_asserts, tests/quiet_asserts_optimized, tests/trace_asserts_optimized | covered |
| Asserts on pool threads while a crash reports cannot chase the ring; only the first crash reports | tests/assert_ring_threads | covered |
| A library `assert` never enters the ring | tests/library_guards_untraced | covered |
| Site ids are content-derived; an `--optimized` build writes only the id | tests/trace_asserts_optimized | covered |
| An `--optimized` crash line is `spite.crash<TAB>id` then the values | | missing |
| `<program>.crashes`: header, one sorted line per site with file, line, column, class, function, kind, condition, typed operands | tests/crashes_map | added (compiler fails: no `.crashes` file) |

### What a native fault reports

| Rule | Program(s) | Status |
|---|---|---|
| A null read in a foreign library reports `spite.fault` with the last foreign call | tests/native_fault_foreign | covered |
| A stack overflow is reported on the thread that overflowed | tests/native_fault_stack | covered |
| An illegal instruction is reported | tests/native_fault_illegal | covered |
| A corrupted heap is reported | tests/native_fault_heap | covered |
| Frames: consecutive calls of one function as `repeated=`, at most 16 lines, `spite.frame<TAB>more` | tests/native_fault_stack | covered (the frame lines are not checked) |
| The fault report includes the assert ring | | missing |

## memory.md

Rules: 107. Covered before: 34. Covered now: 81 (24 of them fail on the master compiler). Superseded: 15 (D661, D662, D755, D759). Missing: 7.

| Rule | Program(s) | Status |
| --- | --- | --- |
| A non-scalar value is a reference: assigning it shares the object | tests/copy_semantics, tests/references_basic | covered |
| A scalar (number, Boolean, enum) is copied on assignment | tests/memory_scalar_copies | added |
| Passing a class instance shares it with the callee | tests/parameter_writes, tests/held_items | covered |
| Storing in a list or attribute shares the object | tests/deep_copies, tests/stored_in_place | covered |
| A release to zero runs `drop()` | tests/drop_hook | covered |
| `drop()` runs before the object's attributes are released | tests/memory_drop_order | added |
| Overwriting a name releases the old object at the assignment | tests/memory_overwrite_releases | added |
| Removing an element from a list releases it at the removal | tests/memory_removal_releases | added |
| A singleton is never counted and is destroyed at exit | tests/singleton_teardown, tests/singleton_held_at_exit | covered |
| Atomic counts only where threads can share a class; plain otherwise | none | superseded by D662 (not observable) |
| `drop()` takes no parameters and returns nothing | conformance/reserved_signature | covered |
| `drop()` is never called by name | conformance/memory_drop_called | added (compiler fails: `resource.drop()` compiles) |
| `==` calls `equals` when the class declares it | tests/operators, conformance/equals_signature | covered |
| `==` on two instances without `equals` compares identity | tests/shared_copies, tests/union_identity | covered |
| A union compares identity | tests/union_identity | covered |
| A comparison releases the operand it produced (`kept[index] == shape`) | tests/union_identity | covered |
| A `String` compares by content, short or long | tests/memory_string_content_equality | added |
| `copy()` is shallow: new object, same references inside | tests/deep_copies, tests/copy_semantics | covered |
| `deep_copy()` copies every reference attribute and element | tests/deep_copies, tests/shared_copies | covered |
| A union or `type` attribute is deep copied through its run-time class | tests/deep_copy_unions | covered |
| A union or a shape value answers `deep_copy()` itself | tests/deep_copy_unions, tests/shape_copies | covered |
| A class declaring its own `deep_copy()` is copied through it wherever reached | tests/memory_own_deep_copy | added |
| A self-referring structure is copied node by node | tests/deep_copy_unions | covered |
| `deep_copy()` keeps a graph's shape: one copy per object, cycles kept | tests/deep_copy_graphs | covered |
| An object reached twice through classes that cannot reach themselves is copied twice | tests/memory_deep_copy_twice | added |
| `Weak<T>`: making, `get()`, `Weak<Integer>` refused, the table | none | superseded by D755 (no Weak) |
| A `Weak` stays on its thread (error at the `Parallel`) | tests/cycles_across_threads | superseded by D755 |
| Cycles leak unless broken by hand | tests/parent_back_reference, tests/cycles_across_threads | superseded by D755 and D759 (the compiler picks the back reference; tests cover the new rule) |
| A scalar parameter is a copy; a reference parameter is the caller's object | tests/memory_scalar_copies, tests/parameter_writes | covered |
| An argument the caller holds is passed without a count | none | superseded by D662 (not observable) |
| A getter's fresh result used directly as an argument is released after the statement | tests/memory_temporary_released | added (compiler fails: the temporary is never released, its `drop()` never runs) |
| A local read from a list, assigned and never read again, is refused (direct read) | conformance/replaced_items | covered |
| The same when a function answers the item, naming the writing function | conformance/memory_replaced_answered | added (compiler fails: compiles) |
| The same with no writing function: "Replace it through a function of ..." | conformance/memory_replaced_unwritten | added (compiler fails: compiles) |
| The rule is judged inside the branches of a last `if` | conformance/memory_replaced_branch | added (compiler fails: compiles) |
| Allowed: read after, assigned in a loop, from `copy()` or `first()` | tests/memory_replaced_allowed | added |
| A library class is not checked by the replaced-item rule | none | missing |
| `--debug-memory` prints `allocations: N frees: N` at exit, balanced | tests/memory_debug_memory_balance | added (compiler fails: nothing is printed) |
| `--debug-memory` names the classes of leaked objects | none | missing (no spec program can leak once D759 frees cycles) |
| `--debug-memory` fills fresh memory with 0xA5 | none | superseded by D662 (no program reads raw memory) |
| Only a `--debug-memory` build carries the table; `live_allocations()`, `live_bytes()` | none | superseded by D662 |
| `value.memory` (`address`, `bytes`, `section`) and its read-only error | none | superseded by D662 |
| `Memory` is a namespace; `Memory.Address` and its reads and writes; only `library/` calls them | none | superseded by D661 and D662 |
| `Memory.Heap()` allocate, resize, free, counts | none | superseded by D661 and D662 |
| `TypedMemory<T>` | none | superseded by D661 and D662 |
| A number's storage line `var _memory = Buffer(4)`; other spellings refused | none | missing (the library now writes `var _buffer = Buffer(4)`; the spec page is out of date) |
| A number's storage size must be the C type's | none | missing |
| Reading `_memory` inside a number is refused: write `this` | conformance/number_memory_read | covered |
| Placement: register, frame, in the value, constant, frame objects, pools, heap | none | superseded by D662 (not observable) |
| Allocators, `Memory.Arena`, `.memory.allocator` and its errors, generic allocator error | none | superseded by D661 and D662 (tests retired) |
| A Vector of a class keeps its items inline | none | superseded by D1707 (one List) |
| `Vector<Integer>`/`Items<Integer>` refused, naming `List<Integer>` | tests/plain_items | superseded by D1707 (one List) |
| A Vector item class must hold only numbers, Booleans, enums, Strings | none | superseded by D1707 (one List) |
| A borrowed item's writes write the vector's own item | none | superseded by D1707 (one List) |
| Borrowed item kept in an attribute is refused | none | superseded by D1707 (one List) |
| Borrowed item put in a list is refused | none | superseded by D1707 (one List) |
| Borrowed item returned is refused | none | superseded by D1707 (one List) |
| Borrowed item made into a function value is refused | none | superseded by D1707 (one List) |
| Borrowed item given a second name is refused | none | superseded by D1707 (one List) |
| Borrowed name assigned again is refused | none | superseded by D1707 (one List) |
| Borrowed from a collection nothing keeps (`copy().find_by_...`) is refused | none | superseded by D1707 (one List) |
| Not read after `append` | none | superseded by D1707 (one List) |
| Not read after `reserve` | none | superseded by D1707 (one List) |
| Not read after a removal (`remove_at`) | none | superseded by D1707 (one List) |
| Not read after `swap` | none | superseded by D1707 (one List) |
| Not read after a call of the program that grows the vector (call effects) | none | superseded by D1707 (one List) |
| A resizing statement in a loop ends the borrow for the whole loop | none | superseded by D1707 (one List) |
| A collection read with `[]` from an attribute is that attribute's element | tests/queued_borrows | covered |
| A call through a `type` reaches only the classes it is handed | tests/queued_borrows | covered |
| Inside the item's class, `this` is not a value | none | missing (rule on collections.md) |
| A reopened `Vector`/`Items` function is checked; the library's own is trusted | none | superseded by D1707 (one List) |
| `append`/`set_at` copy the value in; the value stays an ordinary object | none | superseded by D1707 (one List) |
| A row literal of borrowed items is passed to a `type` parameter and writes the items | tests/list_rows | covered |
| A row with a counted attribute (a String) is refused | none | superseded by D1707 (one List) |
| A row passed to anything but a function called by name with a `type` parameter is refused | none | superseded by D1707 (one List) |
| A row kept in an attribute is refused | none | superseded by D1707 (one List) |
| A row returned is refused | none | superseded by D1707 (one List) |
| A row given a second name is refused | none | superseded by D1707 (one List) |
| A row's own attribute assigned is refused | none | superseded by D1707 (one List) |
| A row assigned again, a row's required function as a value | none | missing |
| A call handed a row may not resize what it borrows from | none | superseded by D1707 (one List) |
| Walked rows (`fill_attributes`, `attribute: Spite.AttributeDeclaration`), sparse columns, `attribute.index`, walked locals, lent list elements | tests/walked_rows, tests/sparse_rows, tests/walked_row_locals, tests/lent_list_elements, tests/lent_list_elements_parallel | covered |
| Arguments a plural fills (`phase_each(made_arguments(...))`, `argument: Spite.Argument`) and their errors | tests/lent_arguments, tests/streamed_rows, tests/ignored_lent_argument, conformance/argument_walk_without_function | added |
| A walked line names the attribute's class by its full name | none | missing |
| `Items<T>` whose `T` fits lends as a Vector; errors begin with the choice | tests/list_columns | superseded by D1707 (one List) |
| `remove_swapping` on `Items` counts as a removal | none | superseded by D1707 (one List) |
| `Items<T>` whose `T` does not fit lends nothing: its items are counted | tests/list_unfit_counted | superseded by D1707 (one List) |
| A function returning a singleton's Vector/Items item lends it to its caller | tests/lent_results | superseded by D1707 (one List) |
| Lending decided per instance with compile-time conditions folded | tests/folded_lends | covered |
| Narrowing keeps a lent item borrowed | tests/narrowed_lends | covered |
| A lent result is not returned further | none | superseded by D1707 (one List) |
| A borrowed item does not reuse a name already used in the block | none | superseded by D1707 (one List) |
| A lending function is never a function value | none | superseded by D1707 (one List) |
| A class whose function lends does not fit a `type` requiring it | none | superseded by D1707 (one List) |
| A lending function returns only items or null | none | superseded by D1707 (one List) |
| An item lent to a call is written in place, and lent on | tests/lent_to_calls | covered |
| A lent parameter kept in an attribute is refused | none | superseded by D1707 (one List) |
| A lent parameter put in a list is refused | none | superseded by D1707 (one List) |
| A lent parameter returned is refused | none | superseded by D1707 (one List) |
| A lent parameter made into a function value is refused | none | superseded by D1707 (one List) |
| A lent parameter given a second name is refused | none | superseded by D1707 (one List) |
| A call lent an item may not resize its collection | none | superseded by D1707 (one List) |
| Not lent to a library function: the error says why | none | superseded by D1707 (one List) |
| Not lent to a union parameter: the error says why | none | superseded by D1707 (one List) |
| Not lent from another expression: name it first | none | missing (a call inside an argument is refused first) |
| Run-time cost statements (rows in the frame, `___lent_` copies) | none | superseded by D662 (not observable) |

## standard_library.md

Rules: 100. Covered before: 50. Covered now: 86.

| Rule | Program(s) | Status |
|---|---|---|
| Library values are reference counted and freed with their last holder | tests/references_basic, tests/owned_objects | covered |
| A protocol or format no company owns is in the library; a brand client is a package | none | not a program rule |
| String is immutable; short text has no allocation; building text in a loop is linear | tests/text_building | covered |
| length, is_empty, trim, upper_case, lower_case | tests/string_methods | covered |
| slice is clamped, "" for an empty or reversed range | tests/string_slice_edges | added |
| text[index] is the whole character at that byte, null out of range or inside one | tests/string_get_at, conformance/string_index_unproven | added |
| code_at is 0 out of range | tests/string_code_at_edges | added |
| contains, starts_with, ends_with | tests/string_methods | covered |
| index_of answers -1 when absent | tests/string_methods (prints null) | superseded by D669 |
| replace replaces every occurrence | tests/string_replace_every | added |
| split; an empty separator splits into characters; lines splits on \n | tests/string_methods | covered |
| to_integer, to_long, to_double: spaces, sign, null for anything else or out of range | tests/text_to_number | covered |
| to_tiny, to_short, to_unsigned_short, to_unsigned_long ranges; to_float ".5" | tests/string_small_numbers | added |
| to_byte and unsigned forms null for a negative number | tests/text_to_number, tests/string_small_numbers | covered |
| Assigning a String to a numeric variable parses it | tests/text_to_number | covered |
| ==, !=, <, > compare by content | tests/string_methods | covered |
| The right side of + casts to String (numbers, Boolean, enum) | tests/string_plus_casts | added |
| to_string and to_debug of a String (quoted, escapes) | tests/string_to_debug_escapes, tests/debug_values | added |
| to_bytes answers a Buffer of the UTF-8 | tests/utf8_text | covered |
| pluralize/singularize: ending rules, irregulars, uncountables, case kept, last word only, "" | tests/string_pluralize_rules, tests/inflection_fold | added |
| A program changes the irregulars or uncountables by reopening (whole table) | tests/inflection_table | covered (rule moved to String's attributes by D770) |
| pluralize on a literal is folded while compiling | tests/inflection_fold | covered |
| Float and Double maths members and their edges | tests/maths_functions, tests/double_maths, tests/maths_precision | covered |
| minimum, maximum, clamp pass not-a-number on | tests/nan_passes | covered |
| Whole-number absolute, minimum, maximum, clamp (high wins) | tests/maths_functions, tests/maths_whole_minimum_cast | covered |
| The smallest signed value's absolute wraps to itself | tests/smallest_absolute | covered |
| A whole number's minimum(other) casts other first ("a Byte's minimum(300) compares with 44") | tests/maths_whole_minimum_cast (uses 30) | contradicted by values_and_types.md: a narrowing cast that does not fit halts, so the 44 example cannot hold |
| Constants: pi, tau, euler_number, infinity, not_a_number, largest, smallest | tests/maths_functions | covered |
| Assigning a constant is an error | conformance/maths_constant_assigned | covered |
| Calling a constant is an error | conformance/maths_constant_called | added (compiler fails: "a Spite.Class has no attribute or function") |
| A constant read on a value is an error | conformance/maths_constant_on_value | covered |
| A value's function called on the class is an error | conformance/maths_value_function_on_class | added (compiler fails: generic message) |
| A name that is no constant of the class is an error | conformance/maths_unknown_constant | added (compiler fails: "'Integer' is a class, not a value") |
| Maths on constants is folded | tests/maths_folding | covered |
| Other languages' short names are errors (sqrt on Float) | conformance/maths_other_spellings | covered |
| The same on a whole number (abs) | conformance/maths_whole_spelling | added (compiler fails: generic "has no attribute or function") |
| No fused_multiply_add | none | missing |
| File read, write, append, exists, remove, name; read of a missing file is null | tests/file_text_basics | added |
| File bytes: size, modified, read_bytes, read_bytes_at, write_bytes, append_bytes | tests/binary_files | covered |
| File.move_to refuses an existing target, moves files and folders | tests/moving_files | covered |
| File.lock between processes, unlock | tests/file_lock | added (compiler fails: File has no 'lock') |
| `File.read_as_buffer()` reads null outside the file; an empty file is an empty buffer | tests/file_as_buffer | covered |
| Directory entries: folders first, then files, each sorted by name; a union to switch on | tests/directory_entries | covered |
| A missing Directory: exists false, no entries | tests/directory_missing | added |
| Process run, output, run_attached, working directory, environment | tests/process_paths, tests/process_settings, tests/system | covered |
| Program exit flushes the console first | tests/program_exit_flushes | added |
| Program environment null when unset; executable_path | tests/program_environment | added |
| Program sleep | tests/frame_waits | covered |
| Program live_allocations | none | superseded by D662 |
| Clock elapsed and now | tests/clock_reading, tests/clock_reads | covered |
| Benchmark(work) answer and duration | tests/benchmark_answer | added (compiler fails: unknown type 'Benchmark') |
| FileSystemWatcher | tests/file_watching | covered |
| Socket: listen, connect, accept, lines, bytes, _now calls, closed, IPv6 | tests/socket_bytes, tests/socket_waits, tests/concurrent_sockets, tests/ipv6_addresses, tests/datagrams | covered |
| Concurrent and Parallel handles | tests/concurrent_waits, tests/parallel_work | covered (concurrency.md) |
| ThreadPool, ThreadLocal, Lock, ThreadSlot classes | retired/ | superseded by D662 |
| DynamicLibrary and ForeignBytes | tests/foreign_library | covered (compiler fails: exit code); ForeignBytes reads and halts missing |
| Memory.Heap, Memory.Arena, Memory.Address, TypedMemory as program classes | none | superseded by D661, D662 |
| Console is a singleton, one instance everywhere | tests/console_class | covered |
| Calling a member on Console() directly is an error | conformance/console_called_directly | added |
| read_line answers null only at the end; drops \r before the line feed | tests/console_input, tests/console_read_carriage_return | added |
| The entry returning normally is exit code 0 | tests/hello, tests/exit_code | covered |
| Printing is to_string(); every number, Boolean, String, Symbol, enum, class prints | tests/printable_values | covered |
| A class without to_string is not Printable | conformance/print_without_to_string | covered |
| A List is not Printable | conformance/console_print_list | added |
| null prints as null | tests/list_empty_ends | superseded by D754 (added) |
| print, error, debug flush their line; write does not; error flushes output first | tests/console_streams_order | added |
| Two threads printing never split a line | tests/debug_threads | covered |
| debug and to_debug: class, list, dictionary, text, enum, null, nested, own to_debug | tests/debug_values, tests/debug_dictionary_keys | covered |
| to_debug leaves private attributes out; Name {} with nothing to show | tests/debug_private_hidden | added |
| A cycle ends at Name {...} | tests/debug_values | covered |
| The REPL keeps its own display | tests/interactive_loop | covered (repl.md) |
| WebSocket handshake, frames, pings, closes, failures | tests/web_socket_exchange, tests/web_socket_failures | covered |
| Buffer(size) is required, the size never a limit | tests/buffer_bytes, conformance/byte_reads | covered |
| Buffer(-1) halts | tests/buffer_negative_size | covered |
| buffer[index] null out of range; set_at out of range halts | tests/buffer_index_out_of_range | added |
| append, truncate, clear, copy, count, is_empty as on a List | tests/buffer_truncate_position, tests/binary_bad_input | added |
| Byte order is named in every read and write | tests/buffer_bytes | covered |
| read_<number>() moves position; null leaves it; remaining() | none | superseded by D1707 (removals of #23) |
| position below 0 or above count() halts | tests/buffer_position_out_of_range | added |
| truncate or clear below the position moves it | tests/buffer_truncate_position | added |
| read_<number>_at answers null outside | tests/buffer_crash | covered |
| write_<number>_at outside halts | tests/buffer_write_at_outside | added |
| A float keeps its IEEE bits, not-a-number included | tests/buffer_float_bits | added |
| Old read form with a position is an error naming _at | conformance/buffer_old_position_form | added (compiler fails: "takes 0 arguments") |
| Old write_<number> name is an error naming write_<number>_at | conformance/buffer_old_write_name | added (compiler fails: generic "has no attribute or function") |
| read_bytes(count), read_bytes_at: null unless inside; append_bytes(other, start, count) | tests/buffer_read_runs | added |
| A negative count halts | tests/buffer_read_negative_count | added |
| append_bytes outside the other buffer halts | tests/buffer_append_bytes_outside | added |
| text(position, count): null for invalid UTF-8, halt outside | tests/buffer_text_runs | added |
| to_utf8_text: null for every invalid form | tests/utf8_text | covered |
| A List has none of the byte functions | conformance/list_byte_functions | added (compiler fails: generic message) |
| Files: read_bytes whole, read_bytes_at short at the end, empty file empty buffer | tests/binary_files | covered |
| A negative file position or count halts | none | missing |
| Socket bytes: read_bytes, read_bytes_now, write_bytes_now | tests/socket_bytes | covered |
| A proven read_<number>_at answers the number; an unproven one is T? | tests/fixed_offsets, conformance/byte_reads | covered |
| ForeignBytes reads and writes, halts on a bad address or count | none | missing |
| Address forms of File, Socket and BinaryReader are errors | none | superseded by D662 (no address a program can hold) |
| A List<Byte> where a Buffer is expected is the type error | none | missing |
| String, List, Dictionary, number text are Spite over the floor | none | not a program rule |
| examples/calculator | none | not a program rule |

## programs.md

Rules: 40. Covered before: 20. Covered now: 35.

| Rule | Program(s) | Status |
|---|---|---|
| A program is a folder; its entry is the file named after it | tests/hello | covered |
| A program named by a file is an error | | missing (the runner names folders only) |
| A folder with no entry file is an error naming the file looked for | conformance/programs_no_entry_file | added (compiler fails: names 'programsnoentryfile.spite') |
| The entry constructor takes no arguments | conformance/entry_constructor_arguments | covered |
| The program ends with exit code 0 when the constructor returns | tests/exit_code | covered |
| `Arguments()`: `count()`, `get(index)`, `.player_name` as `String?` | tests/program_arguments, tests/launcher_arguments | covered |
| A flag not given reads `null` | tests/programs_arguments_beside_settings | added |
| A bare `--` is an ordinary argument | tests/programs_bare_double_dash | added |
| The launcher loads library, system folder, then the program | tests/launcher_arguments, tests/system_folder | covered |
| A `load` names its folder with literal text, may sit under a `Build` `if` | conformance/load_unknown_condition, tests/load_on_build | covered |
| `Environment` reopened with one `var` per setting | tests/environment_settings | covered |
| Setting from the command line, bare `--name` for a `Boolean` | tests/programs_setting_bare_boolean, tests/environment_settings | added |
| Setting from the environment variable in upper case | | missing (the runner sets no environment) |
| Setting falls back to its declared default (negative Integer too) | tests/programs_setting_default | added |
| Settings are kebab-case | tests/kebab_setting | covered |
| A snake_case setting given to the compiler is an error | conformance/underscore_setting | added (by the compiler.md audit) |
| A snake_case setting given to the built program stops it, exit 1 | tests/programs_setting_snake_case_run | added (compiler fails: accepted, the setting keeps its default) |
| A setting named like a `Build` field is an error | conformance/setting_named_like_flag | covered |
| A setting is declared with only a literal default | conformance/environment_setting | covered |
| A setting value of the wrong type given to the compiler is an error | conformance/setting_value, conformance/bare_flag_not_boolean | added (by the compiler.md audit) |
| A setting value of the wrong type in a direct run crashes | tests/programs_setting_value_run | added |
| The compiler passes a setting on to the program it runs | | missing |
| No `--` separator between compiler flags and the program's | conformance/separator_flag | added (by the compiler.md audit) |
| `--hot-reload` reads settings again after a reload | | missing (repl page) |
| `Build` fields are constants; a flag sets one | tests/build_settings | covered |
| A field nobody sets keeps the program's declared default | tests/programs_build_default_kept | added |
| The program's `build.spite` wins over a package's | tests/build_precedence | covered |
| Flags are kebab-case; `_` is an error | conformance/underscore_flag | covered |
| `--format` and a `format` field are errors | conformance/format_flag, conformance/format_setting | covered |
| `target_operating_system` cannot be declared by a program | conformance/target_in_build | covered |
| An unknown target system | conformance/target_operating_system | covered |
| A bare `Boolean` flag is `=true` | tests/programs_build_bare_boolean | added |
| Any other bare flag names the value it needs | conformance/programs_bare_value_flag | added |
| A value of the wrong type for a `Build` field | conformance/build_setting_type, conformance/flag_value | covered |
| A flag naming no field lists the fields | conformance/unknown_compiler_flag | covered |
| A flag given twice | conformance/flag_given_twice | covered |
| Giving `operating_system` is an error | conformance/operating_system_flag | added (by the compiler.md audit) |
| Giving `program` is an error | conformance/program_flag | added (by the compiler.md audit) |
| `--final-classes` prints `Build` with its declared defaults | | missing |
| `Build` costs nothing at run time | tests/build_settings | covered |

## testing.md

Rules: 8. Covered before: 1. Covered now: 5.

| Rule | Program(s) | Status |
|---|---|---|
| An expectation is `crash <condition>`; the first failure ends the run | tests/testing_failing_expectation, tests/crash_report | added |
| Discovery walks `Spite.Class.instances` for `...Tests` classes and their `test_` functions | tests/testing_discovery | added (compiler fails: a walked class cannot be called to make an instance) |
| A test runs on an instance the runner makes, through `Spite.Call` | tests/testing_discovery | added (compiler fails: as above) |
| The entry class's `.functions` is empty | tests/reflection_stand_in | covered |
| A test that takes arguments is the unfilled-argument error of `Spite.Call` | | missing |
| Running one test or class by the first argument, crashing on a name that matches nothing | | missing (runner convention) |
| Every test is a leak test: a second run leaves `live_allocations()` unchanged | | missing (runner convention) |
| The constructor is the set-up | tests/testing_discovery | added (compiler fails: as above) |

## time.md

Rules: 21. Covered before: 9. Covered now: 21.

| Rule | Program(s) | Status |
|---|---|---|
| `Instant` arithmetic, comparison, prints in UTC with `Z` | tests/instant_arithmetic | covered |
| `Duration` units `nanoseconds` to `hours`, `total`, `part` | tests/time_duration_units, tests/instant_arithmetic | added |
| `total('microseconds')` rounds toward zero | tests/time_duration_units | added |
| `Duration(1, 'days')` is an error | conformance/time_duration_days | added |
| `Period` years, months, days; `'weeks'` is seven days | tests/time_period_weeks, tests/calendar_math | added |
| Comparing periods with `<` is an error | conformance/time_period_comparison | added (compiler fails: generic 'has no attribute or function' text) |
| `Date` functions; a month added past the end clamps (Jan 31 + 1 month = Feb 29, 2024) | tests/calendar_math | covered |
| `Time`, `DateTime` | tests/calendar_math, tests/daylight_saving | covered |
| Old spellings (`LocalDate`) name the new ones | conformance/time_old_spelling | added |
| An `Instant` and a local reading do not compare | conformance/time_instant_against_local | added (compiler fails: says 'a DateTime cannot be used where a Instant is needed') |
| `local.to_instant_in(zone, ambiguity)` resolves gaps and overlaps by `'compatible'`, `'earlier'`, `'later'` | tests/daylight_saving | covered |
| A zone is one `TimeZone`; `instant.to_date_time_in(zone)`, `instant.to_string_in(zone)`, `zone.identifier` | tests/daylight_saving, tests/fixed_offsets, tests/zone_files | covered |
| An impossible `Date` built from numbers crashes | tests/time_impossible_date | added |
| An impossible `Time` built from numbers crashes | tests/time_impossible_time | added |
| Text that cannot exist reads as `null`; `to_instant()` needs an offset, `to_date_time()` refuses one | tests/time_text_errors | covered |
| A leap second in text (`23:59:60`) is `null` | tests/time_text_errors | covered |
| A bracketed zone must agree with the offset (any known zone after `Z`); an unknown zone, a second zone or an annotation other than `u-ca=iso8601` is `null` | tests/time_text_errors | covered |
| ISO 8601 writing is each type's `to_string()` | tests/time_text_round_trips | covered |
| Zones: `find`, `utc`, `fixed_offset`, `system`, `read_tzif` | tests/zone_files, tests/fixed_offsets, tests/daylight_saving | covered |
| `Clock.elapsed_nanoseconds()`, `elapsed_milliseconds()`, `now()` | tests/clock_reading, tests/clock_reads | covered |
| `Benchmark(work)` runs `work` once, keeps `answer` and `duration`; `Benchmark<Nothing>` | tests/time_benchmark | added (compiler fails: the library has no Benchmark) |

## json.md

Rules: 41. Covered before: 14. Covered now: 35.

| Rule | Program(s) | Status |
|---|---|---|
| JsonWriter writes a class's attributes in declaration order | tests/json_camel_case, tests/json_infinity | covered |
| A union attribute makes JsonWriter an error naming the path | conformance/json_unwritable | covered |
| The same check for JsonReader ("cannot read") | conformance/json_reader_union | added (compiler fails: compiler crash in class_of) |
| The same check for BinaryWriter ("the bytes do not say which member") | conformance/binary_writer_union | added (compiler fails: compiles) |
| A type or a function value attribute "has no JSON form" | none | missing |
| One mistake is one error | conformance/json_unwritable | covered |
| write(value: T?) writes null for an empty T?; a T? attribute is null | tests/json_writes_null | added |
| A writer and a reader are made once and reused | tests/json_strict_reader | added |
| read answers T?; read_or_crash halts | none | superseded by D1707 (removals of #23) |
| read_or_crash's crash line carries wanted= and position= | none | superseded by D1707 (removals of #23) |
| The old name Json is an error naming the pair | conformance/json_split | covered |
| BinaryWriter.write and BinaryReader.read round-trip lists, dictionaries, T?, enums, Boolean | tests/binary_round_trip | added |
| Many values in one buffer are a `BinaryWriter<List<T>>`; `read` wants exactly one value | tests/binary_many_values | added |
| A negative start for read_from halts | none | superseded by D1707 (removals of #23) |
| reader.read_memory is an error | none | superseded by D662 |
| JSON escapes ", \, \n, \r, \t on writing and reads them back | tests/json_escapes_written | added |
| Other control characters written as \u00XX | none | missing |
| \uXXXX read back as UTF-8, surrogate pairs included | tests/json_unicode_read | added |
| Private attributes are left out | tests/json_private_skipped | added |
| Attributes holding a singleton are left out | tests/serializer_singletons | covered |
| A Dictionary's entries in insertion order; a whole-number key quoted | tests/number_keys, tests/enum_key_json | covered |
| An attribute only handed to a writer counts as read | tests/json_camel_case | covered |
| Binary format: little-endian numbers, Boolean one byte, enum index, varint lengths | tests/binary_bad_input, tests/binary_bad_enum | added |
| schema() answers the FNV-1a hash, writer and reader agree | tests/binary_schema | covered |
| BinaryReader answers null for left-over bytes, a cut value, a bad Boolean byte, empty bytes | tests/binary_bad_input | added |
| null for an enum index past the last value | tests/binary_bad_enum | added |
| null for a count or length past the bytes left | tests/binary_count_past_end | added |
| null for a plain Symbol name the program does not use (binary) | none | missing |
| A binary file round trip | tests/binary_files | covered |
| An unknown key is skipped, whatever it holds | tests/json_lenient_reader, tests/json_camel_case | added |
| A missing attribute keeps its default | tests/json_lenient_reader | added |
| A missing brace, text after the value, a wrong kind, empty text: read is null | tests/json_strict_reader | added |
| camelCase and PascalCase keys read into snake_case; the later key wins | tests/json_camel_case | covered |
| A serializer's keys map renames an attribute both ways and replaces its name | tests/json_custom_keys | added (compiler fails: JsonWriter has no 'keys') |
| A keys map naming a missing, private or singleton attribute is an error | none | missing |
| json_key_<attribute>() is an error | conformance/json_key | covered |
| A number reads into the attribute's number type: 3.7 into an Integer is 3 | tests/json_number_into_integer | added (compiler fails: read answers null) |
| A whole number reads into a decimal attribute | tests/json_integer_into_decimal | added |
| Infinity and not-a-number crash JsonWriter, naming value and path | tests/json_infinity | covered (expected crash line names the library, not shown=/path=) |
| A plain Symbol attribute is written as its name; an unused name reads as null | tests/json_symbols | covered |
| --final-classes does not print a generic writer's functions | none | missing |

## reflection.md

Rules: 64. Covered before: 32. Covered now: 52 (14 of them fail on the master compiler). Superseded: 4 (D662). Missing: 6.

| Rule | Program(s) | Status |
| --- | --- | --- |
| Every class is a `Spite.Class`; printing one prints its name | tests/reflection, tests/class_text | covered |
| A namespace is a `Spite.Namespace`; printing one prints its dotted name | tests/namespace_objects, tests/reflection_objects | covered (reflection_objects: compiler fails, refused last `if`, D797 rewrites it) |
| `Spite` is reserved: a `spite/` folder only reopens its classes | conformance/new_spite_class, conformance/reserved_spite_namespace | covered |
| Members are get-only attributes read without `()` | tests/reflection_objects, tests/function_values | covered |
| Assigning a member is "'name' is read-only" | conformance/reflection_read_only | covered |
| A `_` name is private to its class, naming the getter | conformance/classes_private_name, conformance/classes_private_function | covered |
| `.functions` holds the template instances made for the class | none | missing |
| `.functions` is in the order made and leaves out the constructor | tests/reflection_stand_in, tests/function_values | covered |
| A library class describes its members (`Memory.Heap.functions`) | none | superseded by D662 (no Memory.Heap) |
| `.class` of an attribute or argument is a `Spite.Class` compared by identity | tests/argument_class, tests/function_values | covered |
| `.namespace`/`.parent` are nullable, the chain ends at `null` | tests/namespace_objects | covered |
| `Spite.Namespace.instances` lists every namespace, parents first | tests/reflection_objects | covered (compiler fails, see above) |
| `.enums` of a namespace | tests/namespace_enums | covered |
| A class and a namespace with the same dotted name is an error | conformance/class_and_namespace | covered |
| `.source_files` and `.source_directories` in load order | tests/reflection_sources, tests/class_walk | covered (class_walk: compiler fails, unknown type 'Spite') |
| `value.memory` and "only a named value has memory of its own" | none | superseded by D662 |
| A class's members are declarations, an instance's are bound | tests/private_walk | covered |
| A declaration given to a `Spite.Attribute` parameter is refused | conformance/declaration_walk | covered |
| A bound attribute given to a `Spite.AttributeDeclaration` parameter is refused | conformance/reflection_attribute_to_declaration | added |
| A bound attribute's `.value` reads and writes the field | tests/private_walk | covered |
| `.value` of a declaration is refused | conformance/reflection_declaration_value | added (compiler fails: walks it as the field's String and says it has no 'value') |
| `value.attributes[attribute]` (or `value[attribute]`) reads and writes the field | tests/private_walk, tests/attribute_uses, tests/template_inference | covered |
| A bound function is the function value of that instance | tests/function_values | covered |
| `Spite.Call(declaration, instance)` fills arguments and `call()` runs it | tests/reflection_call_runs | added (compiler fails: unknown type 'Spite.Call') |
| A call with an argument left unfilled is refused | conformance/reflection_call_unfilled | added (compiler fails: unknown type 'Spite.Call') |
| A call on an instance of another class is refused | conformance/reflection_call_other_class | added (compiler fails: unknown type 'Spite.Call') |
| An unfilled argument the compiler cannot see halts `call()` naming it | none | missing |
| A walk over another class's attributes sees its private ones | tests/private_walk | covered |
| A number's, a Boolean's and a String's `.attributes` is empty | tests/reflection_value_attributes_empty | added (compiler fails: an Integer has no 'attributes') |
| A List's or Dictionary's class object has no attributes | tests/reflection_collection_attributes_empty | added (compiler fails: a Dictionary's class answers 5) |
| A class name read as a value is its `Spite.Class` (`Weapon.name`, `.namespace`) | tests/reflection_class_name_value | added |
| A function of the class called on its name (`Weapon.debug()`) is refused | conformance/reflection_class_function_call | added (compiler fails: says a Spite.Class has no 'debug') |
| A class name is its class object where a `Spite.Class` is wanted (argument, typed var, assignment, return) | tests/reflection_class_object_passed | added |
| A bare class name anywhere else is "'Health' is a class, not a value" | conformance/class_as_value | covered |
| `==` with a `Spite.Class` on the left compares class objects | tests/reflection_class_comparison | added |
| `class` and `namespace` inside a function; a local named `class` wins | tests/reflection_class_inside | added (compiler fails: `crash namespace` is refused, it types `namespace` as not nullable) |
| `class ClassKeyword {` fails at 1:1 | conformance/class_keyword | covered |
| `.class` through a `type` or union answers the real class; a literal answers `Object` | tests/class_through_shape | covered |
| `Person.instances` lists live instances | none | superseded by D662 |
| A class object's `.functions` come from a stand-in that runs no constructor | tests/reflection_stand_in | covered |
| A singleton's stand-in never runs its `drop()` | tests/singleton_stand_in | covered |
| The entry class's `.functions` is empty | tests/reflection_entry_functions_empty | added |
| A class object is made once, behind a lock only with threads | tests/threaded_class_objects | covered |
| `Spite.Class.instances` is every class of the program, not the library's | tests/every_class, tests/reflection_narrowing | covered |
| Constants fold: questions, counts, names; `if` on a constant keeps only what runs | tests/folded_checks, tests/codegen_member_fold, tests/reflection_narrowing | covered |
| `[]` with a literal name absent: the branch is never compiled | tests/reflection_narrowing | covered |
| `each(function)` over a constant list is unrolled | tests/private_walk | covered |
| A function called with a constant reflection argument is specialised (`_for_` in `--final-classes`) | none | missing |
| A function typed by its reflection parameter (`: argument.class`) | tests/reflection_walks, conformance/function_reflection | covered (reflection_walks: compiler fails, unknown type 'argument.class') |
| Run-time reflection objects answer from tables; `.accesses` at run time | tests/run_time_kinds, tests/run_time_accesses | covered |
| `.returned_literal` of a function that is not one literal `return` halts at run time | none | missing |
| A run-time object handed to a function typed by its reflection parameter is refused | none | missing |
| Naming a type through a non-constant reflection object is refused | conformance/reflection_unknown_class_type | added (compiler fails: says only "unknown type 'kind'") |
| A specialisation's parameter used as a run-time value is refused | conformance/reflection_specialised_kept | added (compiler fails: says a String cannot be a Spite.AttributeDeclaration) |
| Tables are kept only for what a program reads at run time | none | superseded by D662 (not observable) |
| An attribute named like a reflection name (`attributes`) is refused | conformance/reflection_names | covered |
| An accessor `get_attributes` is refused the same way | conformance/reflection_names_accessor | added |
| A function named like a reflection name (`functions`) is refused | conformance/reflection_names_function | added |
| Locals and parameters may use the reflection names | tests/reflection_names_locals | added |
| Declaring `is_singleton` in a class is refused | conformance/singleton_function | covered |
| A class file cannot replace a function of its class object (`Gadget.to_string()`) | tests/reflection_class_function_not_replaced | added |
| Reopening `Spite.Class` gives every class object the member | tests/reflection_reopened_class | added |
| A changed default of `Spite.Class` shows in `--final-classes` | none | missing |

## metaprogramming.md

Rules: 67. Covered before: 31. Covered now: 49.

| Rule | Program(s) | Status |
|---|---|---|
| A parameter `Spite.AttributeDeclaration` named by a word of its function makes a template over the members of the class its body reads it on (D798) | tests/plural_templates, tests/member_templates, tests/template_inference, conformance/template_unused_attribute, conformance/template_unknown_value, conformance/template_no_common_attribute | added |
| A template is instantiated only for the names called; the template emits nothing | tests/kept_templates | covered |
| `attribute: Spite.Attribute<Owner>` (any range type with `<Owner>`) is an error: the class comes from the body (D798) | conformance/template_attribute_spelling | added |
| Inside an instance the parameter is the member's declaration; `attribute.class` as a type and as a value | tests/every_attribute | covered |
| `attribute.name`, `camel_case_name`, `pascal_case_name`, `index` are constants | tests/reflection_walks | covered |
| `owner.attributes[attribute]` reads or writes the member; a class's own `attributes[attribute]` skips its getter and setter | tests/accessors, tests/private_walk | covered |
| The class may be a codegen value; a non-class value answers nothing | tests/plural_templates | covered |
| The class may be a `type`: the walk ranges over the shape's members | tests/shape_members | covered |
| An exact function wins over a template for one name and direction; a generated getter of an owning attribute is retained | tests/accessors | covered |
| A template that makes a function the class already has is an error | conformance/template_function_clash | covered |
| Reading through `x.attributes[attribute]` counts as a read; `attribute.name` alone does not | conformance/unused_attributes | covered |
| A walk sees private attributes; naming `_x` outside stays the private error | tests/private_walk | covered |
| A template answering `List<member.class>` names its member in the plural | tests/plural_templates | covered |
| `map_names` finds `name`, `map_people` finds `person`, `map_health` stays `health` | tests/member_templates, tests/map_uncountable_members | added |
| The singular where the plural differs is an error: "'map_name' answers every 'name' ..." | conformance/map_singular_name, conformance/plural_templates | added |
| Two members with one plural is an error naming both | conformance/map_shared_plural | added (compiler fails: two members sharing a plural compile, one is picked silently) |
| A name `pluralize()` cannot round-trip is an error naming the irregulars fix | none | missing |
| A name `pluralize()` leaves alone is its own plural (`map_name_with_namespaces`) | tests/map_uncountable_members | added |
| A singular-named template answering `List<member.class>` is an error naming the plural | conformance/plural_templates | covered |
| A question member is never collected: `map_is_alive()` names filter_, count_, any_, all_ | conformance/map_question_member | added (compiler fails: answers the plural error 'map_is_alives') |
| `filter_<member>_<function>(arguments)` chains through a member's function | tests/union_member_templates, tests/reflection_walks, tests/pinned_components | covered (compiler fails: union_member_templates and reflection_walks do not compile) |
| A list of a union: `filter_files()`, `count_files()`, `any_directories()`, `all_files()`, `remove_where_directories()` | tests/union_member_templates, tests/directory_entries | covered (compiler fails: union_member_templates does not compile) |
| `remove_where_` by class on a Dictionary's values is an error naming `keys()` and `remove(key)` | conformance/dictionary_remove_by_class | added (compiler fails: 'remove_where_dogs' is an unknown function) |
| A union list templates an attribute every member has, typed from it | tests/union_member_templates | covered (compiler fails: union_member_templates does not compile) |
| A name matching both a member class and an attribute is an error naming the fix | none | missing |
| A Dictionary takes the member templates over its values | tests/member_templates, tests/dictionary_templates | covered |
| A question member is asked without its `is_` (`filter_written`), and in full is the same template | tests/function_accesses, tests/member_templates | covered |
| There is no plain `map(function)`; a class has no function values (`Monster.is_alive` names nothing) | none | missing |
| Walk names are selected with a literal `[]`, never built from a text with holes | conformance/hole_values | covered (compiler fails: the error differs) |
| A walk over a constant list is unrolled in its order, compiled once per element | tests/class_walk, tests/reflection_walks, tests/system_phases | covered (compiler fails: class_walk and reflection_walks do not compile) |
| A walk over an empty list calls nothing | tests/empty_plural | covered |
| Questions on constants fold (`is_resumable`, `is_mutated`, `is_stateful`, `is_fixed_size`, `is_singleton`, `count()`, `[]`, `==`) | tests/reflection_objects, tests/run_time_kinds, tests/walked_class_fold | covered (compiler fails: reflection_objects does not compile) |
| A function asking its own `.is_resumable` is an error | none | missing |
| `.is_mutated` follows every call, recursion included; reading and rebinding do not count | tests/run_time_kinds | covered |
| `.is_stateful` is the same study asked of the object | tests/reflection_objects, tests/run_time_kinds | covered (compiler fails: reflection_objects does not compile) |
| An empty walk under `crash ... .count() > 0` folds and is the folded-crash error | conformance/empty_walk_crash | added (compiler fails: compiles) |
| `$name` means replaced at code generation | tests/generics | covered |
| Every codegen value is declared on its own `generic` line, after `singleton`, before the rest | tests/generics, tests/generic_singletons | covered |
| A comma on a `generic` line is "a 'generic' line declares one codegen value ..." | conformance/generic_comma_line | added (compiler fails: reports ',' left over) |
| Call sites are positional, in the order of the lines | tests/generics | covered |
| A `$name` with no line used as a value is the Environment error | conformance/generic_undeclared_value | added (compiler fails: the compiler crashes in name_value) |
| A `$name` with no line used as a type is "this class has no 'generic $other_type' line ..." | conformance/codegen_values | covered |
| Only a class has codegen values; a function lists none (`constructor_codegen_list`) | conformance/constructor_codegen_list | covered |
| A class needs no constructor to take codegen values | tests/codegen_defaults, tests/generic_collections | covered |
| The wrong number of codegen values is "'Box' takes 1 codegen value(s), in this order: ..." | conformance/generic_wrong_count | added (compiler fails: words it 'takes 1 types in <>') |
| Reordering same-kind generic lines compiles and means something else (accepted trade) | none | missing |
| Each set of values is its own class, made only where named | tests/generics, tests/generic_singletons | covered |
| `generics` is a parse error naming the `generic` form | conformance/generic_plural_keyword | added (compiler fails: says a statement cannot stand here) |
| A `generic` line inside a function is an error | conformance/generic_line_in_function | added |
| Values are read from the constructor's arguments: `$name`, `$name?`, `List<$name>` | tests/generic_inferred | added |
| Inference from `Dictionary<$key, $value>` and `Spite.Function<...>` arguments | none | missing |
| A `null` argument says nothing; values not found is "... Write them between < and > before the arguments" | conformance/generic_not_inferred | added (compiler fails: reports 'null' has no type here) |
| `$name()` makes the default: constructor, empty List or Dictionary, 0, "" | tests/codegen_defaults | covered |
| `$name()` of a `type` whose members are attributes makes a real object | tests/shape_defaults | covered |
| `var held: $held_type = null` for a type requiring a function is the error naming the attribute | conformance/function_shape_default | covered |
| A constraint accepts only types that fit, the error naming the constraint and what is lacking | conformance/generic_constraints, tests/generic_constraints | covered |
| A `T?` does not fit a constraint: "it may be null, and null has none of what the type needs" | conformance/generic_constraint_nullable | added (compiler fails: compiles) |
| A constraint naming a non-`type` is the error on the `generic` line | conformance/generic_constraint_not_a_type | covered |
| Conditions on codegen values fold in every build | tests/generics, tests/kept_templates | covered |
| A folded `assert` or `crash` is decided per instance; a folded `assert` needs a result that can say nothing | tests/folded_checks | covered |
| A `crash` that folds to false where the program reaches it is a compile error naming the instance | conformance/folded_crash, conformance/folded_build_crash | covered |
| An instance whose function nobody calls compiles | tests/folded_crash_uncalled | covered |
| The sides of an `and` are checked one at a time | conformance/folded_crash_and_side | added (compiler fails: compiles) |
| A class test never true in one instance folds to false in that copy | tests/generic_class_test, tests/walked_class_fold | covered |
| `$value_type == List`, `Dictionary`, `Null`, `Symbol`, `Number`, `Enum`; a union and a `type` name | tests/codegen_kind_tests, tests/folded_helpers, tests/every_attribute, tests/number_type | added (compiler fails: a union or type name on the right never matches) |
| Only what survives folding is compiled; a non-generic class compiles every function | tests/folded_helpers, tests/folded_functions | covered |
| `$value_type.element_type`, `.key_type`, `.value_type` fold; a missing name is the error listing them | tests/codegen_member_fold, conformance/every_attribute | covered (compiler fails: every_attribute's error differs) |
| An `and` left false or an `or` left true folds without its right side | tests/codegen_short_circuit, tests/codegen_member_fold | added |
| A codegen type reads as its class (`$component_type.name`); alone it is the "is a type here" error | tests/codegen_class_name | covered |

## packages.md

Rules: 52. Covered before: 36. Covered now: 47.

| Rule | Program(s) | Status |
|---|---|---|
| The loaded folder is a root and not a namespace; the folders inside it are | tests/namespaces_load | covered |
| Every folder is a namespace; a file named like its folder is an ordinary class reopened by a later root | tests/folder_namesake | covered |
| A file and a folder of one name is "'Engine.Renderer' is this class and also the namespace ..." | conformance/class_and_namespace | covered |
| A file in one root and a folder in another clash the same way | none | missing |
| A folder named like a class of the standard library is the namespace error | conformance/library_class_namespace | covered |
| The program's folder is a root; every subfolder is a namespace with no `load` | tests/relative_namespaces, tests/system_folder | covered |
| A subfolder a top file of the program loads is left to that line | tests/sibling_load | covered |
| Every file's `load`s are followed, a sibling file's included | tests/sibling_load | covered |
| Overlapping loads are the error at the later one | conformance/overlapping_load | covered |
| The same folder loaded by two packages is loaded once, however spelled | tests/load_shared_package | added |
| A folder a package loads inside its own tree is a root and never a namespace, even under an `if` the build decides the other way | conformance/loaded_folder_namespace, tests/load_on_build | covered |
| Folder names are snake_case; the error names the rename | conformance/folder_name, conformance/load_folder_name | covered |
| Only the last folder a load names is held to snake_case; the folders it climbs through may have any name | conformance/load_path_folder_name | added |
| Folders with no `.spite` file, and `.git`, `.spite`, `.spite-cache`, are never walked | tests/working_directory | covered |
| Folders above a root that a path passes through are not checked | tests/namespaces_load | covered |
| Name lookup: own namespace, folder, parent folders, then global | tests/relative_namespaces, tests/folder_class_namespace | covered |
| A dotted name takes the same walk in every position | tests/relative_namespaces | covered |
| An unknown name is `unknown type '...'`, with 'did you mean' when one class fits | conformance/unknown_type_in_shape, conformance/unknown_type_suggestion | covered |
| The nearest `type`, `union` or `enum` wins | tests/nearest_type | covered |
| A declared type is named through its owner from outside | tests/nested_type_from_outside | covered |
| Two types that read the same are printed through their owners in an error | conformance/same_named_types | covered |
| A generic constructor takes the walk too | tests/namespaced_generics | covered |
| A generic constructor that cannot be made is reported once | conformance/failed_constructor | covered |
| A later root reopens a class: a later `func` or `var` replaces, a new one is added | tests/reopening, tests/reopen_library | covered |
| A reopened `var` must keep its type | conformance/reopened_var_type | added (compiler fails: compiles) |
| An `enum` declared again replaces the earlier one whole | tests/enum_reopening, conformance/enum_reopening_replaces | covered |
| A package calls a function the program declares, with a default behind `Build.functions[...]` | none | missing |
| A field the program's `build.spite` declares is not replaced by a package's | tests/build_precedence | covered |
| The standard library is discovered first, so a program reopens `Console`, `String`, `File` | tests/reopen_library | covered |
| `Environment` is reopened by the program's `environment.spite` | tests/environment_settings, tests/kebab_setting | covered |
| Reopening `Spite.Class` changes every class object | none | missing |
| The entry class is never reopened | conformance/entry_class_reopened | covered |
| A program file reopening a loaded class: where both declare a name, the loaded folder's stays | tests/reopening | covered |
| A program's `spite/` folder may only reopen `Spite` classes | conformance/new_spite_class | covered |
| `load "spite"` is reserved | conformance/reserved_spite_namespace | covered |
| `Spite.Internal` is refused outside the library | conformance/internal_use, conformance/internal_class, conformance/internal_type, conformance/internal_load | covered |
| Each operating system's folder reopens what differs; only one is loaded | tests/system_functions, tests/process_paths | superseded by D776 |
| `load` is a keyword: `load("x")` is the parse error naming the form | conformance/load_parentheses | covered |
| `load` used as a value is the error | conformance/load_as_value | covered |
| `load` cannot name a function | conformance/load_function | covered |
| `load` cannot name a variable | conformance/load_variable_name | added (compiler fails: a parse error 'expected the name of a variable') |
| `load` cannot name a parameter | conformance/load_parameter_name | added (compiler fails: a parse error 'a parameter is declared name: Type') |
| `load` takes literal text; a computed folder is the error | conformance/load_computed_folder | added (compiler fails: worded 'a load names its folder as plain text ...') |
| An absolute load path is refused, naming the relative path to write | conformance/load_absolute_path | added |
| A `load` under an `if` folds on Build fields; only the taken branch loads | tests/load_on_build | covered |
| A condition it cannot fold is the error | conformance/load_unknown_condition | covered |
| A `load` inside a `while` or a `switch` is an error | conformance/load_in_while | added |
| `class.source_files` and a namespace's `source_directories` fold to the files in load order | tests/reflection_sources, tests/source_folder, tests/class_walk | covered (compiler fails: class_walk does not compile) |
| A git load with no commit is "... names a repository without its commit ..." | conformance/load_git_without_commit | added (compiler fails: reads it as a missing folder) |
| A pin that is not 7 to 40 hexadecimal digits is "... which is not a commit ..." | conformance/load_git_branch_pin | added (compiler fails: reports the folder name is not snake_case) |
| A folder after the commit that leaves the repository is the error | conformance/load_git_outside_repository | added (compiler fails: reads it as a missing folder) |
| A repository that is neither a URL nor a path is "... names no repository ..." | conformance/load_git_no_repository | added (compiler fails: reports the folder name is not snake_case) |
| Fetching, the `.files` check, read-only copies, pins inside a package, two versions of one repository | none | missing |
| A bodiless `func` where the compiler supplies nothing is "give it a body" | conformance/bodiless_function | added (compiler fails: compiles and then reports a fault of the compiler) |

## concurrency.md

Rules: 85. Covered before: 53. Covered now: 64.

| Rule | Program(s) | Status |
|---|---|---|
| `Concurrent(function)` runs on the program's thread, starts at once; reading waits, drop waits | tests/concurrent_waits, tests/state_machines | covered |
| `Parallel(function)` runs on the pool with the same reading and join on drop | tests/parallel_work | covered |
| The handle's type is worked out; writing it on the starting `var` is the `written_handle_type` error | conformance/written_handle_type | covered |
| The handle type is still written for a list's element type or a parameter | tests/default_handles, tests/debug_handles, tests/kept_templates | covered |
| `Concurrent<Nothing>` is waited for by dropping; clearing or leaving a list of them waits for all | tests/waits_never_hang | covered |
| The program ends after its `Concurrent`s, before output is flushed and singletons torn down | tests/concurrent_unfinished_at_exit | covered |
| Every attribute of both classes is private; `finished` is the one public member | | missing (no error text stated) |
| A handle joins on first use and keeps the value | tests/implicit_joins | covered |
| Joins where stored or passed as `T`: typed `var`, assignment, argument, variadic `print`, `return` | tests/implicit_joins | covered |
| Joins as an element of a list literal | tests/handle_list_literal | added |
| Joins as an operand and inside text | tests/implicit_joins | covered |
| Joins as the receiver of a member the handle lacks | tests/implicit_joins | covered |
| Joins as a condition: `if`, `crash` | tests/implicit_joins | covered |
| Joins as a condition: `while`, `assert` | tests/handle_conditions | added |
| Narrowing a nullable handle narrows its value | tests/implicit_joins | covered |
| A `Boolean?` handle is refused as a condition | conformance/joined_maybe_boolean | covered |
| Stays a handle in an untyped `var`, for `append`, `finished`, `finished_value`, `class`; member templates over `List<Parallel<T>>` read handles | tests/kept_templates | covered |
| `==` on two handles compares their values | tests/handle_compare | added |
| Only a join the compiler inserted may call `_result()` | | missing (no error text stated) |
| `finished` never waits; a `Concurrent` moves only where the program waits | tests/finished_polling | covered |
| `finished_value()` is `null` while running and the value once finished | tests/finished_value_running | added |
| `finished_value()` is not a wait point for `is_resumable` | | missing |
| A handle's debug text is `running` while it runs and never waits | tests/debug_handles, tests/handle_debug_running | added (compiler fails: `console.debug` joins the handle and prints its value) |
| A handle made at its defaults is finished, joins nothing, frees nothing | tests/default_handles | covered |
| `Lock`/`ThreadSlot` made at defaults give back nothing | | superseded by D662 |
| The pool is the `ThreadPool` singleton; `size()`, `worker_index()`, queue, compiler-supplied entries | | superseded by D662 |
| A `Parallel` inside a `Parallel` cannot deadlock (a join claims an untaken job) | tests/parallel_inside_parallel | added (compiler fails: compiler crashes in `need_text_constants`, "making_room is null") |
| Endless `while true` in a `Parallel`'s work is the `endless_parallel_work` error | conformance/endless_parallel_work | covered |
| A loop that can leave is not that error | tests/parallel_loop_that_leaves | added |
| A loop in a function that waits is not that error | | missing |
| No thread for a program with no `Parallel`; about 25 allocations per `Parallel` | | superseded by D662 |
| Join on drop is kept (the queue holds the call, not the handle) | tests/parallel_work | covered |
| At exit the pool finishes what is queued | | missing |
| `Lock`, `ThreadSlot`, `ThreadLocal<T>` classes | | superseded by D662 |
| `Atomic<T>` class and its misuse errors | | superseded by D662 |
| Functions that reach a wait get a resumable copy: by name, constructor, union, `type`, function value | tests/state_machines, tests/waits_never_hang | covered |
| A singleton's constructor and its locked functions get none; a wait there blocks | tests/locked_waits | covered |
| A `--hot-reload` build starts state machines through a swappable slot | | missing (rule on repl.md) |
| A `Concurrent` holds its function only while it runs (no cycle afterwards) | | missing |
| Waits in `and`/`or`, later `if` conditions, nullable `==`, function values, constructors | tests/waits_never_hang | covered |
| A local, list or object holding a `Concurrent` waits at scope end; reassignment stores, then waits | tests/dropped_handle_waits | covered |
| A `type`'s function held as a value waits by class | tests/type_function_values | covered |
| A call inside a template waits in argument order | | missing |
| A join waiting in place for a `Concurrent` below it, or a wait cycle, halts | tests/concurrent_wait_cycle, tests/concurrent_wait_cycle_in_place | covered |
| A wait inside an expression keeps the written order | tests/wait_order, tests/wait_index_order | covered |
| The waits at the bottom: sleep, console, file and socket calls | tests/concurrent_waits, tests/socket_waits, tests/concurrent_sockets | covered |
| Outside a `Concurrent`, blocking calls get waiting wrappers; plain blocking when nothing else runs | | superseded by D662 |
| A `Concurrent` made in a `Parallel`'s work runs on that thread's loop | tests/concurrent_in_parallel | covered |
| Waiting forever on nothing is a deadlock and a crash | | missing |
| `Scheduler.run_ready()` and `resume_only_when_asked()` | | superseded by D662 |
| A million polls of `finished` with nothing stepped halts | tests/poll_without_step_halts | added (compiler fails: no poll count, the loop spins until an Integer overflows) |
| A `while` polling `finished` that calls nothing is the `unstepped_poll` error | conformance/unstepped_poll | covered |
| A program with threads uses atomic reference counts | tests/threaded_class_objects, tests/cycles_across_threads | covered |
| A singleton first used from two threads is made once | tests/singleton_race | covered |
| `parallel_each_`: every element, split; fewer than two on the caller; `filter_` fuses | tests/parallel_each | covered |
| `parallel_each_` after a `map_` step is an error | conformance/parallel_each_after_map | added (compiler fails: it compiles) |
| The race rule: an attribute holding an object is an error | conformance/parallel_reach | covered |
| The race rule follows the functions the member calls | conformance/parallel_each_reach_through_call | added |
| `parallel_each_` on a list of numbers is an error naming `each_` | conformance/parallel_each_numbers | added (compiler fails: it compiles) |
| `parallel_each_` on a list of a `type` or union is an error | | missing (no error text stated) |
| A reached singleton gets the cheapest safe form: nothing, immutable, atomics, lock | tests/singleton_forms, tests/singleton_guard, tests/lone_atomic_counter | covered |
| A locked singleton's function that touches no state runs unlocked | tests/singleton_stateless_calls | covered |
| Reads and writes of its attributes from other classes take the lock | tests/singleton_lock_calls, tests/singleton_attribute_reads | covered |
| The second walk drops locks no thread names | tests/singleton_counts, tests/plain_counts | covered |
| An endless loop in a locked singleton function is the `endless_locked_loop` error | conformance/endless_locked_loop | covered |
| A locked function waiting for work that calls back into it is the `locked_wait` error | conformance/locked_wait | covered |
| A counted loop of calls to one locked singleton takes the lock once | tests/coarse_locks | covered |
| Reading functions share the lock | tests/singleton_reads | covered |
| While no task is in flight the lock is skipped | tests/unshared_locks | covered |
| A `Weak` a `Parallel` reaches is an error | | missing (rule on memory.md) |
| `Parallel(function)` follows the race rule (`parallel_function_reach`) | conformance/parallel_function_reach | covered |
| A task may keep an object handed over to it | tests/parallel_handover | covered |
| The maker used after the handover is an error | conformance/parallel_handover | covered |
| A handed-over object whose value attribute the task writes is not used again | conformance/parallel_shared_writes | covered |
| A task writing a value attribute of an object not handed over is an error | conformance/parallel_shared_value_writes | covered |
| A write inside `Lock.while_locked` is not counted | | superseded by D662 |
| Reads in a row overlap | tests/overlapped_reads | covered |
| Independent calls in a row run at once | tests/overlapped_calls | covered |
| Objects made for their owner count apart | tests/owned_objects, tests/owned_conditions | covered |
| A loop over a list of different classes runs them at once | tests/row_of_classes | covered |
| A loop whose passes write only their own item runs in bands | tests/band_passes | covered |
| A frame loop's waiting calls are started; at most eight in flight per site | tests/frame_waits | covered |
| A crash inside a started call halts with its own report | | missing |
| The "answered false before every function was compiled" error for a frame loop | | missing |
| A call kept in place is listed under `Waits that hold the frame` | tests/frame_waits_kept | covered |

## foreign_libraries.md

Rules: 58. Covered before: 27. Covered now: 46.

| Rule | Program(s) | Status |
|---|---|---|
| `DynamicLibrary` calls exported functions under the three naming rules | tests/foreign_library | covered (compiler fails: the program exits 1) |
| Constants are read from the header | tests/foreign_library | covered (compiler fails: the program exits 1) |
| Structs and lists of numbers cross, and C's writes come back | tests/foreign_library | covered (compiler fails: the program exits 1) |
| A missing library stops the program naming the file | tests/foreign_missing_library | added (compiler fails: the message does not name the file) |
| A missing symbol stops the program at construction, naming it and the Spite function | tests/foreign_missing_symbol | added (compiler fails: the symbol is looked up at the first call, after the program ran on) |
| Only called symbols are imported, resolved once when the library opens | tests/foreign_missing_symbol | added (compiler fails: resolved at the call) |
| A C enum status is answered as a Spite enum the binding writes with `=` numbers | | missing |
| A C value the Spite enum does not list crashes at the boundary | | missing |
| A status must be used and cannot be compared with a number | | missing (no error text stated) |
| A status cannot be the condition of `crash` or `assert` | | missing (no error text stated) |
| One library per file, naming rule and header; another header is a second instance | tests/foreign_library | covered (compiler fails: the program exits 1) |
| The three arguments are literals, or the `takes three literals` error | conformance/foreign_literal_arguments | added |
| The naming rule is a symbol literal, or the `'shouting' is not a naming rule` error | conformance/foreign_naming_rule | added |
| The file is named with its extension, or the `has no extension` error | conformance/foreign_library_mistakes | covered |
| `_as_long`, `_as_double`, `_as_text` choose the return | tests/foreign_library | covered (compiler fails: the program exits 1) |
| A foreign function is called only through the holder of its `DynamicLibrary(...)` | conformance/foreign_call_through_parameter | added (compiler fails: it compiles) |
| A header that exists as a file is included as a file, otherwise as a system header | tests/foreign_library | covered (compiler fails: the program exits 1) |
| `'windows'` with a header checks a struct's size with `_Static_assert` | | missing |
| `--final-classes` writes no resolved-name comments | | missing (rule on compiler.md) |
| snake_case call is a function, snake_case read is a constant | tests/foreign_library | covered (compiler fails: the program exits 1) |
| A PascalCase read is the C type | | missing |
| A constant without a header is the `a constant comes from the library's header` error | conformance/foreign_constant_without_header | added |
| `.class`, `.attributes`, `.functions` and `DynamicLibrary`'s own members are never foreign symbols | tests/settings_and_libraries_reflected, tests/library_reflection | covered |
| `'windows'` shortens with the abbreviation table, then PascalCase | tests/foreign_library | covered (compiler fails: the program exits 1) |
| Constant reads are the C spelling lowercased | tests/foreign_library | covered (compiler fails: the program exits 1) |
| `missing_function` and `missing_attribute` with `Spite.FunctionDeclaration` and `Spite.AttributeDeclaration` parameters | | missing |
| A number-only `type` crosses as a C struct by address | tests/foreign_library | covered (compiler fails: the program exits 1) |
| `Type.size` is the C struct's `sizeof`, padding included | tests/type_size | covered |
| `.size` of a type holding text is an error | conformance/type_size_mistakes | covered |
| `.size` of a type with only functions is an error | conformance/foreign_size_of_shape | added |
| A `type` with a `String` field is refused at a call, naming the type | conformance/foreign_type_with_text | added (compiler fails: it compiles) |
| A class instance is refused at a call | conformance/foreign_call_mistakes | covered |
| A `List<String>` is refused at a call | conformance/foreign_list_of_text | added |
| With a header a call goes through the prototype; a `void` call reads as `0` | tests/foreign_library | covered (compiler fails: the program exits 1) |
| Without a header whole numbers widen to 64 bits | tests/foreign_library | covered (compiler fails: the program exits 1) |
| A handle is a plain `Long`; there is no `Pointer` type | tests/foreign_library | covered (compiler fails: the program exits 1) |
| `DynamicLibrary` is a singleton keyed by its arguments | tests/foreign_library | covered (compiler fails: the program exits 1) |
| A native fault reports `foreign=`, `library=`, `from=` | tests/native_fault_foreign | covered |
| `drop()` closes the library at exit in reverse creation order | tests/singleton_teardown | covered |
| A function value argument crosses as a pointer valid for that call | tests/foreign_callbacks | covered |
| C calling that function after the call returned stops the program | tests/foreign_callback_after_call | added |
| `ForeignCallback` with `'context_first'` and `'context_last'` | tests/foreign_callbacks | covered |
| `ForeignCallback` takes a function named directly | conformance/foreign_callback_named_directly | added |
| Any other context literal is the `'somewhere' is not where C passes` error | conformance/foreign_callback_context_literal | added |
| A callback parameter that is not a number, `Boolean` or address is an error | conformance/foreign_callback_text_parameter | added |
| A callback result that is not a number, `Boolean` or nothing is an error | conformance/foreign_callback_text_result | added |
| A function value for one call names its parameters `'argument 1'` | conformance/foreign_callback_mistakes | covered |
| An attribute of type `ForeignCallback?` is never reported unread | tests/foreign_callback_kept_attribute | added (compiler fails: "the attribute 'clicking' is never read") |
| A late call to a dropped context callback stops the program | tests/foreign_callback_context_dropped | covered |
| At most 16 777 216 context callbacks alive at once | | missing |
| `'no_context'` needs a singleton's function | conformance/foreign_callback_needs_singleton | added |
| A second `'no_context'` `ForeignCallback` for one function while the first lives stops | tests/foreign_callback_second_alive | added |
| A `'no_context'` call after the drop stops | tests/foreign_callback_dropped | covered |
| A COM-style table built with `Memory.Heap` and `TypedMemory<Long>` | | superseded by D662 |
| C may call a `ForeignCallback` from any thread | tests/foreign_callbacks | covered |
| A context callback of an object follows the race rule | conformance/foreign_callback_thread_reach | added |
| A program with no callback carries none of it | | missing |
| A C union past its first member, varargs and function-like macros are not covered | | missing (no error text stated) |

## game_maths.md

Rules: 24. Covered before: 21. Covered now: 24.

| Rule | Program(s) | Status |
|---|---|---|
| Vectors, matrices, quaternions are plain classes of numbers, tree-shaken, nothing registered | tests/game_vectors, tests/game_matrices | covered |
| Parts `x`, `y`, `z`, `w`; matrices `column_C_row_R`, column-major; `Matrix4<Float>()` is the identity | tests/game_matrices, tests/axis_names | covered |
| A vector's number class is inferred from its parts (`Vector3(1, 2, 3)` is `Vector3<Integer>`) | tests/generic_vectors | covered |
| Whole-number `length()`, `distance_to`, `normalized()` truncate | tests/generic_vectors | covered |
| Matrices and quaternions take `Float` or `Double` only; another class is the folded-crash error | conformance/whole_number_rotation | covered |
| `Matrix4<Double>` members take and answer `Double` classes | tests/double_maths | covered |
| Vector operators, `dot`, `cross`, `length`, `normalized` (zero stays zero), `distance_*`, `linear_interpolate`, `minimum`/`maximum`/`absolute`, `to_string` | tests/game_vectors, tests/generic_vectors | covered |
| `minimum`/`maximum` ignore a not-a-number part | tests/nan_passes | covered |
| `Matrix4` members (multiply order, transforms, inverse null at determinant 0, set_* functions, parts, look-at, perspective, orthographic, normal matrix, to_string) | tests/game_matrices | covered |
| `Matrix3` members | tests/game_matrices | covered |
| `Quaternion` members (axis angle, euler orders, multiply order, rotate, inverse, interpolations, to_matrix) | tests/game_matrices | covered |
| `AxisAlignedBox` members (faces inside, touching intersects, transformed) | tests/game_geometry | covered |
| `Plane` members | tests/game_geometry | covered |
| `Frustum` members | tests/game_geometry | covered |
| `Ray` members, hits answering `Float?` | tests/game_geometry | covered |
| `Color` members (`to_hex`, `to_rgb_text`, `to_hsl_text`, sRGB curve, clamped on writing) | tests/game_colors | covered |
| `Color` has one constructor, `Color(red, green, blue, alpha)`; text becomes a colour only through `to_color()`, a `Color?` | tests/color_from_text | added |
| Bytes that can be text are one holder with two reads: an HTTP body, a socket read finishing a cut character, a datagram, a process's output | tests/byte_holders | added |
| `Process.run()` answers `null` when the command cannot start; its output reads `null` before a run | tests/reads_say_nothing | added |
| `Directory.entries()` answers `null` for a folder that cannot be opened, an empty list for an empty one | tests/directory_missing | added |
| `File.read_as_string()` answers `null` for bytes that are not UTF-8 | tests/reads_say_nothing | added |
| `ColorText.read`: hex, `rgb`/`rgba`, `hsl`/`hsla`, names, anything else `null` | tests/game_colors | covered |
| `ColorText.read` trims and ignores case; `transparent`; `named_hex` answers `#rrggbb` | tests/game_named_colors | added |
| `named_hex` of an unknown name answers `""` | tests/game_named_colors | added (a default a caller cannot tell from a real answer; a D244 bug in the rule itself) |
| `CubicBezier.point_at`, `y_at_x` | tests/game_curves | covered |
| `Easing` forms; `in_back`/`out_back` overshoot | tests/game_curves, tests/game_back_easings | added (the back forms were not tested) |
| `Noise` value and gradient noise, interleaved gradient, deterministic per seed | tests/game_curves | covered |
| Half precision: rounding to even, overflow from 65520, NaN as `0x7e00`, subnormals; bit views | tests/half_precision | covered |
| Cost: answers that never leave live in the frame; `set_` functions write in place | tests/written_codegen_in_frame | covered |

## style.md

Rules: 79. Covered before: 38. Covered now: 67.

| Rule | Program(s) | Status |
|---|---|---|
| An unread local is an error: `'total' is never read: remove it` | conformance/unused_names | covered |
| Only a read counts: a local only assigned is unused | conformance/written_not_read | covered |
| An unread parameter: `... remove it, or name it '_amount' if the signature needs it` | conformance/unread_parameter | covered |
| A `_name` that is read is an error | conformance/read_underscore_name | covered |
| An unread local is an error whatever its name | conformance/unused_names | covered |
| A function nobody calls is not reported, and its reads count | tests/uncalled_function_reads | added |
| Operator function parameters are exempt | tests/operator_parameter_exempt | added |
| A template's parameters are exempt from the unused-parameter rule | none | missing |
| A one-parameter setter answering attribute access is exempt | tests/setter_parameter_exempt | added |
| A two-parameter setter follows the ordinary rule | conformance/two_parameter_setter_unread | added |
| A function replacing another through reopening is exempt | tests/reopening | covered |
| An unread attribute: `the attribute 'world' is never read: remove it` | conformance/unused_attributes, tests/attribute_uses | covered |
| A walk counts only when it reads the attribute's value | tests/attribute_uses | covered |
| A read takes the value (own class, another class, union or type, getter) | tests/attribute_uses, tests/accessors | covered |
| Writing is not reading: an attribute only assigned is unused | conformance/attribute_only_written | added |
| Source-level: a folded branch or an uncompiled generic function still reads | tests/folded_uses | covered |
| A private attribute nothing reads is an error | conformance/private_attribute_unread | added |
| An attribute kept only for its constructor's side effect is an error | conformance/constructor_side_effect_attribute | added |
| Number and String storage, Build and Environment attributes are never checked | tests/build_settings, tests/environment_settings | covered |
| A library is judged on its own code | tests/hello | covered |
| A loaded package's public attributes are not checked; an added one is | tests/package_attributes, conformance/package_attributes | covered |
| An unused singleton binding is an error everywhere, a loaded package included | conformance/loaded_singleton_unread | added (compiler fails: compiles) |
| The attribute check runs only when the program has no other error | conformance/class_test_not_member | added (compiler fails: reports an unread attribute before the program's own error) |
| No call is an argument, naming both calls | conformance/call_as_argument | covered |
| The same for a constructor's arguments | conformance/call_in_constructor_argument | added |
| Anything inside an argument counts (`count_words(text) + 1`) | conformance/call_inside_argument_expression | added |
| A method on a call's result is not an argument | tests/calls_outside_arguments | added |
| Only a name calls a function: `(a + b).length()` is an error | conformance/computed_receiver | covered |
| Reading an attribute of a computed value is not refused | none | missing |
| A hole is read like a line of its own: a call in a hole is legal | tests/calls_outside_arguments, tests/interpolation | added |
| A call inside a hole's call argument is an error | conformance/call_in_hole_argument | added |
| A call in a condition, return, assignment or index is not an argument | tests/calls_outside_arguments | added |
| A constructor call is never an argument, naming the variable to make | conformance/constructor_as_argument, conformance/long_line_construction | covered |
| A constructor inside a constructor's arguments | conformance/constructor_in_constructor | added |
| A constructor as a receiver, and `Parallel(worker.run)`, are fine | tests/json_camel_case, tests/parallel_work | covered |
| A singleton constructor passed as an argument keeps its own error | conformance/inline_singleton | covered |
| A call repeated at the start of both branches is an error | conformance/repeated_branch_call | covered |
| A call repeated in every branch of an `else if` chain: "computed in every branch" | conformance/repeated_branch_call_chain | added |
| A chain where only some branches share the call is not reported | tests/repeated_branch_partial | added |
| Four spaces, no tabs, no trailing whitespace, one newline at the end | none | missing (a formatting program rewrites itself; the runner needs to format a copy) |
| A single named codegen value in `<...>` becomes positional | none | missing |
| Every compile formats and prints `formatted <path>` | conformance/blank_line | covered (compiler fails: compiles; the file is rewritten by the run) |
| One space around binary operators, the minimum parentheses, `else if` on one line | none | missing (as above) |
| A call or literal over 120 columns is broken one entry per line | none | missing (as above) |
| An empty line in a function body is deleted; empty lines outside collapse to one | conformance/blank_line | covered (compiler fails: compiles) |
| A file the formatter refuses stops the compile with its reason | none | missing |
| The formatter's safety check: same program, every comment kept | none | missing |
| `crash false` prints as `crash` | none | missing |
| A file is ordered; out of order is an error naming what came before | conformance/declaration_order | covered |
| `union` goes beside `enum`, before `type` | conformance/union_after_type | added |
| Consecutive `var`s may stay grouped; one blank line between other declarations | none | missing |
| K&R braces; empty body as `{ }` | none | missing |
| Comments are `# text` with one space; string contents never reformatted | none | missing |
| A comment is one line, a link to a markdown section, checked to resolve | tests/comment_link, conformance/comment_broken_link, conformance/comment_missing_file | covered |
| The link resolves from the comment's folder, at any depth | tests/comment_link_depth | covered |
| A comment only outside functions | conformance/comment_inside_function | covered |
| A comment that is not a link is an error | conformance/comment_prose_comment | covered |
| `//` is rejected | conformance/comment_slash_comment | covered |
| `/* */` is rejected | conformance/comment_block | added |
| A `.spite` link only under a `.final-classes` folder | conformance/spite_link_outside_final | covered |
| A number in an expression is an error: "146097 is a magic number: ..." | conformance/magic_number | added (compiler fails: compiles; most programs of the suite would fail this rule) |
| 0, 1, -1, 0.0, 1.0 and a naming initializer are exempt | tests/magic_number_exempt | added |
| Names in snake_case; classes, types, enums, unions in PascalCase | conformance/naming, conformance/function_name_case, conformance/enum_value_name | covered |
| File names in snake_case | conformance/file_name_case | added |
| Folder names in snake_case | conformance/folder_name, conformance/load_folder_name | covered |
| One underscore between words (`hit__count`) | conformance/doubled_underscore | covered |
| A trailing underscore (`strike_`) is an error | conformance/trailing_underscore | added |
| The compiler's `___` names reserve no word | tests/generated_names | covered |
| No name is taken by C (`register`, `near`, `interface`) | tests/backend_words | covered |
| Single-letter names are errors | conformance/single_letter_name | covered |
| The axis names `x`, `y`, `z`, `w` (and `_x`) are allowed | tests/allowed_short_names, tests/axis_names | added |
| Abbreviations are errors | conformance/abbreviated_name | covered |
| The suggestion spells out every matching word of the identifier | conformance/abbreviation_two_words | added (compiler fails: suggests one word, "write 'maximum' instead of 'max'") |
| `min` and `max` are abbreviations | conformance/abbreviation_min | added |
| Plurals of abbreviations are refused | conformance/abbreviation_plural | added |
| `id` is allowed | tests/allowed_short_names | added |
| Old type spellings are errors naming the new ones | conformance/old_type_spellings | covered |
| The terminal-`if` lint | conformance/terminal_if | covered |
| A diagnostic names the file the problem is in | conformance/package_attributes, conformance/two_parameter_setter_unread | covered |

## write_it_plainly.md

Rules: 11. Covered before: 4. Covered now: 4.

| Rule | Program(s) | Status |
|---|---|---|
| A program states what it computes; how it runs is the compiler's choice | none | missing (a principle; no one program shows it) |
| An optimisation never changes what a program prints, computes or crashes on | tests/proven_ranges, tests/fused_chains | covered |
| `--optimization-report` names the place and what stopped a proof | tests/optimization_report | covered (compiler fails: the output differs) |
| Nothing runs beside a program; a feature not used is not in it | none | missing (needs a check of the generated program, not of its output) |
| A class is a unit of meaning, not of storage | none | missing (a principle) |
| A collection's generic types say what it holds, not how it is stored | tests/dictionary_folding | covered |
| A profiling build records use; a later build reads it; results never depend on it | none | missing |
| Code that reimplements a recognised library function is an error naming the library form | conformance/hand_copy_of_library | covered (compiler fails: compiles) |
| Timing a loop with the clock where `Benchmark` exists is refused | none | missing |
| A similar piece of code that is not a recognised pattern is never refused | none | missing |
| A plain program reaches the speed of the hand-written concurrent one | none | missing (a benchmark, not a program with an answer) |

## compiler.md

Rules: 51. Covered before: 22. Covered now: 35.

| Rule | Program(s) | Status |
|---|---|---|
| `spite program` builds and runs it, and exits with the program's code | tests/exit_code | covered |
| `--check` formats and compiles, writing no executable | none | missing |
| `--build` builds without running | none | missing |
| `--final-classes=folder` writes the merged classes | none | missing |
| `--optimized` is -O3 with link-time optimisation | tests/quiet_asserts_optimized, tests/trace_asserts_optimized | covered |
| `--development` is inspectable: nothing shaken, internals as ordinary objects | tests/kept_templates | covered |
| `--trace-asserts` prints each failed assert | tests/trace_asserts | covered |
| `spite compiler --verify` and `--release` (the compiler proves itself) | none | missing |
| A Boolean flag may be bare | tests/kebab_setting | covered |
| No folder named prints the usage and exits 1 | none | missing |
| A `.spite` path is "'game/game.spite' is a file, and a program is named by its folder ..." | none | missing |
| A folder without its entry is "'game' holds no program ..." | none | missing |
| An entry constructor with parameters is an error at its line | conformance/entry_constructor_arguments | covered |
| `--check` and `--build` together is the "give one of them" error | conformance/check_and_build_flags | added (compiler fails: '--build' is an unknown option) |
| `--check` never writes, moves or removes an executable; other builds remove it first | none | missing |
| `--run` is the removed-flag error | conformance/removed_run_flag | added (compiler fails: the generic unknown-option error) |
| `--executable` is the removed-flag error | conformance/removed_executable_flag | added (compiler fails: the generic unknown-option error) |
| Outputs go to `.spite/build/<path>/`, or `.spite/elsewhere/<name>_<number>/` | none | missing |
| `--executable-path` with `--check` is the error | conformance/executable_path_with_check | added (compiler fails: '--executable-path' is an unknown option) |
| A C compiler that reports success with no executable is the error | none | missing |
| `.spite`, `.spite-cache` and `.git` folders are never read as part of a program | tests/working_directory | covered |
| The optimisation report: sections, entries, order, `None.` | tests/optimization_report, tests/frame_waits_kept | covered (compiler fails: optimization_report's output differs) |
| A report that cannot be written is `error: could not write '<path>'` | none | missing |
| Translation units, the object cache, LTO flags, `-ffp-contract=off`, `-march=native` | none | missing |
| `--tune-for-this-machine` is not an option | conformance/removed_tuning_flag | added (compiler fails: the generic unknown-option error) |
| `--translation-units` is not an option | conformance/removed_translation_units_flag | added (compiler fails: the generic unknown-option error) |
| No C compiler found is the `CC` error | none | missing |
| Flags are kebab-case; an underscore is the error naming the hyphen form | conformance/underscore_flag | covered |
| A program setting with an underscore is the same error | conformance/underscore_setting | added |
| An `Environment` field named like a `Build` field is the error naming both | conformance/setting_named_like_flag | covered |
| A flag naming a `Build` field sets it as a constant | tests/build_settings, tests/build_precedence | covered |
| An `Environment` setting of the wrong type is "'--volume=loud' was given, but 'Environment.volume' is an Integer ..." | conformance/setting_value | added (compiler fails: the setting is an unknown option) |
| A bare flag is only for a Boolean setting | conformance/bare_flag_not_boolean | added (compiler fails: the setting is an unknown option) |
| A flag naming no field is the error naming both places | conformance/unknown_compiler_flag | covered |
| A value that does not fit a Build field names the forms it takes | conformance/flag_value, conformance/build_setting_type | covered |
| `--repl-port` outside 1 to 65535 is the error | conformance/repl_port_range | added (compiler fails: compiles) |
| `--target-operating-system` names windows, linux or mac | conformance/target_operating_system | covered |
| `target_operating_system` in `build.spite` is the error naming the flag | conformance/target_in_build | covered |
| `--operating-system` cannot be given | conformance/operating_system_flag | added (compiler fails: compiles) |
| `--program` cannot be given | conformance/program_flag | added (compiler fails: compiles) |
| The program receives its settings and every argument that is not a flag, in order | tests/program_arguments, tests/launcher_arguments | covered |
| A bare `--` is not a separator | conformance/separator_flag | added (compiler fails: the generic unknown-option error) |
| A flag or setting given twice is the error | conformance/flag_given_twice | covered |
| Inspectable builds keep every function and make stateless singletons objects; production builds shake | tests/kept_templates | covered |
| The C compiler's messages are dropped when it succeeds | tests/hello | covered |
| The program runs in the folder `spite` was run from | tests/working_directory | covered |
| The launcher passes arguments untouched (no path rewriting) | none | missing |
| At most 25 errors in the program, then `and N more`, then 25 for packages | none | missing |
| Every compile formats first, printing `formatted <path>`; a refused file stops the compile | none | missing |
| `--format` is not an option | conformance/format_flag | covered |
| `var format` in `build.spite` is the error | conformance/format_setting | covered |

## repl.md

Rules: 47. Covered before: 19. Covered now: 25.

| Rule | Program(s) | Status |
|---|---|---|
| A reload moves live objects to a class's new attributes; renames come from the prompt's map | none | missing |
| The map literal of `reload {...}` and its refusals | none | missing |
| A reload pauses every task; a task that reaches no check point in 10 seconds refuses it | none | missing |
| An attribute gone while another is new holds the reload until a map is given | none | missing |
| A class that stops or starts fitting an `Items`' own memory moves its items | none | superseded by D1707 (one List) |
| Dependents are rebuilt; a helper with static state refuses the reload by name | none | missing |
| Enum values keep their numbers across reloads; a removed value halts a `switch` with no `_:` | none | missing |
| `environment.spite` and `build.spite` reload; settings are read again from the start's command line | none | missing |
| Reflection in a REPL build links a `Spite.Attribute` to its live value | tests/interactive_paths | covered |
| Outside a REPL build those functions answer empty, false and null | none | missing |
| A function is callable from the prompt when every parameter is a number, Boolean, text or enum | tests/interactive_paths | covered |
| Paths from `program`, with or without the leading `program.`; through a `T?`; a dictionary by key | tests/interactive_paths, tests/interactive_inspection | covered |
| Calls with literal arguments; a call that answers a value is walked on | tests/interactive_paths, tests/interactive_singletons | covered |
| `count()`, `keys()` (or `scores holds no keys`) and `has(key)` on collections | tests/interactive_refusals | added (compiler fails: `has` is an unknown function) |
| A path after a call answering nothing is refused after the call ran | tests/interactive_singletons, tests/interactive_refusals | covered |
| Singletons by name bind the one instance, never make one; the refusals for a class, an unknown name, arguments | tests/interactive_singletons | covered |
| A singleton not made yet is "Tick has not been made yet ..." | none | missing |
| A singleton made with arguments is named with them; an argument list never made has none | none | missing |
| A name two singletons share is refused naming both | none | missing |
| `singletons()` and `class_names()` are written only into a REPL build | tests/interactive_singletons | covered |
| Breakpoints answer only with --hot-reload and --repl-port; the refusal elsewhere | none | missing |
| `where`, `locals`, `continue` when not stopped answer the breakpoint hint | tests/interactive_refusals | added (compiler fails: read as unknown attributes) |
| `bytes <path>` answers JSON of an object's memory; `bytes 0x.. n` reads a range, never crashing | tests/interactive_singletons | covered |
| Generic functions beyond the program's instances need `eval` | none | missing |
| Assignment of a literal goes through `set_<attribute>` and answers the value read back | tests/interactive_paths | covered |
| Text literals at the prompt read escapes as a program does | tests/interactive_refusals | added |
| A text literal with a hole is refused | tests/interactive_refusals | added |
| `attributes` and `functions`, also of any path; the empty-list answers | tests/interactive_paths, tests/interactive_singletons, tests/interactive_loop | covered |
| A private `_` function is neither listed nor called | tests/interactive_refusals | added |
| `drop()` is never run from the prompt | tests/interactive_singletons | covered |
| `classes` lists singletons (`not made yet`) then the other classes | tests/interactive_singletons | covered |
| Printing: one level deep, collections as `List<T>(n)`, `T?` at the same depth, unions through the member, text unquoted | tests/interactive_inspection, tests/interactive_paths | covered |
| Errors are one line: unknown attribute, index out of range, type mismatch, not callable, unknown function | tests/interactive_paths | covered |
| `eval` and `run` answer only with --hot-reload and --repl-port | none | missing |
| `describe <class>`, its refusals | tests/interactive_inspection | covered |
| `enums`, or `the program declares no enum` | tests/interactive_inspection, tests/interactive_refusals | covered |
| `memory` answers live allocations and bytes | none | superseded by D662 |
| Walking into a union as its active member; `held_class()` | tests/interactive_inspection | covered |
| `null` only into a `T?`; a path's instance only of the attribute's class | tests/interactive_inspection | covered |
| Constructing an object at the prompt answers only with --hot-reload and --repl-port | none | missing |
| `--repl` runs the constructor, then reads commands until `exit` or end of input, memory balanced | tests/interactive_loop | covered |
| `--repl-port` serves one-line JSON on 127.0.0.1 only; the port is part of the build | none | missing |
| Every loop is a check point in a --repl-port or --hot-reload build, and only there | none | missing |
| A port another program holds is `error: the REPL could not listen on 127.0.0.1:<port>` | none | missing |
| `exit` over the port answers, closes and ends at the next wait | none | missing |
| `spite connect <port>` and its `nothing is listening` error | none | missing |
| `reload` outside a --hot-reload build is "'reload' answers in a program built with --hot-reload ..." | tests/interactive_refusals | added (compiler fails: read as an unknown attribute) |
| Live reload: slots, the watcher, `wait_reload`, waits held, what reloads and what is refused, load order | none | missing |

