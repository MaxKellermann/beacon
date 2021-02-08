// SPDX-License-Identifier: BSD-2-Clause
// Copyright CM4all GmbH
// author: Max Kellermann <mk@cm4all.com>

#pragma once

#include "BinaryValue.hxx"

#include "util/Compiler.h"

#include <libpq-fe.h>

#include <cassert>
#include <cstdlib>
#include <string>
#include <utility>

namespace Pg {

/**
 * A thin C++ wrapper for a PGresult pointer.
 */
class Result {
	PGresult *result;

public:
	Result():result(nullptr) {}
	explicit Result(PGresult *_result):result(_result) {}

	Result(Result &&other) noexcept
		:result(std::exchange(other.result, nullptr)) {}

	~Result() noexcept {
		if (result != nullptr)
			::PQclear(result);
	}

	bool IsDefined() const noexcept {
		return result != nullptr;
	}

	Result &operator=(Result &&other) noexcept {
		using std::swap;
		swap(result, other.result);
		return *this;
	}

	[[gnu::pure]]
	ExecStatusType GetStatus() const noexcept {
		assert(IsDefined());

		return ::PQresultStatus(result);
	}

	[[gnu::pure]]
	bool IsCommandSuccessful() const noexcept {
		return GetStatus() == PGRES_COMMAND_OK;
	}

	[[gnu::pure]]
	bool IsQuerySuccessful() const noexcept {
		return GetStatus() == PGRES_TUPLES_OK;
	}

	[[gnu::pure]]
	bool IsError() const noexcept {
		const auto status = GetStatus();
		return status == PGRES_BAD_RESPONSE ||
			status == PGRES_NONFATAL_ERROR ||
			status == PGRES_FATAL_ERROR;
	}

	[[gnu::pure]]
	const char *GetErrorMessage() const noexcept {
		assert(IsDefined());

		return ::PQresultErrorMessage(result);
	}

	[[gnu::pure]]
	const char *GetErrorField(int fieldcode) const noexcept {
		assert(IsDefined());

		return PQresultErrorField(result, fieldcode);
	}

	[[gnu::pure]]
	const char *GetErrorType() const noexcept {
		assert(IsDefined());

		return GetErrorField(PG_DIAG_SQLSTATE);
	}

	/**
	 * Returns the number of rows that were affected by the command.
	 * The caller is responsible for checking GetStatus().
	 */
	[[gnu::pure]]
	unsigned GetAffectedRows() const noexcept {
		assert(IsDefined());
		assert(IsCommandSuccessful());

		return std::strtoul(::PQcmdTuples(result), nullptr, 10);
	}

	/**
	 * Returns true if there are no rows in the result.
	 */
	[[gnu::pure]]
	bool IsEmpty() const noexcept {
		assert(IsDefined());

		return ::PQntuples(result) == 0;
	}

	[[gnu::pure]]
	unsigned GetRowCount() const noexcept {
		assert(IsDefined());

		return ::PQntuples(result);
	}

	[[gnu::pure]]
	unsigned GetColumnCount() const noexcept {
		assert(IsDefined());

		return ::PQnfields(result);
	}

	[[gnu::pure]]
	const char *GetColumnName(unsigned column) const noexcept {
		assert(IsDefined());

		return ::PQfname(result, column);
	}

	[[gnu::pure]]
	bool IsColumnBinary(unsigned column) const noexcept {
		assert(IsDefined());

		return ::PQfformat(result, column);
	}

	[[gnu::pure]]
	Oid GetColumnType(unsigned column) const noexcept {
		assert(IsDefined());

		return ::PQftype(result, column);
	}

	[[gnu::pure]]
	bool IsColumnTypeBinary(unsigned column) const noexcept {
		/* 17 = bytea */
		return GetColumnType(column) == 17;
	}

	/**
	 * Obtains the given value, and return an empty string if the
	 * value is NULL.  Call IsValueNull() to find out whether the
	 * real value was NULL or an empty string.
	 */
	[[gnu::pure]]
	const char *GetValue(unsigned row, unsigned column) const noexcept {
		assert(IsDefined());

		return ::PQgetvalue(result, row, column);
	}

	[[gnu::pure]]
	unsigned GetValueLength(unsigned row, unsigned column) const noexcept {
		assert(IsDefined());

		return ::PQgetlength(result, row, column);
	}

	[[gnu::pure]]
	bool IsValueNull(unsigned row, unsigned column) const noexcept {
		assert(IsDefined());

		return ::PQgetisnull(result, row, column);
	}

	/**
	 * Obtains the given value, but return nullptr instead of an
	 * empty string if the value is NULL.
	 */
	[[gnu::pure]]
	const char *GetValueOrNull(unsigned row, unsigned column) const noexcept {
		assert(IsDefined());

		return IsValueNull(row, column)
			? nullptr
			: GetValue(row, column);
	}

	[[gnu::pure]]
	BinaryValue GetBinaryValue(unsigned row, unsigned column) const noexcept {
		assert(IsColumnBinary(column));

		return BinaryValue(GetValue(row, column),
				   GetValueLength(row, column));
	}

	/**
	 * Returns the only value (row 0, column 0) from the result.
	 * Returns an empty string if the result is not valid or if there
	 * is no row or if the value is nullptr.
	 */
	[[gnu::pure]]
	std::string GetOnlyStringChecked() const noexcept;

	class RowIterator {
		PGresult *result;
		unsigned row;

	public:
		constexpr RowIterator(PGresult *_result, unsigned _row) noexcept
			:result(_result), row(_row) {}

		constexpr bool operator==(const RowIterator &other) const noexcept {
			return row == other.row;
		}

		constexpr bool operator!=(const RowIterator &other) const noexcept {
			return row != other.row;
		}

		RowIterator &operator++() noexcept {
			++row;
			return *this;
		}

		RowIterator &operator*() noexcept {
			return *this;
		}

		gcc_pure
		const char *GetValue(unsigned column) const noexcept {
			assert(result != nullptr);
			assert(row < (unsigned)::PQntuples(result));
			assert(column < (unsigned)::PQnfields(result));

			return ::PQgetvalue(result, row, column);
		}

		[[gnu::pure]]
		unsigned GetValueLength(unsigned column) const noexcept {
			assert(result != nullptr);
			assert(row < (unsigned)::PQntuples(result));
			assert(column < (unsigned)::PQnfields(result));

			return ::PQgetlength(result, row, column);
		}

		[[gnu::pure]]
		bool IsValueNull(unsigned column) const noexcept {
			assert(result != nullptr);
			assert(row < (unsigned)::PQntuples(result));
			assert(column < (unsigned)::PQnfields(result));

			return ::PQgetisnull(result, row, column);
		}

		[[gnu::pure]]
		const char *GetValueOrNull(unsigned column) const noexcept {
			assert(result != nullptr);
			assert(row < (unsigned)::PQntuples(result));
			assert(column < (unsigned)::PQnfields(result));

			return IsValueNull(column)
				? nullptr
				: GetValue(column);
		}

		[[gnu::pure]]
		BinaryValue GetBinaryValue(unsigned column) const noexcept {
			assert(result != nullptr);
			assert(row < (unsigned)::PQntuples(result));
			assert(column < (unsigned)::PQnfields(result));

			return BinaryValue(GetValue(column), GetValueLength(column));
		}
	};

	typedef RowIterator iterator;

	iterator begin() const noexcept {
		return iterator{result, 0};
	}

	iterator end() const noexcept {
		return iterator{result, GetRowCount()};
	}
};

} /* namespace Pg */
