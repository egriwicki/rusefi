package com.rusefi.config.generated;

// this file was generated automatically by rusEFI tool ConfigDefinition.jar based on (unknown script) controllers/trigger\trigger_state.txt Thu Apr 14 20:48:22 EDT 2022

// by class com.rusefi.output.FileJavaFieldsConsumer
import com.rusefi.config.*;

public class TriggerState {
	public static final Field CURRENTGAP = Field.create("CURRENTGAP", 0, FieldType.FLOAT);
	public static final Field TOTALREVOLUTIONCOUNTER = Field.create("TOTALREVOLUTIONCOUNTER", 4, FieldType.INT).setScale(1.0);
	public static final Field[] VALUES = {
	CURRENTGAP,
	TOTALREVOLUTIONCOUNTER,
	};
}
