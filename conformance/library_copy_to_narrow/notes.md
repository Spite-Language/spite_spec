`spite_standard_library/common/holder.spite` sits where the compiler takes a file as standard library code, so this
checks that the library is held to the rule that a local only copying a path to narrow it is refused, like a program
(D680 exempted it until spite_compiler#56). The library's files are never rewritten, so the fix a program gets is an
error here.
