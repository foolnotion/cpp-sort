/*
 * Copyright (c) 2016-2023 Morwenn
 * SPDX-License-Identifier: MIT
 */
#ifndef CPPSORT_DETAIL_CONFIG_H_
#define CPPSORT_DETAIL_CONFIG_H_

////////////////////////////////////////////////////////////
// Make <version> available when possible

// config.h is what should be included to get configuration
// information, which includes standard library feature-test
// macros when available

#if __has_include(<version>)
#   include <version>
#endif

////////////////////////////////////////////////////////////
// Check for __has_* macros

#ifndef __has_include
#   define __has_include(x) 0
#endif

#ifndef __has_cpp_attribute
#   define __has_cpp_attribute(x) 0
#endif

////////////////////////////////////////////////////////////
// Check for C++17 features

// Make sure that there is a single variable before C++17,
// default to static in C++14 to avoid ODR issues

#if defined(__cpp_inline_variables)
#   define CPPSORT_INLINE_VARIABLE inline
#else
#   define CPPSORT_INLINE_VARIABLE static
#endif

////////////////////////////////////////////////////////////
// Check for C++20 features

// There is no feature-test macro for std::identity that can
// be used reliably, so we have to fall back to checking
// compiler and standard versions

#if defined(__cpp_lib_ranges)
#   define CPPSORT_STD_IDENTITY_AVAILABLE 1
#elif defined(__GNUC__)
#   if __GNUC__ > 9 && __cplusplus > 201703L
#       define CPPSORT_STD_IDENTITY_AVAILABLE 1
#   else
#       define CPPSORT_STD_IDENTITY_AVAILABLE 0
#   endif
#else
#   define CPPSORT_STD_IDENTITY_AVAILABLE 0
#endif

////////////////////////////////////////////////////////////
// General: assertions

// Ensure that enabling audits always enables assertions
#if defined(CPPSORT_ENABLE_AUDITS)
#   define CPPSORT_ENABLE_ASSERTIONS
#endif

#if defined(CPPSORT_ENABLE_ASSERTIONS)
#   include <cassert>
#   if defined(CPPSORT_USE_LIBASSERT)
#       include <assert.hpp>
#   endif
#endif

////////////////////////////////////////////////////////////
// CPPSORT_ASSERT

// We want a slightly finer-grain control over assertions
// than just relying on NDEBUG, so assertions have to be
// explicitly enabled in cpp-sort

#if !defined(NDEBUG) && defined(CPPSORT_ENABLE_ASSERTIONS)
#   if !defined(CPPSORT_ASSERT)
#       if defined(CPPSORT_USE_LIBASSERT)
#           define CPPSORT_ASSERT(...) ASSERT(__VA_ARGS__)
#       else
#           define CPPSORT_ARG2(_0, _1, _2, ...) _2
#           define CPPSORT_NARG2(...) CPPSORT_ARG2(__VA_ARGS__, 2, 1, 0)
#           define CPPSORT_ONE_OR_TWO_ARGS_1(condition) assert(condition)
#           define CPPSORT_ONE_OR_TWO_ARGS_2(condition, message) assert(condition && message)
#           define CPPSORT_ONE_OR_TWO_ARGS_N(N, ...) CPPSORT_ONE_OR_TWO_ARGS_##N(__VA_ARGS__)
#           define CPPSORT_ONE_OR_TWO_ARGS(N, ...) CPPSORT_ONE_OR_TWO_ARGS_N(N, __VA_ARGS__)
#           define CPPSORT_ASSERT(...) CPPSORT_ONE_OR_TWO_ARGS(CPPSORT_NARG2(__VA_ARGS__), __VA_ARGS__)
#       endif
#   endif
#else
#   define CPPSORT_ASSERT(...) ((void)0)
#endif

////////////////////////////////////////////////////////////
// CPPSORT_AUDIT

// Some debug checks might be way too expensive for most
// scenarios, but still of great help when debugging tough
// problems, hence this audit feature

#if !defined(NDEBUG) && defined(CPPSORT_ENABLE_AUDITS)
#   ifndef CPPSORT_AUDIT
#       define CPPSORT_AUDIT(...) CPPSORT_ASSERT(__VA_ARGS__)
#   endif
#else
#   define CPPSORT_AUDIT(...) ((void)0)
#endif

////////////////////////////////////////////////////////////
// CPPSORT_ASSUME

// Assumptions may help the compiler to remove unnecessary code;
// some parts of the library may be significantly slower if this
// assumption mechanism isn't supported. In audit mode, we want
// to turn them into assertions to make sure the assumptions are
// actually correct.

#if defined(CPPSORT_ENABLE_AUDITS)
#   define CPPSORT_ASSUME(...) CPPSORT_ASSERT(__VA_ARGS__)
#elif defined(__GNUC__)
#   define CPPSORT_ASSUME(expression) do { if (!(expression)) __builtin_unreachable(); } while(0)
#elif defined(__clang__)
#   define CPPSORT_ASSUME(expression) __builtin_assume(expression)
#elif defined(_MSC_VER)
#   define CPPSORT_ASSUME(expression) __assume(expression)
#else
#   define CPPSORT_ASSUME(cond) ((void)0)
#endif

////////////////////////////////////////////////////////////
// CPPSORT_UNREACHABLE

// Mostly useful to silence compiler warnings in the default
// clause of a switch when we know the default can never be
// reached

#if !defined(NDEBUG) && defined(CPPSORT_ENABLE_AUDITS)
#   define CPPSORT_UNREACHABLE CPPSORT_ASSERT(false, "unreachable")
#elif defined(__GNUC__) || defined(__clang__)
#   define CPPSORT_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#   define CPPSORT_UNREACHABLE __assume(false)
#else
#   define CPPSORT_UNREACHABLE ((void)0)
#endif

////////////////////////////////////////////////////////////
// CPPSORT_DEPRECATED

// [[deprecated]] is available since C++14, hence why this
// macro is not in attributes.h: we hide it behind a macro
// to make sure that it can be silenced

#ifndef CPPSORT_DEPRECATED
#   ifndef CPPSORT_DISABLE_DEPRECATION_WARNINGS
#       define CPPSORT_DEPRECATED(message) [[deprecated(message)]]
#   else
#       define CPPSORT_DEPRECATED(message)
#   endif
#endif

#endif // CPPSORT_DETAIL_CONFIG_H_
