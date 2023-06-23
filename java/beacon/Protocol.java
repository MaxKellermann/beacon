// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

package beacon;

/**
 * Definitions for the Beacon datagram protocol.
 */
class Protocol {
    public static final int MAGIC = 0xb7624363;

    public static final short REQUEST_NOP = 0;
    public static final short REQUEST_PING = 1;
    public static final short REQUEST_FIX = 2;

    public static final short RESPONSE_NOP = 0;
    public static final short RESPONSE_ACK = 1;

    public static final short INVALID_U16 = (short)0xffff;
    public static final short INVALID_S16 = 0x7fff;
}
