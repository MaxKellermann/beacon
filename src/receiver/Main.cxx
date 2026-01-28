// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

#include "Protocol.hxx"
#include "Receiver.hxx"
#include "Database.hxx"
#include "geo/GeoPoint.hxx"
#include "pg/Connection.hxx"
#include "lib/fmt/ExceptionFormatter.hxx"
#include "event/Loop.hxx"
#include "net/IPv4Address.hxx"
#include "util/PrintException.hxx"
#include "config.h"

#ifdef HAVE_LIBSYSTEMD
#include <systemd/sd-daemon.h>
#endif

#include <fmt/core.h>

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

	Beacon::ReceiverDatabase db;

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

	try {
		db.AutoReconnect();
		db.InsertFix(client.address, client.key, location);
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
