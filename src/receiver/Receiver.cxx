// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright 2019 Max Kellermann <max.kellermann@gmail.com>,
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "Receiver.hxx"
#include "Assemble.hxx"
#include "Protocol.hxx"
#include "Import.hxx"
#include "util/ByteOrder.hxx"
#include "util/CRC.hxx"

namespace Beacon {

namespace P = Beacon::Protocol;

Receiver::Receiver(boost::asio::io_context &io_context,
		   boost::asio::ip::udp::endpoint endpoint)
	:socket(io_context, endpoint)
{
	AsyncReceive();
}

Receiver::~Receiver() noexcept
{
	if (socket.is_open()) {
		socket.cancel();
		socket.close();
	}
}

void
Receiver::SendBuffer(const boost::asio::ip::udp::endpoint &endpoint,
		     boost::asio::const_buffer data)
{
	// TODO: use async_send_to()?

	try {
		socket.send_to(boost::asio::const_buffers_1(data), endpoint, 0);
	} catch (...) {
		OnSendError(endpoint, std::current_exception());
	}
}

void
Receiver::OnPing(const Client &client, unsigned id) noexcept
{
	SendPacket(client.endpoint,
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

void
Receiver::OnReceive(const boost::system::error_code &ec, size_t size)
{
	// TODO: use recvmmsg() on Linux

	if (ec) {
		if (ec == boost::asio::error::operation_aborted)
			return;

		socket.close();

		OnError(std::make_exception_ptr(boost::system::system_error(ec)));
		return;
	}

	OnDatagramReceived(std::move(client_buffer), buffer, size);

	AsyncReceive();
}

void
Receiver::AsyncReceive() noexcept
{
	socket.async_receive_from(boost::asio::buffer(buffer, sizeof(buffer)),
				  client_buffer.endpoint,
				  std::bind(&Receiver::OnReceive, this,
					    std::placeholders::_1,
					    std::placeholders::_2));
}

} /* namespace Beacon */
