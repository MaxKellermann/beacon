// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

#include "Handler.hxx"
#include "Response.hxx"
#include "Database.hxx"
#include "GetGPX.hxx"
#include "util/StringCompare.hxx"

#include <nlohmann/json.hpp>

using std::string_view_literals::operator""sv;

static void
HandleList(Beacon::ApiDatabase &db, FCGX_Stream *out)
{
	const auto result = db.SelectList();
	nlohmann::json root = nlohmann::json::array();

	for (const auto &row : result) {
		root.emplace_back(nlohmann::json{
			{"id"sv, row.GetValueView(0)},
			{"time"sv, row.GetValueView(1)}
		});
	}

	SendResponse(out, root);
}

void
HandleRequest(Beacon::ApiDatabase &db,
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
