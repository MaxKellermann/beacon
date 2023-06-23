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
#include "lib/fmt/ExceptionFormatter.hxx"
#include "event/Loop.hxx"
#include "net/IPv4Address.hxx"
#include "net/ToString.hxx"
#include "util/PrintException.hxx"
#include "config.h"

#ifdef HAVE_LIBSYSTEMD
#include <systemd/sd-daemon.h>
#endif

#include <fmt/format.h>

#include <forward_list>
#include <sstream>

#include <inttypes.h>
#include <stdio.h>

class Instance;

class MyReceiver final : public Beacon::Receiver {
	Instance &instance;

public:
	MyReceiver(Instance &instance,
		   SocketAddress address) noexcept;

	void OnFix(const Client &client, GeoPoint location) noexcept override;

	void OnError(std::exception_ptr e) noexcept override {
		PrintException(e);
	}
};

class Instance {
	EventLoop event_loop;

	Pg::Connection db;

	std::forward_list<MyReceiver> receivers;

public:
	explicit Instance(const char *_db)
		:db(_db) {}

	auto &GetEventLoop() noexcept {
		return event_loop;
	}

	auto &GetDatabase() noexcept {
		return db;
	}

	void AddReceiver(SocketAddress address);

	void Run() {
		event_loop.Run();
	}
};

MyReceiver::MyReceiver(Instance &_instance,
		       SocketAddress address) noexcept
	:Beacon::Receiver(_instance.GetEventLoop(), address),
	 instance(_instance)
{
}

void
MyReceiver::OnFix(const Client &client, GeoPoint location) noexcept
{
	fmt::print(stderr, "fix {} {}\n",
		   location.latitude.Degrees(), location.longitude.Degrees());

	auto &db = instance.GetDatabase();

	const fmt::format_int key_buffer{client.key};
	const char *key_s = key_buffer.c_str();

	char location_buffer[128];
	const char *location_s = nullptr;
	if (location.IsValid()) {
		snprintf(location_buffer, sizeof(location_buffer), "POINT(%f %f)",
			 location.longitude.Degrees(),
			 location.latitude.Degrees());
		location_s = location_buffer;
	}

	if (db.GetStatus() == CONNECTION_BAD) {
		fprintf(stderr, "Reconnecting to database\n");

		try {
			db.Reconnect();
		} catch (...) {
			fmt::print(stderr, "Failed to reconnect to database: {}\n",
				   std::current_exception());
			return;
		}
	}

	char address_buffer[256];
	const char *address = "?";
	if (HostToString(address_buffer, sizeof(address_buffer), client.address))
		address = address_buffer;

	try {
		db.ExecuteParams("INSERT INTO fixes(key, client_address, location) VALUES($1, $2, ST_GeomFromText($3, 4326))",
				 key_s,
				 address,
				 location_s);
	} catch (...) {
		fmt::print(stderr, "Failed to insert fix into database: {}\n",
			   std::current_exception());
	}
}

void
Instance::AddReceiver(SocketAddress address)
{
	receivers.emplace_front(*this, address);
}

int
main(int, char **) noexcept
try {
	const char *db = "dbname=beacon"; // TODO make configurable

	Instance instance(db);
	instance.AddReceiver(IPv4Address{Beacon::Protocol::DEFAULT_PORT});

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
