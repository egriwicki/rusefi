package com.rusefi;

import com.opensr5.ConfigurationImage;
import com.opensr5.Logger;
import com.opensr5.ini.field.ScalarIniField;
import com.rusefi.binaryprotocol.BinaryProtocolState;
import com.rusefi.config.Field;
import com.rusefi.config.generated.Fields;
import com.rusefi.io.IoStream;
import com.rusefi.io.LinkConnector;
import com.rusefi.io.LinkManager;
import com.rusefi.io.tcp.BinaryProtocolServer;
import com.rusefi.io.tcp.TcpIoStream;
import com.rusefi.server.rusEFISSLContext;
import com.rusefi.tune.xml.Constant;
import org.jetbrains.annotations.NotNull;

import java.io.IOException;
import java.net.Socket;

public class TestHelper {
    public static final String LOCALHOST = "localhost";

    @NotNull
    public static ScalarIniField createIniField(Field field) {
        return new ScalarIniField(field.getName(), field.getOffset(), "", field.getType(), 1, "0");
    }

    @NotNull
    public static ConfigurationImage prepareImage(int input, ScalarIniField scalarIniField) {
        ConfigurationImage ci = new ConfigurationImage(Fields.TOTAL_CONFIG_SIZE);

        scalarIniField.setValue(ci, new Constant(scalarIniField.getName(), "", Integer.toString(input), scalarIniField.getDigits()));
        return ci;
    }

    @NotNull
    public static BinaryProtocolServer createVirtualController(ConfigurationImage ci, int port, Listener serverSocketCreationCallback, Logger logger) {
        BinaryProtocolState state = new BinaryProtocolState();
        state.setController(ci);
        state.setCurrentOutputs(new byte[1 + Fields.TS_OUTPUT_SIZE]);

        LinkManager linkManager = new LinkManager(logger);
        linkManager.setConnector(LinkConnector.getDetachedConnector(state));
        BinaryProtocolServer server = new BinaryProtocolServer(logger);
        server.start(linkManager, port, serverSocketCreationCallback);
        return server;
    }

    @NotNull
    public static IoStream secureConnectToLocalhost(int controllerPort, Logger logger) {
        IoStream targetEcuSocket;
        try {
            targetEcuSocket = new TcpIoStream("[local]", logger, rusEFISSLContext.getSSLSocket(LOCALHOST, controllerPort));
        } catch (IOException e) {
            throw new IllegalStateException("Failed to connect to controller " + LOCALHOST + ":" + controllerPort);
        }
        return targetEcuSocket;
    }

    @NotNull
    public static IoStream connectToLocalhost(int controllerPort, Logger logger) {
        IoStream targetEcuSocket;
        try {
            targetEcuSocket = new TcpIoStream("[local]", logger, new Socket(LOCALHOST, controllerPort));
        } catch (IOException e) {
            throw new IllegalStateException("Failed to connect to controller " + LOCALHOST + ":" + controllerPort);
        }
        return targetEcuSocket;
    }
}
