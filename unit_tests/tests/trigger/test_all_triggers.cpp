/**
 * @file test_all_triggers.cpp
 */
#include "pch.h"

// uncomment to test only newest trigger
//#define TEST_FROM_TRIGGER_ID ((int)TT_UNUSED - 1)
#define TEST_FROM_TRIGGER_ID 1

struct TriggerExportHelper
{
	FILE* fp;

	TriggerExportHelper() {
		fp = fopen (TRIGGERS_FILE_NAME, "w+");

		fprintf(fp, "# Generated by rusEfi unit test suite\n");
		fprintf(fp, "# This file is used by TriggerImage tool\n");
		fprintf(fp, "# See 'gen_trigger_images.bat'\n");
	}

	~TriggerExportHelper() {
		fclose(fp);
		printf("All triggers exported to %s\n", TRIGGERS_FILE_NAME);
	}
};

static TriggerExportHelper exportHelper;

class AllTriggersFixture : public ::testing::TestWithParam<trigger_type_e> {
};

INSTANTIATE_TEST_SUITE_P(
	Triggers,
	AllTriggersFixture,
	// Test all triggers from the first valid trigger thru the last
	// (Skip index 0, that's custom toothed wheel which is covered by others)
	::testing::Range((trigger_type_e)TEST_FROM_TRIGGER_ID, TT_UNUSED)
);

extern bool printTriggerDebug;
extern bool printTriggerTrace;

TEST_P(AllTriggersFixture, TestTrigger) {
	// handy debugging options
	//printTriggerDebug = true;
	//printTriggerTrace = true;

	auto tt = GetParam();
	auto fp = exportHelper.fp;

	printf("Exporting %s\r\n", getTrigger_type_e(tt));

	persistent_config_s pc;
	Engine e;
	Engine* engine = &e;
	EngineTestHelperBase base(engine, &pc.engineConfiguration, &pc);

	engineConfiguration->trigger.type = tt;
	engineConfiguration->ambiguousOperationMode = FOUR_STROKE_CAM_SENSOR;

	TriggerWaveform *shape = &engine->triggerCentral.triggerShape;
	TriggerFormDetails *triggerFormDetails = &engine->triggerCentral.triggerFormDetails;
	engine->initializeTriggerWaveform(PASS_ENGINE_PARAMETER_SIGNATURE);

	ASSERT_FALSE(shape->shapeDefinitionError) << "Trigger shapeDefinitionError";

	fprintf(fp, "TRIGGERTYPE %d %d %s %.2f\n", tt, shape->getLength(), getTrigger_type_e(tt), shape->tdcPosition);

	fprintf(fp, "# duty %.2f %.2f\n", shape->expectedDutyCycle[0], shape->expectedDutyCycle[1]);

	for (size_t i = 0; i < shape->getLength(); i++) {
		int triggerDefinitionCoordinate = (shape->getTriggerWaveformSynchPointIndex() + i) % shape->getSize();

		fprintf(fp, "event %d %d %d %.2f\n",
				i,
				shape->triggerSignalIndeces[triggerDefinitionCoordinate],
				shape->triggerSignalStates[triggerDefinitionCoordinate],
				triggerFormDetails->eventAngles[i]);
	}
}
