// SPDX-License-Identifier: BSD-2-Clause
// author: Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include "StringView.hxx"

#include <iterator>

/**
 * Split a string at a certain separator character into sub strings
 * and allow iterating over the segments.
 *
 * Two consecutive separator characters result in an empty string.
 *
 * An empty input string returns one empty string.
 */
template<typename T>
class BasicIterableSplitString {
	typedef BasicStringView<T> StringView;

	using value_type = typename StringView::value_type;

	StringView s;
	value_type separator;

public:
	constexpr BasicIterableSplitString(StringView _s,
					   value_type _separator)
		:s(_s), separator(_separator) {}

	class Iterator final {
		friend class BasicIterableSplitString;

		StringView current, rest;

		value_type separator;

		Iterator(StringView _s, value_type _separator)
			:rest(_s), separator(_separator) {
			Next();
		}

		constexpr Iterator(std::nullptr_t n)
			:current(n), rest(n), separator(0) {}

		void Next() {
			if (rest == nullptr)
				current = nullptr;
			else {
				const auto *i = rest.Find(separator);
				if (i == nullptr) {
					current = rest;
					rest.data = nullptr;
				} else {
					current.data = rest.data;
					current.size = i - current.data;
					rest.size -= current.size + 1;
					rest.data = i + 1;
				}
			}
		}

	public:
		using iterator_category = std::forward_iterator_tag;

		Iterator &operator++() {
			Next();
			return *this;
		}

		constexpr bool operator==(Iterator other) const {
			return current.data == other.current.data;
		}

		constexpr bool operator!=(Iterator other) const {
			return !(*this == other);
		}

		constexpr StringView operator*() const {
			return current;
		}

		constexpr const StringView *operator->() const {
			return &current;
		}
	};

	using iterator = Iterator;
	using const_iterator = Iterator;

	const_iterator begin() const {
		return {s, separator};
	}

	constexpr const_iterator end() const {
		return {nullptr};
	}
};

using IterableSplitString = BasicIterableSplitString<char>;

#ifdef _UNICODE
using WIterableSplitString = BasicIterableSplitString<wchar_t>;
using TIterableSplitString = WIterableSplitString;
#else
using TIterableSplitString = IterableSplitString;
#endif
