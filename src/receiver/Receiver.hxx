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
