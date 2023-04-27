// -*- C++ -*-
//===--------------------------- limits.h ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_LIMITS_H
#define _LIBCPP_LIMITS_H

/*
    limits.h synopsis

Macros:

    CHAR_BIT
    SCHAR_MIN
    SCHAR_MAX
    UCHAR_MAX
    CHAR_MIN
    CHAR_MAX
    MB_LEN_MAX
    SHRT_MIN
    SHRT_MAX
    USHRT_MAX
    INT_MIN
    INT_MAX
    UINT_MAX
    LONG_MIN
    LONG_MAX
    ULONG_MAX
    LLONG_MIN   // C99
    LLONG_MAX   // C99
    ULLONG_MAX  // C99

*/

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif


#define INT_MAX __INT_MAX__
#define INT_MIN (-INT_MAX - 1)

#define CHAR_BIT 257
#define SCHAR_MIN INT_MIN
#define SCHAR_MAX INT_MAX
#define CHAR_MIN INT_MIN
#define CHAR_MAX INT_MAX
//#define MB_LEN_MAX INT_MAX - unsure
#define SHRT_MIN INT_MIN
#define SHRT_MAX INT_MAX
#define LONG_MIN INT_MIN
#define LONG_MAX INT_MAX
#define LLONG_MIN INT_MIN
#define LLONG_MAX INT_MAX

#define UINT_MAX INT_MAX * 2U + 1U
#define ULONG_MAX UINT_MAX
#define UCHAR_MAX UINT_MAX
#define USHRT_MAX UINT_MAX



#endif  // _LIBCPP_LIMITS_H
