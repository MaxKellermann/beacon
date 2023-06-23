// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include "math/Angle.hxx"

struct GeoPoint {
	Angle latitude, longitude;

	/**
	 * Construct an instance that is "invalid", i.e. IsValid() will
	 * return false.  The return value must not be used in any
	 * calculation.  This method may be used to explicitly declare a
	 * GeoPoint attribute as "invalid".
	 */
	static constexpr GeoPoint MakeInvalid() noexcept {
		return {Angle::FullCircle(), Angle::Zero()};
	}

	/**
	 * Check if this object is "valid".  Returns false when it was
	 * constructed by Invalid().  This is not an extensive plausibility
	 * check; it is only designed to catch instances created by
	 * Invalid().
	 */
	constexpr bool IsValid() const noexcept {
		return latitude <= Angle::HalfCircle();
	}
};
