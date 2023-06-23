// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

#include "Response.hxx"

#include <json/json.h>

void
NotFound(FCGX_Stream *out) noexcept
{
	FCGX_PutS("Status: 404 Not Found\n"
		  "Content-Type: text/plain\n"
		  "\n"
		  "Not found\n", out);
}

void
SendResponse(FCGX_Stream *out, const Json::Value &root) noexcept
{
	FCGX_PutS("Content-Type: application/json\n"
		  "\n",
		  out);

	Json::FastWriter fw;
	FCGX_PutS(fw.write(root).c_str(), out);
}
