package com.rusefi.maintenance;

import com.rusefi.FileLog;
import com.rusefi.Launcher;
import com.rusefi.Timeouts;
import com.rusefi.autodetect.PortDetector;
import com.rusefi.binaryprotocol.BinaryProtocol;
import com.rusefi.config.generated.Fields;
import com.rusefi.io.IoStream;
import com.rusefi.io.serial.PortHolder;
import com.rusefi.io.serial.SerialIoStreamJSerialComm;
import com.rusefi.ui.StatusWindow;
import com.rusefi.ui.util.URLLabel;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;

/**
 * @see FirmwareFlasher
 */
public class DfuFlasher {
    public static final String DFU_BINARY = Launcher.TOOLS_PATH + File.separator + "DfuSe/DfuSeCommand.exe";
    // TODO: integration with DFU command line tool
    static final String DFU_COMMAND = DFU_BINARY + " -c -d --v --fn " + Launcher.INPUT_FILES_PATH + File.separator +
            "rusefi.dfu";
    private static final String DFU_SETUP_EXE = "https://github.com/rusefi/rusefi_external_utils/raw/master/DFU_mode/DfuSe_Demo_V3.0.6_Setup.exe";

    private final JButton button = new JButton("Auto Program via DFU");
    private final JButton manualButton = new JButton("Manual Program via DFU");

    public DfuFlasher(JComboBox<String> comboPorts) {
        button.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent event) {
                doAutoDfu(comboPorts);
            }
        });

        manualButton.addActionListener(e -> runDfuProgramming());
    }

    public static void doAutoDfu(JComboBox<String> comboPorts) {
        String port = comboPorts.getSelectedItem().toString();
        port = PortDetector.autoDetectSerialIfNeeded(port);
        if (port == null) {
            JOptionPane.showMessageDialog(Launcher.getFrame(), "rusEfi serial port not detected");
            return;
        }
        IoStream stream = SerialIoStreamJSerialComm.open(port, PortHolder.BAUD_RATE, FileLog.LOGGER);
        byte[] command = BinaryProtocol.getTextCommandBytes(Fields.CMD_REBOOT_DFU);
        try {
            stream.sendPacket(command, FileLog.LOGGER);
            stream.close();
        } catch (IOException ignored) {
        }
        runDfuProgramming();
    }

    public static void runDfuProgramming() {
        StatusWindow wnd = new StatusWindow();
        wnd.showFrame("DFU status " + Launcher.CONSOLE_VERSION);
        ExecHelper.submitAction(() -> executeDFU(wnd), DfuFlasher.class + " thread");
    }

    private static void executeDFU(StatusWindow wnd) {
        wnd.appendMsg("Giving time for USB enumeration...");
        try {
            Thread.sleep(3 * Timeouts.SECOND);
        } catch (InterruptedException e) {
            throw new IllegalStateException(e);
        }
        StringBuffer stdout = new StringBuffer();
        String errorResponse = ExecHelper.executeCommand(FirmwareFlasher.BINARY_LOCATION,
                FirmwareFlasher.BINARY_LOCATION + File.separator + DFU_COMMAND,
                DFU_BINARY, wnd, stdout);
        if (stdout.toString().contains("Verify successful")) {
            wnd.appendMsg("SUCCESS!");
        } else {
            if (stdout.length() == 0 && errorResponse.length() == 0) {
                // looks like DFU util is not installed properly?
                // ugly temporary solution
                // see https://github.com/rusefi/rusefi/issues/1170
                // see https://github.com/rusefi/rusefi/issues/1182
                URLLabel.open(DFU_SETUP_EXE);
                wnd.appendMsg("Please install DfuSe_Demo_V3.0.6_Setup.exe, power cycle your device and try again.");
            } else {
                wnd.appendMsg(stdout.length() + " / " + errorResponse.length());
            }
            wnd.appendMsg("ERROR: does not look like DFU has worked!");
        }
        wnd.appendMsg("Please power cycle device to exit DFU mode");
    }

    /**
     * connect via serial + initiate software DFU jump + program
     */
    public Component getAutoButton() {
        return button;
    }

    // todo: maybe not couple these two different buttons in same class?
    public Component getManualButton() {
        return manualButton;
    }
}
