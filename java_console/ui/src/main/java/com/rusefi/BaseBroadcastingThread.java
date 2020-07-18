package com.rusefi;

import com.opensr5.Logger;
import com.rusefi.binaryprotocol.IncomingDataBuffer;
import com.rusefi.config.generated.Fields;
import com.rusefi.io.commands.HelloCommand;
import com.rusefi.io.tcp.BinaryProtocolServer;
import com.rusefi.io.tcp.TcpIoStream;
import com.rusefi.server.SessionDetails;

import java.io.IOException;
import java.net.Socket;

import static com.rusefi.io.tcp.BinaryProtocolServer.getPacketLength;
import static com.rusefi.io.tcp.BinaryProtocolServer.readPromisedBytes;

public class BaseBroadcastingThread {
    private final Thread thread;

    public BaseBroadcastingThread(Socket socket, SessionDetails sessionDetails, Logger logger) throws IOException {
        TcpIoStream stream = new TcpIoStream(logger, socket);
        IncomingDataBuffer in = stream.getDataBuffer();

        thread = new Thread(() -> {
            try {
                while (true) {
                    int length = getPacketLength(in, () -> {
                        throw new UnsupportedOperationException();
                    });
                    BinaryProtocolServer.Packet packet = readPromisedBytes(in, length);
                    byte[] payload = packet.getPacket();

                    byte command = payload[0];

                    if (command == Fields.TS_HELLO_COMMAND) {
                        // respond on hello request with information about session
                        new HelloCommand(logger, sessionDetails.toJson()).handle(stream);
                    } else {
                        handleCommand(packet, stream);
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        });
    }

    protected void handleCommand(BinaryProtocolServer.Packet packet, TcpIoStream stream) throws IOException {
    }

    public void start() {
        thread.start();
    }
}
