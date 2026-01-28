// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include "pg/Connection.hxx"

#include <cstdint>

struct GeoPoint;
class SocketAddress;

namespace Beacon {

class ReceiverDatabase {
	Pg::Connection db;

public:
	[[nodiscard]]
	explicit ReceiverDatabase(const char *conninfo)
		:db(conninfo) {}

	void AutoReconnect();

	void InsertFix(SocketAddress address, uint_least64_t key, GeoPoint location);
};

} /* namespace Beacon */
