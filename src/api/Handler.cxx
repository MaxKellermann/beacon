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

#include "Handler.hxx"
#include "Response.hxx"
#include "GetGPX.hxx"
#include "pg/Connection.hxx"
#include "util/StringCompare.hxx"

#include <json/json.h>

static void
HandleList(Pg::Connection &db, FCGX_Stream *out)
{
	const auto result = db.Execute("SELECT key,"
				       "to_char(MAX(time), 'YYYY-MM-DD\"T\"HH24:MI:SS.MS\"Z\"')"
				       " FROM fixes"
				       " WHERE time > now() - '4 hours'::interval"
				       " GROUP BY key");
	Json::Value root(Json::arrayValue);

	for (const auto &row : result) {
		Json::Value item(Json::objectValue);
		item["id"] = row.GetValue(0);
		item["time"] = row.GetValue(1);
		root.append(std::move(item));
	}

	SendResponse(out, root);
}

void
HandleRequest(Pg::Connection &db,
	      FCGX_Stream *in, FCGX_Stream *out, FCGX_Stream *err,
	      FCGX_ParamArray envp) noexcept
{
	(void)in;
	(void)err;

	const char *const path_info = FCGX_GetParam("PATH_INFO", envp);
	if (path_info == nullptr) {
		NotFound(out);
		return;
	}

	if (auto gpx = StringAfterPrefix(path_info, "gpx/"))
		HandleGPX(db, gpx, out, envp);
	else if (StringIsEqual(path_info, "list"))
		HandleList(db, out);
	else
		NotFound(out);
}
