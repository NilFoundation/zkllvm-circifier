//---------------------------------------------------------------------------//
// Copyright 2022 =nil; Foundation
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//
#ifndef LLVM_EVMSTACK_H
#define LLVM_EVMSTACK_H

#include "llvm/CodeGen/Register.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Casting.h"

//#include <array>
#include <iostream>

namespace llvm  {


class StackElement {
public:

  using ValueType = unsigned;

  static constexpr ValueType INVALID_VALUE = std::numeric_limits<unsigned>::max();

  StackElement() = default;
  explicit StackElement(ValueType V) : StackElement(V, 0) {}
  StackElement(Register R, uint16_t Users)
      : StackElement(Register::virtReg2Index(R), Users) {}
  StackElement(ValueType V, uint16_t Users) : Value(V), NumUsers(Users) {}

  ValueType getValue() const {
    return Value;
  }

  bool isValid() const {
    return getValue() != INVALID_VALUE;
  }

  void consume() {
    assert(NumUsers != 0);
    NumUsers--;
  }

  bool isDead() const {
    return NumUsers == 0;
  }

  void addUser() {
    NumUsers++;
  }

  unsigned getNumUsers() const {
    return NumUsers;
  }

  void addUsers(unsigned Users) {
    NumUsers += Users;
  }

  template<typename T>
  void dump(T& OS) {
    OS << '%' << Value << '-' << NumUsers;
  }

  void dump() {
    dump(std::cerr);
  }

private:
  ValueType Value{INVALID_VALUE};
  uint16_t NumUsers{0};
};

class Stack {
public:

  static constexpr int INVALID_DEPTH = -1;

  Stack() {
    Vector.reserve(1024);
  }

  unsigned size() const {
    return Vector.size();
  }

  bool empty() const {
    return size() == 0;
  }

  void push(StackElement::ValueType V, unsigned NumUsers) {
    Vector.emplace_back(V, NumUsers);
  }

  void push(Register Reg, unsigned NumUsers) {
    push(Register::virtReg2Index(Reg), NumUsers);
  }

  void pop() {
    Vector.pop_back();
  }

  StackElement& top() {
    return Vector.back();
  }

  StackElement& dup(int Index) {
    push(operator[](Index).getValue(), 0);
    return Vector.back();
  }
  void swap(int Index) {
    assert(Index >= 0 && static_cast<unsigned>(Index) < size());
    std::iter_swap(Vector.rbegin(), Vector.rbegin() + Index);
  }

  int find(Register Reg) {
    return find(Register::virtReg2Index(Reg));
  }

  int find(StackElement::ValueType Value) {
    for (auto It = Vector.rbegin(); It != Vector.rend(); ++It) {
      if (It->getValue() == Value)
        return std::distance(Vector.rbegin(), It);
    }
    return -1;
  }

  StackElement& operator[](unsigned Index) {
    assert(Index < size());
    return Vector[size() - Index - 1];
  }

  static bool isValidIndex(int I) {
    return I >= 0;
  }

  template<typename T>
  void dump(T& OS) {
    OS << "Stack: ";
    const char* Sep = "";
    for (unsigned i = 0; i < size(); i++) {
      OS << Sep;
      OS << i << ":";
      operator[](i).dump(OS);
      Sep = ", ";
    }
    OS << '\n';
  }

  void dump() {
    dump(std::cerr);
  }

private:
  std::vector<StackElement> Vector;
};

template<typename T>
T &operator<<(T &OS, Stack& S) {
  S.dump(OS);
  return OS;
}

} // namespace llvm

#endif // LLVM_EVMSTACK_H
