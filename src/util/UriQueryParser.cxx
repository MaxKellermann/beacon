// SPDX-License-Identifier: BSD-2-Clause
// author: Max Kellermann <max.kellermann@gmail.com>

#include "UriQueryParser.hxx"
#include "IterableSplitString.hxx"
#include "StringCompare.hxx"

std::string_view
UriFindRawQueryParameter(std::string_view query_string,
			 std::string_view name) noexcept
{
	for (std::string_view i : IterableSplitString(query_string, '&')) {
		if (i.starts_with(name)) {
			if (i.size() == name.size())
				return "";

			if (i[name.size()] == '=')
				return i.substr(name.size() + 1);
		}
	}

	return {};
}
