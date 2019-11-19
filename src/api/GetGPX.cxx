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
