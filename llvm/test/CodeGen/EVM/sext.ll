; RUN: llc < %s -mtriple=evm -filetype=asm | FileCheck %s

define i256 @sext1(i8 %x, i8 %y) {
; CHECK-LABEL: sext1:
  %1 = sext i8 %y to i256
; CHECK: SIGNEXTEND
  ret i256 %1
}

define i256 @sext2(i8 %x) {
; CHECK-LABEL: sext2:
  %1 = sext i8 %x to i256
; CHECK: SIGNEXTEND
  ret i256 %1
}
