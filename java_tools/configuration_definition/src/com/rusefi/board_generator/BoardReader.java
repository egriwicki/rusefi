package com.rusefi.board_generator;

import com.rusefi.EnumsReader;
import com.rusefi.enum_reader.Value;
import com.rusefi.util.LazyFile;
import com.rusefi.util.Output;
import com.rusefi.util.SystemOut;
import org.yaml.snakeyaml.Yaml;

import java.io.*;
import java.util.Collection;
import java.util.Map;
import java.util.Objects;

/**
 * This tool read mapping yaml file and produces a .txt file with defines in the rusefi.txt format
 *
 * This is a lazy to implement mapping as a separate phase of code generation. Technically this could be merged into
 * the primary generation to avoid the intermediate file.
 */
public class BoardReader {
    private static final String INVALID = "INVALID";

    private static final String KEY_BOARD_NAME = "-board";
    private static final String KEY_OUTFOLDER = "-out";
    private static final String KEY_FIRMWARE_PATH = "-firmware_path";
    private final static String KEY_ENUM_INPUT_FILE = "-enumInputFile";

    private static final String OUTPUT_FILE_PREFIX = "_prefix.txt";
    private static final String MAPPING_YAML = "mapping.yaml";

    public static void main(String[] args) throws IOException {
        if (args.length < 2) {
            SystemOut.println("Please specify\r\n"
                    + KEY_BOARD_NAME + " x\r\n"
                    + KEY_FIRMWARE_PATH + " x\r\n"
            );
            return;
        }
        String boardName = null;
        String firmwarePath = "firmware";
        String outputPath = ".";
        for (int i = 0; i < args.length - 1; i += 2) {
            String key = args[i];
            if (key.equals(KEY_BOARD_NAME)) {
                boardName = args[i + 1];
            } else if (key.equals(KEY_FIRMWARE_PATH)) {
                firmwarePath = args[i + 1];
            } else if (key.equals(KEY_ENUM_INPUT_FILE)) {
                String inputFile = args[i + 1];
                EnumsReader.process(new FileReader(firmwarePath + File.separator + inputFile));
            } else if (key.equals(KEY_OUTFOLDER)) {
                outputPath = args[i + 1];
            }
        }

        Yaml yaml = new Yaml();
        String fileName = firmwarePath + "/config/boards/" + boardName + "/" + MAPPING_YAML;
        Map<String, Object> data = yaml.load(new FileReader(fileName));
        if (data == null) {
            SystemOut.println("Null yaml for " + fileName);
        } else {
            SystemOut.println(data);

            Output bw = new LazyFile(outputPath + File.separator + boardName + OUTPUT_FILE_PREFIX);

            bw.write(processSection(data, "brain_pin_e", "output_pin_e", "outputs", "GPIO_UNASSIGNED"));
            bw.write(processSection(data, "adc_channel_e", "adc_channel_e", "analog_inputs", "EFI_ADC_NONE"));

            bw.write(processSection(data, "brain_pin_e", "brain_input_pin_e", "event_inputs", "GPIO_UNASSIGNED"));
            bw.write(processSection(data, "brain_pin_e", "switch_input_pin_e", "switch_inputs", "GPIO_UNASSIGNED"));

            bw.close();
        }
        SystemOut.close();
    }

    private static String processSection(Map<String, Object> data, String headerEnumName, String outputEnumName, String sectionName, String NOTHING_NAME) {
        Objects.requireNonNull(data, "data");
        Map<String, Object> outputs = (Map<String, Object>) data.get(sectionName);
        if (outputs == null)
            return "";

        Objects.requireNonNull(data, "enums");
        Map<String, Value> enumMap = EnumsReader.enums.get(headerEnumName);
        Objects.requireNonNull(enumMap, "enum for " + headerEnumName);
        SystemOut.println(enumMap.size());

        StringBuffer sb = new StringBuffer();

        int maxValue = getMaxValue(enumMap.values());

        for (int i = 0; i < maxValue; i++) {
            if (sb.length() > 0)
                sb.append(",");

            String code;

            Value v = findByOrdinal(i, enumMap.values());

            if (v == null) {
                code = INVALID;
            } else if (v.getName().equals(NOTHING_NAME)) {
                code = "NONE";
            } else if (outputs.containsKey(v.getName())) {
                code = (String) outputs.get(v.getName());
            } else {
                code = INVALID;
            }
            sb.append("\"" + code + "\"");
        }

        return " #define " + outputEnumName + "_enum " + sb + "\r\n";
    }

    private static int getMaxValue(Collection<Value> values) {
        int result = -1;
        for (Value v : values)
            result = Math.max(result, v.getIntValue());
        return result;
    }

    private static Value findByOrdinal(int ordinal, Collection<Value> values) {
        for (Value v : values)
            if (v.getValue().equals(String.valueOf(ordinal)))
                return v;
        return null;
    }
}
