package com.rusefi.ui.livedocs;

import com.opensr5.ConfigurationImage;
import com.rusefi.binaryprotocol.BinaryProtocol;
import com.rusefi.config.Field;
import com.rusefi.config.generated.EngineState;
import com.rusefi.config.generated.Fields;
import com.rusefi.config.generated.ThermistorState;
import com.rusefi.core.Sensor;
import com.rusefi.core.SensorCentral;
import com.rusefi.ldmp.*;
import com.rusefi.ldmp.generated.SpeedDensityMeta;
import com.rusefi.ldmp.generated.ThermistorsMeta;
import com.rusefi.ldmp.generated.TpsMeta;
import com.rusefi.ui.livedocs.controls.Toolbox;
import com.rusefi.ui.util.UiUtils;
import org.jetbrains.annotations.NotNull;
import org.putgemin.VerticalFlowLayout;

import javax.swing.*;
import java.awt.*;
import java.util.ArrayList;
import java.util.List;

import static com.rusefi.config.Field.niceToString;

public class LiveDocPanel {
    @NotNull
    public static JPanel getPanel(String title, String instancePrefix, final int id, Field[] values, Request[] content) {

        ActionPanel ap = createComponents(title, content, values, instancePrefix);
        JPanel panel = ap.getPanel();

        LiveDocHolder holder = new LiveDocHolder(id, ap.actionsList, values) {
            @Override
            public boolean isVisible() {
                boolean isVisible = !panel.getVisibleRect().isEmpty();
                return isVisible && isRecursivelyVisible(panel);
            }
        };

        LiveDocsRegistry.INSTANCE.register(holder);
        return panel;
    }

    static boolean isRecursivelyVisible(Container c) {
        Container parent = c.getParent();
        return c.isVisible() && (parent == null || isRecursivelyVisible(parent));
    }

    private static ActionPanel createComponents(String title, Request[] content, Field[] values, String instancePrefix) {
        ActionPanel result = new ActionPanel(title);

        for (Request r : content) {
            if (r instanceof TextRequest) {
                TextRequest request = (TextRequest) r;
                if (request.isEol()) {
                    result.newLine();
                } else {
                    result.addControl(new JLabel(request.getValue().replaceAll("_", " ")));
                }
            } else if (r instanceof FieldRequest) {
                FieldRequest request = (FieldRequest) r;
                Field field = Field.findField(values, "", request.getField());
                JLabel label = new JLabel("*");
                label.setIcon(UiUtils.loadIcon("livedocs/variable.png"));
                label.setToolTipText("Variable " + field.getName());
                result.addControl(label);
                result.actionsList.add(new RefreshActions() {
                    @Override
                    public void refresh(BinaryProtocol bp, byte[] response) {
                        Number fieldValue = field.getValue(new ConfigurationImage(response));
                        String value = niceToString(fieldValue);
                        label.setText(value);
                    }
                });
            } else if (r instanceof ConfigRequest) {
                ConfigRequest request = (ConfigRequest) r;
                Field field = Field.findField(Fields.VALUES, instancePrefix, request.getField());

                JLabel label = new JLabel("*");
                label.setIcon(UiUtils.loadIcon("livedocs/setting.png"));
                label.setToolTipText("Configuration " + field.getName());
                result.actionsList.add(new RefreshActions() {
                    @Override
                    public void refresh(BinaryProtocol bp, byte[] response) {
                        String value = field.getAnyValue(bp.getController()).toString();
                        label.setText(value);
                    }
                });
                result.addControl(label);
            } else if (r instanceof SensorRequest) {
                SensorRequest request = (SensorRequest) r;
                Sensor sensor = Sensor.find(request.getValue());
                JLabel label = new JLabel("*");
                result.addControl(label);
                label.setIcon(UiUtils.loadIcon("livedocs/gauge.png"));
                label.setToolTipText("Sensor " + request.getValue());
                result.actionsList.add(new RefreshActions() {
                    @Override
                    public void refresh(BinaryProtocol bp, byte[] response) {
                        double value = SensorCentral.getInstance().getValue(sensor);
                        label.setText(niceToString(value));
                    }
                });
            } else if (r instanceof IfRequest) {
                IfRequest request = (IfRequest) r;

                ActionPanel panel = createIfRequestPanel(request, values);

                result.actionsList.addAll(panel.actionsList);

                result.addControl(panel.getPanel());
            } else {
                throw new UnsupportedOperationException(r.toString());
            }
        }
        return result;
    }

    private static ActionPanel createIfRequestPanel(IfRequest request, Field[] values) {

        Field conditionField = Field.findField(values, "", request.getVariable());

        JPanel result = new JPanel(new VerticalFlowLayout());

        JLabel conditionLabel = new JLabel(request.getVariable());
        result.add(conditionLabel);


        ActionPanel trueAP = createComponents("", request.trueBlock.toArray(new Request[0]), values, "");
        ActionPanel falseAP = createComponents("", request.falseBlock.toArray(new Request[0]), values, "");

        result.add(trueAP.getPanel());
        result.add(falseAP.getPanel());

        List<RefreshActions> combined = new ArrayList<>(trueAP.actionsList);
        combined.addAll(falseAP.actionsList);

        combined.add(new RefreshActions() {
            @Override
            public void refresh(BinaryProtocol bp, byte[] response) {
                int value = (int) conditionField.getValue(new ConfigurationImage(response));
                conditionLabel.setText(request.getVariable() + " is " + (value == 1 ? "TRUE" : "FALSE"));
                JPanel active;
                JPanel passive;
                if (value == 1) {
                    active = trueAP.getPanel();
                    passive = falseAP.getPanel();
                } else {
                    active = falseAP.getPanel();
                    passive = trueAP.getPanel();
                }
                active.setBorder(BorderFactory.createLineBorder(Color.green));
                passive.setBorder(BorderFactory.createLineBorder(Color.red));
                Toolbox.setEnabledRecursive(active, true);
                Toolbox.setEnabledRecursive(passive, false);
            }
        });

        return new ActionPanel(result, combined);
    }

    @NotNull
    public static JPanel createLiveDocumentationPanel() {
        JPanel liveDocs = new JPanel(new VerticalFlowLayout());

        liveDocs.add(getPanel("Coolant Sensor", "CLT", Fields.LDS_CLT_INDEX,
                ThermistorState.VALUES, ThermistorsMeta.CONTENT));

        liveDocs.add(getPanel("Intake Air Sensor", "IAT", Fields.LDS_IAT_INDEX,
                ThermistorState.VALUES, ThermistorsMeta.CONTENT));

        liveDocs.add(getPanel("Throttle Position Sensor", "", Fields.LDS_ENGINE_STATE_INDEX,
                EngineState.VALUES, TpsMeta.TPS_SECTION));

        liveDocs.add(getPanel("tCharge", "", Fields.LDS_ENGINE_STATE_INDEX,
                EngineState.VALUES, SpeedDensityMeta.CONTENT));

        return liveDocs;
    }
}
