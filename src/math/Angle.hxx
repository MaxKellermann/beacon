// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2010-2019 Max Kellermann <max.kellermann@gmail.com>,
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

#pragma once

#include "Constants.hxx"

class Angle {
	double value;

	explicit constexpr Angle(const double _value) noexcept
		:value(_value) {}

public:
	Angle() = default;

	constexpr static Angle Native(const double _value) noexcept {
		return Angle(_value);
	}

	constexpr static Angle Zero() noexcept {
		return Native(double(0));
	}

	static constexpr Angle Radians(const double _value) noexcept {
		return Angle(_value);
	}

	constexpr static Angle Degrees(double value) noexcept {
		return Radians(value * DEG_TO_RAD);
	}

	/**
	 * Construct an instance that describes a "full circle" (360
	 * degrees).
	 */
	constexpr static Angle FullCircle() noexcept {
		return Native(M_2PI);
	}

	/**
	 * Construct an instance that describes a "half circle" (180
	 * degrees).
	 */
	constexpr static Angle HalfCircle() noexcept{
		return Native(M_PI);
	}

	constexpr double Radians() const noexcept {
		return value;
	}

	constexpr double Degrees() const noexcept {
		return value * RAD_TO_DEG;
	}

	constexpr bool operator<(const Angle x) const noexcept {
		return value < x.value;
	}

	constexpr bool operator>(const Angle x) const noexcept {
		return value > x.value;
	}

	constexpr bool operator<=(const Angle x) const noexcept {
		return value <= x.value;
	}

	constexpr bool operator>=(const Angle x) const noexcept {
		return value >= x.value;
	}
};
