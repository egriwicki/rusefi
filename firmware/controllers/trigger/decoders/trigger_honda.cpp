/*
 * @file	trigger_honda.cpp
 *
 * @date May 27, 2016
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#include "pch.h"

#include "trigger_honda.h"
#include "trigger_universal.h"

void configureHondaCbr600(TriggerWaveform *s) {
	s->initialize(FOUR_STROKE_CAM_SENSOR);
	s->useOnlyPrimaryForSync = true;
	s->setTriggerSynchronizationGap(6);


	int totalTeethCount = 24;
	int skippedCount = 0;

	addSkippedToothTriggerEvents(T_SECONDARY, s, totalTeethCount, skippedCount, 0.5, 0, 720,
	0, 349);

	s->addEvent720(350.0f, T_PRIMARY, TV_FALL);
	s->addEvent720(360.0f, T_PRIMARY, TV_RISE);

	s->addEvent720(360 + 0.2, T_SECONDARY, TV_FALL);

	addSkippedToothTriggerEvents(T_SECONDARY, s, totalTeethCount, skippedCount, 0.5, 0, 720,
	361, 649);



	s->addEvent720(650.0f, T_PRIMARY, TV_FALL);
	s->addEvent720(660.0f, T_PRIMARY, TV_RISE);

	s->addEvent720(660 + 0.2, T_SECONDARY, TV_FALL);


	addSkippedToothTriggerEvents(T_SECONDARY, s, totalTeethCount, skippedCount, 0.5, 0, 720,
	661, 709);


//	exit(-1);

	s->addEvent720(710.0f, T_PRIMARY, TV_FALL);

	s->addEvent720(720.0f - 1, T_SECONDARY, TV_FALL);

	s->addEvent720(720.0f, T_PRIMARY, TV_RISE);
}

void configureOnePlus16(TriggerWaveform *s) {
	s->initialize(FOUR_STROKE_CAM_SENSOR);

	int count = 16;
	float tooth = s->getCycleDuration() / count;
	float width = tooth / 2; // for VR we only handle rises so width does not matter much

	s->addEventAngle(1, T_PRIMARY, TV_RISE);
	s->addEventAngle(5, T_PRIMARY, TV_FALL);

	for (int i = 1; i <= count; i++) {
		s->addEventAngle(tooth * i - width, T_SECONDARY, TV_RISE);
		s->addEventAngle(tooth * i,         T_SECONDARY, TV_FALL);
	}

	s->isSynchronizationNeeded = false;
}

static void kseriesTooth(TriggerWaveform* s, float end) {
	// for VR we only handle rises so width does not matter much
	s->addEvent360(end - 4, T_PRIMARY, TV_RISE);
	s->addEvent360(end    , T_PRIMARY, TV_FALL);
}

// TT_HONDA_K_12_1
void configureHondaK_12_1(TriggerWaveform *s) {
	s->initialize(FOUR_STROKE_CRANK_SENSOR);

	// nominal gap 0.33
	s->setSecondTriggerSynchronizationGap2(0.2f, 0.5f);
	// nominal gap 2.0
	s->setTriggerSynchronizationGap2(1.6f, 2.4f);

	int count = 12;
	float tooth = 360 / count; // hint: tooth = 30

	// Extra "+1" tooth happens 1/3 of the way between first two teeth
	kseriesTooth(s, tooth / 3);

	for (int i = 1; i <= count; i++) {
		kseriesTooth(s, tooth * i);
	}
}

/**
 * Exhaust cam shaft, not variable on Honda K
 * 2003 Honda Element
 */
void configureHondaK_4_1(TriggerWaveform *s) {
	s->initialize(FOUR_STROKE_CAM_SENSOR);

	s->setTriggerSynchronizationGap3(/*gapIndex*/0, 1.11, 2.38);
	s->setTriggerSynchronizationGap3(/*gapIndex*/1, 0.28, 0.474);

	angle_t start = 55.5;
	angle_t end = 70.5;
	s->addEvent360(start + 90 * 0, T_PRIMARY, TV_RISE);
	s->addEvent360(end + 90 * 0, T_PRIMARY, TV_FALL);

	s->addEvent360(start + 90 * 1, T_PRIMARY, TV_RISE);
	s->addEvent360(end + 90 * 1, T_PRIMARY, TV_FALL);

	s->addEvent360(start + 90 * 2, T_PRIMARY, TV_RISE);
	s->addEvent360(end + 90 * 2, T_PRIMARY, TV_FALL);

	s->addEvent360(start + 90 * 3, T_PRIMARY, TV_RISE);
	s->addEvent360(end + 90 * 3, T_PRIMARY, TV_FALL);

	s->addEvent360(353, T_PRIMARY, TV_RISE);
	s->addEvent360(360, T_PRIMARY, TV_FALL);
}
