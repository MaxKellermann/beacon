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

#pragma once

#include <boost/asio/ip/udp.hpp>

#include <exception>

#include <stdint.h>

struct GeoPoint;

namespace Beacon {

class Receiver {
	boost::asio::ip::udp::socket socket;

	uint8_t buffer[4096];

public:
	struct Client {
		boost::asio::ip::udp::endpoint endpoint;
		uint64_t key;
	};

private:
	Client client_buffer;

public:
	Receiver(boost::asio::io_context &io_context,
		 boost::asio::ip::udp::endpoint endpoint);

	~Receiver() noexcept;

	void SendBuffer(const boost::asio::ip::udp::endpoint &endpoint,
			boost::asio::const_buffer data);

	template<typename P>
	void SendPacket(const boost::asio::ip::udp::endpoint &endpoint,
			const P &packet) {
		SendBuffer(endpoint, boost::asio::buffer(&packet, sizeof(packet)));
	}

private:
	void OnDatagramReceived(Client &&client, void *data, size_t length);
	void OnReceive(const boost::system::error_code &ec, size_t size);
	void AsyncReceive() noexcept;

protected:
	virtual void OnPing(const Client &client, unsigned id) noexcept;

	virtual void OnFix(const Client &client, GeoPoint location) noexcept = 0;

	/**
	 * An error has occurred while sending a response to a client.  This
	 * error is non-fatal.
	 */
	virtual void OnSendError(const boost::asio::ip::udp::endpoint &,
				 std::exception_ptr) noexcept {}

	/**
	 * An error has occurred, and the receiver is defunct.
	 */
	virtual void OnError(std::exception_ptr e) noexcept = 0;
};

} /* namespace Beacon */
