	.text
	.file	"bubble.ll"
	.globl	main                    # -- Begin function main
	.type	main,@function
main:                                   # @main
# %bb.0:                                # %entry
	JUMPDEST
	PUSH1 	128
	PUSH1 	64
	MSTORE
	PUSH2 	sort
	PUSH1 	0
	MLOAD
	DUP1
	PUSH1 	32
	ADD
	MSTORE
	PUSH1 	0
	MLOAD
	PUSH1 	64
	ADD
	DUP1
	PUSH1 	0
	MSTORE
	PUSH1 	32
	MSTORE
	PUSH1 	4
	GETPC
	ADD
	SWAP1                           # putlocal: 0

	JUMP
	JUMPDEST
	PUSH1 	0
	MLOAD
	PUSH1 	32
	SWAP1
	SUB
	MLOAD
	PUSH1 	0
	MSTORE
	PUSH1 	0
	SWAP1
	DUP2
	MSTORE
	PUSH1 	32
	SWAP1
	RETURN
Lfunc_end0:
	.size	main, Lfunc_end0-main
                                        # -- End function
	.globl	sort                    # -- Begin function sort
	.type	sort,@function
sort:                                   # @sort
# %bb.0:
	JUMPDEST
	PUSH1 	0
	MLOAD
	PUSH2 	256
	ADD
	MSTORE                          # putlocal: 8

	PUSH1 	128
	PUSH1 	32
	PUSH1 	0
	MLOAD
	ADD
	ADD
	PUSH1 	0
	MLOAD
	PUSH2 	288
	ADD
	MSTORE                          # putlocal: 9

	PUSH1 	2
	PUSH1 	0
	MLOAD
	PUSH2 	320
	ADD
	MSTORE                          # putlocal: 10

	PUSH1 	0
	MLOAD
	PUSH2 	320
	ADD
	MLOAD                           # putlocal: 10

	PUSH1 	0
	MLOAD
	PUSH2 	288
	ADD
	MLOAD                           # putlocal: 9

	MSTORE
	PUSH1 	96
	PUSH1 	32
	PUSH1 	0
	MLOAD
	ADD
	ADD
	PUSH1 	0
	MLOAD
	PUSH2 	352
	ADD
	MSTORE                          # putlocal: 11

	PUSH1 	1
	PUSH1 	0
	MLOAD
	PUSH2 	384
	ADD
	MSTORE                          # putlocal: 12

	PUSH1 	0
	MLOAD
	PUSH2 	384
	ADD
	MLOAD                           # putlocal: 12

	PUSH1 	0
	MLOAD
	PUSH2 	352
	ADD
	MLOAD                           # putlocal: 11

	MSTORE
	PUSH1 	64
	PUSH1 	32
	PUSH1 	0
	MLOAD
	ADD
	ADD
	PUSH1 	0
	MLOAD
	PUSH2 	416
	ADD
	MSTORE                          # putlocal: 13

	PUSH1 	3
	PUSH1 	0
	MLOAD
	PUSH2 	448
	ADD
	MSTORE                          # putlocal: 14

	PUSH1 	0
	MLOAD
	PUSH2 	448
	ADD
	MLOAD                           # putlocal: 14

	PUSH1 	0
	MLOAD
	PUSH2 	416
	ADD
	MLOAD                           # putlocal: 13

	MSTORE
	PUSH1 	32
	PUSH1 	32
	PUSH1 	0
	MLOAD
	ADD
	ADD
	PUSH1 	0
	MLOAD
	PUSH2 	480
	ADD
	MSTORE                          # putlocal: 15

	PUSH1 	4
	PUSH1 	0
	MLOAD
	PUSH2 	512
	ADD
	MSTORE                          # putlocal: 16

	PUSH1 	0
	MLOAD
	PUSH2 	512
	ADD
	MLOAD                           # putlocal: 16

	PUSH1 	0
	MLOAD
	PUSH2 	480
	ADD
	MLOAD                           # putlocal: 15

	MSTORE
	PUSH1 	5
	PUSH1 	0
	MLOAD
	PUSH2 	544
	ADD
	MSTORE                          # putlocal: 17

	PUSH1 	32
	PUSH1 	0
	MLOAD
	ADD
	PUSH1 	0
	MLOAD
	PUSH2 	544
	ADD
	MLOAD                           # putlocal: 17

	SWAP1
	MSTORE
	PUSH2 	bubbleSort
	PUSH1 	32
	PUSH1 	0
	MLOAD
	ADD
	DUP2
	PUSH1 	0
	MLOAD
	PUSH2 	544
	ADD
	MLOAD                           # putlocal: 17

	DUP3
	PUSH1 	0
	MLOAD
	DUP1
	PUSH2 	608
	ADD
	MSTORE
	PUSH1 	0
	MLOAD
	PUSH2 	640
	ADD
	DUP1
	PUSH1 	0
	MSTORE
	PUSH1 	32
	MSTORE
	PUSH1 	4
	GETPC
	ADD
	SWAP3                           # putlocal: 0

	JUMP
	JUMPDEST
	PUSH1 	0
	MLOAD
	PUSH1 	32
	SWAP1
	SUB
	MLOAD
	PUSH1 	0
	MSTORE
	POP
	POP
	PUSH1 	32
	PUSH1 	0
	MLOAD
	ADD
	MLOAD
	PUSH1 	0
	MLOAD
	PUSH2 	384
	ADD
	MLOAD                           # putlocal: 12

	SWAP1
	EQ
	ISZERO
	PUSH2 	LBB1_6
	JUMPI
	PUSH2 	LBB1_1
	JUMP
LBB1_1:
	JUMPDEST
	PUSH1 	0
	MLOAD
	PUSH2 	480
	ADD
	MLOAD                           # putlocal: 15

	MLOAD
	PUSH1 	0
	MLOAD
	PUSH2 	320
	ADD
	MLOAD                           # putlocal: 10

	SWAP1
	EQ
	ISZERO
	PUSH2 	LBB1_6
	JUMPI
	PUSH2 	LBB1_2
	JUMP
LBB1_2:
	JUMPDEST
	PUSH1 	0
	MLOAD
	PUSH2 	416
	ADD
	MLOAD                           # putlocal: 13

	MLOAD
	PUSH1 	0
	MLOAD
	PUSH2 	448
	ADD
	MLOAD                           # putlocal: 14

	SWAP1
	EQ
	ISZERO
	PUSH2 	LBB1_6
	JUMPI
	PUSH2 	LBB1_3
	JUMP
LBB1_3:
	JUMPDEST
	PUSH1 	0
	MLOAD
	PUSH2 	352
	ADD
	MLOAD                           # putlocal: 11

	MLOAD
	PUSH1 	0
	MLOAD
	PUSH2 	512
	ADD
	MLOAD                           # putlocal: 16

	SWAP1
	EQ
	ISZERO
	PUSH2 	LBB1_6
	JUMPI
	PUSH2 	LBB1_4
	JUMP
LBB1_4:
	JUMPDEST
	PUSH1 	0
	MLOAD
	PUSH2 	288
	ADD
	MLOAD                           # putlocal: 9

	MLOAD
	PUSH1 	0
	MLOAD
	PUSH2 	544
	ADD
	MLOAD                           # putlocal: 17

	SWAP1
	EQ
	ISZERO
	PUSH2 	LBB1_6
	JUMPI
	PUSH2 	LBB1_5
	JUMP
LBB1_5:
	JUMPDEST
	PUSH1 	0
	MLOAD
	PUSH1 	0
	MLOAD
	PUSH2 	384
	ADD
	MLOAD                           # putlocal: 12

	SWAP1
	MSTORE
	PUSH1 	0
	MLOAD
	MLOAD
	PUSH1 	0
	MLOAD
	PUSH2 	288
	ADD
	MSTORE                          # putlocal: 9

	PUSH1 	0
	MLOAD
	PUSH2 	288
	ADD
	MLOAD                           # putlocal: 9

	PUSH1 	0
	MLOAD
	PUSH2 	256
	ADD
	MLOAD                           # putlocal: 8

	JUMP
LBB1_6:
	JUMPDEST
	PUSH1 	0
	PUSH1 	0
	MLOAD
	MSTORE
	PUSH1 	0
	MLOAD
	MLOAD
	PUSH1 	0
	MLOAD
	PUSH2 	288
	ADD
	MSTORE                          # putlocal: 9

	PUSH1 	0
	MLOAD
	PUSH2 	288
	ADD
	MLOAD                           # putlocal: 9

	PUSH1 	0
	MLOAD
	PUSH2 	256
	ADD
	MLOAD                           # putlocal: 8

	JUMP
Lfunc_end1:
	.size	sort, Lfunc_end1-sort
                                        # -- End function
	.globl	bubbleSort              # -- Begin function bubbleSort
	.type	bubbleSort,@function
bubbleSort:                             # @bubbleSort
# %bb.0:
	JUMPDEST
	SWAP2
	PUSH1 	0
	MLOAD
	PUSH2 	256
	ADD
	MSTORE                          # putlocal: 8

	PUSH1 	32
	PUSH1 	0
	MLOAD
	ADD
	MSTORE
	PUSH1 	0
	MLOAD
	MSTORE
	PUSH1 	0
	PUSH1 	0
	MLOAD
	PUSH2 	288
	ADD
	MSTORE                          # putlocal: 9

	PUSH1 	64
	PUSH1 	0
	MLOAD
	ADD
	PUSH1 	0
	MLOAD
	PUSH2 	288
	ADD
	MLOAD                           # putlocal: 9

	SWAP1
	MSTORE
	PUSH32 	-1
	PUSH1 	0
	MLOAD
	PUSH2 	320
	ADD
	MSTORE                          # putlocal: 10

	PUSH1 	5
	PUSH1 	0
	MLOAD
	PUSH2 	352
	ADD
	MSTORE                          # putlocal: 11

LBB2_1:                                 # =>This Loop Header: Depth=1
                                        #     Child Loop BB2_3 Depth 2
	JUMPDEST
	PUSH1 	32
	PUSH1 	0
	MLOAD
	ADD
	MLOAD
	PUSH1 	0
	MLOAD
	PUSH2 	320
	ADD
	MLOAD                           # putlocal: 10

	SWAP1
	ADD
	PUSH1 	64
	PUSH1 	0
	MLOAD
	ADD
	MLOAD
	SLT
	ISZERO
	PUSH2 	LBB2_8
	JUMPI
	PUSH2 	LBB2_2
	JUMP
LBB2_2:                                 #   in Loop: Header=BB2_1 Depth=1
	JUMPDEST
	PUSH1 	96
	PUSH1 	0
	MLOAD
	ADD
	PUSH1 	0
	MLOAD
	PUSH2 	288
	ADD
	MLOAD                           # putlocal: 9

	SWAP1
	MSTORE
LBB2_3:                                 #   Parent Loop BB2_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	JUMPDEST
	PUSH1 	64
	PUSH1 	0
	MLOAD
	ADD
	MLOAD
	PUSH1 	0
	MLOAD
	PUSH2 	320
	ADD
	MLOAD                           # putlocal: 10

	SWAP1
	XOR
	PUSH1 	32
	PUSH1 	0
	MLOAD
	ADD
	MLOAD
	SWAP1
	ADD
	PUSH1 	96
	PUSH1 	0
	MLOAD
	ADD
	MLOAD
	SLT
	ISZERO
	PUSH1 	1
	PUSH1 	0
	MLOAD
	PUSH2 	288
	ADD
	MSTORE                          # putlocal: 9

	PUSH2 	LBB2_7
	JUMPI
	PUSH2 	LBB2_4
	JUMP
LBB2_4:                                 #   in Loop: Header=BB2_3 Depth=2
	JUMPDEST
	PUSH1 	96
	PUSH1 	0
	MLOAD
	ADD
	MLOAD
	PUSH1 	0
	MLOAD
	PUSH2 	352
	ADD
	MLOAD                           # putlocal: 11

	SHL
	PUSH1 	0
	MLOAD
	MLOAD
	ADD
	DUP1
	MLOAD
	PUSH1 	32
	PUSH1 	0
	MLOAD
	PUSH2 	384
	ADD
	MSTORE                          # putlocal: 12

	SWAP1
	PUSH1 	0
	MLOAD
	PUSH2 	384
	ADD
	MLOAD                           # putlocal: 12

	SWAP1
	ADD
	MLOAD
	SWAP1
	SGT
	ISZERO
	PUSH2 	LBB2_6
	JUMPI
	PUSH2 	LBB2_5
	JUMP
LBB2_5:                                 #   in Loop: Header=BB2_3 Depth=2
	JUMPDEST
	PUSH1 	96
	PUSH1 	0
	MLOAD
	ADD
	MLOAD
	PUSH1 	0
	MLOAD
	PUSH2 	352
	ADD
	MLOAD                           # putlocal: 11

	SHL
	PUSH1 	0
	MLOAD
	MLOAD
	ADD
	PUSH1 	0
	MLOAD
	PUSH2 	384
	ADD
	MLOAD                           # putlocal: 12

	DUP2
	ADD
	PUSH2 	swap
	DUP1
	DUP3
	DUP5
	PUSH1 	0
	MLOAD
	DUP1
	PUSH2 	448
	ADD
	MSTORE
	PUSH1 	0
	MLOAD
	PUSH2 	480
	ADD
	DUP1
	PUSH1 	0
	MSTORE
	PUSH1 	32
	MSTORE
	PUSH1 	4
	GETPC
	ADD
	SWAP3                           # putlocal: 0

	JUMP
	JUMPDEST
	PUSH1 	0
	MLOAD
	PUSH1 	32
	SWAP1
	SUB
	MLOAD
	PUSH1 	0
	MSTORE
	POP
	POP
	POP
	SWAP2
LBB2_6:                                 #   in Loop: Header=BB2_3 Depth=2
	JUMPDEST
	PUSH1 	96
	PUSH1 	0
	MLOAD
	ADD
	MLOAD
	PUSH1 	0
	MLOAD
	PUSH2 	288
	ADD
	MLOAD                           # putlocal: 9

	SWAP1
	ADD
	PUSH1 	96
	PUSH1 	0
	MLOAD
	ADD
	MSTORE
	PUSH2 	LBB2_3
	JUMP
LBB2_7:                                 #   in Loop: Header=BB2_1 Depth=1
	JUMPDEST
	PUSH1 	64
	PUSH1 	0
	MLOAD
	ADD
	MLOAD
	PUSH1 	0
	MLOAD
	PUSH2 	288
	ADD
	MLOAD                           # putlocal: 9

	SWAP1
	ADD
	PUSH1 	64
	PUSH1 	0
	MLOAD
	ADD
	MSTORE
	PUSH2 	LBB2_1
	JUMP
LBB2_8:
	JUMPDEST
	PUSH1 	0
	MLOAD
	PUSH2 	256
	ADD
	MLOAD                           # putlocal: 8

	JUMP
Lfunc_end2:
	.size	bubbleSort, Lfunc_end2-bubbleSort
                                        # -- End function
	.globl	swap                    # -- Begin function swap
	.type	swap,@function
swap:                                   # @swap
# %bb.0:
	JUMPDEST
	PUSH1 	0
	MLOAD
	DUP2
	SWAP1
	MSTORE
	PUSH1 	32
	PUSH1 	0
	MLOAD
	ADD
	DUP3
	SWAP1
	MSTORE
	DUP1
	MLOAD
	PUSH1 	64
	PUSH1 	0
	MLOAD
	ADD
	MSTORE
	SWAP1
	MLOAD
	SWAP1
	MSTORE
	PUSH1 	32
	PUSH1 	0
	MLOAD
	ADD
	MLOAD
	PUSH1 	64
	PUSH1 	0
	MLOAD
	ADD
	MLOAD
	SWAP1
	MSTORE
	JUMP
Lfunc_end3:
	.size	swap, Lfunc_end3-swap
                                        # -- End function
