/**
 * @file	wave_analyzer.cpp
 * @brief	Initialization of Input Capture pins used for rusEfi console sniffer
 *
 * This file is responsible for sniffing of external digital signals and registering
 * these digital events in WaveChart used by the Engine Sniffer tab of rusEfi Console.
 *
 * this is rusEfi build-in logic analyzer
 *
 * @date Jan 7, 2013
 * @author Andrey Belomutskiy, (c) 2012-2018
 */

#include "global.h"
#include "os_access.h"
#include "wave_analyzer.h"
#include "eficonsole.h"
#include "data_buffer.h"
#include "pin_repository.h"
#include "allsensors.h"
#include "engine_configuration.h"
#include "trigger_central.h"
#include "os_util.h"
#include "engine_math.h"
#include "engine.h"
#include "rpm_calculator.h"
#include "engine_sniffer.h"
#include "digital_input_exti.h"

#if EFI_WAVE_ANALYZER

EXTERN_ENGINE;

#define CHART_RESET_DELAY 1
#define MAX_ICU_COUNT 5

#if EFI_ENGINE_SNIFFER
extern WaveChart waveChart;
#endif
extern bool hasFirmwareErrorFlag;

/**
 * Difference between current 1st trigger event and previous 1st trigger event.
 */
static volatile uint32_t engineCycleDurationUs;
static volatile efitime_t previousEngineCycleTimeUs = 0;

static int waveReaderCount = 0;
static WaveReader readers[MAX_ICU_COUNT];

static Logging * logger;

static void ensureInitialized(WaveReader *reader) {
	/*may be*/UNUSED(reader);
	efiAssertVoid(CUSTOM_ERR_6654, reader->isStarted, "wave analyzer NOT INITIALIZED");
}

#if EFI_WAVE_ANALYZER

void WaveReader::onRiseEvent() {
	efitick_t nowUs = getTimeNowUs();
	riseEventCounter++;
	lastActivityTimeUs = nowUs;
	assertIsrContext(CUSTOM_ERR_6670);
	addEngineSnifferEvent(name, PROTOCOL_ES_UP);

	uint32_t width = nowUs - periodEventTimeUs;
	last_wave_low_widthUs = width;

	signalPeriodUs = nowUs - widthEventTimeUs;
	widthEventTimeUs = nowUs;
}

void WaveReader::onFallEvent() {
	efitick_t nowUs = getTimeNowUs();
	fallEventCounter++;
	lastActivityTimeUs = nowUs;
	assertIsrContext(CUSTOM_ERR_6670);
	addEngineSnifferEvent(name, PROTOCOL_ES_DOWN);

	efitick_t width = nowUs - widthEventTimeUs;
	last_wave_high_widthUs = width;

	int revolutionCounter = engine->rpmCalculator.getRevolutionCounter();

	totalOnTimeAccumulatorUs += width;
	if (currentRevolutionCounter != revolutionCounter) {
		/**
		 * We are here in case of a new engine cycle
		 */
		currentRevolutionCounter = revolutionCounter;
		prevTotalOnTimeUs = totalOnTimeAccumulatorUs;
		totalOnTimeAccumulatorUs = 0;

		waveOffsetUs = nowUs - previousEngineCycleTimeUs;

	}

	periodEventTimeUs = nowUs;

//	uint32_t period = engineCycleDurationUs;  // local copy of volatile variable
}

static void waveCallback(void* data) {
	WaveReader *reader = reinterpret_cast<WaveReader*>(data);

	bool rise = palReadLine(reader->pin) == PAL_HIGH;

	if (rise) {
		reader->onRiseEvent();
	} else {
		reader->onFallEvent();
	}
}

static void initWave(const char *name, int index) {
	brain_pin_e brainPin = CONFIGB(logicAnalyzerPins)[index];

	if (brainPin == GPIO_UNASSIGNED)
		return;

	waveReaderCount++;
	efiAssertVoid(CUSTOM_ERR_6655, index < MAX_ICU_COUNT, "too many ICUs");
	WaveReader *reader = &readers[index];
	reader->name = name;
	reader->pin = PAL_LINE(getHwPort("wave", brainPin), getHwPin("wave", brainPin));

	efiExtiEnablePin("wave input", brainPin, PAL_EVENT_MODE_BOTH_EDGES, waveCallback, reinterpret_cast<void*>(reader));
	reader->isStarted = true;

	print("wave%d input on %s\r\n", index, hwPortname(brainPin));
}
#endif

static void waTriggerEventListener(trigger_event_e ckpSignalType, uint32_t index DECLARE_ENGINE_PARAMETER_SUFFIX) {
	(void)ckpSignalType;
	if (index != 0) {
		return;
	}
	efitick_t nowUs = getTimeNowUs();
	engineCycleDurationUs = nowUs - previousEngineCycleTimeUs;
	previousEngineCycleTimeUs = nowUs;
}

/*
static uint32_t getWaveLowWidth(int index) {
	WaveReader *reader = &readers[index];
	ensureInitialized(reader);
	return reader->last_wave_low_widthUs;
}
*/

static float getSignalOnTime(int index) {
	WaveReader *reader = &readers[index];
	ensureInitialized(reader);
	if (getTimeNowUs() - reader->lastActivityTimeUs > 4 * US_PER_SECOND) {
		return 0.0f; // dwell time has expired
	}
	return reader->last_wave_high_widthUs / 1000.0f;
}

static efitime_t getWaveOffset(int index) {
	WaveReader *reader = &readers[index];
	ensureInitialized(reader);
	return reader->waveOffsetUs;
}

static float getSignalPeriodMs(int index) {
	WaveReader *reader = &readers[index];
	ensureInitialized(reader);
	return reader->signalPeriodUs / 1000.0f;
}

//static efitime_t getWidthEventTime(int index) {
//	WaveReader *reader = &readers[index];
//	ensureInitialized(reader);
//	return reader->widthEventTimeUs;
//}

static void reportWave(Logging *logging, int index) {
	if (!readers[index].isStarted) {
		return;
	}

	float dwellMs = getSignalOnTime(index);
	float periodMs = getSignalPeriodMs(index);

	appendPrintf(logging, "duty%d%s", index, DELIMETER);
	appendFloat(logging, 100.0f * dwellMs / periodMs, 2);
	appendPrintf(logging, "%s", DELIMETER);

	/**
	 * that's the ON time of the LAST signal
	 */
	appendPrintf(logging, "dwell%d%s", index, DELIMETER);
	appendFloat(logging, dwellMs, 2);
	appendPrintf(logging, "%s", DELIMETER);

	/**
	 * that's the total ON time during the previous engine cycle
	 */
	appendPrintf(logging, "total_dwell%d%s", index, DELIMETER);
	appendFloat(logging, readers[index].prevTotalOnTimeUs / 1000.0f, 2);
	appendPrintf(logging, "%s", DELIMETER);

	appendPrintf(logging, "period%d%s", index, DELIMETER);
	appendFloat(logging, periodMs, 2);
	appendPrintf(logging, "%s", DELIMETER);

	uint32_t offsetUs = getWaveOffset(index);
	int rpm = GET_RPM();
	if (rpm != 0) {
		float oneDegreeUs = getOneDegreeTimeUs(rpm);

		appendPrintf(logging, "advance%d%s", index, DELIMETER);
		float angle = (offsetUs / oneDegreeUs) - tdcPosition();
		fixAngle(angle, "waveAn", CUSTOM_ERR_6564);
		appendFloat(logging, angle, 3);
		appendPrintf(logging, "%s", DELIMETER);
	}
}

void printWave(Logging *logging) {
	reportWave(logging, 0);
	reportWave(logging, 1);
}

void showWaveInfo(void) {
	scheduleMsg(logger, "logic input #1: %d/%d", readers[0].fallEventCounter, readers[0].riseEventCounter);
}

void initWaveAnalyzer(Logging *sharedLogger) {
	logger = sharedLogger;
	if (hasFirmwareError()) {
		return;
	}
#if EFI_WAVE_ANALYZER
	initWave(PROTOCOL_WA_CHANNEL_1, 0);
	initWave(PROTOCOL_WA_CHANNEL_2, 1);
	initWave(PROTOCOL_WA_CHANNEL_3, 2);
	initWave(PROTOCOL_WA_CHANNEL_4, 3);

	addTriggerEventListener(waTriggerEventListener, "wave analyzer", engine);

	addConsoleAction("waveinfo", showWaveInfo);
#else
	print("wave disabled\r\n");
#endif
}

#endif
