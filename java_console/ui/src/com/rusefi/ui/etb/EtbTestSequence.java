package com.rusefi.ui.etb;

import com.rusefi.TimeBasedBuffer;
import com.rusefi.core.Sensor;
import com.rusefi.etb.EtbTarget;
import com.rusefi.etb.StandardTestSequence;
import com.rusefi.etb.TestSequenceStep;

import javax.swing.*;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

import static com.rusefi.Timeouts.SECOND;

/**
 * Here we initiate a sequence of commands which would change target electronic throttle position so that we can
 * see how far the result would be how soon.
 * <p>
 * Error standard deviation is posted to {@link Sensor#ETB_CONTROL_QUALITY}
 * <p>
 * https://github.com/rusefi/rusefi/issues/494
 * <p>
 * 11/16/2017
 * (c) Andrey Belomutskiy
 *
 * @see TimeBasedBuffer
 */
public class EtbTestSequence {
    protected static final ScheduledExecutorService executor = Executors.newSingleThreadScheduledExecutor();
    public static final int durationSeconds = 300;
    public static final int frequencyHz = 100;
    public static final int PAST_DEPTH = 200 / (1000 / frequencyHz);

    private static final String BUTTON_TEXT = "Measure Control Quality";

    private final JButton button = new JButton(BUTTON_TEXT);
    private final JLabel result = new JLabel("Result: N/A");

    public EtbTestSequence() {
        button.addActionListener(e -> {
            button.setEnabled(false);
            // 3000 data points at 10Hz should be 300 seconds worth of data
            StandardTestSequence.metric.start(/* buffer size: */durationSeconds * frequencyHz,
                    /*period, ms: */ 1000 / frequencyHz);

            AtomicInteger stepCounter = new AtomicInteger();
            AtomicInteger totalSteps = new AtomicInteger();

            TestSequenceStep lastStep = new TestSequenceStep(SECOND, EtbTarget.Condition.YES) {
                @Override
                protected void doJob() {
                    button.setEnabled(true);
                    button.setText(BUTTON_TEXT);
                    double finalValue = StandardTestSequence.metric.getStandardDeviation();
                    result.setText(String.format("Final Result: %.3f", finalValue));
                }
            };

            Runnable onEachStep = () -> SwingUtilities.invokeLater(() -> {
                String state = stepCounter.incrementAndGet() + "/" + totalSteps.get();
                button.setText("Running " + state);
                double value = StandardTestSequence.metric.getStandardDeviation();
                result.setText(String.format(state + " Result: %.3f", value));
            });

            TestSequenceStep firstStep = new EtbTarget(10 * SECOND, 4, /*position*/onEachStep, TestSequenceStep.Condition.YES);
            TestSequenceStep result = StandardTestSequence.addSequence(firstStep, onEachStep, TestSequenceStep.Condition.YES);
            result.addNext(lastStep);

            totalSteps.set(TestSequenceStep.count(firstStep));

            firstStep.execute(executor);
        });
    }

    public JButton getButton() {
        return button;
    }

    public JLabel getResult() {
        return result;
    }
}
