// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2019 Max Kellermann <max.kellermann@gmail.com>,
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
