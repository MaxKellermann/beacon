// SPDX-License-Identifier: BSD-2-Clause
// author: Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include "Compiler.h"

struct StringView;

/**
 * Find the first query parameter with the given name and return its
 * raw value (without unescaping).
 *
 * @return the raw value (pointing into the #query_string parameter)
 * or nullptr if the parameter does not exist
 */
gcc_pure
StringView
UriFindRawQueryParameter(StringView query_string, StringView name) noexcept;
