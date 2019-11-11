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

#include "Protocol.hxx"
#include "Receiver.hxx"
#include "geo/GeoPoint.hxx"
#include "util/PrintException.hxx"
#include "config.h"

#ifdef HAVE_LIBSYSTEMD
#include <systemd/sd-daemon.h>
#endif

#include <boost/asio/io_context.hpp>

#include <forward_list>

class Instance;

class MyReceiver final : public Beacon::Receiver {
	Instance &instance;

public:
	MyReceiver(Instance &instance,
		   boost::asio::ip::udp::endpoint endpoint) noexcept;

	void OnFix(const Client &client, GeoPoint location) noexcept override;

	void OnError(std::exception_ptr e) noexcept override {
		PrintException(e);
	}
};

class Instance {
	boost::asio::io_context io_context;

	std::forward_list<MyReceiver> receivers;

public:
	auto &get_io_context() noexcept {
		return io_context;
	}

	void AddReceiver(boost::asio::ip::udp::endpoint endpoint);

	void Run() {
		io_context.run();
	}
};

MyReceiver::MyReceiver(Instance &_instance,
		       boost::asio::ip::udp::endpoint endpoint) noexcept
	:Beacon::Receiver(_instance.get_io_context(), endpoint),
	 instance(_instance)
{
}

void
MyReceiver::OnFix(const Client &, GeoPoint location) noexcept
{
	fprintf(stderr, "fix %f %f\n",
		location.latitude.Degrees(), location.longitude.Degrees());
}

void
Instance::AddReceiver(boost::asio::ip::udp::endpoint endpoint)
{
	receivers.emplace_front(*this, endpoint);
}

int
main(int, char **) noexcept
try {
	Instance instance;
	instance.AddReceiver(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(),
							    Beacon::Protocol::DEFAULT_PORT));

#ifdef HAVE_LIBSYSTEMD
	/* tell systemd we're ready */
	sd_notify(0, "READY=1");
#endif

	instance.Run();

	return EXIT_SUCCESS;
} catch (...) {
	PrintException(std::current_exception());
	return EXIT_FAILURE;
}
