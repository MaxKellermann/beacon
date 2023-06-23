// SPDX-License-Identifier: BSD-2-Clause
// author: Max Kellermann <max.kellermann@gmail.com>

#include "StringView.hxx"
#include "CharUtil.hxx"

template<typename T>
void
BasicStringView<T>::StripLeft() noexcept
{
	while (!empty() && IsWhitespaceOrNull(front()))
		pop_front();
}

template<typename T>
void
BasicStringView<T>::StripRight() noexcept
{
	while (!empty() && IsWhitespaceOrNull(back()))
		pop_back();
}

template struct BasicStringView<char>;

#ifdef _UNICODE
template struct BasicStringView<wchar_t>;
#endif
