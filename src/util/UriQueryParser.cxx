// SPDX-License-Identifier: BSD-2-Clause
// author: Max Kellermann <max.kellermann@gmail.com>

#include "UriQueryParser.hxx"
#include "IterableSplitString.hxx"
#include "StringView.hxx"

StringView
UriFindRawQueryParameter(StringView query_string, StringView name) noexcept
{
	for (StringView i : IterableSplitString(query_string, '&')) {
		if (i.StartsWith(name)) {
			if (i.size == name.size)
				return "";

			if (i[name.size] == '=') {
				i.skip_front(name.size + 1);
				return i;
			}
		}
	}

	return nullptr;
}
