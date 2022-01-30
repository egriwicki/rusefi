package com.rusefi.livedata;

import com.rusefi.CodeWalkthrough;
import com.rusefi.config.Field;
import com.rusefi.enums.live_data_e;
import com.rusefi.ldmp.StateDictionary;
import com.rusefi.ui.livedata.SourceCodePainter;
import com.rusefi.ui.livedata.VariableValueSource;
import org.antlr.v4.runtime.tree.ParseTree;
import org.junit.Ignore;
import org.junit.Test;

import java.io.IOException;
import java.net.URISyntaxException;

import static com.rusefi.livedata.LiveDataParserPanel.getParseTree;
import static com.rusefi.ui.LiveDataPane.CPP_SUFFIX;
import static org.junit.Assert.assertTrue;

public class LiveDataConventionTest {
    @Test
    @Ignore
    public void assertLiveDataConvention() throws IOException, URISyntaxException {
        for (live_data_e view : live_data_e.values()) {
            String fileName = StateDictionary.INSTANCE.getFileName(view) + CPP_SUFFIX;
            Field[] values = StateDictionary.INSTANCE.getFields(view);
            assertFile(fileName, values);
        }
    }

    private void assertFile(String fileName, Field[] values) throws IOException, URISyntaxException {
        VariableValueSource valueSource = name -> {
            Field f = Field.findFieldOrNull(values, "", name);
            if (f == null) {
                return null;
            }
            System.out.println("getValue");
            return new VariableValueSource.VariableState(null, 0);
        };


        String sourceCode = LiveDataParserPanel.getContent(LiveDataParserPanel.class, fileName);
        assertTrue("No content " + fileName + " size=" + sourceCode.length(), sourceCode.length() > 100);

        ParseTree tree = getParseTree(sourceCode);
        ParseResult parseResult = CodeWalkthrough.applyVariables(valueSource, sourceCode, SourceCodePainter.VOID, tree);

        assertTrue("Broken live data constraint in " + fileName + ": " + parseResult.geBrokenConditions(), parseResult.geBrokenConditions().isEmpty());

    }
}
