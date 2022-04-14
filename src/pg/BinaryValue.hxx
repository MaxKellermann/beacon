// SPDX-License-Identifier: BSD-2-Clause
// Copyright CM4all GmbH
// author: Max Kellermann <mk@cm4all.com>

#pragma once

#include "util/ConstBuffer.hxx"

#include <cstddef>

namespace Pg {

struct BinaryValue : ConstBuffer<void> {
	using ConstBuffer::ConstBuffer;

	gcc_pure
	bool ToBool() const noexcept {
		return size == 1 && data != nullptr && *(const bool *)data;
	}
};

} /* namespace Pg */
