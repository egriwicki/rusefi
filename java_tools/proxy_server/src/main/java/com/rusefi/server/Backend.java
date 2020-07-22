package com.rusefi.server;

import com.opensr5.Logger;
import com.rusefi.Listener;
import com.rusefi.io.IoStream;
import com.rusefi.io.commands.HelloCommand;
import com.rusefi.io.tcp.BinaryProtocolProxy;
import com.rusefi.io.tcp.BinaryProtocolServer;
import com.rusefi.io.tcp.TcpIoStream;
import com.rusefi.tools.online.ProxyClient;
import org.jetbrains.annotations.NotNull;
import org.takes.Take;
import org.takes.facets.fork.FkRegex;
import org.takes.facets.fork.TkFork;
import org.takes.http.FtBasic;
import org.takes.rs.RsHtml;
import org.takes.rs.RsJson;

import javax.json.Json;
import javax.json.JsonArrayBuilder;
import javax.json.JsonObject;
import java.io.Closeable;
import java.io.IOException;
import java.net.BindException;
import java.net.Socket;
import java.util.*;
import java.util.concurrent.atomic.AtomicLong;
import java.util.function.Function;

/**
 * See NetworkConnectorStartup
 */
public class Backend implements Closeable {
    public static final String VERSION_PATH = "/version";
    public static final String BACKEND_VERSION = "0.0001";
    public static final int SERVER_PORT_FOR_CONTROLLERS = 8003;

    private final FkRegex showOnlineUsers = new FkRegex(ProxyClient.LIST_PATH,
            (Take) req -> getUsersOnline()
    );
    private boolean isClosed;

    // guarded by own monitor
    private final Set<ControllerConnectionState> clients = new HashSet<>();
    // guarded by clients
    private HashMap<ControllerKey, ControllerConnectionState> byId = new HashMap<>();
    //    private final int clientTimeout;
    private final UserDetailsResolver userDetailsResolver;
    private final Logger logger;
    public final static AtomicLong totalSessions = new AtomicLong();
    public int serverPortForApplications;
    public int serverPortForControllers;

    public Backend(UserDetailsResolver userDetailsResolver, int httpPort, Logger logger) {
//        this.clientTimeout = clientTimeout;
        this.userDetailsResolver = userDetailsResolver;
        this.logger = logger;


        new Thread(() -> {
            try {
                System.out.println("Starting http backend on " + httpPort);
                try {
                    new FtBasic(
                            new TkFork(showOnlineUsers,
                                    new Monitoring(this).showStatistics,
                                    new FkRegex(VERSION_PATH, BACKEND_VERSION),
                                    new FkRegex("/", new RsHtml("<html><body>\n" +
                                            "<a href='https://rusefi.com/online/'>rusEFI Online</a>\n" +
                                            "<br/>\n" +
                                            "<a href='" + Monitoring.STATUS + "'>Status</a>\n" +
                                            "<br/>\n" +
                                            "<a href='" + ProxyClient.LIST_PATH + "'>List</a>\n" +
                                            "<br/>\n" +
                                            "<br/>\n" +
                                            "</body></html>\n"))
                            ), httpPort
                    ).start(() -> isClosed);
                } catch (BindException e) {
                    throw new IllegalStateException("While binding " + httpPort, e);
                }
                logger.info("Shutting down backend on port " + httpPort);
            } catch (IOException e) {
                throw new IllegalStateException(e);
            }

        }, "Http Server Thread").start();


//        new Thread(() -> {
//            while (true) {
//                runCleanup();
//                sleep(clientTimeout);
//            }
//        }, "rusEFI Server Cleanup").start();
    }

    public void runApplicationConnector(int serverPortForApplications, Listener serverSocketCreationCallback) {
        this.serverPortForApplications = serverPortForApplications;
        // connection from authenticator app which proxies for Tuner Studio
        // authenticator pushed hello packet on connect
        System.out.println("Starting application connector at " + serverPortForApplications);
        BinaryProtocolServer.tcpServerSocket(logger, new Function<Socket, Runnable>() {
            @Override
            public Runnable apply(Socket applicationSocket) {
                return new Runnable() {
                    @Override
                    public void run() {
                        totalSessions.incrementAndGet();
                        // connection from authenticator app which proxies for Tuner Studio
                        IoStream applicationClientStream = null;
                        try {
                            applicationClientStream = new TcpIoStream("[app] ", logger, applicationSocket);

                            // authenticator pushed hello packet on connect
                            String jsonString = HelloCommand.getHelloResponse(applicationClientStream.getDataBuffer(), logger);
                            if (jsonString == null)
                                return;
                            ApplicationRequest applicationRequest = ApplicationRequest.valueOf(jsonString);
                            logger.info("Application Connected: " + applicationRequest);

                            ControllerKey controllerKey = new ControllerKey(applicationRequest.getTargetUserId(), applicationRequest.getSessionDetails().getControllerInfo());
                            ControllerConnectionState state;
                            synchronized (clients) {
                                state = byId.get(controllerKey);
                            }
                            if (state == null) {
                                applicationClientStream.close();
                                onDisconnectApplication();
                                logger.info("No controller for " + controllerKey);
                                return;
                            }

                            BinaryProtocolProxy.runProxy(state.getStream(), applicationClientStream);

                        } catch (Throwable e) {
                            if (applicationClientStream != null)
                                applicationClientStream.close();
                            e.printStackTrace();
                            logger.info("Got error " + e);
                            onDisconnectApplication();
                        }
                    }
                };
            }
        }, serverPortForApplications, "ApplicationServer", serverSocketCreationCallback, BinaryProtocolServer.SECURE_SOCKET_FACTORY);
    }

    protected void onDisconnectApplication() {
        logger.info("Disconnecting application");
    }

    public void runControllerConnector(int serverPortForControllers, Listener serverSocketCreationCallback) {
        this.serverPortForControllers = serverPortForControllers;
        System.out.println("Starting controller connector at " + serverPortForControllers);
        BinaryProtocolServer.tcpServerSocket(logger, new Function<Socket, Runnable>() {
            @Override
            public Runnable apply(Socket controllerSocket) {
                return new Runnable() {
                    @Override
                    public void run() {
                        totalSessions.incrementAndGet();
                        ControllerConnectionState controllerConnectionState = new ControllerConnectionState(controllerSocket, logger, getUserDetailsResolver());
                        try {
                            controllerConnectionState.requestControllerInfo();

                            // IMPORTANT: has to happen before we register controller while we still have exclusive access
                            controllerConnectionState.getOutputs();

                            register(controllerConnectionState);
                        } catch (Throwable e) {
                            close(controllerConnectionState);
                        }
                    }
                };
            }
        }, serverPortForControllers, "ControllerServer", serverSocketCreationCallback, BinaryProtocolServer.SECURE_SOCKET_FACTORY);
    }

    @NotNull
    private RsJson getUsersOnline() throws IOException {
        JsonArrayBuilder builder = Json.createArrayBuilder();
        List<ControllerConnectionState> clients = getClients();
        for (ControllerConnectionState client : clients) {

            JsonObject clientObject = Json.createObjectBuilder()
                    .add(UserDetails.USER_ID, client.getUserDetails().getUserId())
                    .add(UserDetails.USERNAME, client.getUserDetails().getUserName())
                    .add(ControllerInfo.SIGNATURE, client.getSessionDetails().getControllerInfo().getSignature())
                    .add(ControllerInfo.VEHICLE_NAME, client.getSessionDetails().getControllerInfo().getVehicleName())
                    .add(ControllerInfo.ENGINE_MAKE, client.getSessionDetails().getControllerInfo().getEngineMake())
                    .add(ControllerInfo.ENGINE_CODE, client.getSessionDetails().getControllerInfo().getEngineCode())
                    .build();
            builder.add(clientObject);
        }
        return new RsJson(builder.build());
    }

    public UserDetailsResolver getUserDetailsResolver() {
        return userDetailsResolver;
    }

//    private void runCleanup() {
//        List<ClientConnectionState> inactiveClients = new ArrayList<>();
//
//        synchronized (clients) {
//            long now = System.currentTimeMillis();
//            for (ClientConnectionState client : clients) {
//                if (now - client.getLastActivityTimestamp() > clientTimeout)
//                    inactiveClients.add(client);
//            }
//        }
//
//        for (ClientConnectionState inactiveClient : inactiveClients) {
//            close(inactiveClient);
//        }
//
//    }

    public void register(ControllerConnectionState controllerConnectionState) {
        Objects.requireNonNull(controllerConnectionState.getControllerKey(), "ControllerKey");
        synchronized (clients) {
            clients.add(controllerConnectionState);
            byId.put(controllerConnectionState.getControllerKey(), controllerConnectionState);
        }
        onRegister(controllerConnectionState);
    }

    protected void onRegister(ControllerConnectionState controllerConnectionState) {
    }

    public void close(ControllerConnectionState inactiveClient) {
        inactiveClient.close();
        synchronized (clients) {
            // in case of exception in the initialization phase we do not even add client into the the collection
            clients.remove(inactiveClient);
            byId.remove(inactiveClient.getControllerKey());
        }
    }

    @Override
    public void close() {
        isClosed = true;
    }

    public List<ControllerConnectionState> getClients() {
        synchronized (clients) {
            return new ArrayList<>(clients);
        }
    }

    public int getCount() {
        synchronized (clients) {
            return clients.size();
        }
    }

}
