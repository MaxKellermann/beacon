// SPDX-License-Identifier: BSD-2-Clause
// Copyright CM4all GmbH
// author: Max Kellermann <mk@cm4all.com>

#include "Error.hxx"
#include "util/StringCompare.hxx"

#include <string.h>

bool
Pg::Error::IsType(const char *expected_type) const noexcept
{
	const char *actual_type = GetType();
	return actual_type != nullptr &&
		strcmp(actual_type, expected_type) == 0;
}

bool
Pg::Error::HasTypePrefix(StringView type_prefix) const noexcept
{
	const char *actual_type = GetType();
	return actual_type != nullptr &&
		StringStartsWith(actual_type, type_prefix);
}

bool
Pg::Error::IsFatal() const noexcept
{
	/* Class 08 - Connection Exception; see
	   https://www.postgresql.org/docs/9.6/errcodes-appendix.html */
	return HasTypePrefix("08");
}
