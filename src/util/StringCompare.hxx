// SPDX-License-Identifier: BSD-2-Clause
// author: Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include "StringView.hxx"
#include "StringAPI.hxx"
#include "Compiler.h"

#ifdef _UNICODE
#include "WStringCompare.hxx"
#endif

gcc_pure gcc_nonnull_all
static inline bool
StringIsEmpty(const char *string) noexcept
{
	return *string == 0;
}

gcc_pure gcc_nonnull_all
static inline bool
StringStartsWith(const char *haystack, StringView needle) noexcept
{
	return StringIsEqual(haystack, needle.data, needle.size);
}

gcc_pure gcc_nonnull_all
bool
StringEndsWith(const char *haystack, const char *needle) noexcept;

gcc_pure gcc_nonnull_all
bool
StringEndsWithIgnoreCase(const char *haystack, const char *needle) noexcept;

/**
 * Returns the portion of the string after a prefix.  If the string
 * does not begin with the specified prefix, this function returns
 * nullptr.
 */
gcc_pure gcc_nonnull_all
static inline const char *
StringAfterPrefix(const char *haystack, StringView needle) noexcept
{
	return StringStartsWith(haystack, needle)
		? haystack + needle.size
		: nullptr;
}

gcc_pure gcc_nonnull_all
static inline bool
StringStartsWithIgnoreCase(const char *haystack, StringView needle) noexcept
{
	return StringIsEqualIgnoreCase(haystack, needle.data, needle.size);
}

gcc_pure gcc_nonnull_all
static inline const char *
StringAfterPrefixIgnoreCase(const char *haystack, StringView needle) noexcept
{
	return StringStartsWithIgnoreCase(haystack, needle)
		? haystack + needle.size
		: nullptr;
}

/**
 * Check if the given string ends with the specified suffix.  If yes,
 * returns the position of the suffix, and nullptr otherwise.
 */
gcc_pure gcc_nonnull_all
const char *
FindStringSuffix(const char *p, const char *suffix) noexcept;
