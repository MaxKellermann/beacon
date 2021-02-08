// SPDX-License-Identifier: BSD-2-Clause
// Copyright CM4all GmbH
// author: Max Kellermann <mk@cm4all.com>

#pragma once

#include "Result.hxx"

#include <exception>

struct StringView;

namespace Pg {

class Error final : public std::exception {
	Result result;

public:
	Error(const Error &other) = delete;
	Error(Error &&other) noexcept
		:result(std::move(other.result)) {}
	explicit Error(Result &&_result) noexcept
		:result(std::move(_result)) {}

	Error &operator=(const Error &other) = delete;

	Error &operator=(Error &&other) noexcept {
		result = std::move(other.result);
		return *this;
	}

	Error &operator=(Result &&other) noexcept {
		result = std::move(other);
		return *this;
	}

	[[gnu::pure]]
	ExecStatusType GetStatus() const noexcept {
		return result.GetStatus();
	}

	[[gnu::pure]]
	const char *GetType() const noexcept {
		return result.GetErrorType();
	}

	[[gnu::pure]]
	bool IsType(const char *type) const noexcept;

	[[gnu::pure]]
	bool HasTypePrefix(StringView type_prefix) const noexcept;

	/**
	 * Is this error fatal, i.e. has the connection become
	 * unusable?
	 */
	[[gnu::pure]]
	bool IsFatal() const noexcept;

	/**
	 * Is this a serialization failure, i.e. a problem with "BEGIN
	 * SERIALIZABLE" or Pg::Connection::BeginSerializable().
	 */
	[[gnu::pure]]
	bool IsSerializationFailure() const noexcept {
		// https://www.postgresql.org/docs/current/static/errcodes-appendix.html
		return IsType("40001");
	}

	[[gnu::pure]]
	const char *what() const noexcept override {
		return result.GetErrorMessage();
	}
};

} /* namespace Pg */
