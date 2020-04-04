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

#include "Protocol.hxx"
#include "Receiver.hxx"
#include "geo/GeoPoint.hxx"
#include "pg/Connection.hxx"
#include "util/PrintException.hxx"
#include "config.h"

#ifdef HAVE_LIBSYSTEMD
#include <systemd/sd-daemon.h>
#endif

#include <boost/asio/io_context.hpp>

#include <forward_list>
#include <sstream>

#include <inttypes.h>
#include <stdio.h>

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

	Pg::Connection db;

	std::forward_list<MyReceiver> receivers;

public:
	explicit Instance(const char *_db)
		:db(_db) {}

	auto &get_io_context() noexcept {
		return io_context;
	}

	auto &GetDatabase() noexcept {
		return db;
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

static std::string
ToStringWithoutPort(boost::asio::ip::udp::endpoint endpoint) noexcept
{
	std::stringstream buffer;
	buffer << endpoint;
	auto value = buffer.str();
	auto colon = value.find(':');
	if (colon != value.npos)
		// TODO: IPv6?
		value.erase(colon);
	return value;
}

void
MyReceiver::OnFix(const Client &client, GeoPoint location) noexcept
{
	fprintf(stderr, "fix %f %f\n",
		location.latitude.Degrees(), location.longitude.Degrees());

	auto &db = instance.GetDatabase();

	char key_buffer[32];
	snprintf(key_buffer, sizeof(key_buffer), "%" PRIu64, client.key);
	const char *key_s = key_buffer;

	char location_buffer[128];
	const char *location_s = nullptr;
	if (location.IsValid()) {
		snprintf(location_buffer, sizeof(location_buffer), "POINT(%f %f)",
			 location.longitude.Degrees(),
			 location.latitude.Degrees());
		location_s = location_buffer;
	}

	try {
		db.ExecuteParams("INSERT INTO fixes(key, client_address, location) VALUES($1, $2, ST_GeomFromText($3, 4326))",
				 key_s,
				 ToStringWithoutPort(client.endpoint).c_str(),
				 location_s);
	} catch (...) {
		fprintf(stderr, "Failed to insert fix into database: ");
		PrintException(std::current_exception());
		// TODO what now - reconnect or abort?
	}
}

void
Instance::AddReceiver(boost::asio::ip::udp::endpoint endpoint)
{
	receivers.emplace_front(*this, endpoint);
}

int
main(int, char **) noexcept
try {
	const char *db = "dbname=beacon"; // TODO make configurable

	Instance instance(db);
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
