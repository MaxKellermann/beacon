// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

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
