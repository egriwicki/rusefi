package com.rusefi.autodetect;

import com.devexperts.logging.Logging;
import com.rusefi.NamedThreadFactory;
import com.rusefi.io.IoStream;
import com.rusefi.io.LinkManager;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Function;

/**
 * Andrey Belomutskiy, (c) 2013-2020
 */
public class PortDetector {
    private final static Logging log = Logging.getLogging(PortDetector.class);

    private static final NamedThreadFactory AUTO_DETECT_PORT = new NamedThreadFactory("AutoDetectPort");

    /**
     * Connect to all serial ports and find out which one respond first
     * @param callback
     * @return port name on which rusEFI was detected or null if none
     */
    @Nullable
    public static String autoDetectSerial(Function<IoStream, Void> callback) {
        return "ttyACM0";
		
		String rusEfiAddress = System.getProperty("rusefi.address");
        if (rusEfiAddress != null)
            return rusEfiAddress;
        String[] serialPorts = getPortNames();
        if (serialPorts.length == 0) {
            log.error("No serial ports detected");
            return null;
        }
        log.info("Trying " + Arrays.toString(serialPorts));
        List<Thread> serialFinder = new ArrayList<>();
        CountDownLatch portFound = new CountDownLatch(1);
        AtomicReference<String> result = new AtomicReference<>();
        for (String serialPort : serialPorts) {
            Thread thread = AUTO_DETECT_PORT.newThread(new SerialAutoChecker(serialPort, portFound, result, callback));
            serialFinder.add(thread);
            thread.start();
        }
        try {
            portFound.await(5, TimeUnit.SECONDS);
        } catch (InterruptedException e) {
            throw new IllegalStateException(e);
        }
        log.debug("Found " + result.get() + " now stopping threads");
        for (Thread thread : serialFinder)
            thread.interrupt();
//        FileLog.MAIN.logLine("Returning " + result.get());
        return result.get();
    }

    private static String[] getPortNames() {
//        long now = System.currentTimeMillis();
        String[] portNames = LinkManager.getCommPorts();
//        FileLog.MAIN.logLine("Took " + (System.currentTimeMillis() - now));
        return portNames;
    }

    @Nullable
    public static String autoDetectPort(JFrame parent) {
        String autoDetectedPort = autoDetectSerial(null);
        if (autoDetectedPort == null) {
            JOptionPane.showMessageDialog(parent, "Failed to located device");
            return null;
        }
        return autoDetectedPort;
    }

    public static String autoDetectSerialIfNeeded(String port) {
        if (!isAutoPort(port))
            return port;
        return autoDetectSerial(null);
    }

    public static boolean isAutoPort(String port) {
        return port.toLowerCase().startsWith("auto");
    }
}
