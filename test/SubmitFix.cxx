// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

#include "receiver/Protocol.hxx"
#include "receiver/Export.hxx"
#include "net/AddressInfo.hxx"
#include "net/Resolver.hxx"
#include "net/SocketAddress.hxx"
#include "net/SocketError.hxx"
#include "net/UniqueSocketDescriptor.hxx"
#include "util/PrintException.hxx"
#include "util/CRC.hxx"
#include "util/SpanCast.hxx"

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
	auto nbytes = s.WriteNoWait(ReferenceAsBytes(packet), address);
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

	static constexpr struct addrinfo hints{
		.ai_flags = AI_ADDRCONFIG,
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_DGRAM,
	};

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
