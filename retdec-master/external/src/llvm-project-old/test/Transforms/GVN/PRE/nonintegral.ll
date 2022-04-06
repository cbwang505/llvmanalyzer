; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt -gvn -S < %s | FileCheck %s

target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128-ni:4"
target triple = "x86_64-unknown-linux-gnu"

define void @nipre(double addrspace(4)** noalias %p, i64 addrspace(4)** noalias %p2, i8 %jmp) {

; CHECK-LABEL: @nipre(
; CHECK:    [[PCAST:%.*]] = bitcast double addrspace(4)** [[P:%.*]] to i64 addrspace(4)**
; CHECK:       a:
; CHECK:    [[L1:%.*]] = load i64 addrspace(4)*, i64 addrspace(4)** [[PCAST]]
; CHECK:    [[TMP0:%.*]] = bitcast i64 addrspace(4)* [[L1]] to double addrspace(4)*
; CHECK:       b:
; CHECK:    [[L2:%.*]] = load i64 addrspace(4)*, i64 addrspace(4)** [[PCAST]]
; CHECK:    [[TMP1:%.*]] = bitcast i64 addrspace(4)* [[L2]] to double addrspace(4)*
; CHECK:       c:
; CHECK-NEXT:    [[L3_PRE:%.*]] = load double addrspace(4)*, double addrspace(4)** %p

entry:
  %pcast = bitcast double addrspace(4)** %p to i64 addrspace(4)**
  switch i8 %jmp, label %c [ i8 0, label %a
  i8 1, label %b]
a:
  %l1 = load i64 addrspace(4)*, i64 addrspace(4)** %pcast
  store i64 addrspace(4)* %l1, i64 addrspace(4)** %p2
  br label %tail
b:
  %l2 = load i64 addrspace(4)*, i64 addrspace(4)** %pcast
  store i64 addrspace(4)* %l2, i64 addrspace(4)** %p2
  br label %tail
c:
  br label %tail
tail:
  %l3 = load double addrspace(4)*, double addrspace(4)** %p
  %l3cast = bitcast double addrspace(4)* %l3 to i64 addrspace(4)*
  store i64 addrspace(4)* %l3cast, i64 addrspace(4)** %p2
  ret void
}