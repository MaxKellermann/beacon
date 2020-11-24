// SPDX-License-Identifier: BSD-2-Clause
// Copyright CM4all GmbH
// author: Max Kellermann <mk@cm4all.com>

#pragma once

#include "util/ConstBuffer.hxx"

#include <cstddef>

namespace Pg {

struct BinaryValue : ConstBuffer<void> {
	BinaryValue() = default;

	constexpr BinaryValue(ConstBuffer<void> _buffer) noexcept
		:ConstBuffer<void>(_buffer) {}

	constexpr BinaryValue(const void *_value, size_t _size) noexcept
		:ConstBuffer<void>(_value, _size) {}

	gcc_pure
	bool ToBool() const noexcept {
		return size == 1 && data != nullptr && *(const bool *)data;
	}
};

} /* namespace Pg */
