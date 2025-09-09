// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

#include "Receiver.hxx"
#include "Assemble.hxx"
#include "Protocol.hxx"
#include "Import.hxx"
#include "net/SocketError.hxx"
#include "net/UniqueSocketDescriptor.hxx"
#include "util/ByteOrder.hxx"
#include "util/CRC.hxx"

namespace Beacon {

namespace P = Beacon::Protocol;

static UniqueSocketDescriptor
CreateBindDatagramSocket(SocketAddress address)
{
	UniqueSocketDescriptor fd;
	if (!fd.Create(address.GetFamily(), SOCK_DGRAM, 0))
		throw MakeSocketError("Failed to create socket");

	if (!fd.Bind(address))
		throw MakeErrno("Failed to bind socket");

	return fd;
}

Receiver::Receiver(EventLoop &event_loop, SocketAddress address)
	:socket(event_loop, CreateBindDatagramSocket(address), *this)
{
}

void
Receiver::SendBuffer(SocketAddress address, std::span<const std::byte> src)
{
	// TODO: use async_send_to()?

	try {
		socket.Reply(address, src);
	} catch (...) {
		OnSendError(address, std::current_exception());
	}
}

void
Receiver::OnPing(const Client &client, unsigned id) noexcept
{
	SendPacket(client.address,
		   Beacon::Protocol::MakeAck(client.key, id, 0));
}

inline void
Receiver::OnDatagramReceived(Client &&client,
			     void *data, size_t length)
{
	auto &header = *(P::Header *)data;
	if (length < sizeof(header))
		return;

	const uint16_t received_crc = FromBE16(header.crc);
	header.crc = 0;

	const uint16_t calculated_crc = UpdateCRC16CCITT(data, length, 0);
	if (received_crc != calculated_crc)
		return;

	client.key = FromBE64(header.key);

	const auto &ping = *(const P::PingPacket *)data;
	const auto &fix = *(const P::FixPacket *)data;

	switch (P::RequestType(FromBE16(header.type))) {
	case P::RequestType::NOP:
		break;

	case P::RequestType::PING:
		if (length < sizeof(ping))
			return;

		OnPing(client, FromBE16(ping.id));
		break;

	case P::RequestType::FIX:
		if (length < sizeof(fix))
			return;

		OnFix(client, ImportGeoPoint(fix.location));
		break;
	}
}

bool
Receiver::OnUdpDatagram(std::span<const std::byte> payload,
			std::span<UniqueFileDescriptor>,
			SocketAddress address, int)
{
	// TODO: use recvmmsg() on Linux

	Client client;
	client.address = address;
	OnDatagramReceived(std::move(client),
			   const_cast<std::byte *>(payload.data()), // TOOD no const_cast, please
			   payload.size());
	return true;
}

void
Receiver::OnUdpError(std::exception_ptr &&error) noexcept
{
	socket.Close();
	OnError(std::move(error));
}

} /* namespace Beacon */
