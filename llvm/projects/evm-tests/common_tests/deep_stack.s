// Testing SWAP and DUP instructions with arbitrary stack depth

// EVM_RUN: function: test, input: []
    .type	test,@function
    .globl  test
test:
    JUMPDEST
    PUSH1 1  // 1
    PUSH1 2  // 1 2
    PUSH1 1  // 1 2 1
    SWAP     // 2 1
    SUB      // -1
    PUSH1 1  // -1 0
    DUP      // -1 -1
    SUB      // 0
    PUSH4 failed
    JUMPI
    JUMP
failed:
    JUMPDEST
    INVALID

