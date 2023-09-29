package com.rusefi.config.generated;

// this file was generated automatically by rusEFI tool config_definition_base.jar based on (unknown script) controllers/math/lambda_monitor.txt Thu Sep 28 13:44:07 UTC 2023

// by class com.rusefi.output.FileJavaFieldsConsumer
import com.rusefi.config.*;

public class LambdaMonitor {
	public static final Field LAMBDACURRENTLYGOOD = Field.create("LAMBDACURRENTLYGOOD", 0, FieldType.BIT, 0).setBaseOffset(1416);
	public static final Field LAMBDAMONITORCUT = Field.create("LAMBDAMONITORCUT", 0, FieldType.BIT, 1).setBaseOffset(1416);
	public static final Field LAMBDATIMESINCEGOOD = Field.create("LAMBDATIMESINCEGOOD", 4, FieldType.INT16).setScale(0.01).setBaseOffset(1416);
	public static final Field ALIGNMENTFILL_AT_6 = Field.create("ALIGNMENTFILL_AT_6", 6, FieldType.INT8).setScale(1.0).setBaseOffset(1416);
	public static final Field[] VALUES = {
	LAMBDACURRENTLYGOOD,
	LAMBDAMONITORCUT,
	LAMBDATIMESINCEGOOD,
	ALIGNMENTFILL_AT_6,
	};
}
