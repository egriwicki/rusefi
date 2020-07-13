package com.rusefi;

import com.opensr5.Logger;
import com.rusefi.binaryprotocol.IncomingDataBuffer;
import com.rusefi.config.generated.Fields;
import com.rusefi.io.commands.HelloCommand;
import com.rusefi.io.tcp.BinaryProtocolServer;
import com.rusefi.io.tcp.TcpIoStream;

import java.io.IOException;
import java.net.Socket;

import static com.rusefi.io.TcpCommunicationIntegrationTest.LOCALHOST;
import static com.rusefi.io.tcp.BinaryProtocolServer.getPacketLength;
import static com.rusefi.io.tcp.BinaryProtocolServer.readPromisedBytes;

public class MockRusEfiDevice {
    private final String authToken;
    private final String signature;
    private final Logger logger;

    public MockRusEfiDevice(String authToken, String signature, Logger logger) {
        this.authToken = authToken;
        this.signature = signature;
        this.logger = logger;
    }

    public void connect(int serverPort) throws IOException {
        TcpIoStream stream = new TcpIoStream(logger, new Socket(LOCALHOST, serverPort));
        IncomingDataBuffer in = stream.getDataBuffer();

        new Thread(() -> {
            try {
                while (true) {
                    int length = getPacketLength(in, () -> {
                        throw new UnsupportedOperationException();
                    });
                    BinaryProtocolServer.Packet packet = readPromisedBytes(in, length);
                    byte[] payload = packet.getPacket();

                    byte command = payload[0];


                    if (command == Fields.TS_HELLO_COMMAND) {
                        new HelloCommand(logger, authToken + signature).handle(packet, stream);
                    } else {
                        handleCommand();
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }).start();
    }

    protected void handleCommand() {
    }
}
