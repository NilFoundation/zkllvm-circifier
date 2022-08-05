; RUN: llc -march=evm -filetype=asm < %s | FileCheck %s

define i256 @am1(i256* %a) nounwind {
; CHECK-LABEL: am1:
  %1 = load i256, i256* %a
; CHECK: MLOAD
  ret i256 %1
}

@foo = external global i256

define i256 @am2() nounwind {
; CHECK-LABEL: am2:
  %1 = load i256, i256* @foo
; CHECK: MLOAD
  ret i256 %1
}

