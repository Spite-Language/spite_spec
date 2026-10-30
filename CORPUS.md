# Corpus

Every program of the first repository (`spite_language`, branch `next`): its `conformance/<stage>/<name>/` programs,
its `diagnostics/<name>/` programs, its `examples/` and the titled code blocks of its `docs/` and `specs/` pages (a
folder of blocks is one program), and where SpiteSpec tests the same behaviour.

- **covered**: a SpiteSpec program already tests it (the path given);
- **ported**: it was missing, and now lives at the path given, rewritten to today's language where a decision
  changed it (the note names the decision);
- **not applicable**: its point is something a later decision removed (named), such as the classes D662 hides.

A ported program that the compiler gets wrong says why after "fails:"; each of those is a compiler or library gap,
listed again under [Gaps](#gaps).

Old programs: 975. Covered: 801. Ported: 99. Not applicable:
75. Ported programs that fail: 2.

## Gaps

Ported programs the compiler (`spite_all2`, 2026-10-24) gets wrong, one line each.

- `tests/pinned_widgets`: a load pinned to a commit (`load "../..@<commit>/..."`) is refused: fetching repositories is not built.
- `tests/recooking`: the library has no `Reload` singleton (`reload` of a `--hot-reload` build).

## Programs

| Old program | Status | SpiteSpec | Note |
|---|---|---|---|
| conformance/stage1/arithmetic | covered | tests/arithmetic |  |
| conformance/stage1/assert_default | covered | tests/assert_default |  |
| conformance/stage1/byte_order_mark | covered | tests/byte_order_mark |  |
| conformance/stage1/control_flow | covered | tests/control_flow |  |
| conformance/stage1/functions | covered | tests/functions |  |
| conformance/stage1/hello | covered | tests/hello |  |
| conformance/stage1/strings | covered | tests/strings |  |
| conformance/stage2/constructor_params | covered | tests/constructor_params |  |
| conformance/stage2/copy_semantics | covered | tests/copy_semantics |  |
| conformance/stage2/drop_hook | covered | tests/drop_hook |  |
| conformance/stage2/multiple_classes | covered | tests/multiple_classes |  |
| conformance/stage2/namespaces_load | covered | tests/namespaces_load |  |
| conformance/stage2/nullable_narrowing | covered | tests/nullable_narrowing |  |
| conformance/stage2/references_basic | covered | tests/references_basic |  |
| conformance/stage2/temporaries | covered | tests/temporaries |  |
| conformance/stage3/dictionaries | covered | tests/dictionaries |  |
| conformance/stage3/enums | covered | tests/enums |  |
| conformance/stage3/interpolation | covered | tests/interpolation |  |
| conformance/stage3/lists | covered | tests/lists |  |
| conformance/stage3/string_methods | covered | tests/string_methods |  |
| conformance/stage3/text_escapes | covered | tests/text_escapes |  |
| conformance/stage3/unions | covered | tests/unions |  |
| conformance/stage4/directory_entries | covered | tests/directory_entries |  |
| conformance/stage4/process_paths | covered | tests/process_paths |  |
| conformance/stage4/process_settings | covered | tests/process_settings |  |
| conformance/stage4/system | covered | tests/system |  |
| conformance/stage5/crash | covered | tests/crash |  |
| conformance/stage5/crash_receiver | covered | tests/crash_receiver |  |
| conformance/stage5/crash_report | covered | tests/crash_report |  |
| conformance/stage5/library_guards_untraced | covered | tests/library_guards_untraced |  |
| conformance/stage6/accessors | covered | tests/accessors |  |
| conformance/stage6/allocator_choice | not applicable (D662) | - | arenas and x.memory.allocator are hidden |
| conformance/stage6/allocator_reads | not applicable (D662) | retired/allocator_reads | kept in retired/allocator_reads (D665) |
| conformance/stage6/and_in_expression | ported | tests/and_in_expression |  |
| conformance/stage6/and_narrowing | covered | tests/and_narrowing |  |
| conformance/stage6/any_keys | covered | tests/any_keys |  |
| conformance/stage6/argument_class | covered | tests/argument_class |  |
| conformance/stage6/assert_maybe_boolean | covered | tests/assert_maybe_boolean |  |
| conformance/stage6/assert_ring_threads | covered | tests/assert_ring_threads |  |
| conformance/stage6/assert_runs | covered | tests/assert_runs |  |
| conformance/stage6/atomic_values | not applicable (D662) | retired/atomic_values | kept in retired/atomic_values (D665) |
| conformance/stage6/attribute_object | covered | tests/attribute_object |  |
| conformance/stage6/attribute_uses | covered | tests/attribute_uses |  |
| conformance/stage6/axis_names | covered | tests/axis_names |  |
| conformance/stage6/backend_words | covered | tests/backend_words |  |
| conformance/stage6/band_passes | covered | tests/band_passes |  |
| conformance/stage6/binary_files | covered | tests/binary_files |  |
| conformance/stage6/binary_schema | covered | tests/binary_schema |  |
| conformance/stage6/bitwise_functions | covered | tests/bitwise_functions |  |
| conformance/stage6/bound_proofs | covered | tests/bound_proofs |  |
| conformance/stage6/build_precedence | covered | tests/build_precedence |  |
| conformance/stage6/build_settings | covered | tests/build_settings |  |
| conformance/stage6/bulk_removal | covered | tests/bulk_removal |  |
| conformance/stage6/calendar_math | covered | tests/calendar_math |  |
| conformance/stage6/call_function_arguments | covered | tests/call_function_arguments |  |
| conformance/stage6/chain_comparison | covered | tests/chain_comparison |  |
| conformance/stage6/class_argument | covered | tests/class_argument |  |
| conformance/stage6/class_attributes | covered | tests/class_attributes |  |
| conformance/stage6/class_pools | covered | tests/class_pools |  |
| conformance/stage6/class_test | covered | tests/class_test |  |
| conformance/stage6/class_text | covered | tests/class_text |  |
| conformance/stage6/class_through_shape | covered | tests/class_through_shape |  |
| conformance/stage6/class_walk | covered | tests/class_walk |  |
| conformance/stage6/clock_reading | covered | tests/clock_reading |  |
| conformance/stage6/clock_reads | covered | tests/clock_reads |  |
| conformance/stage6/coarse_locks | covered | tests/coarse_locks |  |
| conformance/stage6/codegen_class_name | covered | tests/codegen_class_name |  |
| conformance/stage6/codegen_class_test | covered | tests/codegen_class_test |  |
| conformance/stage6/codegen_defaults | covered | tests/codegen_defaults |  |
| conformance/stage6/codegen_member_fold | covered | tests/codegen_member_fold |  |
| conformance/stage6/comment_link | covered | tests/comment_link |  |
| conformance/stage6/comment_link_depth | covered | tests/comment_link_depth |  |
| conformance/stage6/concurrent_in_parallel | covered | tests/concurrent_in_parallel |  |
| conformance/stage6/concurrent_sockets | covered | tests/concurrent_sockets |  |
| conformance/stage6/concurrent_unfinished_at_exit | not applicable (D1715) | retired/concurrent_unfinished_at_exit | kept in retired/concurrent_unfinished_at_exit |
| conformance/stage6/concurrent_wait_cycle | not applicable (D1715) | retired/concurrent_wait_cycle | kept in retired/concurrent_wait_cycle |
| conformance/stage6/concurrent_wait_cycle_in_place | not applicable (D1715) | retired/concurrent_wait_cycle_in_place | kept in retired/concurrent_wait_cycle_in_place |
| conformance/stage6/concurrent_waits | not applicable (D1715) | retired/concurrent_waits | kept in retired/concurrent_waits |
| conformance/stage6/configured_tables | covered | tests/configured_tables |  |
| conformance/stage6/console_class | covered | tests/console_class |  |
| conformance/stage6/console_error | covered | tests/console_error |  |
| conformance/stage6/console_input | covered | tests/console_input |  |
| conformance/stage6/count_bound_proofs | covered | tests/count_bound_proofs |  |
| conformance/stage6/count_named_function | covered | tests/count_named_function |  |
| conformance/stage6/counted_loops | covered | tests/counted_loops |  |
| conformance/stage6/counter_room | covered | tests/counter_room |  |
| conformance/stage6/crash_compound | covered | tests/crash_compound |  |
| conformance/stage6/crash_maybe_boolean | covered | tests/crash_maybe_boolean |  |
| conformance/stage6/crash_missing_item | covered | tests/crash_missing_item |  |
| conformance/stage6/crash_missing_key | covered | tests/crash_missing_key |  |
| conformance/stage6/crash_missing_link | covered | tests/crash_missing_link |  |
| conformance/stage6/crash_missing_value | covered | tests/crash_missing_value |  |
| conformance/stage6/crash_scope_values | covered | tests/crash_scope_values |  |
| conformance/stage6/datagrams | covered | tests/datagrams |  |
| conformance/stage6/daylight_saving | covered | tests/daylight_saving |  |
| conformance/stage6/debug_dictionary_keys | covered | tests/debug_dictionary_keys |  |
| conformance/stage6/debug_function_attributes | covered | tests/debug_function_attributes |  |
| conformance/stage6/debug_handles | covered | tests/debug_handles |  |
| conformance/stage6/debug_threads | covered | tests/debug_threads |  |
| conformance/stage6/debug_values | covered | tests/debug_values |  |
| conformance/stage6/decimal_narrowing | covered | tests/decimal_narrowing |  |
| conformance/stage6/deep_copies | covered | tests/deep_copies |  |
| conformance/stage6/deep_copy_graphs | covered | tests/deep_copy_graphs |  |
| conformance/stage6/deep_copy_unions | covered | tests/deep_copy_unions |  |
| conformance/stage6/default_handles | not applicable (D1715) | retired/default_handles | kept in retired/default_handles |
| conformance/stage6/development_internals | not applicable (D662) | - | Memory.Heap, TypedMemory and Build.instances are hidden |
| conformance/stage6/dictionary_folding | covered | tests/dictionary_folding |  |
| conformance/stage6/dictionary_read_kept | covered | tests/dictionary_read_kept |  |
| conformance/stage6/dictionary_templates | covered | tests/dictionary_templates |  |
| conformance/stage6/direct_text | covered | tests/direct_text |  |
| conformance/stage6/discarded_results | covered | tests/discarded_results |  |
| conformance/stage6/division_by_zero | covered | tests/division_by_zero |  |
| conformance/stage6/double_maths | covered | tests/double_maths |  |
| conformance/stage6/dropped_handle_waits | not applicable (D1715) | retired/dropped_handle_waits | kept in retired/dropped_handle_waits |
| conformance/stage6/empty_folder_range | covered | tests/empty_folder_range |  |
| conformance/stage6/empty_plural | covered | tests/empty_plural |  |
| conformance/stage6/encodings | covered | tests/encodings |  |
| conformance/stage6/entry_function_waits | covered | tests/entry_function_waits |  |
| conformance/stage6/enum_key_json | covered | tests/enum_key_json |  |
| conformance/stage6/enum_reopening | covered | tests/enum_reopening |  |
| conformance/stage6/enum_to_integer | covered | tests/enum_to_integer |  |
| conformance/stage6/enum_value_templates | covered | tests/enum_value_templates |  |
| conformance/stage6/environment_settings | covered | tests/environment_settings |  |
| conformance/stage6/every_attribute | covered | tests/every_attribute |  |
| conformance/stage6/every_class | covered | tests/every_class |  |
| conformance/stage6/file_watching | covered | tests/file_watching |  |
| conformance/stage6/finished_polling | not applicable (D1715) | retired/finished_polling | kept in retired/finished_polling |
| conformance/stage6/finished_values | not applicable (D662) | retired/finished_values | kept in retired/finished_values (D665) |
| conformance/stage6/finished_workers | not applicable (D662) | retired/finished_workers | kept in retired/finished_workers (D665) |
| conformance/stage6/fixed_offsets | covered | tests/fixed_offsets |  |
| conformance/stage6/float_precision | covered | tests/float_precision |  |
| conformance/stage6/folded_argument_count | covered | tests/folded_argument_count |  |
| conformance/stage6/folded_checks | covered | tests/folded_checks |  |
| conformance/stage6/folded_crash_uncalled | covered | tests/folded_crash_uncalled |  |
| conformance/stage6/folded_function_value | covered | tests/folded_function_value |  |
| conformance/stage6/folded_functions | covered | tests/folded_functions |  |
| conformance/stage6/folded_helpers | covered | tests/folded_helpers |  |
| conformance/stage6/folded_lends | covered | tests/folded_lends |  |
| conformance/stage6/folded_uses | covered | tests/folded_uses |  |
| conformance/stage6/folder_class_namespace | covered | tests/folder_class_namespace |  |
| conformance/stage6/folder_namesake | covered | tests/folder_namesake |  |
| conformance/stage6/foreign_callback_context_dropped | covered | tests/foreign_callback_context_dropped |  |
| conformance/stage6/foreign_callback_dropped | covered | tests/foreign_callback_dropped |  |
| conformance/stage6/foreign_callbacks | covered | tests/foreign_callbacks |  |
| conformance/stage6/foreign_library | covered | tests/foreign_library |  |
| conformance/stage6/frame_held_attributes | not applicable (D662) | - | its point is where objects live (heap counts, x.memory.section) |
| conformance/stage6/frame_objects | ported | tests/frame_objects | D662/D665: heap counts and x.memory.section lines dropped, the rest kept |
| conformance/stage6/frame_waits | covered | tests/frame_waits |  |
| conformance/stage6/frame_waits_kept | covered | tests/frame_waits_kept |  |
| conformance/stage6/frames_between_waits | not applicable (D662) | retired/frames_between_waits | kept in retired/frames_between_waits (D665) |
| conformance/stage6/function_accesses | covered | tests/function_accesses |  |
| conformance/stage6/function_value_in_frame | covered | tests/function_value_in_frame |  |
| conformance/stage6/function_values | covered | tests/function_values |  |
| conformance/stage6/fused_chain_allocations | covered | tests/fused_chain_allocations |  |
| conformance/stage6/fused_chains | covered | tests/fused_chains |  |
| conformance/stage6/game_colors | covered | tests/game_colors |  |
| conformance/stage6/game_curves | covered | tests/game_curves |  |
| conformance/stage6/game_geometry | covered | tests/game_geometry |  |
| conformance/stage6/game_matrices | covered | tests/game_matrices |  |
| conformance/stage6/game_vectors | covered | tests/game_vectors |  |
| conformance/stage6/generated_names | covered | tests/generated_names |  |
| conformance/stage6/generic_class_test | covered | tests/generic_class_test |  |
| conformance/stage6/generic_constraints | covered | tests/generic_constraints |  |
| conformance/stage6/generic_singletons | covered | tests/generic_singletons |  |
| conformance/stage6/generic_vectors | covered | tests/generic_vectors |  |
| conformance/stage6/generics | covered | tests/generics |  |
| conformance/stage6/half_precision | covered | tests/half_precision |  |
| conformance/stage6/hashes | covered | tests/hashes |  |
| conformance/stage6/held_arguments | covered | tests/held_arguments |  |
| conformance/stage6/held_items | covered | tests/held_items |  |
| conformance/stage6/held_paths | covered | tests/held_paths |  |
| conformance/stage6/held_value_functions | covered | tests/held_value_functions |  |
| conformance/stage6/http_exchange | covered | tests/http_exchange |  |
| conformance/stage6/http_keep_alive | covered | tests/http_keep_alive |  |
| conformance/stage6/ignored_lent_argument | covered | tests/ignored_lent_argument |  |
| conformance/stage6/implicit_joins | covered | tests/implicit_joins |  |
| conformance/stage6/index_of | covered | tests/index_of |  |
| conformance/stage6/indexable_class | covered | tests/indexable_class |  |
| conformance/stage6/inflection_fold | covered | tests/inflection_fold |  |
| conformance/stage6/inflection_table | covered | tests/inflection_table |  |
| conformance/stage6/instant_arithmetic | covered | tests/instant_arithmetic |  |
| conformance/stage6/integer_overflow | covered | tests/integer_overflow |  |
| conformance/stage6/interactive_inspection | covered | tests/interactive_inspection |  |
| conformance/stage6/interactive_loop | covered | tests/interactive_loop |  |
| conformance/stage6/interactive_nullable_results | covered | tests/interactive_nullable_results |  |
| conformance/stage6/interactive_paths | covered | tests/interactive_paths |  |
| conformance/stage6/interactive_singletons | covered | tests/interactive_singletons |  |
| conformance/stage6/interactive_templates | covered | tests/interactive_templates |  |
| conformance/stage6/ipv6_addresses | covered | tests/ipv6_addresses |  |
| conformance/stage6/items_at_once | covered | tests/list_at_once |  |
| conformance/stage6/items_columns | covered | tests/list_columns |  |
| conformance/stage6/items_narrowed | covered | tests/list_narrowed |  |
| conformance/stage6/items_remove_out_of_range | removed (D1707) | - |  |
| conformance/stage6/items_remove_swapping_out_of_range | removed (D1707) | - |  |
| conformance/stage6/json_camel_case | covered | tests/json_camel_case |  |
| conformance/stage6/json_crash | removed (D1707) | - | read_or_crash went (#23) |
| conformance/stage6/json_infinity | covered | tests/json_infinity |  |
| conformance/stage6/json_symbols | covered | tests/json_symbols |  |
| conformance/stage6/kebab_setting | covered | tests/kebab_setting |  |
| conformance/stage6/kept_buffer_address | not applicable (D662) | - | Memory.Address and TypedMemory are hidden |
| conformance/stage6/kept_templates | covered | tests/kept_templates |  |
| conformance/stage6/late_shape_caller | covered | tests/late_shape_caller |  |
| conformance/stage6/launcher_arguments | covered | tests/launcher_arguments |  |
| conformance/stage6/leaving_if | covered | tests/leaving_if |  |
| conformance/stage6/lent_arguments | covered | tests/lent_arguments |  |
| conformance/stage6/lent_buffers | not applicable (D662) | - | Memory.Heap and Memory.Address are hidden |
| conformance/stage6/lent_list_elements | covered | tests/lent_list_elements |  |
| conformance/stage6/lent_list_elements_parallel | covered | tests/lent_list_elements_parallel |  |
| conformance/stage6/lent_results | covered | tests/lent_results |  |
| conformance/stage6/lent_to_calls | covered | tests/lent_to_calls |  |
| conformance/stage6/library_functions_passed | covered | tests/library_functions_passed |  |
| conformance/stage6/library_reflection | covered | tests/library_reflection |  |
| conformance/stage6/list_base_counter | ported | tests/list_base_counter | |
| conformance/stage6/list_byte_numbers | ported | tests/list_byte_numbers | D662/D665: the ForeignBytes over a heap block part dropped |
| conformance/stage6/listed_values | covered | tests/listed_values |  |
| conformance/stage6/live_enum | covered | tests/live_enum |  |
| conformance/stage6/live_instances | not applicable (D662) | - | Monster.instances is gone |
| conformance/stage6/live_load_order | covered | tests/live_load_order |  |
| conformance/stage6/live_settings | covered | tests/live_settings |  |
| conformance/stage6/load_on_build | covered | tests/load_on_build |  |
| conformance/stage6/locked_bare_return | covered | tests/locked_bare_return |  |
| conformance/stage6/locked_waits | covered | tests/locked_waits |  |
| conformance/stage6/lone_atomic_counter | covered | tests/lone_atomic_counter |  |
| conformance/stage6/loop_after_assert | ported | tests/loop_after_assert |  |
| conformance/stage6/loop_reads_changed_attribute | ported | tests/loop_reads_changed_attribute |  |
| conformance/stage6/mapped_files | ported | tests/file_as_buffer | MappedFile went (#23); its reads are a Buffer |
| conformance/stage6/maths_folding | covered | tests/maths_folding |  |
| conformance/stage6/maths_functions | covered | tests/maths_functions |  |
| conformance/stage6/maths_precision | covered | tests/maths_precision |  |
| conformance/stage6/member_templates | covered | tests/member_templates |  |
| conformance/stage6/memory_floor | not applicable (D662) | - | Memory.Heap and TypedMemory are hidden |
| conformance/stage6/memory_inspector | not applicable (D662) | - | Memory.Inspector and x.memory are hidden |
| conformance/stage6/mirrored_write_backs | covered | tests/mirrored_write_backs |  |
| conformance/stage6/moving_files | covered | tests/moving_files |  |
| conformance/stage6/must_write_scratch | covered | tests/must_write_scratch |  |
| conformance/stage6/namespace_enums | covered | tests/namespace_enums |  |
| conformance/stage6/namespace_objects | covered | tests/namespace_objects |  |
| conformance/stage6/namespaced_generics | covered | tests/namespaced_generics |  |
| conformance/stage6/namespaced_library_names | ported | tests/namespaced_library_names | D662: the Memory.Arena line dropped; user Function and Arena classes kept |
| conformance/stage6/nan_passes | covered | tests/nan_passes |  |
| conformance/stage6/narrowed_lends | covered | tests/narrowed_lends |  |
| conformance/stage6/narrowed_value_assignment | covered | tests/narrowed_value_assignment |  |
| conformance/stage6/narrowing_overflow | covered | tests/narrowing_overflow |  |
| conformance/stage6/native_fault_foreign | covered | tests/native_fault_foreign |  |
| conformance/stage6/native_fault_heap | covered | tests/native_fault_heap |  |
| conformance/stage6/native_fault_illegal | covered | tests/native_fault_illegal |  |
| conformance/stage6/native_fault_stack | covered | tests/native_fault_stack |  |
| conformance/stage6/nearest_type | covered | tests/nearest_type |  |
| conformance/stage6/negative_shift | covered | tests/negative_shift |  |
| conformance/stage6/nested_lists | covered | tests/nested_lists |  |
| conformance/stage6/nested_type_from_outside | covered | tests/nested_type_from_outside |  |
| conformance/stage6/nullable_values | covered | tests/nullable_values |  |
| conformance/stage6/number_keys | covered | tests/number_keys |  |
| conformance/stage6/number_parameter | not applicable (D1733) | - | a parameter typed Number: no type stands for every number |
| conformance/stage6/number_type | covered | tests/number_type |  |
| conformance/stage6/numbered_enums | covered | tests/numbered_enums |  |
| conformance/stage6/numbers_are_classes | ported | tests/numbers_are_classes | D662: the Memory.Heap.functions line dropped |
| conformance/stage6/numeric_types | covered | tests/numeric_types |  |
| conformance/stage6/operators | covered | tests/operators |  |
| conformance/stage6/optimization_report | covered | tests/optimization_report |  |
| conformance/stage6/overlapped_calls | covered | tests/overlapped_calls |  |
| conformance/stage6/overlapped_reads | covered | tests/overlapped_reads |  |
| conformance/stage6/own_collection_templates | covered | tests/own_collection_templates |  |
| conformance/stage6/owned_conditions | covered | tests/owned_conditions |  |
| conformance/stage6/owned_objects | covered | tests/owned_objects |  |
| conformance/stage6/package_attributes | covered | tests/package_attributes |  |
| conformance/stage6/parallel_each | covered | tests/each_over_particles | now `each_step()`, which the compiler may band (D1735) |
| conformance/stage6/parallel_handover | covered | tests/parallel_handover |  |
| conformance/stage6/parallel_receivers | covered | tests/parallel_receivers |  |
| conformance/stage6/parallel_stages | not applicable (D662) | retired/parallel_stages | kept in retired/parallel_stages (D665) |
| conformance/stage6/parallel_work | covered | tests/parallel_work |  |
| conformance/stage6/parameter_write_rows | covered | tests/parameter_write_rows |  |
| conformance/stage6/parameter_writes | covered | tests/parameter_writes |  |
| conformance/stage6/passed_functions | covered | tests/passed_functions |  |
| conformance/stage6/passed_symbol | covered | tests/passed_symbol |  |
| conformance/stage6/path_narrowing | covered | tests/path_narrowing |  |
| conformance/stage6/pinned_components | covered | tests/pinned_components |  |
| conformance/stage6/placed_free | not applicable (D662) | - | heap.allocate and free are hidden |
| conformance/stage6/plain_counts | covered | tests/plain_counts |  |
| conformance/stage6/plain_items | covered | tests/plain_items |  |
| conformance/stage6/plural_templates | covered | tests/plural_templates |  |
| conformance/stage6/pooled_loads_wait | not applicable (D1715) | retired/pooled_loads_wait | kept in retired/pooled_loads_wait |
| conformance/stage6/present_zero | covered | tests/present_zero |  |
| conformance/stage6/printable_values | covered | tests/printable_values |  |
| conformance/stage6/private_walk | covered | tests/private_walk |  |
| conformance/stage6/program_arguments | covered | tests/program_arguments |  |
| conformance/stage6/proof_again_in_loop | covered | tests/proof_again_in_loop |  |
| conformance/stage6/proven_ranges | covered | tests/proven_ranges |  |
| conformance/stage6/proven_read_outside | covered | tests/proven_read_outside |  |
| conformance/stage6/queued_borrows | covered | tests/queued_borrows |  |
| conformance/stage6/quiet_asserts | covered | tests/quiet_asserts |  |
| conformance/stage6/quiet_asserts_optimized | covered | tests/quiet_asserts_optimized |  |
| conformance/stage6/rebinding | covered | tests/rebinding |  |
| conformance/stage6/receiver_call_effects | covered | tests/receiver_call_effects |  |
| conformance/stage6/reflection | covered | tests/reflection |  |
| conformance/stage6/reflection_narrowing | covered | tests/reflection_narrowing |  |
| conformance/stage6/reflection_objects | covered | tests/reflection_objects |  |
| conformance/stage6/reflection_sources | covered | tests/reflection_sources |  |
| conformance/stage6/reflection_stand_in | covered | tests/reflection_stand_in |  |
| conformance/stage6/reflection_walks | covered | tests/reflection_walks |  |
| conformance/stage6/relative_namespaces | covered | tests/relative_namespaces |  |
| conformance/stage6/reopen_library | covered | tests/reopen_library |  |
| conformance/stage6/reopening | covered | tests/reopening |  |
| conformance/stage6/report_local_types | covered | tests/report_local_types |  |
| conformance/stage6/rest_case | covered | tests/rest_case |  |
| conformance/stage6/returning_branch_proof | covered | tests/returning_branch_proof |  |
| conformance/stage6/row_of_classes | covered | tests/row_of_classes |  |
| conformance/stage6/run_time_accesses | covered | tests/run_time_accesses |  |
| conformance/stage6/run_time_kinds | covered | tests/run_time_kinds |  |
| conformance/stage6/same_name_call_effects | covered | tests/same_name_call_effects |  |
| conformance/stage6/serializer_singletons | covered | tests/serializer_singletons |  |
| conformance/stage6/settings_and_libraries_reflected | covered | tests/settings_and_libraries_reflected |  |
| conformance/stage6/shape_attributes | covered | tests/shape_attributes |  |
| conformance/stage6/shape_copies | covered | tests/shape_copies |  |
| conformance/stage6/shape_defaults | covered | tests/shape_defaults |  |
| conformance/stage6/shape_functions | covered | tests/shape_functions |  |
| conformance/stage6/shape_members | covered | tests/shape_members |  |
| conformance/stage6/shape_operator_cut | not applicable (D1733) | - | an operator on Number parameters in a REPL build |
| conformance/stage6/shape_operator_unlike | covered | tests/shape_operator_unlike |  |
| conformance/stage6/shape_operators | covered | tests/shape_operators |  |
| conformance/stage6/shapes | covered | tests/shapes |  |
| conformance/stage6/shared_copies | covered | tests/shared_copies |  |
| conformance/stage6/short_text | ported | tests/short_text | D662: the x.memory.section line dropped |
| conformance/stage6/sibling_load | covered | tests/sibling_load |  |
| conformance/stage6/signedness_change | covered | tests/signedness_change |  |
| conformance/stage6/singleton_attribute_reads | covered | tests/singleton_attribute_reads |  |
| conformance/stage6/singleton_circle | covered | tests/singleton_circle |  |
| conformance/stage6/singleton_counts | covered | tests/singleton_counts |  |
| conformance/stage6/singleton_forms | covered | tests/singleton_forms |  |
| conformance/stage6/singleton_freed_after_teardown | covered | tests/singleton_freed_after_teardown |  |
| conformance/stage6/singleton_function_values | covered | tests/singleton_function_values |  |
| conformance/stage6/singleton_guard | covered | tests/singleton_guard |  |
| conformance/stage6/singleton_held_at_exit | covered | tests/singleton_held_at_exit |  |
| conformance/stage6/singleton_lock_calls | covered | tests/singleton_lock_calls |  |
| conformance/stage6/singleton_race | covered | tests/singleton_race |  |
| conformance/stage6/singleton_reads | covered | tests/singleton_reads |  |
| conformance/stage6/singleton_stand_in | covered | tests/singleton_stand_in |  |
| conformance/stage6/singleton_stateless_calls | not applicable (D1715) | retired/singleton_stateless_calls | kept in retired/singleton_stateless_calls |
| conformance/stage6/singleton_teardown | covered | tests/singleton_teardown |  |
| conformance/stage6/singleton_used_after_exit | covered | tests/singleton_used_after_exit |  |
| conformance/stage6/singletons | covered | tests/singletons |  |
| conformance/stage6/slot_write_backs | covered | tests/slot_write_backs |  |
| conformance/stage6/smallest_absolute | covered | tests/smallest_absolute |  |
| conformance/stage6/smallest_divided | covered | tests/smallest_divided |  |
| conformance/stage6/socket_bytes | covered | tests/socket_bytes |  |
| conformance/stage6/socket_waits | covered | tests/socket_waits |  |
| conformance/stage6/sorting_many | covered | tests/sorting_many |  |
| conformance/stage6/source_folder | covered | tests/source_folder |  |
| conformance/stage6/sparse_rows | covered | tests/sparse_rows |  |
| conformance/stage6/specialised_elsewhere | covered | tests/specialised_elsewhere |  |
| conformance/stage6/state_machines | not applicable (D1715) | retired/state_machines | kept in retired/state_machines |
| conformance/stage6/stored_in_place | covered | tests/stored_in_place |  |
| conformance/stage6/streamed_rows | covered | tests/streamed_rows |  |
| conformance/stage6/strided_counted_loops | covered | tests/strided_counted_loops |  |
| conformance/stage6/structural_index | covered | tests/structural_index |  |
| conformance/stage6/symbol_class_function | covered | tests/symbol_class_function |  |
| conformance/stage6/symbol_codegen | covered | tests/symbol_codegen |  |
| conformance/stage6/system_folder | covered | tests/system_folder |  |
| conformance/stage6/system_functions | covered | tests/system_functions |  |
| conformance/stage6/system_phases | covered | tests/system_phases |  |
| conformance/stage6/tagged_values | covered | tests/tagged_values |  |
| conformance/stage6/template_argument_wait | not applicable (D662) | retired/template_argument_wait | kept in retired/template_argument_wait (D665) |
| conformance/stage6/template_lend_drop | covered | tests/template_lend_drop |  |
| conformance/stage6/tested_classes | covered | tests/tested_classes |  |
| conformance/stage6/tested_items | covered | tests/tested_items |  |
| conformance/stage6/text_building | covered | tests/text_building |  |
| conformance/stage6/text_member_templates | covered | tests/text_member_templates |  |
| conformance/stage6/text_to_enum | covered | tests/text_to_enum |  |
| conformance/stage6/text_to_number | covered | tests/text_to_number |  |
| conformance/stage6/thread_local_growth | not applicable (D662) | retired/thread_local_growth | kept in retired/thread_local_growth (D665) |
| conformance/stage6/thread_locals | not applicable (D662) | retired/thread_locals | kept in retired/thread_locals (D665) |
| conformance/stage6/thread_pool_reuse | not applicable (D662) | retired/thread_pool_reuse | kept in retired/thread_pool_reuse (D665) |
| conformance/stage6/threaded_class_objects | covered | tests/threaded_class_objects |  |
| conformance/stage6/time_text_errors | covered | tests/time_text_errors |  |
| conformance/stage6/time_text_round_trips | covered | tests/time_text_round_trips |  |
| conformance/stage6/trace_asserts | covered | tests/trace_asserts |  |
| conformance/stage6/trace_asserts_optimized | covered | tests/trace_asserts_optimized |  |
| conformance/stage6/type_function_values | not applicable (D1715) | retired/type_function_values | kept in retired/type_function_values |
| conformance/stage6/type_size | covered | tests/type_size |  |
| conformance/stage6/typed_functions | covered | tests/typed_functions |  |
| conformance/stage6/union_identity | covered | tests/union_identity |  |
| conformance/stage6/union_member_templates | covered | tests/union_member_templates |  |
| conformance/stage6/unreached_function_reads | covered | tests/unreached_function_reads |  |
| conformance/stage6/unshared_locks | covered | tests/unshared_locks |  |
| conformance/stage6/unsigned_overflow | covered | tests/unsigned_overflow |  |
| conformance/stage6/utf8_text | covered | tests/utf8_text |  |
| conformance/stage6/value_switch | covered | tests/value_switch |  |
| conformance/stage6/variadic_answers_in_frame | covered | tests/variadic_answers_in_frame |  |
| conformance/stage6/variadic_arguments | covered | tests/variadic_arguments |  |
| conformance/stage6/vector_find_sort | covered | tests/list_find_sort |  |
| conformance/stage6/vector_items | ported | tests/list_items_run | D662: the arena placement became a plain second Vector ('kept elsewhere 1') |
| conformance/stage6/vector_remove_out_of_range | removed (D1707) | - |  |
| conformance/stage6/vector_rows | covered | tests/list_rows |  |
| conformance/stage6/wait_index_order | covered | tests/wait_index_order |  |
| conformance/stage6/wait_order | covered | tests/wait_order |  |
| conformance/stage6/waiting_systems | not applicable (D1715) | retired/waiting_systems | kept in retired/waiting_systems |
| conformance/stage6/waits_never_hang | not applicable (D1715) | retired/waits_never_hang | kept in retired/waits_never_hang |
| conformance/stage6/walked_class_fold | covered | tests/walked_class_fold |  |
| conformance/stage6/walked_row_locals | covered | tests/walked_row_locals |  |
| conformance/stage6/walked_rows | covered | tests/walked_rows |  |
| conformance/stage6/weak_parent | covered | tests/parent_back_reference |  |
| conformance/stage6/web_socket_exchange | not applicable (D1715) | retired/web_socket_exchange | kept in retired/web_socket_exchange |
| conformance/stage6/web_socket_failures | not applicable (D1715) | retired/web_socket_failures | kept in retired/web_socket_failures |
| conformance/stage6/working_directory | covered | tests/working_directory |  |
| conformance/stage6/wrapping_functions | covered | tests/wrapping_functions |  |
| conformance/stage6/written_codegen_in_frame | covered | tests/written_codegen_in_frame |  |
| conformance/stage6/zone_files | covered | tests/zone_files |  |
| diagnostics/address_forms | not applicable (D662) | retired/address_forms | kept in retired/address_forms (D665) |
| diagnostics/allocator_after_use | not applicable (D662) | retired/allocator_after_use | kept in retired/allocator_after_use (D665) |
| diagnostics/allocator_reads | not applicable (D662) | retired/allocator_reads | kept in retired/allocator_reads (D665) |
| diagnostics/allocator_resizes | not applicable (D662) | retired/allocator_resizes | kept in retired/allocator_resizes (D665) |
| diagnostics/argument_class | covered | conformance/argument_class |  |
| diagnostics/argument_class_name | covered | conformance/argument_class_name |  |
| diagnostics/argument_count | covered | conformance/argument_count |  |
| diagnostics/atomic_misuse | not applicable (D662) | retired/atomic_misuse | kept in retired/atomic_misuse (D665) |
| diagnostics/bitwise_on_float | covered | conformance/bitwise_on_float |  |
| diagnostics/blank_line | covered | conformance/blank_line |  |
| diagnostics/borrowed_name | covered | conformance/borrowed_name |  |
| diagnostics/borrowed_new | covered | conformance/borrowed_new |  |
| diagnostics/borrowed_operators | covered | conformance/borrowed_operators |  |
| diagnostics/borrowed_syntax | covered | conformance/borrowed_syntax |  |
| diagnostics/borrowed_this | covered | conformance/borrowed_this |  |
| diagnostics/bound_proofs_undone | covered | conformance/bound_proofs_undone |  |
| diagnostics/branch_undoes_proof | covered | conformance/branch_undoes_proof |  |
| diagnostics/build_setting_type | covered | conformance/build_setting_type |  |
| diagnostics/bulk_removal | covered | conformance/bulk_removal |  |
| diagnostics/byte_reads | covered | conformance/byte_reads |  |
| diagnostics/call_as_argument | covered | conformance/call_as_argument |  |
| diagnostics/call_undoes_proof | covered | conformance/call_undoes_proof |  |
| diagnostics/cast_by_name | covered | conformance/cast_by_name |  |
| diagnostics/chain_outside_comparison | covered | conformance/type_mistakes |  |
| diagnostics/check_proves_nothing | covered | conformance/check_proves_nothing |  |
| diagnostics/class_and_namespace | covered | conformance/class_and_namespace |  |
| diagnostics/class_as_value | covered | conformance/class_as_value |  |
| diagnostics/class_keyword | covered | conformance/class_keyword |  |
| diagnostics/codegen_values | covered | conformance/codegen_values |  |
| diagnostics/comment_broken_link | covered | conformance/comment_broken_link |  |
| diagnostics/comment_inside_function | covered | conformance/comment_inside_function |  |
| diagnostics/comment_missing_file | covered | conformance/comment_missing_file |  |
| diagnostics/comment_prose_comment | covered | conformance/comment_prose_comment |  |
| diagnostics/comment_slash_comment | covered | conformance/comment_slash_comment |  |
| diagnostics/common_mistakes | covered | conformance/common_mistakes |  |
| diagnostics/computed_receiver | covered | conformance/computed_receiver |  |
| diagnostics/conditions_and_returns | covered | conformance/conditions_and_returns |  |
| diagnostics/console_flush | covered | conformance/console_flush |  |
| diagnostics/constructor_as_argument | covered | conformance/constructor_as_argument |  |
| diagnostics/constructor_codegen_list | covered | conformance/constructor_codegen_list |  |
| diagnostics/copy_to_narrow | covered | conformance/copy_to_narrow |  |
| diagnostics/copy_to_narrow_conditions | covered | conformance/copy_to_narrow_conditions |  |
| diagnostics/count_proves_no_key | covered | conformance/count_proves_no_key |  |
| diagnostics/count_undoes_proof | covered | conformance/count_undoes_proof |  |
| diagnostics/declaration_one_per_line | covered | conformance/declaration_one_per_line |  |
| diagnostics/declaration_order | covered | conformance/declaration_order |  |
| diagnostics/declaration_walk | covered | conformance/declaration_walk |  |
| diagnostics/default_answer | covered | conformance/default_answer_enum |  |
| diagnostics/default_guard | covered | conformance/default_guard |  |
| diagnostics/dictionary_type_arguments | covered | conformance/dictionary_type_arguments |  |
| diagnostics/dispatched_resizes | removed (D1707) | - |  |
| diagnostics/division_by_constant_zero | covered | conformance/division_by_constant_zero |  |
| diagnostics/doubled_underscore | covered | conformance/doubled_underscore |  |
| diagnostics/dropped_construction | covered | conformance/dropped_construction |  |
| diagnostics/duplicate_function | covered | conformance/duplicate_function |  |
| diagnostics/empty_collection_guard | covered | conformance/empty_collection_guard |  |
| diagnostics/empty_constructor | covered | conformance/empty_constructor |  |
| diagnostics/empty_folder_range | covered | tests/empty_folder_range |  |
| diagnostics/endless_locked_loop | not applicable (D1715) | - | conformance/endless_locked_loop removed with spite_compiler#224 |
| diagnostics/endless_parallel_work | not applicable (D1715) | - | conformance/endless_parallel_work removed with spite_compiler#224 |
| diagnostics/entry_class_reopened | covered | conformance/entry_class_reopened |  |
| diagnostics/entry_constructor_arguments | covered | conformance/entry_constructor_arguments |  |
| diagnostics/enum_from_text | covered | conformance/enum_from_text |  |
| diagnostics/enum_numbers | covered | conformance/enum_numbers |  |
| diagnostics/enum_reopening_replaces | covered | conformance/enum_reopening_replaces |  |
| diagnostics/enum_value_templates | covered | conformance/enum_value_templates |  |
| diagnostics/environment_setting | covered | conformance/environment_setting |  |
| diagnostics/equals_keys | covered | conformance/equals_keys |  |
| diagnostics/every_attribute | covered | conformance/every_attribute |  |
| diagnostics/failed_constructor | covered | conformance/failed_constructor |  |
| diagnostics/falling_end | covered | conformance/falling_end |  |
| diagnostics/file_scope_statement | covered | conformance/file_scope_statement |  |
| diagnostics/flag_given_twice | covered | conformance/flag_given_twice |  |
| diagnostics/flag_value | covered | conformance/flag_value |  |
| diagnostics/folded_build_crash | covered | conformance/folded_build_crash |  |
| diagnostics/folded_crash | covered | conformance/folded_crash |  |
| diagnostics/folder_name | covered | conformance/folder_name |  |
| diagnostics/foreign_call_mistakes | covered | conformance/foreign_call_mistakes |  |
| diagnostics/foreign_callback_mistakes | covered | conformance/foreign_callback_mistakes |  |
| diagnostics/foreign_library_mistakes | covered | conformance/foreign_library_mistakes |  |
| diagnostics/format_flag | covered | conformance/format_flag |  |
| diagnostics/format_setting | covered | conformance/format_setting |  |
| diagnostics/found_item_borrows | removed (D1707) | - |  |
| diagnostics/from_conversion | covered | conformance/from_conversion |  |
| diagnostics/function_owner | covered | tests/function_owner |  |
| diagnostics/function_reflection | covered | conformance/function_reflection |  |
| diagnostics/function_shape_default | covered | conformance/function_shape_default |  |
| diagnostics/function_value_mistakes | covered | conformance/function_value_mistakes |  |
| diagnostics/function_waits_paradox | covered | conformance/function_waits_paradox |  |
| diagnostics/generic_class_in_comparison | covered | conformance/generic_class_in_comparison |  |
| diagnostics/generic_constraint_not_a_type | covered | conformance/generic_constraint_not_a_type |  |
| diagnostics/generic_constraints | covered | conformance/generic_constraints |  |
| diagnostics/grouped_operand_text | covered | tests/grouped_operand_text |  |
| diagnostics/hand_copy_of_library | covered | conformance/hand_copy_of_library |  |
| diagnostics/hidden_class | covered | conformance/hidden_class |  |
| diagnostics/hole_values | covered | conformance/hole_values |  |
| diagnostics/index_reads | covered | conformance/index_reads |  |
| diagnostics/indexable_unproven | covered | conformance/indexable_unproven |  |
| diagnostics/inline_singleton | covered | conformance/inline_singleton |  |
| diagnostics/inline_singleton_attribute | covered | conformance/inline_singleton_attribute |  |
| diagnostics/io_system_writes_row | covered | conformance/io_system_writes_row |  |
| diagnostics/items_borrows | removed (D1707) | - |  |
| diagnostics/items_nullable_unproven | covered | conformance/list_nullable_unproven |  |
| diagnostics/joined_maybe_boolean | not applicable (D1715) | - | conformance/joined_maybe_boolean removed with spite_compiler#224 |
| diagnostics/joined_text | covered | conformance/joined_text |  |
| diagnostics/json_key | covered | conformance/json_key |  |
| diagnostics/json_split | covered | conformance/json_split |  |
| diagnostics/json_unwritable | covered | conformance/json_unwritable |  |
| diagnostics/known_fact_checked | covered | conformance/known_fact_checked |  |
| diagnostics/left_over_after_block | covered | conformance/left_over_after_block |  |
| diagnostics/left_over_after_return | covered | conformance/left_over_after_return |  |
| diagnostics/lent_arguments | covered | tests/lent_arguments |  |
| diagnostics/lent_element_guard | dropped |  | a fallback now returns the object a class makes with no arguments (D1765) |
| diagnostics/lent_escapes | removed (D1707) | - |  |
| diagnostics/lent_results | removed (D1707) | - |  |
| diagnostics/lent_to_calls | removed (D1707) | - |  |
| diagnostics/library_class_namespace | covered | conformance/library_class_namespace |  |
| diagnostics/library_function_mistakes | covered | conformance/library_function_mistakes |  |
| diagnostics/literal_does_not_fit | covered | conformance/literal_does_not_fit |  |
| diagnostics/load_as_value | covered | conformance/load_as_value |  |
| diagnostics/load_folder_name | covered | conformance/load_folder_name |  |
| diagnostics/load_function | covered | conformance/load_function |  |
| diagnostics/load_parentheses | covered | conformance/load_parentheses |  |
| diagnostics/load_unknown_condition | covered | conformance/load_unknown_condition |  |
| diagnostics/loaded_folder_namespace | covered | conformance/loaded_folder_namespace |  |
| diagnostics/local_singleton | covered | conformance/local_singleton |  |
| diagnostics/locked_wait | not applicable (D1715) | - | conformance/locked_wait removed with spite_compiler#224 |
| diagnostics/lone_hole | covered | conformance/lone_hole |  |
| diagnostics/long_line_construction | covered | conformance/long_line_construction |  |
| diagnostics/lost_writes | covered | conformance/lost_writes |  |
| diagnostics/maths_constant_assigned | covered | conformance/maths_constant_assigned |  |
| diagnostics/maths_constant_on_value | covered | conformance/maths_constant_on_value |  |
| diagnostics/maths_other_spellings | covered | conformance/maths_other_spellings |  |
| diagnostics/maybe_boolean_conditions | covered | conformance/maybe_boolean_conditions |  |
| diagnostics/maybe_in_arithmetic | covered | conformance/maybe_in_arithmetic |  |
| diagnostics/maybe_null_into_type | ported | conformance/maybe_null_into_type | first error only |
| diagnostics/member_template_mistakes | covered | conformance/member_template_mistakes |  |
| diagnostics/memory_access | not applicable (D662) | retired/memory_access | kept in retired/memory_access (D665) |
| diagnostics/missing_switch_case | covered | conformance/missing_switch_case |  |
| diagnostics/named_like_library | covered | conformance/named_like_library |  |
| diagnostics/namespace_nullable | covered | conformance/namespace_nullable |  |
| diagnostics/naming | covered | conformance/naming |  |
| diagnostics/narrowed_name_reassigned | covered | conformance/narrowed_name_reassigned |  |
| diagnostics/nested_if_else | covered | conformance/nested_if_else |  |
| diagnostics/new_spite_class | covered | conformance/new_spite_class |  |
| diagnostics/null_comparison | covered | conformance/null_comparison |  |
| diagnostics/null_default_locals | covered | conformance/null_default_locals |  |
| diagnostics/null_defaults | covered | conformance/null_defaults |  |
| diagnostics/number_memory | not applicable (D662) | retired/number_memory | kept in retired/number_memory (D665) |
| diagnostics/number_memory_read | covered | conformance/number_memory_read |  |
| diagnostics/number_memory_spelling | not applicable (D662) | retired/number_memory_spelling | kept in retired/number_memory_spelling (D665) |
| diagnostics/number_type_mistakes | covered | conformance/number_type_mistakes |  |
| diagnostics/old_ampersand | covered | conformance/old_ampersand |  |
| diagnostics/old_break | covered | conformance/old_break |  |
| diagnostics/old_declaration_equal | covered | conformance/declaration_one_per_line |  |
| diagnostics/old_do | covered | conformance/old_do |  |
| diagnostics/old_let | covered | conformance/old_let |  |
| diagnostics/old_list_names | covered | conformance/old_list_names |  |
| diagnostics/old_nullable_spelling | ported | conformance/old_nullable_spelling | error line written in the SpiteSpec form (old one was the formatter's line:column form) |
| diagnostics/old_reference_type | covered | conformance/old_reference_type |  |
| diagnostics/old_shift | covered | conformance/old_shift |  |
| diagnostics/old_type_spellings | covered | conformance/old_type_spellings |  |
| diagnostics/operator_function_call | covered | conformance/operator_function_call |  |
| diagnostics/overlapping_load | covered | conformance/overlapping_load |  |
| diagnostics/own_template_borrows | removed (D1707) | - |  |
| diagnostics/package_attributes | covered | conformance/package_attributes |  |
| diagnostics/parallel_function_reach | not applicable (D1715) | - | conformance/parallel_function_reach removed with spite_compiler#224 |
| diagnostics/parallel_handover | not applicable (D1715) | - | conformance/parallel_handover removed with spite_compiler#224 |
| diagnostics/parallel_reach | not applicable (D1735) | - | its point is the race rule of `parallel_each_`, which is gone |
| diagnostics/parallel_shared_value_writes | not applicable (D1715) | - | conformance/parallel_shared_value_writes removed with spite_compiler#224 |
| diagnostics/parallel_shared_writes | not applicable (D1715) | - | conformance/parallel_shared_writes removed with spite_compiler#224 |
| diagnostics/parameter_write_question | covered | conformance/parameter_write_question |  |
| diagnostics/passed_functions | covered | conformance/passed_functions |  |
| diagnostics/passed_symbol | covered | tests/passed_symbol |  |
| diagnostics/path_narrow_assignment | covered | conformance/path_narrow_assignment |  |
| diagnostics/path_narrow_else | covered | conformance/path_narrow_else |  |
| diagnostics/path_narrow_loop | covered | conformance/path_narrow_loop |  |
| diagnostics/plain_get_at | covered | conformance/plain_get_at |  |
| diagnostics/plain_items | removed (D1707) | - |  |
| diagnostics/plain_symbol_on_list | covered | conformance/plain_symbol_on_list |  |
| diagnostics/plural_templates | covered | conformance/plural_templates |  |
| diagnostics/print_without_to_string | covered | conformance/print_without_to_string |  |
| diagnostics/proven_element | covered | conformance/proven_element |  |
| diagnostics/proven_index_check | covered | conformance/proven_index_check |  |
| diagnostics/read_only_attribute | covered | conformance/read_only_attribute |  |
| diagnostics/receiver_call_undoes_proof | covered | conformance/receiver_call_undoes_proof |  |
| diagnostics/reflection_names | covered | conformance/reflection_names |  |
| diagnostics/reflection_read_only | covered | conformance/reflection_read_only |  |
| diagnostics/removed_executable_flag | covered | conformance/removed_executable_flag |  |
| diagnostics/removed_run_flag | covered | conformance/removed_run_flag |  |
| diagnostics/removed_translation_units_flag | covered | conformance/removed_translation_units_flag |  |
| diagnostics/removed_tuning_flag | covered | conformance/removed_tuning_flag |  |
| diagnostics/reopened_vector_borrows | removed (D1707) | - |  |
| diagnostics/repeated_branch_call | covered | conformance/repeated_branch_call |  |
| diagnostics/replaced_items | covered | conformance/replaced_items |  |
| diagnostics/reserved_signature | covered | conformance/reserved_signature |  |
| diagnostics/reserved_spite_namespace | covered | conformance/reserved_spite_namespace |  |
| diagnostics/return_type_colon | covered | conformance/return_type_colon |  |
| diagnostics/returned_local | covered | conformance/returned_local |  |
| diagnostics/returned_text | not applicable (D797) | - | returned_text was deleted, its scheme replaced |
| diagnostics/returning_guard | covered | conformance/returning_guard |  |
| diagnostics/same_named_types | covered | conformance/same_named_types |  |
| diagnostics/separator_flag | covered | conformance/separator_flag |  |
| diagnostics/setting_named_like_flag | covered | conformance/setting_named_like_flag |  |
| diagnostics/setting_value | covered | conformance/setting_value |  |
| diagnostics/several_mistakes | covered | conformance/several_mistakes |  |
| diagnostics/shadowed_function | covered | conformance/shadowed_function |  |
| diagnostics/shape_function_mismatch | covered | conformance/shape_function_mismatch |  |
| diagnostics/shape_mismatch | covered | conformance/shape_mismatch |  |
| diagnostics/shape_parameter_name | covered | conformance/shape_parameter_name |  |
| diagnostics/singleton_arguments | covered | conformance/singleton_arguments |  |
| diagnostics/singleton_circle | covered | conformance/singleton_circle |  |
| diagnostics/singleton_function | covered | conformance/singleton_function |  |
| diagnostics/snapshot_argument_writes | covered | conformance/snapshot_argument_writes |  |
| diagnostics/sparse_rows | removed (D1707) | - |  |
| diagnostics/spinning_loop | covered | conformance/spinning_loop |  |
| diagnostics/spite_link_outside_final | covered | conformance/spite_link_outside_final |  |
| diagnostics/store_through_nullable | covered | conformance/store_through_nullable |  |
| diagnostics/structural_index_undone | covered | conformance/structural_index_undone |  |
| diagnostics/switch_chain | covered | conformance/switch_chain |  |
| diagnostics/switch_rest_case | covered | conformance/switch_rest_case |  |
| diagnostics/switch_single_case | covered | conformance/switch_single_case |  |
| diagnostics/symbol_for_text | covered | conformance/symbol_for_text |  |
| diagnostics/target_in_build | covered | conformance/target_in_build |  |
| diagnostics/target_operating_system | covered | conformance/target_operating_system |  |
| diagnostics/template_attribute_spelling | covered | conformance/template_attribute_spelling |  |
| diagnostics/template_function_clash | covered | conformance/template_function_clash |  |
| diagnostics/template_walk | covered | conformance/template_walk |  |
| diagnostics/terminal_if | covered | conformance/terminal_if |  |
| diagnostics/text_is_to_string | covered | conformance/text_is_to_string |  |
| diagnostics/text_items_passed | removed (D1707) | - |  |
| diagnostics/text_to_number | covered | conformance/text_to_number |  |
| diagnostics/type_mistakes | covered | conformance/type_mistakes |  |
| diagnostics/type_size_mistakes | covered | conformance/type_size_mistakes |  |
| diagnostics/unclosed_text | covered | conformance/unclosed_text |  |
| diagnostics/underscore_flag | covered | conformance/underscore_flag |  |
| diagnostics/underscore_setting | covered | conformance/underscore_setting |  |
| diagnostics/union_value_members | covered | conformance/union_value_members |  |
| diagnostics/unkeyable_types | covered | conformance/unkeyable_types |  |
| diagnostics/unknown_compiler_flag | covered | conformance/unknown_compiler_flag |  |
| diagnostics/unknown_type_in_shape | covered | conformance/unknown_type_in_shape |  |
| diagnostics/unknown_type_suggestion | covered | conformance/unknown_type_suggestion |  |
| diagnostics/unstepped_poll | not applicable (D1715) | - | conformance/unstepped_poll removed with spite_compiler#224 |
| diagnostics/unused_attributes | covered | conformance/unused_attributes |  |
| diagnostics/unused_names | covered | conformance/unused_names |  |
| diagnostics/upper_is_upper_case | covered | conformance/upper_is_upper_case |  |
| diagnostics/user_atomic_bare | not applicable (D662) | - | Atomic is no class at all now; Spite.Internal refusal is conformance/internal_use |
| diagnostics/user_atomic_internal_name | not applicable (D662) | - | as user_atomic_bare |
| diagnostics/user_internal_class | covered | conformance/internal_class |  |
| diagnostics/user_internal_use | covered | conformance/internal_use |  |
| diagnostics/user_lock_bare | not applicable (D662) | - | Lock is no class at all now |
| diagnostics/value_switch | covered | conformance/value_switch |  |
| diagnostics/variadic_mistakes | covered | conformance/variadic_mistakes |  |
| diagnostics/variadic_not_last | covered | conformance/variadic_not_last |  |
| diagnostics/vector_borrows | removed (D1707) | - |  |
| diagnostics/vector_items | removed (D1707) | - |  |
| diagnostics/vector_reserve_borrows | removed (D1707) | - |  |
| diagnostics/vector_rows | removed (D1707) | - |  |
| diagnostics/vector_template_walk | removed (D1707) | - |  |
| diagnostics/walk_with_function | covered | conformance/walk_with_function |  |
| diagnostics/walked_rows | removed (D1707) | - |  |
| diagnostics/weak_across_threads | not applicable (D755) | - | there is no Weak |
| diagnostics/whole_number_rotation | covered | conformance/whole_number_rotation |  |
| diagnostics/wider_bitwise_operand | covered | conformance/wider_bitwise_operand |  |
| diagnostics/wider_comparison | covered | conformance/wider_comparison |  |
| diagnostics/wider_right_operand | covered | conformance/wider_right_operand |  |
| diagnostics/written_handle_type | not applicable (D1715) | - | conformance/written_handle_type removed with spite_compiler#224 |
| diagnostics/written_not_read | covered | conformance/written_not_read |  |
| examples/arsenal | ported | tests/arsenal | D628: --name=server moved from flags.txt to arguments.txt |
| examples/battle | ported | tests/battle |  |
| examples/calculator | ported | tests/calculator | program.txt path is tests/calculator/program.txt |
| examples/dungeon | ported | tests/dungeon | D798: Symbol parameters are Spite.AttributeDeclaration |
| examples/event_bus | ported | tests/event_bus | |
| examples/game | ported | tests/game | game_no_mod.spite (a second entry) left out |
| examples/hello | ported | tests/hello_example |  |
| examples/library_card | ported | tests/library_card |  |
| examples/linked_walk | ported | tests/linked_walk |  |
| examples/party | ported | tests/party |  |
| examples/repository_list | ported | tests/repository_list | D798: Symbol parameters are Spite.AttributeDeclaration |
| examples/vectors | ported | tests/lists_inline |  |
| docs/classes_and_files.md#file_scope_error | covered | conformance/file_scope_statement |  |
| docs/classes_and_files.md#ordered_file | ported | tests/ordered_file |  |
| docs/classes_and_files.md#person_basics | covered | tests/accessors |  |
| docs/classes_and_files.md#team_roster | ported | tests/team_roster |  |
| docs/classes_and_files.md#empty_constructor_error | covered | conformance/empty_constructor |  |
| docs/classes_and_files.md#dropped_report | covered | conformance/dropped_construction |  |
| docs/classes_and_files.md#singleton_basics | covered | tests/singletons | is_singleton is read as an attribute of Spite.Class there |
| docs/classes_and_files.md#radio | covered | tests/classes_singleton_per_arguments |  |
| docs/classes_and_files.md#generic_singleton | covered | tests/generic_singletons |  |
| docs/classes_and_files.md#singleton_inline_error | covered | conformance/inline_singleton_attribute |  |
| docs/classes_and_files.md#level_up | ported | tests/level_up |  |
| docs/classes_and_files.md#shared_member | covered | tests/copy_semantics |  |
| docs/classes_and_files.md#this_basics | covered | tests/this_passed |  |
| docs/classes_and_files.md#printing_a_class | ported | tests/printing_a_class |  |
| docs/classes_and_files.md#own_vector | covered | conformance/named_like_library |  |
| docs/collections.md#dictionary_tasks | covered | tests/dictionaries | has() is contains() there |
| docs/collections.md#dictionary_literals | covered | tests/dictionary_literals |  |
| docs/collections.md#number_keys_tasks | covered | tests/number_keys |  |
| docs/collections.md#party_keys | covered | tests/any_keys |  |
| docs/collections.md#vector_basics | ported | tests/list_shared_items |  |
| docs/collections.md#vector_borrow_mistake | removed (D1707) | - |  |
| docs/collections.md#items_basics | ported | tests/list_item_basics |  |
| docs/collections.md#items_borrow_mistake | removed (D1707) | - |  |
| docs/collections.md#bulk_removal_doc | covered | tests/bulk_removal |  |
| docs/collections.md#computed_member | ported | tests/computed_member |  |
| docs/collections.md#list_helpers | covered | tests/member_templates |  |
| docs/collections.md#list_query | covered | tests/member_templates |  |
| docs/collections.md#template_mistake | covered | conformance/template_count_numeric |  |
| docs/collections.md#template_loop_error | covered | conformance/walk_sum_member |  |
| docs/collections.md#chained_members | covered | tests/union_member_templates |  |
| docs/collections.md#post_stages | covered | tests/enum_value_templates |  |
| docs/collections.md#passed_function | covered | tests/passed_functions |  |
| docs/collections.md#passed_function_mistake | covered | conformance/passed_returns_nothing |  |
| docs/collections.md#plain_vector | covered | tests/plain_items |  |
| docs/collections.md#fused_chain | covered | tests/fused_chains |  |
| docs/collections.md#list_average | covered | tests/own_collection_templates |  |
| docs/collections.md#plain_symbol_template | covered | conformance/plain_symbol_on_list | D798: the error now names Spite.AttributeDeclaration instead of Symbol<$element_type> |
| docs/concurrency.md#concurrent_tour | not applicable (D1715) | retired/concurrent_waits | kept in retired/concurrent_waits |
| docs/concurrency.md#reads_in_a_row | ported | tests/reads_in_a_row |  |
| docs/concurrency.md#written_order | covered | tests/wait_order |  |
| docs/concurrency.md#joined_on_drop | not applicable (D1715) | retired/concurrent_waits | kept in retired/concurrent_waits |
| docs/concurrency.md#polled_loading | not applicable (D1715) | retired/finished_polling | kept in retired/finished_polling |
| docs/concurrency.md#frame_io | not applicable (D662) | - | its point is Scheduler.resume_only_when_asked and run_ready; Scheduler is hidden |
| docs/concurrency.md#frame_waits | covered | tests/frame_waits |  |
| docs/concurrency.md#parallel_tour | covered | tests/parallel_work |  |
| docs/concurrency.md#per_thread_buffers | ported | tests/per_thread_buffers |  |
| docs/concurrency.md#shared_counter | covered | tests/singleton_lock_calls |  |
| docs/concurrency.md#locked_wait_doc | not applicable (D1715) | - | conformance/locked_wait removed with spite_compiler#224 |
| docs/concurrency.md#handed_over_doc | covered | tests/parallel_handover |  |
| docs/concurrency.md#parallel_pass | covered | tests/each_over_particles | now `each_step()`, which the compiler may band (D1735) |
| docs/concurrency.md#parallel_reach | not applicable (D1735) | - | its point is the race rule of `parallel_each_`, which is gone |
| docs/concurrency.md#frame_loop | ported | tests/frame_loop |  |
| docs/concurrency.md#busy_loop | ported | tests/busy_loop |  |
| docs/control_flow.md#if_chains | ported | tests/if_chains |  |
| docs/control_flow.md#nested_decision_error | covered | conformance/control_nested_in_else_if |  |
| docs/control_flow.md#nested_decision | ported | tests/nested_decision |  |
| docs/control_flow.md#for_rejected | covered | conformance/control_old_for |  |
| docs/control_flow.md#while_basics | covered | tests/number_keys |  |
| docs/control_flow.md#early_exit | ported | tests/early_exit |  |
| docs/control_flow.md#switch_cases | ported | tests/switch_cases |  |
| docs/control_flow.md#value_switch_doc | covered | tests/value_switch |  |
| docs/control_flow.md#missing_value_error | covered | conformance/control_switch_enum_missing |  |
| docs/control_flow.md#if_chain_error | covered | conformance/switch_chain |  |
| docs/control_flow.md#class_test_doc | covered | tests/class_test |  |
| docs/control_flow.md#generic_class_test_doc | covered | tests/generic_class_test |  |
| docs/control_flow.md#codegen_class_test_doc | covered | tests/codegen_class_test |  |
| docs/control_flow.md#single_case_switch_error | covered | conformance/switch_single_case |  |
| docs/control_flow.md#after_return_error | ported | conformance/after_return_error | |
| docs/control_flow.md#returned_local_error | covered | conformance/returned_local |  |
| docs/failure.md#null_comparison_error | covered | conformance/null_comparison |  |
| docs/failure.md#nullable_narrowing | covered | tests/nullable_narrowing |  |
| docs/failure.md#path_narrowing_doc | covered | tests/path_narrowing | path fallback through a.b.c; while walk in tests/while_narrowing_walk |
| docs/failure.md#copy_to_narrow_error | covered | conformance/copy_to_narrow |  |
| docs/failure.md#call_undoes_proof_error | covered | conformance/call_undoes_proof |  |
| docs/failure.md#call_keeps_proof | ported | tests/call_keeps_proof |  |
| docs/failure.md#shadowing_doc | ported | tests/shadowing_doc |  |
| docs/failure.md#index_reads_doc | ported | tests/index_reads_doc |  |
| docs/failure.md#kept_read | covered | tests/narrowed_read_local |  |
| docs/failure.md#read_in_index_error | covered | conformance/nested_index_read | same error text |
| docs/failure.md#nullable_comparison | covered | tests/nullable_comparison |  |
| docs/failure.md#assert_guard | covered | tests/assert_answers_empty |  |
| docs/failure.md#default_answer_error | covered | conformance/default_answer_enum |  |
| docs/failure.md#default_answer_fixed | ported | tests/default_answer_fixed |  |
| docs/failure.md#leaving_if | covered | tests/leaving_if |  |
| docs/failure.md#falling_end_error | covered | conformance/falling_end |  |
| docs/failure.md#falling_end_fixed | covered | tests/every_path_returns |  |
| docs/failure.md#default_guard_error | covered | conformance/guard_inside_loop |  |
| docs/failure.md#default_guard_fixed | covered | tests/assert_answers_empty |  |
| docs/failure.md#default_answer | ported | tests/default_answer_doc | the default answer written as a fallback (D1765) |
| docs/failure.md#terminal_if_error | covered | conformance/terminal_if |  |
| docs/failure.md#terminal_if_fixed | ported | tests/terminal_if_fixed |  |
| docs/failure.md#constructor_assert_error | covered | conformance/assert_in_entry_constructor |  |
| docs/failure.md#crash_guard | covered | tests/crash |  |
| docs/foreign_libraries.md#foreign_call | ported | tests/foreign_call | a system library (ucrtbase strlen), not a fixture |
| docs/foreign_libraries.md#sorted_by_c | not applicable (D662) | - | the comparator reads TypedMemory at a Memory.Address; C calling back is covered by tests/foreign_callbacks |
| docs/functions_and_operators.md#missing_colon_error | covered | conformance/return_type_colon |  |
| docs/functions_and_operators.md#function_values_doc | covered | tests/typed_functions |  |
| docs/functions_and_operators.md#shadowed_function_error | covered | conformance/functions_shadowed_local |  |
| docs/functions_and_operators.md#variadic_doc | covered | tests/variadic_arguments |  |
| docs/functions_and_operators.md#variadic_not_a_list | covered | conformance/functions_variadic_untyped |  |
| docs/functions_and_operators.md#operators_as_functions | covered | tests/operators |  |
| docs/functions_and_operators.md#operator_missing_error | covered | conformance/functions_operator_missing_sum |  |
| docs/functions_and_operators.md#interception | covered | tests/accessors | D798: Symbol parameters now Spite.AttributeDeclaration |
| docs/functions_and_operators.md#read_only_doc | ported | tests/read_only_doc |  |
| docs/functions_and_operators.md#read_only_error | covered | conformance/read_only_attribute |  |
| docs/functions_and_operators.md#operator_by_name | covered | conformance/operator_call_on_a_class | D907 |
| docs/functions_and_operators.md#argument_casting | covered | tests/functions_argument_cast |  |
| docs/functions_and_operators.md#drop_with_parameter | covered | conformance/reserved_signature |  |
| docs/game_maths.md#game_vectors_basics | covered | tests/game_vectors |  |
| docs/game_maths.md#game_grid_vectors | ported | tests/game_grid_vectors |  |
| docs/game_maths.md#game_matrices_basics | covered | tests/game_matrices |  |
| docs/game_maths.md#double_precision | covered | tests/double_maths |  |
| docs/game_maths.md#game_rotations_basics | covered | tests/game_matrices |  |
| docs/game_maths.md#game_picking_basics | covered | tests/game_geometry |  |
| docs/game_maths.md#game_curves_basics | covered | tests/game_curves |  |
| docs/game_maths.md#half_precision_basics | covered | tests/half_precision |  |
| docs/getting_started.md#hello_world | covered | tests/hello |  |
| docs/getting_started.md#tour_classes | ported | tests/tour_classes |  |
| docs/json.md#json_basics | ported | tests/json_basics | |
| docs/json.md#binary_basics | covered | tests/binary_round_trip |  |
| docs/json.md#binary_stream | covered | tests/binary_many_values |  |
| docs/json.md#schema_header | covered | tests/binary_schema |  |
| docs/json.md#json_input | ported | tests/json_input |  |
| docs/json.md#json_camel_case | covered | tests/json_camel_case |  |
| docs/json.md#json_key_function | covered | conformance/json_key |  |
| docs/json.md#json_values | ported | tests/json_values |  |
| docs/memory.md#sharing_basics | covered | tests/copy_semantics |  |
| docs/memory.md#copy_basics | covered | tests/copy_semantics |  |
| docs/memory.md#vector_row_doc | ported | tests/list_rows | the doc example is now row_doc |
| docs/memory.md#walked_row_doc | covered | tests/walked_rows |  |
| docs/memory.md#sparse_row_doc | covered | tests/sparse_rows |  |
| docs/memory.md#lent_arguments_doc | covered | tests/ignored_lent_argument |  |
| docs/memory.md#lent_result_doc | removed (D1707) | - |  |
| docs/memory.md#lent_call_doc | covered | tests/lent_to_calls |  |
| docs/memory.md#drop_basics | covered | tests/drop_hook |  |
| docs/memory.md#tree_basics | ported | tests/tree_basics |  |
| docs/memory.md#weak_family | covered | tests/parent_back_reference | D755: no Weak, plain references |
| docs/memory.md#ring_buffer_program | not applicable (D662) | - | Memory.Heap, TypedMemory and live_allocations |
| docs/memory.md#memory_sections | not applicable (D662) | - | reads x.memory |
| docs/memory.md#heap_scores | not applicable (D662) | - | reads memory.allocator |
| docs/metaprogramming.md#walks_and_templates | covered | tests/reflection_walks, tests/member_templates | attribute walk, show_health template, filter_alive().sum_health() |
| docs/metaprogramming.md#walked_arguments | covered | tests/reflection_walks |  |
| docs/metaprogramming.md#asking_functions | ported | tests/asking_functions |  |
| docs/metaprogramming.md#generic_questions | covered | tests/reflection_narrowing |  |
| docs/metaprogramming.md#name_templates | ported | tests/name_templates | D798: Spite.AttributeDeclaration<Label> written Spite.AttributeDeclaration |
| docs/metaprogramming.md#own_accessors | covered | tests/symbol_codegen |  |
| docs/metaprogramming.md#generics_basics | covered | tests/generics, tests/generic_inferred |  |
| docs/metaprogramming.md#codegen_values_doc | covered | tests/generics |  |
| docs/metaprogramming.md#codegen_count_error | covered | conformance/generic_wrong_count | same error on an own class |
| docs/metaprogramming.md#generic_constraint | covered | tests/generic_constraints |  |
| docs/metaprogramming.md#generic_constraint_error | covered | conformance/generic_constraints | same error with type Printable |
| docs/metaprogramming.md#describe_kind | ported | tests/describe_kind |  |
| docs/metaprogramming.md#generic_never_true | ported | tests/generic_never_true |  |
| docs/metaprogramming.md#codegen_class_name | covered | tests/codegen_class_name |  |
| docs/metaprogramming.md#fits_vector_doc | ported | tests/fits_vector_doc | |
| docs/optimizations.md#folded_branch | ported | tests/folded_branch |  |
| docs/optimizations.md#build_folding | ported | tests/build_folding | build=optimized:true given as flags.txt --optimized |
| docs/optimizations.md#fused_order | ported | tests/fused_order |  |
| docs/optimizations.md#text_append | ported | tests/text_append |  |
| docs/optimizations.md#frame_placement | not applicable (D662) | - | reads Memory.Heap live allocations |
| docs/optimizations.md#lazy_singleton | ported | tests/lazy_singleton |  |
| docs/optimizations.md#production_internals | not applicable (D662) | - | counts Memory.Heap instances |
| docs/optimizations.md#inspectable_internals | not applicable (D662) | - | counts Memory.Heap instances under --development |
| docs/optimizations.md#plain_counts | covered | tests/plain_counts |  |
| docs/optimizations.md#a_wait_in_a_frame_doc | ported | tests/a_wait_in_a_frame_doc |  |
| docs/optimizations.md#unshared_ledger | ported | tests/unshared_ledger |  |
| docs/optimizations.md#locked_tally | covered | tests/coarse_locks |  |
| docs/optimizations.md#tested_once | ported | tests/tested_once |  |
| docs/optimizations.md#held_attributes | ported | tests/held_attributes |  |
| docs/optimizations.md#stored_in_place | covered | tests/stored_in_place |  |
| docs/optimizations.md#items_used_at_once | ported | tests/list_used_at_once |  |
| docs/optimizations.md#foreign_names | ported | tests/foreign_names |  |
| docs/optimizations.md#slot_write_back | ported | tests/slot_write_back |  |
| docs/optimizations.md#tested_items | covered | tests/tested_items |  |
| docs/optimizations.md#written_back_across_calls | ported | tests/written_back_across_calls |  |
| docs/optimizations.md#held_names | ported | tests/held_names |  |
| docs/optimizations.md#listed_steps | ported | tests/listed_steps |  |
| docs/optimizations.md#filled_tariff | ported | tests/filled_tariff |  |
| docs/optimizations.md#nearby_points | ported | tests/nearby_points |  |
| docs/optimizations.md#chain_links | ported | tests/chain_links |  |
| docs/optimizations.md#words_by_length | ported | tests/words_by_length |  |
| docs/optimizations.md#letters_twice | ported | tests/letters_twice |  |
| docs/optimizations.md#shared_copies | covered | tests/shared_copies |  |
| docs/optimizations.md#proven_sum | ported | tests/proven_sum |  |
| docs/optimizations.md#frame_lists | covered | tests/frame_lists |  |
| docs/optimizations.md#inlined_release | ported | tests/inlined_release |  |
| docs/optimizations.md#calls_in_a_row | ported | tests/calls_in_a_row |  |
| docs/optimizations.md#row_of_voices | ported | tests/row_of_voices |  |
| docs/optimizations.md#bands_of_orbits | ported | tests/bands_of_orbits |  |
| docs/optimizations.md#owner_meters | ported | tests/owner_meters |  |
| docs/optimizations.md#own_shelves | ported | tests/own_shelves |  |
| docs/optimizations.md#cold_crash | ported | tests/cold_crash |  |
| docs/packages.md#own_entry_class | ported | tests/own_entry_class |  |
| docs/packages.md#package_demo | covered | tests/reopening |  |
| docs/packages.md#reopen_string | covered | tests/classes_text_binds_singleton_locally |  |
| docs/packages.md#phase_mod | covered | tests/enum_reopening |  |
| docs/packages.md#reopen_spite_class | covered | tests/reopen_library |  |
| docs/packages.md#spite_namespace_error | covered | conformance/reserved_spite_namespace |  |
| docs/packages.md#load_parentheses_error | covered | conformance/load_parentheses |  |
| docs/packages.md#pinned_widgets | ported | tests/pinned_widgets | pins the SpiteSpec repository itself (../..@26e6476/tests/namespaces_load/package) instead of the old repository's commit; fails: a load pinned to a commit (`load "../..@<commit>/..."`) is refused: fetching repositories is not built |
| docs/programs.md#entry_arguments_error | covered | conformance/entry_constructor_arguments |  |
| docs/programs.md#raw_arguments | covered | tests/program_arguments, tests/programs_bare_double_dash |  |
| docs/programs.md#program_settings | covered | tests/environment_settings, tests/kebab_setting |  |
| docs/programs.md#setting_type_error | covered | conformance/environment_setting |  |
| docs/programs.md#build_settings | covered | tests/build_settings |  |
| docs/programs.md#unknown_flag_error | covered | conformance/flag_value |  |
| docs/programs.md#build_over_package | covered | tests/build_precedence |  |
| docs/programs.md#target_system | covered | tests/build_settings |  |
| docs/programs.md#loaded_program | ported | tests/loaded_program | |
| docs/reflection.md#every_class_an_object | covered | tests/reflection_objects | with tests/class_walk for source_files |
| docs/reflection.md#class_and_instance | ported | tests/class_and_instance | instance-bound function lookup not covered elsewhere |
| docs/reflection.md#declaration_value | covered | conformance/reflection_declaration_value |  |
| docs/reflection.md#passing_a_class | covered | tests/class_argument |  |
| docs/reflection.md#namespace_walk | covered | tests/namespace_objects |  |
| docs/reflection.md#access_report | covered | tests/function_accesses |  |
| docs/reflection.md#constant_selection | covered | tests/reflection_walks | is_mutated in tests/run_time_kinds |
| docs/reflection.md#constant_namespaces | covered | tests/reflection_walks |  |
| docs/reflection.md#attribute_objects | covered | tests/attribute_object |  |
| docs/reflection.md#run_time_type | ported | conformance/run_time_type |  |
| docs/reflection.md#live_registry | not applicable (D662) | - | Monster.instances, live instances are not observable |
| docs/reflection.md#reflection_names | covered | conformance/reflection_names |  |
| docs/reflection.md#reflection_read_only | covered | conformance/reflection_read_only |  |
| docs/repl.md#repl_program | ported | tests/repl_program |  |
| docs/repl.md#repl_world | covered | tests/interactive_singletons |  |
| docs/repl.md#repl_radio | covered | tests/classes_singleton_per_arguments |  |
| docs/repl.md#hot_counter | ported | tests/hot_counter |  |
| docs/repl.md#live_party | ported | tests/live_party |  |
| docs/repl.md#recooking | ported | tests/recooking | needs the Reload singleton (not in the new library); fails: the library has no `Reload` singleton (`reload` of a `--hot-reload` build) |
| docs/standard_library.md#string_members | covered | tests/string_methods |  |
| docs/standard_library.md#file_tasks | covered | tests/file_text_basics |  |
| docs/standard_library.md#byte_records | covered | tests/binary_files |  |
| docs/standard_library.md#moving_files | covered | tests/moving_files |  |
| docs/standard_library.md#mapped_records | ported | tests/file_as_buffer |  |
| docs/standard_library.md#directory_tasks | covered | tests/system | filter_directories; create/exists in tests/directory_missing |
| docs/standard_library.md#directory_walk | covered | tests/directory_entries |  |
| docs/standard_library.md#watch_folder | covered | tests/file_watching |  |
| docs/standard_library.md#process_tasks | covered | tests/process_settings |  |
| docs/standard_library.md#printable_doc | covered | tests/printable_values |  |
| docs/standard_library.md#debug_doc | covered | tests/debug_values |  |
| docs/standard_library.md#console_input_doc | covered | tests/console_input |  |
| docs/standard_library.md#program_basics | covered | tests/program_environment |  |
| docs/standard_library.md#clock_basics | covered | tests/clock_reading |  |
| docs/standard_library.md#byte_numbers | covered | tests/buffer_bytes |  |
| docs/style.md#declaration_order_error | covered | conformance/declaration_order |  |
| docs/style.md#lint_error | covered | conformance/abbreviated_name |  |
| docs/style.md#single_letter | covered | conformance/single_letter_name |  |
| docs/style.md#unused_error | covered | conformance/unused_names |  |
| docs/style.md#written_not_read | covered | conformance/written_not_read |  |
| docs/style.md#unused_marker | ported | conformance/unused_marker | an attribute read only through reflection is still unread |
| docs/style.md#prose_comment_error | covered | conformance/comment_prose_comment |  |
| docs/style.md#call_argument_error | covered | conformance/call_as_argument |  |
| docs/style.md#call_argument_fixed | ported | tests/call_argument_fixed |  |
| docs/style.md#constructor_argument_error | covered | conformance/constructor_as_argument |  |
| docs/style.md#repeated_branch_error | covered | conformance/repeated_branch_call |  |
| docs/style.md#computed_receiver_error | covered | conformance/reflection_names_function |  |
| docs/testing.md#test_package | covered | tests/testing_discovery |  |
| docs/time.md#time_reading_is_not_instant | covered | conformance/time_instant_against_local |  |
| docs/time.md#time_instants | covered | tests/instant_arithmetic |  |
| docs/time.md#time_benchmark | covered | tests/time_benchmark |  |
| docs/time.md#time_calendar | covered | tests/calendar_math |  |
| docs/time.md#time_zones_named | covered | tests/daylight_saving |  |
| docs/time.md#time_tomorrow | covered | tests/daylight_saving |  |
| docs/time.md#time_offsets | covered | tests/fixed_offsets |  |
| docs/time.md#time_text_reading | covered | tests/time_text_round_trips | with tests/time_text_errors |
| docs/values_and_types.md#numeric_overflow | covered | tests/wrapping_functions |  |
| docs/values_and_types.md#wrapping_by_name | covered | tests/wrapping_functions |  |
| docs/values_and_types.md#wider_first | covered | conformance/wider_multiplication_order |  |
| docs/values_and_types.md#constant_overflow | covered | conformance/constant_overflow |  |
| docs/values_and_types.md#casting_edge | covered | conformance/comparison_float_right |  |
| docs/values_and_types.md#casting_fixed | ported | tests/casting_fixed |  |
| docs/values_and_types.md#number_methods | covered | tests/live_settings | reopened Integer; bits() in tests/maths_folding |
| docs/values_and_types.md#bitwise_basics | covered | tests/bitwise_functions |  |
| docs/values_and_types.md#maths_basics | covered | tests/maths_functions |  |
| docs/values_and_types.md#number_type_doc | covered | tests/number_type |  |
| docs/values_and_types.md#number_parameter_doc | not applicable (D1733) | - | a parameter typed Number |
| docs/values_and_types.md#lone_hole | covered | conformance/lone_hole |  |
| docs/values_and_types.md#direct_text | covered | tests/direct_text |  |
| docs/values_and_types.md#text_building | covered | tests/text_building |  |
| docs/values_and_types.md#string_basics | covered | tests/string_methods |  |
| docs/values_and_types.md#enum_basics | covered | tests/enum_qualified_names |  |
| docs/values_and_types.md#enum_from_text | covered | tests/text_to_enum |  |
| docs/values_and_types.md#enum_walk | covered | tests/enum_values_walk |  |
| docs/values_and_types.md#numbered_roles | covered | tests/numbered_enums |  |
| docs/values_and_types.md#role_number_twice | covered | conformance/enum_numbers |  |
| docs/values_and_types.md#union_basics | covered | tests/class_test |  |
| docs/values_and_types.md#duck_typing | covered | tests/class_through_shape |  |
| docs/values_and_types.md#shape_functions_doc | covered | tests/shape_functions |  |
| docs/values_and_types.md#shape_values_doc | covered | tests/shape_members |  |
