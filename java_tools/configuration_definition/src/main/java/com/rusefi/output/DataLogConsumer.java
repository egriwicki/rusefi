package com.rusefi.output;

import com.rusefi.ConfigField;
import com.rusefi.ReaderState;
import com.rusefi.TypesHelper;
import com.rusefi.VariableRegistry;
import org.jetbrains.annotations.NotNull;

import java.io.CharArrayWriter;
import java.io.FileWriter;
import java.io.IOException;

import static org.abego.treelayout.internal.util.java.lang.string.StringUtil.quote;

public class DataLogConsumer implements ConfigurationConsumer {
    private final String fileName;
    private final ReaderState state;
    private final CharArrayWriter tsWriter = new CharArrayWriter();

    public DataLogConsumer(String fileName, ReaderState state) {
        this.fileName = fileName;
        this.state = state;
    }

    @Override
    public void startFile() throws IOException {
        System.out.println("startFile");

    }

    @Override
    public void endFile() throws IOException {
        System.out.println("endFile");

    }

    @Override
    public void handleEndStruct(ConfigStructure structure) throws IOException {
        if (state.stack.isEmpty()) {
            FieldIterator iterator = new FieldIterator(structure.tsFields);
            String content = handleFields(structure, iterator, "");
            tsWriter.append(content);
        }

        if (fileName != null) {
            FileWriter fw = new FileWriter(fileName);
            fw.write(tsWriter.toCharArray());
            fw.close();
        }
    }

    private String handleFields(ConfigStructure structure, FieldIterator iterator, String prefix) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < structure.tsFields.size(); i++) {
            iterator.start(i);

            String content = handle(iterator.cf, prefix);
            sb.append(content);
            iterator.end();

        }
        return sb.toString();
    }

    private String handle(ConfigField configField, String prefix) {
        if (configField.getName().contains("unused"))
            return "";

        ConfigStructure cs = configField.getState().structures.get(configField.getType());
        if (cs != null) {
            String extraPrefix = cs.withPrefix ? configField.getName() + "_" : "";
            return handleFields(cs, new FieldIterator(cs.tsFields), extraPrefix);
        }


        if (configField.isArray()) {

            return "";
        }
//        entry = seconds,         @@GAUGE_NAME_UPTIME@@,
//        entry = RPMValue,        @@GAUGE_NAME_RPM@@,

        String typeString;
        String autoscaleSpec = configField.autoscaleSpec();
        if (TypesHelper.isFloat(configField.getType()) || (autoscaleSpec != null && !autoscaleSpec.equals("1, 1"))) {
            typeString = "float,  \"%.3f\"";
        } else {
            typeString = "int,    \"%d\"";
        }

        String comment = getComment(configField, state.variableRegistry);

        return "entry = " + prefix + configField.getName() + ", " + comment + ", " + typeString + "\n";
    }

    @NotNull
    public static String getComment(ConfigField configField, VariableRegistry variableRegistry) {
        String comment = variableRegistry.applyVariables(configField.getComment());
        String[] comments = comment == null ? new String[0] : comment.split("\\\\n");
        comment = (comments.length > 0) ? comments[0] : "";

        if (comment.isEmpty())
            comment = configField.getName();

        if (comment.charAt(0) != '"')
            comment = quote(comment);
        return comment;
    }

    public CharArrayWriter getTsWriter() {
        return tsWriter;
    }
}
