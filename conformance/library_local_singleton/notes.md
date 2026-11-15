`spite_standard_library/common/reporter.spite` sits where the compiler takes a file as standard library code, so this
checks that the library is held to the rule that a singleton is bound beside the attributes, never as a local, like a
program (D680 exempted it until spite_compiler#56).
