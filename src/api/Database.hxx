// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include "pg/Connection.hxx"

#include <cstdint>

struct GeoPoint;
class SocketAddress;

namespace Beacon {

class ApiDatabase {
	Pg::Connection db;

public:
	[[nodiscard]]
	explicit ApiDatabase(const char *conninfo);

	Pg::Result SelectList();

	Pg::Result SelectFixes(const uint64_t key);
	Pg::Result SelectFixesSince(const uint64_t key, const char *since);
};

} /* namespace Beacon */
