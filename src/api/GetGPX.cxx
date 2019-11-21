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

#include "GetGPX.hxx"
#include "Response.hxx"
#include "pg/Connection.hxx"
#include "util/StringCompare.hxx"
#include "util/UriQueryParser.hxx"

#include <fcgiapp.h>

#include <string>

static std::string
GetQueryParameter(FCGX_ParamArray envp, StringView name) noexcept
{
	const char *query_string = FCGX_GetParam("QUERY_STRING", envp);
	if (query_string == nullptr)
		return {};

	auto value = UriFindRawQueryParameter(query_string, name);
	if (value.IsNull())
		return {};

	// TODO: unescape
	return {value.data, value.size};
}

static auto
SelectFixes(Pg::Connection &db, const uint64_t key, const std::string &since)
{
	if (!since.empty())
		return db.ExecuteParams(false,
					"SELECT ST_X(location),ST_Y(location),"
					"to_char(time, 'YYYY-MM-DD\"T\"HH24:MI:SS.MS\"Z\"')"
					" FROM fixes"
					" WHERE key=$1 AND time>=$2"
					" AND time > now() - '4 hours'::interval"
					" ORDER BY time LIMIT 16384",
					key, since.c_str());
	return db.ExecuteParams(false,
				"SELECT ST_X(location),ST_Y(location),"
				"to_char(time, 'YYYY-MM-DD\"T\"HH24:MI:SS.MS\"Z\"')"
				" FROM fixes"
				" WHERE key=$1"
				" AND time > now() - '4 hours'::interval"
				" ORDER BY time LIMIT 16384",
				key);
}

void
HandleGPX(Pg::Connection &db,
	  const char *path_info,
	  FCGX_Stream *out, FCGX_ParamArray envp) noexcept
{
	char *endptr;
	const uint64_t key = strtoull(path_info, &endptr, 10);
	if (endptr == path_info || (*endptr != 0 && !StringIsEqual(endptr, ".gpx"))) {
		NotFound(out);
		return;
	}

	const auto since = GetQueryParameter(envp, "since");

	auto result = SelectFixes(db, key, since);
	if (result.IsEmpty()) {
		NotFound(out);
		return;
	}

	FCGX_PutS("Content-Type: application/gpx+xml\n"
		  "\n"
		  "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
		  "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\"\n"
		  "  creator=\"beacon\" version=\"1.0\">\n"
		  "<trk><trkseg>\n",
		  out);

	for (const auto &row : result) {
		const auto longitude = row.GetValue(0);
		const auto latitude = row.GetValue(1);
		if (*longitude == 0 || *latitude == 0)
			/* skip records without a known location */
			continue;

		const auto time = row.GetValue(2);

		FCGX_FPrintF(out,
			     "<trkpt lat=\"%s\" lon=\"%s\"><time>%s</time></trkpt>\n",
			     latitude, longitude, time);

	}

	FCGX_PutS("</trkseg></trk></gpx>\n", out);
}
