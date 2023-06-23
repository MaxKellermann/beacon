// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include <stdint.h>

namespace Beacon::Protocol {

struct AckPacket;

[[gnu::const]]
AckPacket
MakeAck(uint64_t key, uint16_t id, uint32_t flags) noexcept;

} /* namespace Beacon::Protocol */
