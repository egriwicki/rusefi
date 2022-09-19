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

		fprintf(fp, "# Generated by rusEFI unit test suite\n");
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
	memset(&pc, 0, sizeof(pc));
	Engine e;
	Engine* engine = &e;
	EngineTestHelperBase base(engine, &pc.engineConfiguration, &pc);

	engineConfiguration->trigger.type = tt;
    setCamOperationMode();

	TriggerWaveform *shape = &engine->triggerCentral.triggerShape;
	TriggerFormDetails *triggerFormDetails = &engine->triggerCentral.triggerFormDetails;
	engine->updateTriggerWaveform();

	ASSERT_FALSE(shape->shapeDefinitionError) << "Trigger shapeDefinitionError";

	fprintf(fp, "TRIGGERTYPE %d %d %s %.2f\n", tt, shape->getLength(), getTrigger_type_e(tt), shape->tdcPosition);

	fprintf(fp, "%s=%s\n", TRIGGER_HARDCODED_OPERATION_MODE, shape->knownOperationMode ? "true" : "false");
	fprintf(fp, "%s=%s\n", TRIGGER_IS_CRANK_KEY, shape->knownOperationMode && (shape->getWheelOperationMode() == FOUR_STROKE_CRANK_SENSOR) ? "true" : "false");

	fprintf(fp, "%s=%s\n", TRIGGER_HAS_SECOND_CHANNEL, shape->needSecondTriggerInput ? "true" : "false");
	fprintf(fp, "%s=%s\n", TRIGGER_IS_SECOND_WHEEL_CAM, shape->isSecondWheelCam ? "true" : "false");
	fprintf(fp, "%s=%d\n", TRIGGER_GAPS_COUNT, shape->gapTrackingLength);
	for (int i = 0; i < shape->gapTrackingLength; i++) {
		fprintf(fp, "%s.%d=%f\n", TRIGGER_GAP_FROM, i, shape->syncronizationRatioFrom[i]);
		fprintf(fp, "%s.%d=%f\n", TRIGGER_GAP_TO, i, shape->syncronizationRatioTo[i]);
	}
	fprintf(fp, "# end of meta section\n");

	for (size_t i = 0; i < shape->getLength(); i++) {
		int triggerDefinitionCoordinate = (shape->getTriggerWaveformSynchPointIndex() + i) % shape->getSize();

		fprintf(fp, "event %d %d %d %.2f\n",
				i,
				shape->triggerSignalIndeces[triggerDefinitionCoordinate],
				shape->triggerSignalStates[triggerDefinitionCoordinate],
				triggerFormDetails->eventAngles[i]);
	}
}
