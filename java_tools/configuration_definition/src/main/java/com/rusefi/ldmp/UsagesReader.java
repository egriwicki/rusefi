package com.rusefi.ldmp;

import com.devexperts.logging.Logging;
import com.rusefi.ConfigDefinition;
import com.rusefi.ReaderState;
import com.rusefi.output.FragmentDialogConsumer;
import com.rusefi.output.JavaSensorsConsumer;
import com.rusefi.output.OutputsSectionConsumer;
import org.yaml.snakeyaml.Yaml;

import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

public class UsagesReader {
    private final static Logging log = Logging.getLogging(UsagesReader.class);

    private final static String header = "// generated by gen_live_documentation.sh / UsagesReader.java\n";

    private final StringBuilder enumContent = new StringBuilder(header +
            "#pragma once\n" +
            "\n" +
            "typedef enum {\n");

    private final StringBuilder totalSensors = new StringBuilder();

    private final StringBuilder fancyNewStuff = new StringBuilder();

    private final StringBuilder fancyNewMenu = new StringBuilder();

    private final StringBuilder fragmentsContent = new StringBuilder(header);

    public static void main(String[] args) throws IOException {
        if (args.length != 1) {
            System.err.println("One parameter expected: name of live data yaml input file");
            System.exit(-1);
        }
        String yamlFileName = args[0];
        Yaml yaml = new Yaml();
        Map<String, Object> data = yaml.load(new FileReader(yamlFileName));

        UsagesReader usagesReader = new UsagesReader();

        int sensorTsPosition = usagesReader.handleYaml(data, null);
        usagesReader.writeFiles();

        log.info("TS_TOTAL_OUTPUT_SIZE=" + sensorTsPosition);
        try (FileWriter fw = new FileWriter("console/binary/generated/total_live_data_generated.h")) {
            fw.write(header);
            fw.write("#define TS_TOTAL_OUTPUT_SIZE " + sensorTsPosition);
        }

        try (FileWriter fw = new FileWriter("console/binary/generated/sensors.java")) {
            fw.write(usagesReader.totalSensors.toString());
        }

        try (FileWriter fw = new FileWriter("console/binary/generated/fancy_content.ini")) {
            fw.write(usagesReader.fancyNewStuff.toString());
        }

        try (FileWriter fw = new FileWriter("console/binary/generated/fancy_menu.ini")) {
            fw.write(usagesReader.fancyNewMenu.toString());
        }
    }

    interface EntryHandler {
        void onEntry(String name, List elements) throws IOException;
    }

    private int handleYaml(Map<String, Object> data, EntryHandler _handler) throws IOException {
        JavaSensorsConsumer javaSensorsConsumer = new JavaSensorsConsumer();
        String tsOutputsDestination = "console/binary/";

        OutputsSectionConsumer outputsSections = new OutputsSectionConsumer(tsOutputsDestination + File.separator + "generated/output_channels.ini");

        EntryHandler handler = new EntryHandler() {

            @Override
            public void onEntry(String name, List elements) throws IOException {
                String javaName = (String) elements.get(0);
                String folder = (String) elements.get(1);

                int startingPosition = javaSensorsConsumer.sensorTsPosition;
                log.info("Starting " + name + " at " + startingPosition);

                boolean withCDefines = false;
                String prepend = "";
                for (int i = 2; i < elements.size(); i++) {
                    String keyValue = (String) elements.get(i);
                    String[] pair = keyValue.trim().split("=");
                    String key = pair[0];
                    String value = pair[1];
                    if (key.equals(ConfigDefinition.KEY_WITH_C_DEFINES)) {
                        withCDefines = Boolean.parseBoolean(value);
                    } else if (key.equals(ConfigDefinition.KEY_PREPEND)) {
                        prepend = value;
                    }
                }

//            String macroName = elements.size() > 2 ? ((String)elements.get(2)).trim() : "";


                ReaderState state = new ReaderState();
                state.setDefinitionInputFile(folder + File.separator + name + ".txt");
                state.withC_Defines = withCDefines;

                state.addDestination(javaSensorsConsumer, outputsSections);
                FragmentDialogConsumer fragmentDialogConsumer = new FragmentDialogConsumer(name);
                state.addDestination(fragmentDialogConsumer);

                state.addPrepend(prepend);
                state.addCHeaderDestination(folder + File.separator + name + "_generated.h");
                state.addJavaDestination("../java_console/models/src/main/java/com/rusefi/config/generated/" + javaName);
                state.doJob();

                fancyNewStuff.append(fragmentDialogConsumer.getContent());

                fancyNewMenu.append(fragmentDialogConsumer.menuLine());

                log.info("Done with " + name + " at " + javaSensorsConsumer.sensorTsPosition);
            }
        };

        LinkedHashMap<?, ?> liveDocs = (LinkedHashMap) data.get("Usages");

        for (Map.Entry entry : liveDocs.entrySet()) {
            String name = (String) entry.getKey();
            System.out.println(" " + name);
            System.out.println("  " + entry.getValue());
            List elements = (List) entry.getValue();

            handler.onEntry(name, elements);

            String enumName = "LDS_" + name;
            String type = name + "_s"; // convention
            enumContent.append(enumName + ",\n");

            fragmentsContent
                    .append("getLiveDataAddr<")
                    .append(type)
                    .append(">(),\n");
        }
        enumContent.append("} live_data_e;\n");

        totalSensors.append(javaSensorsConsumer.getContent());

        return javaSensorsConsumer.sensorTsPosition;
    }

    private void writeFiles() throws IOException {
        try (FileWriter fw = new FileWriter("console/binary/generated/live_data_ids.h")) {
            fw.write(enumContent.toString());
        }

        try (FileWriter fw = new FileWriter("console/binary/generated/live_data_fragments.h")) {
            fw.write(fragmentsContent.toString());
        }
    }
}
