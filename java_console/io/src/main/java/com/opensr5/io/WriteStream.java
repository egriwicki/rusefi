package com.opensr5.io;

import java.io.IOException;

/**
 * Andrey Belomutskiy, (c) 2013-2020
 * 3/1/2017
 */
public interface WriteStream {
    /**
     * this blocking method would transmit the outgoing bytes
     */
    void write(byte[] bytes) throws IOException;

    default void write(byte value) throws IOException {
        write(new byte[]{value});
    }

    default void writeInt(int v) throws IOException {
        byte[] array = new byte[4];

        array[0] = (byte) ((v >>> 24) & 0xFF);
        array[0] = (byte) ((v >>> 16) & 0xFF);
        array[0] = (byte) ((v >>> 8) & 0xFF);
        array[0] = (byte) ((v >>> 0) & 0xFF);
        write(array);
    }
}
