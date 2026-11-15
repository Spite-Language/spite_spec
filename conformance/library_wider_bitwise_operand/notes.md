`spite_standard_library/common/masks.spite` sits where the compiler takes a file as standard library code, so this
checks that the library is held to the wider-operand rule of `bits_and`, `bits_or` and `bits_exclusive_or` like a
program (D748 exempted it until spite_compiler#19).
