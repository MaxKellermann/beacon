// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

package beacon;

import java.io.Closeable;
import java.io.IOException;
import java.net.SocketException;
import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.net.SocketAddress;

/**
 * A client for the Beacon datagram protocol.
 */
public class BeaconClient implements Closeable {
    private final long key;
    private final SocketAddress serverAddress;
    private final DatagramSocket socket;

    public BeaconClient(SocketAddress _serverAddress, long _key) throws SocketException {
        key = _key;
        serverAddress = _serverAddress;
        socket = new DatagramSocket();
    }

    @Override
    public void close() {
        socket.close();
    }

    private static void setAngle(byte[] data, int offset, double value) {
        BigEndian.setBE32(data, offset, (int)(value * 1000000));
    }

    private static void updateCrc(byte[] data) {
        BigEndian.setBE16(data, 4, (short)0);
        BigEndian.setBE16(data, 4, CRC16CCITT.calculate(data));
    }

    private static byte[] makeFixPayload(long key,
                                         double latitude, double longitude) {
        byte[] data = new byte[32];

        BigEndian.setBE32(data, 0, Protocol.MAGIC);
        BigEndian.setBE16(data, 6, Protocol.REQUEST_FIX);
        BigEndian.setBE64(data, 8, key);

        setAngle(data, 16, latitude);
        setAngle(data, 20, longitude);

        BigEndian.setBE16(data, 24, Protocol.INVALID_U16); // no direction
        BigEndian.setBE16(data, 26, Protocol.INVALID_U16); // no speed
        BigEndian.setBE16(data, 28, Protocol.INVALID_S16); // no altitude

        updateCrc(data);

        return data;
    }

    private DatagramPacket makeFixPacket(long key,
                                         double latitude,
                                         double longitude,
                                         SocketAddress address) {
        byte[] payload = makeFixPayload(key, latitude, longitude);
        return new DatagramPacket(payload, payload.length, address);
    }

    private DatagramPacket makeFixPacket(double latitude,
                                         double longitude) {
        return makeFixPacket(key, latitude, longitude, serverAddress);
    }

    public void sendFix(double latitude, double longitude) throws IOException {
        socket.send(makeFixPacket(latitude, longitude));
    }
}
