// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

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
