; RUN: llc -O0 < %s | FileCheck %s

; CHECK: JUMPDEST
; CHECK: PUSH1 18
; CHECK-NEXT: {{DUP$}}
; CHECK: PUSH1 17
; CHECK-NEXT: {{SWAP$}}
define i64 @dup(i64 %a0, i64 %a1, i64 %a2, i64 %a3, i64 %a4, i64 %a5, i64 %a6, i64 %a7, i64 %a8, i64 %a9,
                i64 %a10, i64 %a11, i64 %a12, i64 %a13, i64 %a14, i64 %a15, i64 %a16, i64 %a17) {
    %sum.0 = add nuw i64 %a0, %a17
    %sum.0.1 = add nuw i64 %sum.0, %a17
    %sum.1 = add nuw i64 %sum.0.1, %a2
    %sum.2 = add nuw i64 %sum.1, %a3
    %sum.3 = add nuw i64 %sum.2, %a4
    %sum.4 = add nuw i64 %sum.3, %a5
    %sum.5 = add nuw i64 %sum.4, %a6
    %sum.6 = add nuw i64 %sum.5, %a7
    %sum.7 = add nuw i64 %sum.6, %a8
    %sum.8 = add nuw i64 %sum.7, %a9
    %sum.9 = add nuw i64 %sum.8, %a10
    %sum.10 = add nuw i64 %sum.9, %a11
    %sum.11 = add nuw i64 %sum.10, %a12
    %sum.12 = add nuw i64 %sum.11, %a13
    %sum.13 = add nuw i64 %sum.12, %a14
    %sum.14 = add nuw i64 %sum.13, %a15
    %sum.15 = add nuw i64 %sum.14, %a16
    %sum.16 = add nuw i64 %sum.15, %a1
    ret i64 %sum.16
}
