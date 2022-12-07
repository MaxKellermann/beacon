// SPDX-License-Identifier: BSD-2-Clause
// Copyright CM4all GmbH
// author: Max Kellermann <mk@cm4all.com>

#pragma once

#include <cstdint>

namespace Pg {

/**
 * C++ representation of a PostgreSQL "serial" value.
 */
class Serial {
	using value_type = uint_least32_t;
	value_type value;

public:
	Serial() = default;
	explicit constexpr Serial(value_type _value) noexcept:value(_value) {}

	constexpr value_type get() const noexcept {
		return value;
	}

	constexpr operator bool() const noexcept {
		return value != 0;
	}

	/**
	 * Convert a string to a #Serial instance.  Throws
	 * std::invalid_argument on error.
	 */
	static Serial Parse(const char *s);
};

}
