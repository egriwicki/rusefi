package com.rusefi.ts_plugin;

import com.rusefi.LocalApplicationProxy;
import com.rusefi.NamedThreadFactory;
import com.rusefi.SignatureHelper;
import com.rusefi.autoupdate.AutoupdateUtil;
import com.rusefi.io.tcp.ServerHolder;
import com.rusefi.io.tcp.TcpIoStream;
import com.rusefi.server.ApplicationRequest;
import com.rusefi.server.ControllerInfo;
import com.rusefi.server.SessionDetails;
import com.rusefi.tools.online.HttpUtil;
import com.rusefi.tools.online.ProxyClient;
import com.rusefi.tools.online.PublicSession;
import com.rusefi.ui.AuthTokenPanel;
import com.rusefi.ui.util.URLLabel;
import org.putgemin.VerticalFlowLayout;

import javax.swing.*;
import java.awt.*;
import java.io.IOException;
import java.util.List;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicReference;

import static com.rusefi.ui.storage.PersistentConfiguration.getConfig;

/**
 * see RemoteTabSandbox
 */
public class RemoteTab {
    private static final String APPLICATION_PORT = "application_port";
    private final JComponent content = new JPanel(new BorderLayout());

    private final JPanel list = new JPanel(new VerticalFlowLayout());
    private final JTextField oneTimePasswordControl = new JTextField("0") {
        @Override
        public Dimension getPreferredSize() {
            Dimension size = super.getPreferredSize();
            // todo: dynamic calculation of desired with based on String width?
            return new Dimension(100, size.height);
        }
    };

    private final Executor listDownloadExecutor = Executors.newSingleThreadExecutor(new NamedThreadFactory("online list downloader"));

    public RemoteTab() {
        JButton refresh = new JButton("Refresh List");
        refresh.addActionListener(e -> requestListDownload());

        JTextField applicationPort = new JTextField() {
            @Override
            public Dimension getPreferredSize() {
                Dimension size = super.getPreferredSize();
                // todo: dynamic calculation of desired with based on String width?
                return new Dimension(100, size.height);
            }
        };
        IntegerDocumentFilter.install(applicationPort);
        IntegerDocumentFilter.install(oneTimePasswordControl);
        String portProperty = getLocalPort();
        applicationPort.setText(portProperty);

        JPanel topLines = new JPanel(new VerticalFlowLayout());


        JPanel topPanel = new JPanel(new FlowLayout());
        topPanel.add(refresh);
        topPanel.add(new JLabel("    Local Port: "));
        topPanel.add(applicationPort);
        topPanel.add(new JLabel("   One time password:"));
        topPanel.add(oneTimePasswordControl);

        topLines.add(topPanel);
        topLines.add(new URLLabel("https://github.com/rusefi/rusefi/wiki/HOWTO-Remote-Tuning"));

        content.add(topLines, BorderLayout.NORTH);
        content.add(list, BorderLayout.CENTER);
        list.add(new JLabel("Requesting list of ECUs"));
        requestListDownload();
    }

    private String getLocalPort() {
        return getConfig().getRoot().getProperty(APPLICATION_PORT, "29001");
    }

    private void requestListDownload() {
        listDownloadExecutor.execute(() -> {
            List<PublicSession> userDetails;
            try {
                userDetails = ProxyClient.getOnlineApplications(HttpUtil.PROXY_JSON_API_HTTP_PORT);
                SwingUtilities.invokeLater(() -> showList(userDetails));
            } catch (IOException e) {
                e.printStackTrace();
                return;
            }
        });
    }

    private void showList(List<PublicSession> userDetails) {
        list.removeAll();
        if (userDetails.isEmpty()) {
            list.add(new JLabel("No ECUs are broadcasting at the moment :("));
        } else {
            for (PublicSession user : userDetails) {
                list.add(createPanel(user));
            }
        }
        AutoupdateUtil.trueLayout(list);
    }

    private JComponent createPanel(PublicSession publicSession) {
        JComponent userPanel = new JPanel(new FlowLayout());
        userPanel.add(new JLabel(publicSession.getUserDetails().getUserName()));
        ControllerInfo controllerInfo = publicSession.getControllerInfo();
        userPanel.add(new JLabel(controllerInfo.getVehicleName() + " " + controllerInfo.getEngineMake() + " " + controllerInfo.getEngineCode()));

        userPanel.add(new URLLabel(SignatureHelper.getUrl(controllerInfo.getSignature())));

        if (publicSession.isUsed()) {
            userPanel.add(new JLabel(" used by " + publicSession.getOwnerName()));
        } else {
            JButton connect = new JButton("Connect");
            connect.addActionListener(event -> {

                setStatus("Connecting to " + publicSession.getUserDetails().getUserName());

                new Thread(() -> runAuthenticator(publicSession, controllerInfo), "Authenticator").start();
            });
            userPanel.add(connect);
        }

        return userPanel;
    }

    private void setStatus(String text) {
        list.removeAll();
        list.add(new JLabel(text));
        AutoupdateUtil.trueLayout(list);
    }

    private void runAuthenticator(PublicSession publicSession, ControllerInfo controllerInfo) {
        SessionDetails sessionDetails = new SessionDetails(controllerInfo, AuthTokenPanel.getAuthToken(),
                Integer.parseInt(oneTimePasswordControl.getText()));

        ApplicationRequest applicationRequest = new ApplicationRequest(sessionDetails, publicSession.getUserDetails().getUserId());

        try {
            AtomicReference<ServerHolder> serverHolderAtomicReference = new AtomicReference<>();

            TcpIoStream.DisconnectListener disconnectListener = () -> SwingUtilities.invokeLater(() -> {
                setStatus("Disconnected");
                ServerHolder serverHolder = serverHolderAtomicReference.get();
                if (serverHolder != null)
                    serverHolder.close();
            });

            ServerHolder serverHolder = LocalApplicationProxy.startAndRun(
                    LocalApplicationProxy.SERVER_PORT_FOR_APPLICATIONS,
                    applicationRequest,
                    Integer.parseInt(getLocalPort()),
                    HttpUtil.PROXY_JSON_API_HTTP_PORT, disconnectListener);
            serverHolderAtomicReference.set(serverHolder);
        } catch (IOException e) {
            setStatus("IO error: " + e);
        }
    }

    public JComponent getContent() {
        return content;
    }
}
