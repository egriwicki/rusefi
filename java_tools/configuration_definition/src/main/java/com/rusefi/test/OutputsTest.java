package com.rusefi.test;

import com.rusefi.ReaderState;
import com.rusefi.output.DataLogConsumer;
import com.rusefi.output.GaugeConsumer;
import com.rusefi.output.OutputsSectionConsumer;
import org.junit.Test;

import java.io.*;
import java.util.Arrays;
import java.util.Collections;

import static org.junit.Assert.assertEquals;

public class OutputsTest {
    private com.rusefi.output.GaugeConsumer GaugeConsumer;

    @Test
    public void generateSomething() throws IOException {
        String test = "struct total\n" +
                "float afr_type;PID dTime;\"ms\",      1,      0,       0, 3000,      0\n" +
                "uint8_t afr_typet;PID dTime;\"ms\",      1,      0,       0, 3000,      0\n" +
                "bit isForcedInduction;Does the vehicle have a turbo or supercharger?\n" +
                "bit enableFan1WithAc;+Turn on this fan when AC is on.\n" +
                "end_struct\n";
        ReaderState state = new ReaderState();

        OutputsSectionConsumer tsProjectConsumer = new OutputsSectionConsumer(null, state);
        state.readBufferedReader(test, Collections.singletonList(tsProjectConsumer));


        assertEquals("afr_type = scalar, F32, 0, \"ms\", 1, 0\n" +
                "afr_typet = scalar, U08, 4, \"ms\", 1, 0\n" +
                "isForcedInduction = bits, U32, 5, [0:0]\n" +
                "enableFan1WithAc = bits, U32, 5, [1:1]\n" +
                "unusedBit_4_2 = bits, U32, 5, [2:2]\n" +
                "unusedBit_4_3 = bits, U32, 5, [3:3]\n" +
                "unusedBit_4_4 = bits, U32, 5, [4:4]\n" +
                "unusedBit_4_5 = bits, U32, 5, [5:5]\n" +
                "unusedBit_4_6 = bits, U32, 5, [6:6]\n" +
                "unusedBit_4_7 = bits, U32, 5, [7:7]\n" +
                "unusedBit_4_8 = bits, U32, 5, [8:8]\n" +
                "unusedBit_4_9 = bits, U32, 5, [9:9]\n" +
                "unusedBit_4_10 = bits, U32, 5, [10:10]\n" +
                "unusedBit_4_11 = bits, U32, 5, [11:11]\n" +
                "unusedBit_4_12 = bits, U32, 5, [12:12]\n" +
                "unusedBit_4_13 = bits, U32, 5, [13:13]\n" +
                "unusedBit_4_14 = bits, U32, 5, [14:14]\n" +
                "unusedBit_4_15 = bits, U32, 5, [15:15]\n" +
                "unusedBit_4_16 = bits, U32, 5, [16:16]\n" +
                "unusedBit_4_17 = bits, U32, 5, [17:17]\n" +
                "unusedBit_4_18 = bits, U32, 5, [18:18]\n" +
                "unusedBit_4_19 = bits, U32, 5, [19:19]\n" +
                "unusedBit_4_20 = bits, U32, 5, [20:20]\n" +
                "unusedBit_4_21 = bits, U32, 5, [21:21]\n" +
                "unusedBit_4_22 = bits, U32, 5, [22:22]\n" +
                "unusedBit_4_23 = bits, U32, 5, [23:23]\n" +
                "unusedBit_4_24 = bits, U32, 5, [24:24]\n" +
                "unusedBit_4_25 = bits, U32, 5, [25:25]\n" +
                "unusedBit_4_26 = bits, U32, 5, [26:26]\n" +
                "unusedBit_4_27 = bits, U32, 5, [27:27]\n" +
                "unusedBit_4_28 = bits, U32, 5, [28:28]\n" +
                "unusedBit_4_29 = bits, U32, 5, [29:29]\n" +
                "unusedBit_4_30 = bits, U32, 5, [30:30]\n" +
                "unusedBit_4_31 = bits, U32, 5, [31:31]\n" +
                "alignmentFill_at_9 = array, U08, 9, [3], \"units\", 1, 0\n", new String(tsProjectConsumer.getTsWriter().toCharArray()));

    }

    @Test
    public void generateDataLog() throws IOException {
        String test = "struct total\n" +
                "bit issue_294_31,\"si_example\",\"nada_example\"\n" +
                "uint8_t[2 iterate] autoscale knock;;\"\",1, 0, 0, 0, 0\n" +
                "\tuint16_t autoscale baseFuel;@@GAUGE_NAME_FUEL_BASE@@\\nThis is the raw value we take from the fuel map or base fuel algorithm, before the corrections;\"mg\",{1/@@PACK_MULT_PERCENT@@}, 0, 0, 0, 0\n" +
                "float afr_type;PID dTime;\"ms\",      1,      0,       0, 3000,      0\n" +
                "uint16_t autoscale speedToRpmRatio;s2rpm;\"value\",{1/@@PACK_MULT_PERCENT@@}, 0, 0, 0, 0\n" +
                "uint8_t afr_typet;;\"ms\",      1,      0,       0, 3000,      0\n" +
                "uint8_t autoscale vehicleSpeedKph;;\"kph\",1, 0, 0, 0, 0\n" +
                "bit isForcedInduction;Does the vehicle have a turbo or supercharger?\n" +
                "\tuint8_t unused37;;\"\",1, 0, 0, 0, 0\n" +
                "bit enableFan1WithAc;+Turn on this fan when AC is on.\n" +
                "end_struct\n";
        ReaderState state = new ReaderState();
        state.variableRegistry.register("PACK_MULT_PERCENT", 100);
        state.variableRegistry.register("GAUGE_NAME_FUEL_BASE", "hello");

        DataLogConsumer dataLogConsumer = new DataLogConsumer(null, state);
        state.readBufferedReader(test, Collections.singletonList(dataLogConsumer));
        assertEquals(
                "entry = issue_294_31, \"issue_294_31\", int,    \"%d\"\n" +
                        "entry = knock1, \"knock1\", int,    \"%d\"\n" +
                        "entry = knock2, \"knock2\", int,    \"%d\"\n" +
                        "entry = baseFuel, \"hello\", float,  \"%.3f\"\n" +
                        "entry = afr_type, \"PID dTime\", float,  \"%.3f\"\n" +
                        "entry = speedToRpmRatio, \"s2rpm\", float,  \"%.3f\"\n" +
                        "entry = afr_typet, \"afr_typet\", int,    \"%d\"\n" +
                        "entry = vehicleSpeedKph, \"vehicleSpeedKph\", int,    \"%d\"\n" +
                        "entry = isForcedInduction, \"Does the vehicle have a turbo or supercharger?\", int,    \"%d\"\n" +
                        "entry = enableFan1WithAc, \"+Turn on this fan when AC is on.\", int,    \"%d\"\n", new String(dataLogConsumer.getTsWriter().toCharArray()));

    }

    @Test
    public void sensorStruct() throws IOException {
        String test = "struct total\n" +
                "    struct pid_status_s\n" +
                "    \tfloat iTerm;;\"v\", 1, 0, -10000, 10000, 4\n" +
                "    \tfloat dTerm;;\"v\", 1, 0, -10000, 10000, 4\n" +
                "    end_struct\n" +
                "\tpid_status_s alternatorStatus\n" +
                "\tpid_status_s idleStatus\n" +
                "end_struct\n";

        ReaderState state = new ReaderState();
        DataLogConsumer dataLogConsumer = new DataLogConsumer(null, state);
        GaugeConsumer gaugeConsumer = new GaugeConsumer(null, state);
        state.readBufferedReader(test, Arrays.asList(dataLogConsumer, gaugeConsumer));
        assertEquals(
                "entry = alternatorStatus_iTerm, \"iTerm\", float,  \"%.3f\"\n" +
                        "entry = alternatorStatus_dTerm, \"dTerm\", float,  \"%.3f\"\n" +
                        "entry = idleStatus_iTerm, \"iTerm\", float,  \"%.3f\"\n" +
                        "entry = idleStatus_dTerm, \"dTerm\", float,  \"%.3f\"\n",
                new String(dataLogConsumer.getTsWriter().toCharArray()));

        assertEquals("alternatorStatus_iTermGauge = alternatorStatus_iTerm,\"alternatorStatus_ iTerm\", \"v\", -10000.0,10000.0, -10000.0,10000.0, -10000.0,10000.0, 4,4\n" +
                        "alternatorStatus_dTermGauge = alternatorStatus_dTerm,\"alternatorStatus_ dTerm\", \"v\", -10000.0,10000.0, -10000.0,10000.0, -10000.0,10000.0, 4,4\n" +
                        "idleStatus_iTermGauge = idleStatus_iTerm,\"idleStatus_ iTerm\", \"v\", -10000.0,10000.0, -10000.0,10000.0, -10000.0,10000.0, 4,4\n" +
                        "idleStatus_dTermGauge = idleStatus_dTerm,\"idleStatus_ dTerm\", \"v\", -10000.0,10000.0, -10000.0,10000.0, -10000.0,10000.0, 4,4\n",
                new String(gaugeConsumer.getTsWriter().toCharArray()));

    }
}
