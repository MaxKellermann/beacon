// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2012-2019 Max Kellermann <max.kellermann@gmail.com>,
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

#include "util/ByteOrder.hxx"

#include <limits>

#include <stdint.h>

/*
 * The struct definitions below imply a specific memory layout.  They
 * have been designed in a way that all compilers we know will not
 * implicitly insert padding, because all attributes are aligned
 * properly already.
 *
 * All integers are big-endian.
 *
 */

namespace Beacon::Protocol {

static constexpr uint16_t DEFAULT_PORT = 5598;
static constexpr char DEFAULT_PORT_STRING[] = "5598";

static constexpr uint32_t MAGIC = 0xb7624363;

static constexpr int32_t UINT32_MAX_BE = ToBE32(std::numeric_limits<uint32_t>::max());
static constexpr int32_t INT32_MAX_BE = ToBE32(std::numeric_limits<int32_t>::max());

static constexpr int16_t UINT16_MAX_BE = ToBE16(std::numeric_limits<uint16_t>::max());
static constexpr int16_t INT16_MAX_BE = ToBE16(std::numeric_limits<int16_t>::max());

enum class RequestType : uint16_t {
	NOP = 0,
	PING = 1,
	FIX = 2,
};

enum class ResponseType : uint16_t {
	NOP = 0,
	ACK = 1,
};

/**
 * The datagram payload header.
 */
struct Header {
	/**
	 * Must be #MAGIC.
	 */
	uint32_t magic;

	/**
	 * The CRC of this packet including the header, assuming this
	 * attribute is 0.
	 *
	 * The CRC algorithm is CRC16-CCITT with initial value 0x0000
	 * (XModem) instead of CCITT's default 0xffff.
	 */
	uint16_t crc;

	/**
	 * An #RequestType or #ResponseType value.
	 */
	uint16_t type;

	/**
	 * The authorization key.
	 */
	uint64_t key;

	Header() = default;

	constexpr Header(int16_t _type, uint64_t _key) noexcept
		:magic(ToBE32(MAGIC)), crc(0),
		 type(ToBE16(_type)), key(ToBE64(_key)) {}

	constexpr Header(RequestType _type, uint64_t _key) noexcept
		:Header(int16_t(_type), _key) {}

	constexpr Header(ResponseType _type, uint64_t _key) noexcept
		:Header(int16_t(_type), _key) {}
};

static_assert(sizeof(Header) == 16);

/**
 * Check the network connection and verify the key (#PING).  The
 * server responds with #ACK.
 */
struct PingPacket {
	Header header;

	/**
	 * An arbitrary number chosen by the client, usually a sequence
	 * number.
	 */
	uint16_t id;

	/**
	 * Reserved for future use.  Set to zero.
	 */
	uint16_t reserved;

	/**
	 * Reserved for future use.  Set to zero.
	 */
	uint32_t reserved2;
};

static_assert(sizeof(PingPacket) == 24);

struct Angle {
	/**
	 * Angle in micro degrees.
	 */
	int32_t value;

	static constexpr Angle MakeInvalid() noexcept {
		return {INT32_MAX_BE};
	}

	constexpr bool IsValid() const noexcept {
		return value != INT32_MAX_BE;
	}
};

static_assert(sizeof(Angle) == 4);

struct GeoPoint {
	/**
	 * Positive means north or east.
	 */
	Angle latitude, longitude;

	static constexpr GeoPoint MakeInvalid() noexcept {
		return {Angle::MakeInvalid(), Angle::MakeInvalid()};
	}

	constexpr bool IsValid() const noexcept {
		return latitude.IsValid();
	}
};

static_assert(sizeof(GeoPoint) == 8);

/**
 * A GPS fix being submitted to the server.
 */
struct FixPacket {
	Header header;

	GeoPoint location;

	/**
	 * Movement direction in degrees (0..359).  0xffff means "unknown".
	 */
	uint16_t direction;

	/**
	 * Speed in m/16s.  0xffff means "unknown".
	 */
	uint16_t speed;

	/**
	 * Altitude in m above MSL.  0x7fff means "unknown".
	 */
	int16_t altitude;

	/**
	 * Reserved for future use.  Set to zero.
	 */
	uint16_t reserved;

	FixPacket() = default;

	explicit FixPacket(uint64_t key)
		:header(RequestType::FIX, key),
		 location(GeoPoint::MakeInvalid()),
		 direction(UINT32_MAX_BE),
		 speed(UINT32_MAX_BE),
		 altitude(INT32_MAX_BE),
		 reserved(0) {}
};

static_assert(sizeof(FixPacket) == 32);

/**
 * A generic acknowledge packet sent by the server in response to
 * certain request packets.
 */
struct AckPacket {
	/**
	 * The key was not valid.  Usually, requests with bad keys are
	 * silently discarded, but the server may use this flag to respond
	 * to a bad key in a PING packet.
	 */
	static const uint32_t FLAG_BAD_KEY = 0x1;

	Header header;

	/**
	 * Copy of the request's id value.
	 */
	uint16_t id;

	/**
	 * Reserved for future use.  Set to zero.
	 */
	uint16_t reserved;

	uint32_t flags;
};

static_assert(sizeof(AckPacket) == 24);

} /* namespace Beacon::Protocol */
