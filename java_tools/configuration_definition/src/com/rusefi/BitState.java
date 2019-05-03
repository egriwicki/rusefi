package com.rusefi;

/**
 * (c) Andrey Belomutskiy
 * 6/4/2015.
 */
public class BitState {
    private int bitIndex;

    public void incrementBitIndex(ConfigField cf, ConfigField next) {
        if (!cf.isBit) {
            bitIndex = 0;
            return;
        }
        if (bitIndex == 32)
            throw new IllegalStateException("Too many bits: " + cf.name);
        bitIndex++;
    }

    public void reset() {
        bitIndex = 0;
    }

    public int get() {
        return bitIndex;
    }
}
