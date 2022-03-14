package com.rusefi.ui;

import com.devexperts.logging.Logging;
import com.rusefi.FileLog;
import com.rusefi.ui.util.DefaultExceptionHandler;
import com.rusefi.ui.util.FrameHelper;
import com.rusefi.ui.util.UiUtils;
import org.jetbrains.annotations.NotNull;

import javax.swing.*;
import java.awt.*;

import static com.devexperts.logging.Logging.getLogging;

/**
 * Andrey Belomutskiy, (c) 2013-2020
 * 3/7/2015
 */
public class StatusWindow implements StatusConsumer {
    private static final Logging log = getLogging(StatusWindow.class);

    private static final Color LIGHT_RED = new Color(255, 102, 102);
    private static final Color LIGHT_GREEN = new Color(102, 255 ,102);
    // todo: extract driver from console bundle? find a separate driver bundle?
    private final JTextArea logTextArea = new JTextArea();
    private final JPanel content = new JPanel(new BorderLayout());
    private final JLabel bottomStatusLabel = new JLabel();
    @NotNull
    protected final FrameHelper frameHelper = new FrameHelper();

    public StatusWindow() {
        logTextArea.setLineWrap(true);
        JScrollPane messagesScroll = new JScrollPane(logTextArea, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED) {
            @Override
            public Dimension getPreferredSize() {
                return new Dimension(400, 400);
            }
        };
        content.add(messagesScroll, BorderLayout.CENTER);
        content.add(bottomStatusLabel, BorderLayout.SOUTH);
    }

    @NotNull
    public FrameHelper getFrameHelper() {
        return frameHelper;
    }

    public JPanel getContent() {
        return content;
    }

    public void setErrorState(boolean isErrorState) {
        if (isErrorState) {
            logTextArea.setBackground(LIGHT_RED);
        } else {
            logTextArea.setBackground(LIGHT_GREEN);
        }
    }

    public JFrame getFrame() {
        return frameHelper.getFrame();
    }

    public void showFrame(String title) {
        frameHelper.getFrame().setTitle(title);
        frameHelper.showFrame(content, false);
        UiUtils.centerWindow(frameHelper.getFrame());
        logTextArea.setText(""); // let's remove stuff from previous invocation
    }

    @Override
    public void append(final String string) {
        SwingUtilities.invokeLater(() -> {
            String s = string.replaceAll(Character.toString((char) 219), "");
            log.info(s);
            logTextArea.append(s + "\r\n");
            UiUtils.trueLayout(logTextArea);
        });
    }

    public void setStatus(String status) {
        bottomStatusLabel.setText(status);
    }
}
