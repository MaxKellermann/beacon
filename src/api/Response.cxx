// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

#include "Response.hxx"

#include <nlohmann/json.hpp>

void
NotFound(FCGX_Stream *out) noexcept
{
	FCGX_PutS("Status: 404 Not Found\n"
		  "Content-Type: text/plain\n"
		  "\n"
		  "Not found\n", out);
}

void
SendResponse(FCGX_Stream *out, const nlohmann::json &root) noexcept
{
	FCGX_PutS("Content-Type: application/json\n"
		  "\n",
		  out);

	auto json_str = root.dump();
	FCGX_PutS(json_str.c_str(), out);
}
