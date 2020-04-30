package com.rusefi.output;

import com.rusefi.*;
import com.rusefi.util.LazyFile;
import com.rusefi.util.Output;
import com.rusefi.util.SystemOut;

import java.io.*;

import static com.rusefi.util.IoUtils.CHARSET;
import static com.rusefi.ConfigDefinition.EOL;

public class TSProjectConsumer implements ConfigurationConsumer {
    private static final String TS_FILE_INPUT_NAME = "rusefi.input";
    private static final int LENGTH = 24;
    private static final String CONFIG_DEFINITION_START = "CONFIG_DEFINITION_START";
    private static final String CONFIG_DEFINITION_END = "CONFIG_DEFINITION_END";
    private static final String TS_CONDITION = "@@if_";
    public static final String SETTING_CONTEXT_HELP_END = "SettingContextHelpEnd";
    public static final String SETTING_CONTEXT_HELP = "SettingContextHelp";
    public static String TS_FILE_OUTPUT_NAME = "rusefi.ini";
    private StringBuilder settingContextHelp = new StringBuilder();

    private final CharArrayWriter tsWriter;
    private final String tsPath;
    private final ReaderState state;
    private int totalTsSize;

    public TSProjectConsumer(CharArrayWriter tsWriter, String tsPath, ReaderState state) {
        this.tsWriter = tsWriter;
        this.tsPath = tsPath;
        this.state = state;
    }

    private int writeTunerStudio(ConfigField configField, String prefix, Writer tsHeader, int tsPosition, ConfigField next, int bitIndex) throws IOException {
        String nameWithPrefix = prefix + configField.getName();

        if (configField.getComment() != null && configField.getComment().startsWith(ConfigField.TS_COMMENT_TAG + "")) {
            settingContextHelp.append("\t" + nameWithPrefix + " = \"" + configField.getCommentContent() + "\"" + EOL);
        }
        VariableRegistry.INSTANCE.register(nameWithPrefix + "_offset", tsPosition);

        ConfigStructure cs = configField.getState().structures.get(configField.getType());
        if (cs != null) {
            String extraPrefix = cs.withPrefix ? configField.getName() + "_" : "";
            return writeTunerStudio(cs, prefix + extraPrefix, tsHeader, tsPosition);
        }

        if (configField.isBit()) {
            tsHeader.write("\t" + addTabsUpTo(nameWithPrefix, LENGTH));
            tsHeader.write("= bits,    U32,   ");
            tsHeader.write("\t" + tsPosition + ", [");
            tsHeader.write(bitIndex + ":" + bitIndex);
            tsHeader.write("], \"" + configField.getFalseName() + "\", \"" + configField.getTrueName() + "\"");
            tsHeader.write(EOL);

            tsPosition += configField.getSize(next);
            return tsPosition;
        }

        if (configField.getState().tsCustomLine.containsKey(configField.getType())) {
            String bits = configField.getState().tsCustomLine.get(configField.getType());
            tsHeader.write("\t" + addTabsUpTo(nameWithPrefix, LENGTH));
            int size = configField.getState().tsCustomSize.get(configField.getType());
//            tsHeader.headerWrite("\t" + size + ",");
            //          tsHeader.headerWrite("\t" + tsPosition + ",");
            bits = bits.replaceAll("@OFFSET@", "" + tsPosition);
            tsHeader.write("\t = " + bits);

            tsPosition += size;
        } else if (configField.getTsInfo() == null) {
            tsHeader.write(";no TS info - skipping " + prefix + configField.getName() + " offset " + tsPosition);
            tsPosition += configField.getArraySize() * configField.getElementSize();
        } else if (configField.getArraySize() != 1) {
            tsHeader.write("\t" + addTabsUpTo(nameWithPrefix, LENGTH) + "\t\t= array, ");
            tsHeader.write(TypesHelper.convertToTs(configField.getType()) + ",");
            tsHeader.write("\t" + tsPosition + ",");
            tsHeader.write("\t[" + configField.getArraySize() + "],");
            tsHeader.write("\t" + configField.getTsInfo());

            tsPosition += configField.getArraySize() * configField.getElementSize();
        } else {
            tsHeader.write("\t" + addTabsUpTo(nameWithPrefix, LENGTH) + "\t\t= scalar, ");
            tsHeader.write(TypesHelper.convertToTs(configField.getType()) + ",");
            tsHeader.write("\t" + tsPosition + ",");
            tsHeader.write("\t" + configField.getTsInfo());
            tsPosition += configField.getArraySize() * configField.getElementSize();
        }
        tsHeader.write(EOL);
        return tsPosition;
    }

    private int writeTunerStudio(ConfigStructure configStructure, String prefix, Writer tsHeader, int tsPosition) throws IOException {
        BitState bitState = new BitState();
        for (int i = 0; i < configStructure.tsFields.size(); i++) {
            ConfigField next = i == configStructure.tsFields.size() - 1 ? ConfigField.VOID : configStructure.tsFields.get(i + 1);
            ConfigField cf = configStructure.tsFields.get(i);
            tsPosition = writeTunerStudio(cf, prefix, tsHeader, tsPosition, next, bitState.get());

            bitState.incrementBitIndex(cf, next);
        }
        return tsPosition;
    }

    protected void writeTunerStudioFile(String tsPath, String fieldsSection) throws IOException {
        TsFileContent tsContent = readTsFile(tsPath);
        SystemOut.println("Got " + tsContent.getPrefix().length() + "/" + tsContent.getPostfix().length() + " of " + TS_FILE_INPUT_NAME);

        // File.getPath() would eliminate potential separator at the end of the path
        String fileName = new File(tsPath).getPath() + File.separator + TS_FILE_OUTPUT_NAME;
        Output tsHeader = new LazyFile(fileName);
        writeContent(fieldsSection, tsContent, tsHeader);
    }

    protected void writeContent(String fieldsSection, TsFileContent tsContent, Output tsHeader) throws IOException {
        tsHeader.write(tsContent.getPrefix());

        tsHeader.write("; " + CONFIG_DEFINITION_START + ConfigDefinition.EOL);
        tsHeader.write("; this section " + ConfigDefinition.MESSAGE + ConfigDefinition.EOL + ConfigDefinition.EOL);
        tsHeader.write("pageSize            = " + totalTsSize + ConfigDefinition.EOL);
        tsHeader.write("page = 1" + ConfigDefinition.EOL);
        tsHeader.write(fieldsSection);
        if (settingContextHelp.length() > 0) {
            tsHeader.write("[" + SETTING_CONTEXT_HELP + "]" + ConfigDefinition.EOL);
            tsHeader.write(settingContextHelp.toString() + ConfigDefinition.EOL + ConfigDefinition.EOL);
            tsHeader.write("; " + SETTING_CONTEXT_HELP_END + ConfigDefinition.EOL);
        }
        tsHeader.write("; " + CONFIG_DEFINITION_END + ConfigDefinition.EOL);
        tsHeader.write(tsContent.getPostfix());
        tsHeader.close();
    }

    private static TsFileContent readTsFile(String tsPath) throws IOException {
        String fileName = tsPath + File.separator + TS_FILE_INPUT_NAME;
        BufferedReader r = new BufferedReader(new InputStreamReader(new FileInputStream(fileName), CHARSET.name()));

        StringBuilder prefix = new StringBuilder();
        StringBuilder postfix = new StringBuilder();

        boolean isBeforeStartTag = true;
        boolean isAfterEndTag = false;
        String line;
        while ((line = r.readLine()) != null) {
            if (line.contains(CONFIG_DEFINITION_START)) {
                isBeforeStartTag = false;
                continue;
            }
            if (line.contains(CONFIG_DEFINITION_END)) {
                isAfterEndTag = true;
                continue;
            }

            if (line.contains(TS_CONDITION)) {
                String token = getToken(line);
                String strValue = VariableRegistry.INSTANCE.get(token);
                boolean value = Boolean.valueOf(strValue);
                if (!value)
                    continue; // skipping this line
                line = removeToken(line);
            }

            line = VariableRegistry.INSTANCE.applyVariables(line);

            if (isBeforeStartTag)
                prefix.append(line + ConfigDefinition.EOL);

            if (isAfterEndTag)
                postfix.append(VariableRegistry.INSTANCE.applyVariables(line) + ConfigDefinition.EOL);
        }
        r.close();
        return new TsFileContent(prefix.toString(), postfix.toString());
    }

    public static String removeToken(String line) {
        int index = line.indexOf(TS_CONDITION);
        String token = getToken(line);
        int afterTokenIndex = index + TS_CONDITION.length() + token.length();
        if (afterTokenIndex < line.length())
            afterTokenIndex++; // skipping one whitestace after token
        line = line.substring(0, index) + line.substring(afterTokenIndex);
        return line;
    }

    public static String getToken(String line) {
        int index = line.indexOf(TS_CONDITION) + TS_CONDITION.length();
        String token = "";
        while (index < line.length() && !Character.isWhitespace(line.charAt(index))) {
            token += line.charAt(index);
            index++;
        }
        return token;
    }

    @Override
    public void startFile() {
    }

    @Override
    public void endFile() throws IOException {
        writeTunerStudioFile(tsPath, tsWriter.toString());
    }

    @Override
    public void handleEndStruct(ConfigStructure structure) throws IOException {
        if (state.stack.isEmpty()) {
            totalTsSize = writeTunerStudio(structure, "", tsWriter, 0);
            tsWriter.write("; total TS size = " + totalTsSize + EOL);
            VariableRegistry.INSTANCE.register("TOTAL_CONFIG_SIZE", totalTsSize);
        }
    }


    private static String addTabsUpTo(String name, int length) {
        StringBuilder result = new StringBuilder(name);
        int currentLength = name.length();
        while (currentLength < length) {
            result.append("\t");
            currentLength += 4;
        }
        return result.toString();
    }
}
