#!/usr/bin/env python3
#
# Copyright 2019 Max Kellermann <max.kellermann@gmail.com>,
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# - Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# - Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the
# distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
# FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#

# For debugging purposes, this Python script reads a GPX file and
# "replays" it by sending the data to a Beacon server.

import sys, socket, struct, time
import gpxpy

path, server, key = sys.argv[1:]
key = int(key)

with open(path) as f:
    gpx = gpxpy.parse(f)

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.connect((server, 5598))

def make_packet_header(type, key):
    magic = 0xb7624363
    return struct.pack('>IHHQ', magic, 0, type, key)

def pack_geopoint(latitude, longitude):
    return struct.pack('>ii', int(latitude * 1000000), int(longitude * 1000000))

def crc16_ccitt(crc, data):
    msb = crc >> 8
    lsb = crc & 0xff
    for c in data:
        x = c ^ msb
        x ^= (x >> 4)
        msb = (lsb ^ (x >> 3) ^ (x << 4)) & 0xff
        lsb = (x ^ (x << 5)) & 0xff
    return (msb << 8) + lsb

def apply_crc(data):
    crc = crc16_ccitt(0, data)
    return data[:4] + struct.pack('>H', crc) + data[6:]

def make_fix_packet(key, latitude, longitude):
    REQUEST_TYPE_FIX = 2
    data = make_packet_header(REQUEST_TYPE_FIX, key) + \
        pack_geopoint(latitude=latitude, longitude=longitude) + \
        struct.pack('>HHhH', 0xffff, 0xffff, 0x7fff, 0)
    return apply_crc(data)

for point in gpx.walk(only_points=True):
    print(point.latitude, point.longitude)
    s.send(make_fix_packet(key, latitude=point.latitude,
                           longitude=point.longitude))
    time.sleep(0.2)
