// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

package beacon;

/**
 * Support for encoding integers to big-endian into a byte array.
 */
class BigEndian {
    public static void setBE64(byte[] data, int offset, long value) {
        data[offset] = (byte)(value >> 56);
        data[offset + 1] = (byte)(value >> 48);
        data[offset + 2] = (byte)(value >> 40);
        data[offset + 3] = (byte)(value >> 32);
        data[offset + 4] = (byte)(value >> 24);
        data[offset + 5] = (byte)(value >> 16);
        data[offset + 6] = (byte)(value >> 8);
        data[offset + 7] = (byte)value;
    }

    public static void setBE32(byte[] data, int offset, int value) {
        data[offset] = (byte)(value >> 24);
        data[offset + 1] = (byte)(value >> 16);
        data[offset + 2] = (byte)(value >> 8);
        data[offset + 3] = (byte)value;
    }

    public static void setBE16(byte[] data, int offset, short value) {
        data[offset] = (byte)(value >> 8);
        data[offset + 1] = (byte)value;
    }
}
