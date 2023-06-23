// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include "Protocol.hxx"
#include "geo/GeoPoint.hxx"
#include "util/ByteOrder.hxx"

namespace Beacon::Protocol {

constexpr ::Angle
ImportAngle(Angle src) noexcept
{
	return ::Angle::Degrees(int32_t(FromBE32(src.value)) / 1000000.);
}

/**
 * Convert a #Beacon::Protocol::GeoPoint to a #::GeoPoint.
 */
constexpr ::GeoPoint
ImportGeoPoint(GeoPoint src) noexcept
{
	return src.IsValid()
		? ::GeoPoint{ImportAngle(src.latitude), ImportAngle(src.longitude)}
		: ::GeoPoint::MakeInvalid();
}

} /* namespace Beacon::Protocol */
