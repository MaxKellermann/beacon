// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

#include "Database.hxx"
#include "geo/GeoPoint.hxx"
#include "net/FormatAddress.hxx"
#include "net/SocketAddress.hxx"

#include <fmt/format.h>

#include <cstdio>

namespace Beacon {

void
ReceiverDatabase::AutoReconnect()
{
	if (db.GetStatus() == CONNECTION_BAD) {
		fprintf(stderr, "Reconnecting to database\n");
		db.Reconnect();
	}
}

void
ReceiverDatabase::InsertFix(SocketAddress _address, uint_least64_t key, GeoPoint location)
{
	const fmt::format_int key_buffer{key};
	const char *key_s = key_buffer.c_str();

	char location_buffer[128];
	const char *location_s = nullptr;
	if (location.IsValid()) {
		snprintf(location_buffer, sizeof(location_buffer), "POINT(%f %f)",
			 location.longitude.Degrees(),
			 location.latitude.Degrees());
		location_s = location_buffer;
	}

	char address_buffer[256];
	const char *address = "?";
	if (HostToString(address_buffer, _address))
		address = address_buffer;

	db.ExecuteParams("INSERT INTO fixes(key, client_address, location) VALUES($1, $2, ST_GeomFromText($3, 4326))",
			 key_s,
			 address,
			 location_s);
}

} /* namespace Beacon */
