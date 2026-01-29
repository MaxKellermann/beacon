// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

#include "Database.hxx"

namespace Beacon {

ApiDatabase::ApiDatabase(const char *conninfo)
	:db(conninfo)
{
}

Pg::Result
ApiDatabase::SelectList()
{
	return db.Execute("SELECT key,"
			  "to_char(MAX(time), 'YYYY-MM-DD\"T\"HH24:MI:SS.MS\"Z\"')"
			  " FROM fixes"
			  " WHERE time > now() at time zone 'UTC' - '4 hours'::interval"
			  " GROUP BY key");
}

Pg::Result
ApiDatabase::SelectFixes(const uint64_t key)
{
	return db.ExecuteParams(false,
				"SELECT ST_X(location),ST_Y(location),"
				"to_char(time, 'YYYY-MM-DD\"T\"HH24:MI:SS.MS\"Z\"')"
				" FROM fixes"
				" WHERE key=$1"
				" AND time > now() at time zone 'UTC' - '4 hours'::interval"
				" ORDER BY time LIMIT 16384",
				key);
}

Pg::Result
ApiDatabase::SelectFixesSince(const uint64_t key, const char *since)
{
	return db.ExecuteParams(false,
				"SELECT ST_X(location),ST_Y(location),"
				"to_char(time, 'YYYY-MM-DD\"T\"HH24:MI:SS.MS\"Z\"')"
				" FROM fixes"
				" WHERE key=$1 AND time>=$2"
				" AND time > now() at time zone 'UTC' - '4 hours'::interval"
				" ORDER BY time LIMIT 16384",
				key, since);
}

} /* namespace Beacon */
