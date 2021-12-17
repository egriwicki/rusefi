package com.rusefi.livedata;

import com.rusefi.config.Field;
import com.rusefi.config.generated.*;
import com.rusefi.enums.live_data_e;
import com.rusefi.ldmp.StateDictionary;

/**
 * @see StateDictionary
 */
public enum LiveDataView {
    AC_CONTROL(live_data_e.LDS_AC_CONTROL, AcControl.VALUES, "ac_control.cpp"),
    FUEL_PUMP(live_data_e.LDS_FUEL_PUMP, FuelPump.VALUES, "fuel_pump.cpp"),
    MAIN_RELAY(live_data_e.LDS_MAIN_RELAY, MainRelay.VALUES, "main_relay.cpp"),
    TPS_ACCEL(live_data_e.LDS_TPS_ACCEL, TpsAccelState.VALUES, "accel_enrichment.cpp"),
    BOOST_CONTROL(live_data_e.LDS_BOOST_CONTROL, BoostControl.VALUES, "boost_control.cpp"),
    LAUNCH_CONTROL(live_data_e.LDS_LAUNCH_CONTROL, LaunchControl.VALUES, "launch_control.cpp"),
    ;

    private final live_data_e liveDataE;
    private final Field[] values;
    private final String fileName;

    LiveDataView(live_data_e liveDataE, Field[] values, String fileName) {

        this.liveDataE = liveDataE;
        this.values = values;
        this.fileName = fileName;
    }

    public live_data_e getLiveDataE() {
        return liveDataE;
    }

    public Field[] getValues() {
        return values;
    }

    public String getFileName() {
        return fileName;
    }
}
