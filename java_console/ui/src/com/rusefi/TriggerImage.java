package com.rusefi;

import com.rusefi.ui.engine.UpDownImage;
import com.rusefi.ui.util.FrameHelper;
import com.rusefi.ui.util.UiUtils;
import com.rusefi.waves.EngineReport;
import org.jetbrains.annotations.NotNull;

import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;

/**
 * 06235/15
 * (c) Andrey Belomutskiy 2013-2015
 */
public class TriggerImage {

    public static final String TRIGGERTYPE = "TRIGGERTYPE";
    public static final String TRIGGERS = "triggers";
    public static final String TRIGGERS_TXT = "triggers.txt";
    private static int WIDTH = 320;
    /**
     * number of extra frames
     */
    private static int EXTRA_COUNT = 1;

    private static int WAVE_COUNT = 2;

    public static void main(String[] args) throws IOException, InvocationTargetException, InterruptedException {
        final String path;
        if (args.length != 1) {
            path = ".." + File.separator + "unit_tests";
        } else {
            path = args[0];
        }

        FrameHelper f = new FrameHelper();

        final TriggerPanel triggerPanel = new TriggerPanel(new GridLayout(WAVE_COUNT, 1)) {
            @Override
            public Dimension getPreferredSize() {
                return new Dimension((1 + EXTRA_COUNT) * WIDTH, 480);
            }
        };

        f.showFrame(triggerPanel);

        SwingUtilities.invokeAndWait(new Runnable() {
            @Override
            public void run() {
                try {
                    generateImages(path, triggerPanel);
                } catch (IOException e) {
                    throw new IllegalStateException(e);
                }
            }
        });
        System.exit(-1);
    }

    private static void generateImages(String path, TriggerPanel trigger) throws IOException {
        BufferedReader br = new BufferedReader(new FileReader(path + File.separator + TRIGGERS_TXT));

        String line;
        while ((line = br.readLine()) != null) {
            if (line.trim().startsWith("#")) {
                // skipping a comment
                continue;
            }

            if (line.startsWith(TRIGGERTYPE)) {
                readTrigger(br, line, trigger);
            }
        }
    }

    private static void readTrigger(BufferedReader reader, String line, TriggerPanel triggerPanel) throws IOException {
        String[] tokens = line.split(" ");
        String idStr = tokens[1];
        String eventCountStr = tokens[2];
        String triggerName = tokens[3];
        int eventCount = Integer.parseInt(eventCountStr);
        int id = Integer.parseInt(idStr);

//        if (id != 4)
//            return;

        System.out.println("id=" + id + ", count=" + eventCount + ", name=" + triggerName);

        List<WaveState> waves = readTrigger(reader, eventCount);

        EngineReport re0 = new EngineReport(waves.get(0).list, 720, 720 * (1 + EXTRA_COUNT));
        EngineReport re1 = new EngineReport(waves.get(1).list, 720, 720 * (1 + EXTRA_COUNT));

        triggerPanel.removeAll();
        UpDownImage upDownImage0 = new UpDownImage(re0, "trigger");
        upDownImage0.showText = false;
        triggerPanel.add(upDownImage0);

        UpDownImage upDownImage1 = new UpDownImage(re1, "trigger");
        upDownImage1.showText = false;
        if (!re1.getList().isEmpty())
            triggerPanel.add(upDownImage1);

        triggerPanel.name = triggerName;
        triggerPanel.id = id;

        UiUtils.trueLayout(triggerPanel);
        UiUtils.trueRepaint(triggerPanel);
        new File(TRIGGERS).mkdir();
        UiUtils.saveImage(TRIGGERS + File.separator + "trigger_" + id + ".png", triggerPanel);
    }

    @NotNull
    private static List<WaveState> readTrigger(BufferedReader reader, int count) throws IOException {
        String line;
        String[] tokens;
        List<Signal> signals = new ArrayList<>();

        for (int index = 0; index < count; index++) {
            line = reader.readLine();
            tokens = line.split(" ");
            String signalStr = tokens[2];
            int signal = Integer.parseInt(signalStr);
            String angleStr = tokens[3];
            double angle = Double.parseDouble(angleStr);

            signals.add(new Signal(signal, angle));
        }

        List<Signal> toShow = new ArrayList<>(signals);
        for (int i = 1; i <= 2 + EXTRA_COUNT; i++) {
            for (Signal s : signals)
                toShow.add(new Signal(s.signal, s.angle + i * 720));
        }

        List<WaveState> waves = new ArrayList<>();
        waves.add(new WaveState());
        waves.add(new WaveState());

        for (Signal s : toShow) {
            int waveIndex = s.signal / 1000;
            int signal = s.signal % 1000;

            if (waveIndex > 1) {
                // TT_HONDA_ACCORD_CD
                continue;
//                throw new IllegalStateException(s.signal + " in " + name);
            }

            WaveState waveState = waves.get(waveIndex);
            waveState.handle(signal, s.angle);
        }
        for (WaveState wave : waves)
            wave.wrap();

        return waves;
    }

    private static int angleToTime(double prevUp) {
        return (int) (prevUp);
    }

    private static class Signal {
        private final int signal;
        private final double angle;

        public Signal(int signal, double angle) {
            this.signal = signal;
            this.angle = angle;
        }

        @Override
        public String toString() {
            return "Signal{" +
                    "signal=" + signal +
                    ", angle=" + angle +
                    '}';
        }
    }

    private static class WaveState {
        double unusedDown = Double.NaN;
        double prevUp = Double.NaN;

        List<EngineReport.UpDown> list = new ArrayList<>();

        public void handle(int signal, double angle) {
            if (signal == 1) {
                // down signal
                if (Double.isNaN(prevUp)) {
                    // we have down before up, we would need to use it later
                    unusedDown = angle;
                } else {
                    EngineReport.UpDown ud = new EngineReport.UpDown(angleToTime(prevUp), 0, angleToTime(angle), 0);
                    list.add(ud);
                }
                prevUp = Double.NaN;
            } else {
                // up signal handling
                prevUp = angle;
            }
        }

        public void wrap() {
            if (!Double.isNaN(unusedDown)) {
                list.add(0, new EngineReport.UpDown(angleToTime(prevUp), 0, angleToTime(unusedDown + 720 * (3 + EXTRA_COUNT)), 0));
            }
        }

        public int getMaxTime() {
            if (list.isEmpty())
                return 0;
            return list.get(list.size() - 1).downTime;
        }
    }

    private static class TriggerPanel extends JPanel {
        public String name = "";
        public int id;

        public TriggerPanel(LayoutManager layout) {
            super(layout);
        }

        @Override
        public void paint(Graphics g) {
            super.paint(g);

            g.setColor(Color.black);
            Font f = g.getFont();
            g.setFont(new Font(f.getName(), Font.BOLD, f.getSize() * 3));

            int h = getHeight();

            g.drawString(name, 0, (int) (h * 0.75));
            g.drawString("#" + id, 0, (int) (h * 0.9));
        }
    }
}