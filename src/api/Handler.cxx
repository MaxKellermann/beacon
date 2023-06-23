// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

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
				       " WHERE time > now() at time zone 'UTC' - '4 hours'::interval"
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
