// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2019 Max Kellermann <max.kellermann@gmail.com>,
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Assemble.hxx"
#include "Protocol.hxx"
#include "util/ByteOrder.hxx"
#include "util/CRC.hxx"

#include <assert.h>

namespace Beacon::Protocol {

AckPacket
MakeAck(uint64_t key, uint16_t id, uint32_t flags) noexcept
{
  assert(key != 0);

  AckPacket packet;
  packet.header.magic = ToBE32(MAGIC);
  packet.header.crc = 0;
  packet.header.type = ToBE16(uint16_t(ResponseType::ACK));
  packet.header.key = ToBE64(key);
  packet.id = ToBE16(id);
  packet.reserved = 0;
  packet.flags = ToBE32(flags);

  packet.header.crc = ToBE16(UpdateCRC16CCITT(&packet, sizeof(packet), 0));
  return packet;
}

} /* namespace Beacon::Protocol */
