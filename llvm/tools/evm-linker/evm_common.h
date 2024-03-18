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
#ifndef LLVM_EVM_COMMON_H
#define LLVM_EVM_COMMON_H

#include "llvm/ADT/Twine.h"
#include <iostream>

// Offset to the data where init data will be located.
static constexpr unsigned InitDataOffset = 32 * 2;

// Slot of the size of the arbitrary-sized return data.
static constexpr unsigned ReturnSizeOffset = 32 * 1;

// Default size of memory chunk to be fixed by resolved symbol relocation.
static constexpr unsigned SymbolRelocSizeInBytes = 4;

// Size of function Id, which is hash(signature)[0..4],
// passed to the function selector.
static constexpr unsigned SizeOfFuncId = 4;

class Logger {
public:
  static inline bool Enabled = false;
};

#define LOG()  Logger::Enabled && std::cerr


#endif // LLVM_EVM_COMMON_H
