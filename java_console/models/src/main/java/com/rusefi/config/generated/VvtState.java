package com.rusefi.config.generated;

// this file was generated automatically by rusEFI tool config_definition_base.jar based on (unknown script) controllers/actuators/vvt.txt Thu Sep 28 13:44:07 UTC 2023

// by class com.rusefi.output.FileJavaFieldsConsumer
import com.rusefi.config.*;

public class VvtState {
	public static final Field VVTTARGET = Field.create("VVTTARGET", 0, FieldType.INT16).setScale(0.1).setBaseOffset(1412);
	public static final Field VVTOUTPUT = Field.create("VVTOUTPUT", 2, FieldType.INT8).setScale(0.5).setBaseOffset(1412);
	public static final Field ALIGNMENTFILL_AT_3 = Field.create("ALIGNMENTFILL_AT_3", 3, FieldType.INT8).setScale(1.0).setBaseOffset(1412);
	public static final Field[] VALUES = {
	VVTTARGET,
	VVTOUTPUT,
	ALIGNMENTFILL_AT_3,
	};
}
