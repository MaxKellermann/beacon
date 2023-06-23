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


#include "receiver/Protocol.hxx"
#include "receiver/Export.hxx"
#include "net/AddressInfo.hxx"
#include "net/Resolver.hxx"
#include "net/SocketAddress.hxx"
#include "net/SocketError.hxx"
#include "net/UniqueSocketDescriptor.hxx"
#include "util/PrintException.hxx"
#include "util/CRC.hxx"

#include <stdio.h>
#include <stdlib.h>

namespace P = Beacon::Protocol;

template<typename P>
static void
SendPacket(SocketDescriptor s, SocketAddress address,
	   P &&packet)
{
	packet.header.crc = 0;
	packet.header.crc = ToBE16(UpdateCRC16CCITT(&packet, sizeof(packet), 0));
	auto nbytes = s.Write(&packet, sizeof(packet), address);
	if (nbytes < 0)
		throw MakeSocketError("Failed to send");
}

int
main(int argc, char **argv) noexcept
try {
	if (argc != 5) {
		fprintf(stderr, "Usage: %s SERVER KEY LAT LON\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char *server_s = argv[1];
	const char *key_s = argv[2];
	const char *lat_s = argv[3];
	const char *lon_s = argv[4];

	static constexpr auto hints = MakeAddrInfo(AI_ADDRCONFIG, AF_UNSPEC, SOCK_DGRAM);
	const auto ai = Resolve(server_s, P::DEFAULT_PORT_STRING, &hints);

	const uint64_t key = strtoull(key_s, nullptr, 16);
	const double lat = strtod(lat_s, nullptr);
	const double lon = strtod(lon_s, nullptr);
	const GeoPoint location{Angle::Degrees(lat), Angle::Degrees(lon)};

	P::FixPacket packet(key);
	packet.location = P::ExportGeoPoint(location);

	const auto &server = ai.GetBest();

	UniqueSocketDescriptor socket;
	if (!socket.Create(server.GetFamily(), server.GetType(), server.GetProtocol()))
		throw MakeSocketError("Failed to create socket");

	SendPacket(socket, server, packet);

	return EXIT_SUCCESS;
} catch (...) {
	PrintException(std::current_exception());
	return EXIT_FAILURE;
}
