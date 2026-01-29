// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

#include "Database.hxx"
#include "geo/GeoPoint.hxx"
#include "lib/fmt/Unsafe.hxx"
#include "net/FormatAddress.hxx"
#include "net/SocketAddress.hxx"

#include <fmt/format.h>

namespace Beacon {

ReceiverDatabase::ReceiverDatabase(const char *conninfo)
	:db(conninfo)
{
	Prepare();
}

void
ReceiverDatabase::AutoReconnect()
{
	if (db.GetStatus() == CONNECTION_BAD) {
		fmt::print(stderr, "Reconnecting to database\n");
		db.Reconnect();
		Prepare();
	}
}

void
ReceiverDatabase::InsertFix(SocketAddress _address, uint_least64_t key, GeoPoint location)
{
	const fmt::format_int key_buffer{key};
	const char *key_s = key_buffer.c_str();

	char location_buffer[128];
	const char *location_s = nullptr;
	if (location.IsValid())
		location_s = FmtUnsafeC(location_buffer, "POINT({} {})",
					location.longitude.Degrees(),
					location.latitude.Degrees());

	char address_buffer[256];
	const char *address = nullptr;
	if (HostToString(address_buffer, _address))
		address = address_buffer;

	db.ExecutePrepared("insert_fix", key_s, address, location_s);
}

void
ReceiverDatabase::Prepare()
{
	db.Prepare("insert_fix", "INSERT INTO fixes(key, client_address, location) VALUES($1, $2, ST_GeomFromText($3, 4326))", 3);
}

} /* namespace Beacon */
