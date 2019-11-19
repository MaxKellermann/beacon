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
