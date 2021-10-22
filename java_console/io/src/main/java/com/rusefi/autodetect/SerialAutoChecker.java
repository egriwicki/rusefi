package com.rusefi.autodetect;

import com.devexperts.logging.Logging;
import com.rusefi.binaryprotocol.IncomingDataBuffer;
import com.rusefi.config.generated.Fields;
import com.rusefi.io.IoStream;
import com.rusefi.io.commands.HelloCommand;
import com.rusefi.io.serial.SerialIoStreamJSerialComm;
import org.jetbrains.annotations.Nullable;

import java.io.IOException;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Function;

import static com.rusefi.binaryprotocol.IoHelper.checkResponseCode;

public class SerialAutoChecker {
    private final static Logging log = Logging.getLogging(SerialAutoChecker.class);
    private final String serialPort;
    private final CountDownLatch portFound;

    public SerialAutoChecker(String serialPort, CountDownLatch portFound) {
        this.serialPort = serialPort;
        this.portFound = portFound;
    }

    public String checkResponse(IoStream stream, Function<CallbackContext, Void> callback) {
        IncomingDataBuffer incomingData = stream.getDataBuffer();
        try {
            HelloCommand.send(stream);
            byte[] response = incomingData.getPacket("auto detect");
            if (!checkResponseCode(response, (byte) Fields.TS_RESPONSE_OK))
                return null;
            String signature = new String(response, 1, response.length - 1);
            if (!signature.startsWith(Fields.PROTOCOL_SIGNATURE_PREFIX)) {
                return null;
            }
            log.info("Got signature=" + signature + " from " + serialPort);
            if (callback != null) {
                callback.apply(new CallbackContext(stream, signature));
            }
            return signature;
        } catch (IOException ignore) {
            return null;
        }
    }

    public void openAndCheckResponse(AtomicReference<AutoDetectResult> result, Function<CallbackContext, Void> callback) {
        String signature;
        try (IoStream stream = SerialIoStreamJSerialComm.openPort(serialPort)) {
            signature = checkResponse(stream, callback);
        }
        if (signature != null) {
            /**
             * propagating result after closing the port so that it could be used right away
             */
            result.set(new AutoDetectResult(serialPort, signature));
            portFound.countDown();
        }
    }

    public static class CallbackContext {
        private final IoStream stream;
        private final String signature;

        public CallbackContext(IoStream stream, String signature) {
            this.stream = stream;
            this.signature = signature;
        }

        public String getSignature() {
            return signature;
        }

        public IoStream getStream() {
            return stream;
        }
    }

    public static class AutoDetectResult {

        private final String serialPort;
        private final String signature;

        public AutoDetectResult(String serialPort, String signature) {
            this.serialPort = serialPort;
            this.signature = signature;
        }

        @Nullable
        public String getSerialPort() {
            return serialPort;
        }

        @Nullable
        public String getSignature() {
            return signature;
        }
    }
}
