package com.rusefi.output;

import com.rusefi.*;
import com.rusefi.util.LazyFile;
import com.rusefi.util.SystemOut;

import java.io.IOException;

import static com.rusefi.ConfigDefinition.EOL;

/**
 * Configuration consumer which writes Signature header file
 */
public class SignatureConsumer implements ConfigurationConsumer {
    private final String destHeader;
    VariableRegistry registry;

    public SignatureConsumer(String destHeader, VariableRegistry vregistry) {
        SystemOut.println("Writing Signature header to " + destHeader);
        this.destHeader = destHeader;
        this.registry = vregistry;
    }

    @Override
    public void startFile() {
    }

    @Override
    public void handleEndStruct(ConfigStructure structure) throws IOException {
        registry.writeDefinesToFile(destHeader);
    }

    @Override
    public void endFile() throws IOException {
    }
}
