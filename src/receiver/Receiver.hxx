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

#include "event/net/UdpListener.hxx"
#include "event/net/UdpHandler.hxx"
#include "net/SocketAddress.hxx"

#include <exception>
#include <span>

#include <stdint.h>

struct GeoPoint;

namespace Beacon {

class Receiver : UdpHandler {
	UdpListener socket;

public:
	struct Client {
		SocketAddress address;
		uint64_t key;
	};

public:
	Receiver(EventLoop &event_loop, SocketAddress address);

	void SendBuffer(SocketAddress address, std::span<const std::byte> src);

	template<typename P>
	void SendPacket(SocketAddress address,
			const P &packet) {
		SendBuffer(address, std::as_bytes(std::span{&packet, 1}));
	}

private:
	void OnDatagramReceived(Client &&client, void *data, size_t length);

protected:
	virtual void OnPing(const Client &client, unsigned id) noexcept;

	virtual void OnFix(const Client &client, GeoPoint location) noexcept = 0;

	/**
	 * An error has occurred while sending a response to a client.  This
	 * error is non-fatal.
	 */
	virtual void OnSendError(SocketAddress,
				 std::exception_ptr) noexcept {}

	/**
	 * An error has occurred, and the receiver is defunct.
	 */
	virtual void OnError(std::exception_ptr e) noexcept = 0;

private:
	/* virtual methods from UdpHandler */
	bool OnUdpDatagram(std::span<const std::byte> payload,
			   std::span<UniqueFileDescriptor> fds,
			   SocketAddress address, int uid) final;
	void OnUdpError(std::exception_ptr error) noexcept final;
};

} /* namespace Beacon */
