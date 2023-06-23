// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

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
