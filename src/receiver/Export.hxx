// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

#pragma once

#include "Protocol.hxx"
#include "geo/GeoPoint.hxx"
#include "util/ByteOrder.hxx"

namespace Beacon::Protocol {

constexpr Angle
ExportAngle(::Angle src) noexcept
{
	return {int32_t(ToBE32(int32_t(src.Degrees() * 1000000)))};
}

constexpr GeoPoint
ExportGeoPoint(::GeoPoint src) noexcept
{
	return src.IsValid()
		? GeoPoint{ExportAngle(src.latitude), ExportAngle(src.longitude)}
		: GeoPoint::MakeInvalid();
}

} /* namespace Beacon::Protocol */
