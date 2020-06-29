package com.rusefi;

import com.rusefi.config.generated.Fields;
import com.rusefi.core.EngineState;
import com.rusefi.core.Sensor;
import com.rusefi.core.SensorCentral;
import com.rusefi.io.CommandQueue;
import com.rusefi.io.ConnectionStateListener;
import com.rusefi.io.InvocationConfirmationListener;
import com.rusefi.io.LinkManager;
import com.rusefi.io.tcp.TcpConnector;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import static com.rusefi.waves.EngineReport.isCloseEnough;

/**
 * @author Andrey Belomutskiy
 *         3/19/14.
 */
public class IoUtil {

    /**
     * Send a command and wait for the confirmation
     *
     * @throws IllegalStateException if command was not confirmed
     */
    static void sendCommand(String command, CommandQueue commandQueue) {
        sendCommand(command, CommandQueue.DEFAULT_TIMEOUT, Timeouts.CMD_TIMEOUT, commandQueue);
    }

    public static String getEnableCommand(String settingName) {
        return Fields.CMD_ENABLE + " " + settingName;
    }

    public static String getDisableCommand(String settingName) {
        return Fields.CMD_DISABLE + " " + settingName;
    }

    /**
     * blocking method which would for confirmation from rusEfi
     */
    static void sendCommand(String command, int retryTimeoutMs, int timeoutMs, CommandQueue commandQueue) {
        final CountDownLatch responseLatch = new CountDownLatch(1);
        long time = System.currentTimeMillis();
        FileLog.MAIN.logLine("Sending command [" + command + "]");
        final long begin = System.currentTimeMillis();
        commandQueue.write(command, retryTimeoutMs, new InvocationConfirmationListener() {
            @Override
            public void onCommandConfirmation() {
                responseLatch.countDown();
                FileLog.MAIN.logLine("Got confirmation in " + (System.currentTimeMillis() - begin) + "ms");
            }
        });
        wait(responseLatch, timeoutMs);
        if (responseLatch.getCount() > 0)
            FileLog.MAIN.logLine("No confirmation in " + retryTimeoutMs);
        FileLog.MAIN.logLine("Command [" + command + "] executed in " + (System.currentTimeMillis() - time));
    }

    static void wait(CountDownLatch responseLatch, int milliseconds) {
        try {
            responseLatch.await(milliseconds, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e) {
            throw new IllegalStateException(e);
        }
    }

    static void changeRpm(CommandQueue commandQueue, final int rpm) {
        FileLog.MAIN.logLine("AUTOTEST rpm EN " + rpm);
        sendCommand("rpm " + rpm, commandQueue);
        long time = System.currentTimeMillis();

        final CountDownLatch rpmLatch = new CountDownLatch(1);
        SensorCentral.SensorListener listener = new SensorCentral.SensorListener() {
            @Override
            public void onSensorUpdate(double value) {
                double actualRpm = SensorCentral.getInstance().getValue(Sensor.RPM);
                if (isCloseEnough(rpm, actualRpm))
                    rpmLatch.countDown();
            }
        };
        SensorCentral.getInstance().addListener(Sensor.RPM, listener);
        try {
            rpmLatch.await(40, TimeUnit.SECONDS);
        } catch (InterruptedException e) {
            throw new IllegalStateException(e);
        }
        SensorCentral.getInstance().removeListener(Sensor.RPM, listener);

        double actualRpm = SensorCentral.getInstance().getValue(Sensor.RPM);

        if (!isCloseEnough(rpm, actualRpm))
            throw new IllegalStateException("rpm change did not happen: " + rpm + ", actual " + actualRpm);
//        sendCommand(Fields.CMD_RESET_ENGINE_SNIFFER);
        FileLog.MAIN.logLine("AUTOTEST RPM change [" + rpm + "] executed in " + (System.currentTimeMillis() - time));
    }

    static void waitForFirstResponse() throws InterruptedException {
        FileLog.MAIN.logLine("Let's give it some time to start...");
        final CountDownLatch startup = new CountDownLatch(1);
        SensorCentral.SensorListener listener = value -> startup.countDown();
        long waitStart = System.currentTimeMillis();
        SensorCentral.getInstance().addListener(Sensor.RPM, listener);
        startup.await(5, TimeUnit.SECONDS);
        SensorCentral.getInstance().removeListener(Sensor.RPM, listener);
        FileLog.MAIN.logLine("Got first signal in " + (System.currentTimeMillis() - waitStart));
    }

    static void connectToSimulator(LinkManager linkManager, boolean startProcess) throws InterruptedException {
        if (startProcess) {
            if (!TcpConnector.getAvailablePorts().isEmpty())
                throw new IllegalStateException("Port already binded on startup?");
            SimulatorExecHelper.startSimulator();
        }


//        FileLog.rlog("Waiting for TCP port...");
//        for (int i = 0; i < 180; i++) {
//            if (!TcpConnector.getAvailablePorts().isEmpty())
//                break;
//            Thread.sleep(1000);
//        }
//        if (TcpConnector.getAvailablePorts().isEmpty())
//            throw new IllegalStateException("Did we start it?");
//        /**
//         * If we open a connection just to validate that the process has started, we are getting
//         * weird issues with the second - actual connection
//         */
//        FileLog.rlog("Time for simulator to close the port...");
//        Thread.sleep(3000);
//
//        FileLog.rlog("Got a TCP port! Connecting...");

        /**
         * TCP connector is blocking
         */
        linkManager.startAndConnect("" + TcpConnector.DEFAULT_PORT, ConnectionStateListener.VOID);
        linkManager.getEngineState().registerStringValueAction(Fields.PROTOCOL_VERSION_TAG, (EngineState.ValueCallback<String>) EngineState.ValueCallback.VOID);
        waitForFirstResponse();
    }

    @SuppressWarnings("UnusedDeclaration")
    public static void sleepSeconds(int seconds) {
        FileLog.MAIN.logLine("Sleeping " + seconds + " seconds");
        try {
            Thread.sleep(seconds * 1000L);
        } catch (InterruptedException e) {
            throw new IllegalStateException(e);
        }
    }

    static void realHardwareConnect(LinkManager linkManager, String port) {
        linkManager.getEngineState().registerStringValueAction(Fields.PROTOCOL_VERSION_TAG, (EngineState.ValueCallback<String>) EngineState.ValueCallback.VOID);
        linkManager.getEngineState().registerStringValueAction(Fields.PROTOCOL_OUTPIN, (EngineState.ValueCallback<String>) EngineState.ValueCallback.VOID);
        linkManager.getEngineState().registerStringValueAction(AverageAnglesUtil.KEY, (EngineState.ValueCallback<String>) EngineState.ValueCallback.VOID);

        final CountDownLatch connected = linkManager.connect(port);
        if (connected.getCount() > 0)
            throw new IllegalStateException("Not connected in time");
    }
}
