    .file	"memory.S"

.macro STDLIB_FUNC name
	.type	\name,function
	.hidden	\name
\name:
.endm

.macro STDLIB_FUNC_INVALID name
	STDLIB_FUNC \name
	JUMPDEST
    INVALID
.endm

################################ MEMSET ################################
# INPUT STACK: 1:mem, 2:value, 3:count
#
STDLIB_FUNC memset
    JUMPDEST

    # We can use memset label, but then debugger might be confused.
head1:
    JUMPDEST

    # Exit if count is zero
    DUP3
    ISZERO
    PUSH4   exit1
    JUMPI

    # Decrease count variable
    PUSH1 1
    DUP4
    SUB
    SWAP3
    POP

    # Write value to memory
    DUP2    # value
    DUP2    # mem
    MSTORE8

    # Increase memory variable
    DUP1
    PUSH1 1
    ADD
    SWAP1
    POP

    # Jump to loop head
    PUSH4 head1
    JUMP

exit1:
    JUMPDEST
    POP
    POP
    # TODO: We should return proper value, i.e. mem address
    #POP
    SWAP1
    JUMP

################################ MEMCPY ################################
# INPUT STACK: 1:dst, 2:src, 3:count
#
STDLIB_FUNC memcpy
    JUMPDEST

# We can use memcpy label, but then debugger might be confused.
head2:
    JUMPDEST

    # Exit if count is zero
    DUP3
    ISZERO
    PUSH4   exit2
    JUMPI

    # Exit if count is not mod of 32
    PUSH1 32
    DUP4
    MOD
    PUSH1 0
    EQ
    ISZERO
    PUSH4   fail
    JUMPI

    # Decrease count variable
    PUSH1 32
    DUP4
    SUB
    SWAP3
    POP

    # Copy memory
    DUP2     # src
    MLOAD
    DUP2     # dst
    MSTORE

    # Increase dst memory variable
    DUP1
    PUSH1 32
    ADD
    SWAP1
    POP

    # Increase src memory variable
    DUP2
    PUSH1 32
    ADD
    SWAP2
    POP

    # Jump to loop head
    PUSH4 head2
    JUMP

exit2:
    JUMPDEST
    POP
    POP
    # TODO: We should return proper value, i.e. dst address
    #POP
    SWAP1
    JUMP

fail:
    JUMPDEST
    INVALID

# TODO: Implement memmove
STDLIB_FUNC_INVALID memmove

# operator new(unsigned long)
STDLIB_FUNC_INVALID _Znwm
# operator new(unsigned long, std::align_val_t)
STDLIB_FUNC_INVALID _ZnwmSt11align_val_t
# operator delete(unsigned long)
STDLIB_FUNC_INVALID _ZdlPv
# operator delete(void*, std::align_val_t)
STDLIB_FUNC_INVALID _ZdlPvSt11align_val_t
# Exceptions is not supported in EVM
STDLIB_FUNC_INVALID __cxa_allocate_exception
STDLIB_FUNC_INVALID __cxa_begin_catch
STDLIB_FUNC_INVALID __cxa_pure_virtual
STDLIB_FUNC_INVALID __cxa_throw
# STDLIB_FUNC_INVALID _ZTISt14overflow_error
# Assert
STDLIB_FUNC_INVALID _wassert
# std::terminate
STDLIB_FUNC_INVALID _ZSt9terminatev
# abort
STDLIB_FUNC_INVALID abort

STDLIB_FUNC _ZTVN10__cxxabiv121__vmi_class_type_infoE
    STOP # zero

STDLIB_FUNC _ZTVN10__cxxabiv117__class_type_infoE
    STOP # zero
