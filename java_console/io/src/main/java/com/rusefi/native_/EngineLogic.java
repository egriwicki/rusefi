package com.rusefi.native_;

/**
 * Native interface allowing java to invoke rusEFI unit_tests library
 *
 * See com_rusefi_native__EngineLogic.h
 * See native_impl.cpp
 */
public class EngineLogic {
    public native String getVersion();

    public native void setSensor(String sensorTypeName, double value);

    public native byte[] getConfiguration();

    public native void setConfiguration(byte[] content, int offset, int size);

    public native void setEngineType(int engineType);

    public native void invokePeriodicCallback();

    public native byte[] getOutputs();
}
