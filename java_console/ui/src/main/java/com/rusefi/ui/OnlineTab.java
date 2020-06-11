package com.rusefi.ui;

import com.rusefi.io.LinkManager;
import com.rusefi.tools.online.Online;
import com.rusefi.tune.xml.Msq;
import com.rusefi.ui.util.Misc;
import org.putgemin.VerticalFlowLayout;

import javax.swing.*;

import java.awt.event.ActionEvent;

public class OnlineTab {

    private final JPanel content = new JPanel(new VerticalFlowLayout());

    public OnlineTab() {
        AuthTokenPanel authTokenPanel = new AuthTokenPanel();

        content.add(Misc.getRusEFI_online_manual());

        content.add(authTokenPanel.getContent());

        JButton upload = new JButton("Upload");
        upload.addActionListener(new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                Msq tune = Msq.toMsq(LinkManager.connector.getBinaryProtocol().getControllerConfiguration());
                Online.uploadTune(tune, authTokenPanel, content);
            }
        });
        content.add(upload);
    }

    public JPanel getContent() {
        return content;
    }
}
