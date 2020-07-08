package com.rusefi.io;

import com.fazecast.jSerialComm.SerialPort;
import com.opensr5.Logger;
import com.rusefi.Callable;
import com.rusefi.NamedThreadFactory;
import com.rusefi.binaryprotocol.BinaryProtocol;
import com.rusefi.binaryprotocol.BinaryProtocolState;
import com.rusefi.core.EngineState;
import com.rusefi.io.serial.SerialConnector;
import com.rusefi.io.serial.SerialIoStreamJSerialComm;
import com.rusefi.io.tcp.TcpConnector;
import com.rusefi.io.tcp.TcpIoStream;
import org.jetbrains.annotations.NotNull;

import java.net.Socket;
import java.util.Arrays;
import java.util.Objects;
import java.util.concurrent.*;

/**
 * See TcpCommunicationIntegrationTest
 *
 * @author Andrey Belomutskiy
 * 3/3/14
 */
public class LinkManager {
    @NotNull
    public static LogLevel LOG_LEVEL = LogLevel.INFO;

    public static LinkDecoder ENCODER = new LinkDecoder() {
        @Override
        public String unpack(String packedLine) {
            return packedLine;
        }
    };

    public static final String LOG_VIEWER = "log viewer";
    private final CommandQueue commandQueue;
    private final Logger logger;

    private LinkConnector connector;
    private boolean isStarted;

    public LinkManager(Logger logger) {
        this.logger = logger;
        engineState = new EngineState(new EngineState.EngineStateListenerImpl() {
            @Override
            public void beforeLine(String fullLine) {
                logger.info(fullLine);
                HeartBeatListeners.onDataArrived();
            }
        }, logger);
        commandQueue = new CommandQueue(this, logger);
    }

    @NotNull
    public CountDownLatch connect(String port) {
        final CountDownLatch connected = new CountDownLatch(1);
        startAndConnect(port, new ConnectionStateListener() {
            @Override
            public void onConnectionFailed() {
                System.out.println("CONNECTION FAILED, did you specify the right port name?");
                System.exit(-1);
            }

            @Override
            public void onConnectionEstablished() {
                connected.countDown();
            }
        });
        try {
            connected.await(60, TimeUnit.SECONDS);
        } catch (InterruptedException e) {
            throw new IllegalStateException(e);
        }
        return connected;
    }

    public void execute(Runnable runnable) {
        COMMUNICATION_EXECUTOR.execute(runnable);
    }

    public Future submit(Runnable runnable) {
        return COMMUNICATION_EXECUTOR.submit(runnable);
    }

    public static String[] getCommPorts() {
        SerialPort[] ports = SerialPort.getCommPorts();
        String[] result = new String[ports.length];
        for (int i = 0; i < ports.length; i++)
            result[i] = ports[i].getSystemPortName();
        return result;
    }

    public BinaryProtocol getCurrentStreamState() {
        Objects.requireNonNull(connector, "connector");
        return connector.getBinaryProtocol();
    }

    public BinaryProtocolState getBinaryProtocolState() {
        return connector.getBinaryProtocolState();
    }

    public CommandQueue getCommandQueue() {
        return commandQueue;
    }

    public enum LogLevel {
        INFO,
        DEBUG,
        TRACE;

        public boolean isDebugEnabled() {
            return this == DEBUG || this == TRACE;
        }
    }

    public final LinkedBlockingQueue<Runnable> COMMUNICATION_QUEUE = new LinkedBlockingQueue<>();
    /**
     * All request/responses to underlying controller are happening on this single-threaded executor in a FIFO manner
     *
     */
    public final ExecutorService COMMUNICATION_EXECUTOR = new ThreadPoolExecutor(1, 1,
            0L, TimeUnit.MILLISECONDS,
            COMMUNICATION_QUEUE,
            new NamedThreadFactory("communication executor"));

    static {
/*
        Future future = submit(new Runnable() {
            @Override
            public void run() {
            // WAT? this is hanging?!
                COMMUNICATION_THREAD = Thread.currentThread();
                System.out.println("Done");
            }
        });
        try {
            // let's wait for the above trivial task to finish
            future.get();
            System.out.println("Done2");
        } catch (InterruptedException | ExecutionException e) {
            throw new IllegalStateException(e);
        }
 */
    }

    public static void assertCommunicationThread() {
//        if (Thread.currentThread() != COMMUNICATION_THREAD)
//            throw new IllegalStateException("Communication on wrong thread");
    }

    private EngineState engineState;

    public EngineState getEngineState() {
        return engineState;
    }

    /**
     * This flag controls if mock controls are needed
     * todo: decouple from TcpConnector since not really related
     */
    public static boolean isSimulationMode;

    public void startAndConnect(String port, ConnectionStateListener stateListener) {
        Objects.requireNonNull(port, "port");
        start(port, stateListener);
        connector.connectAndReadConfiguration(stateListener);
    }

    public LinkConnector getConnector() {
        return connector;
    }

    public void start(String port, ConnectionStateListener stateListener) {
        Objects.requireNonNull(port, "port");
        logger.info("LinkManager: Starting " + port);
        if (isLogViewerMode(port)) {
            setConnector(LinkConnector.VOID);
        } else if (TcpConnector.isTcpPort(port)) {
            setConnector(new SerialConnector(this, port, logger, new Callable<IoStream>() {
                @Override
                public IoStream call() {
                    Socket socket;
                    try {
                        int portPart = TcpConnector.getTcpPort(port);
                        String hostname = TcpConnector.getHostname(port);
                        socket = new Socket(hostname, portPart);
                        return new TcpIoStream(logger, LinkManager.this, socket);
                    } catch (Throwable e) {
                        stateListener.onConnectionFailed();
                        return null;
                    }
                }
            }));
            isSimulationMode = true;
        } else {
            setConnector(new SerialConnector(this, port, logger, () -> SerialIoStreamJSerialComm.openPort(port, logger)));
        }
    }

    public void setConnector(LinkConnector connector) {
        if (isStarted) {
            throw new IllegalStateException("Already started");
        }
        isStarted = true;
        this.connector = connector;
    }

    public static boolean isLogViewerMode(String port) {
        Objects.requireNonNull(port, "port");
        return port.equals(LOG_VIEWER);
    }

    public boolean isLogViewer() {
        return connector == LinkConnector.VOID;
    }

    public void send(String command, boolean fireEvent) throws InterruptedException {
        if (this.connector == null)
            throw new NullPointerException("connector");
        this.connector.send(command, fireEvent);
    }

    public void restart() {
        ConnectionStatusLogic.INSTANCE.setValue(ConnectionStatusValue.NOT_CONNECTED);
        connector.restart();
    }

    public void stop() {
        connector.stop();
    }

    public static String unpackConfirmation(String message) {
        if (message.startsWith(CommandQueue.CONFIRMATION_PREFIX))
            return message.substring(CommandQueue.CONFIRMATION_PREFIX.length());
        return null;
    }

    /**
     * @return null if no port located
     */
    public static String getDefaultPort() {
        String[] ports = getCommPorts();
        if (ports.length == 0) {
            System.out.println("Port not specified and no ports found");
            return null;
        }
        String port = ports[ports.length - 1];
        // todo: reuse 'PortDetector.autoDetectPort' here?
        System.out.println("Using last of " + ports.length + " port(s)");
        System.out.println("All ports: " + Arrays.toString(ports));
        return port;
    }
}
