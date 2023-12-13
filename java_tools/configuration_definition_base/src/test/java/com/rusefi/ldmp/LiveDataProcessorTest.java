package com.rusefi.ldmp;

import com.rusefi.ReaderProvider;
import org.junit.Test;

import java.io.*;
import java.util.Map;

import static org.junit.Assert.assertEquals;

public class LiveDataProcessorTest {
    @Test
    public void testTwoSections() throws IOException {
        String testYaml = "Usages:\n" +
                "  - name: wbo_channels\n" +
                "    java: TsOutputs.java\n" +
                "    folder: console/binary\n" +
                "    cppFileName: status_loop\n" +
                "    output_name: [ \"wb1\", \"wb2\" ]\n" +
                "    constexpr: \"engine->outputChannels\"\n" +
        "#  output_channels always goes first at least because it has protocol version at well-known offset\n" +
                "  - name: output_channels\n" +
                "    java: TsOutputs.java\n" +
                "    folder: console/binary\n" +
                "    cppFileName: status_loop\n" +
                "    constexpr: \"engine->outputChannels\"\n"
                ;


        Map<String, Object> data = LiveDataProcessor.getStringObjectMap(new StringReader(testYaml));

        TestFileCaptor captor = new TestFileCaptor();
        LiveDataProcessor liveDataProcessor = new LiveDataProcessor("test", new ReaderProvider() {
            @Override
            public Reader read(String fileName) {
                System.out.println("read " + fileName);
                if (fileName.contains("output_channels")) {
                    return new StringReader("struct_no_prefix output_state_s\n" +
                            "\tuint16_t oootempC;Temperature;\"C\", 1, 0, 500, 1000, 0\n" +
                            "\tuint16_t oooesr;ESR;\"ohm\", 1, 0, 0, 10000, 0\n" +
                            "end_struct");
                } else {
                    return new StringReader("struct_no_prefix wideband_state_s\n" +
                            "\tuint16_t tempC;WBO: Temperature;\"C\", 1, 0, 500, 1000, 0, \"cate\"\n" +
                            "bit bitName\n" +
                            "\tuint16_t esr;WBO: ESR;\"ohm\", 1, 0, 0, 10000, 0\n" +
                            "end_struct");

                }
            }
        }, captor);
        liveDataProcessor.handleYaml(data);
        assertEquals(14, captor.fileCapture.size());

        captor.assertOutput("tempC0 = scalar, U16, 0, \"C\", 1, 0\n" +
                "bitName0 = bits, U32, 4, [0:0]\n" +
                "esr0 = scalar, U16, 8, \"ohm\", 1, 0\n" +
                "; total TS size = 12\n" +
                "oootempC = scalar, U16, 12, \"C\", 1, 0\n" +
                "oooesr = scalar, U16, 14, \"ohm\", 1, 0\n" +
                "; total TS size = 16\n", LiveDataProcessor.OUTPUTS_SECTION_FILE_NAME);

        captor.assertOutput("entry = tempC0, \"WBO: Temperature0\", int,    \"%d\"\n" +
                "entry = bitName0, \"bitName0\", int,    \"%d\"\n" +
                "entry = esr0, \"WBO: ESR0\", int,    \"%d\"\n" +
                ";entry = tempC1, \"WBO: Temperature1\", int,    \"%d\"\n" +
                ";entry = bitName1, \"bitName1\", int,    \"%d\"\n" +
                ";entry = esr1, \"WBO: ESR1\", int,    \"%d\"\n" +
                "entry = oootempC, \"Temperature\", int,    \"%d\"\n" +
                "entry = oooesr, \"ESR\", int,    \"%d\"\n", LiveDataProcessor.DATA_LOG_FILE_NAME);


        captor.assertOutput("// generated by gen_live_documentation.sh / LiveDataProcessor.java\n" +
                "decl_frag<wbo_channels_s, 0>{},\t// wb1\n" +
                "// decl_frag<wbo_channels_s, 1>{},\t// wb2\n" +
                "decl_frag<output_channels_s>{},\n", LiveDataProcessor.DATA_FRAGMENTS_H);

        captor.assertOutput("indicatorPanel = wbo_channels0IndicatorPanel, 2\n" +
                "\tindicator = {bitName0}, \"bitName No\", \"bitName Yes\"\n" +
                "\n" +
                "dialog = wbo_channels0Dialog, \"wbo_channels0\"\n" +
                "\tpanel = wbo_channels0IndicatorPanel\n" +
                "\tliveGraph = wbo_channels0_1_Graph, \"Graph\", South\n" +
                "\t\tgraphLine = tempC0\n" +
                "\t\tgraphLine = esr0\n" +
                "\n" +
                "\n" +
                "dialog = output_channelsDialog, \"output_channels\"\n" +
                "\tliveGraph = output_channels_1_Graph, \"Graph\", South\n" +
                "\t\tgraphLine = oootempC\n" +
                "\t\tgraphLine = oooesr\n" +
                "\n", LiveDataProcessor.FANCY_CONTENT_INI);

        captor.assertOutput("\t\t\tsubMenu = wbo_channels0Dialog, \"wbo_channels0\"\n" +
                "\t\t\tsubMenu = output_channelsDialog, \"output_channels\"\n", LiveDataProcessor.FANCY_MENU_INI);

        captor.assertOutput("\tgaugeCategory = \"cate\"\n" +
                "tempC0Gauge = tempC0,\"WBO: Temperature0\", \"C\", 500.0,1000.0, 500.0,1000.0, 500.0,1000.0, 0,0\n", LiveDataProcessor.GAUGES);
    }
}
