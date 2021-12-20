/*
 * @file	trigger_honda.cpp
 *
 * @date May 27, 2016
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#include "trigger_honda.h"
#include "trigger_universal.h"

#define S24 (720.0f / 24 / 2)

static float addAccordPair(TriggerWaveform *s, float sb, trigger_wheel_e const channelIndex) {
	s->addEvent720(sb, channelIndex, TV_RISE);
	sb += S24;
	s->addEvent720(sb, channelIndex, TV_FALL);
	sb += S24;

	return sb;
}

#define DIP 7.5f
static float addAccordPair3(TriggerWaveform *s, float sb) {
	sb += DIP;
	s->addEvent720(sb, T_CHANNEL_3, TV_RISE);
	sb += DIP;
	s->addEvent720(sb, T_CHANNEL_3, TV_FALL);
	sb += 2 * DIP;
	return sb;
}

/**
 * Thank you Dip!
 * http://forum.pgmfi.org/viewtopic.php?f=2&t=15570start=210#p139007
 */
void configureHondaAccordCDDip(TriggerWaveform *s) {
	s->initialize(FOUR_STROKE_CAM_SENSOR);

	s->initialState[T_SECONDARY] = TV_RISE;
	float sb = 0;
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);

	s->addEvent720(90, T_SECONDARY, TV_FALL);
	sb = 90;
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);

	s->addEvent720(180, T_SECONDARY, TV_RISE);
	sb = 180;
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);

	s->addEvent720(270, T_SECONDARY, TV_FALL);
	sb = 270;
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);


	s->addEvent720(360.0f - DIP, T_PRIMARY, TV_RISE);
	s->addEvent720(360, T_SECONDARY, TV_RISE);
	sb = 360;
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);

	s->addEvent720(450, T_SECONDARY, TV_FALL);
	sb = 450;
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);

	s->addEvent720(540, T_SECONDARY, TV_RISE);
	sb = 540;
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);

	s->addEvent720(630, T_SECONDARY, TV_FALL);
	sb = 630;
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);
	sb = addAccordPair3(s, sb);

	s->addEvent720(720.0f - DIP, T_PRIMARY, TV_FALL);

//	s->addEvent720(720.0f - 12 * sb, T_SECONDARY, TV_FALL);
//	s->addEvent720(720.0f, T_SECONDARY, TV_FALL);

	s->addEvent720(720.0f, T_SECONDARY, TV_RISE);

	s->isSynchronizationNeeded = false;
}

/**
 * '1' is conditional
 * '4' is conditional
 * '24' is always secondary channel
 */
void configureHonda_1_4_24(TriggerWaveform *s, bool withOneEventSignal, bool withFourEventSignal,
		trigger_wheel_e const oneEventWave,
		trigger_wheel_e const fourEventWave,
		float prefix) {
	s->initialize(FOUR_STROKE_CAM_SENSOR);


	float sb = 5.0f + prefix;

	float tdcWidth = 0.1854 * 720 / 4;

	s->isSynchronizationNeeded = false;

	sb = addAccordPair(s, sb, T_SECONDARY);

	if (withOneEventSignal)
		s->addEvent720(sb - S24 / 2, oneEventWave, TV_RISE);

	sb = addAccordPair(s, sb, T_SECONDARY);
	sb = addAccordPair(s, sb, T_SECONDARY);
	if (withOneEventSignal)
		s->addEvent720(sb - S24 / 2, oneEventWave, TV_FALL);
	sb = addAccordPair(s, sb, T_SECONDARY);
	sb = addAccordPair(s, sb, T_SECONDARY);
	if (withFourEventSignal) {
		s->addEvent720(1 * 180.0f + prefix - tdcWidth, fourEventWave, TV_RISE);
	}
	sb = addAccordPair(s, sb, T_SECONDARY);
	if (withFourEventSignal) {
		s->addEvent720(1 * 180.0f + prefix, fourEventWave, TV_FALL);
	}

	sb = addAccordPair(s, sb, T_SECONDARY);
	sb = addAccordPair(s, sb,T_SECONDARY);
	sb = addAccordPair(s, sb, T_SECONDARY);
	sb = addAccordPair(s, sb, T_SECONDARY);
	sb = addAccordPair(s, sb, T_SECONDARY);

	if (withFourEventSignal) {
		s->addEvent720(2 * 180.0f + prefix - tdcWidth, fourEventWave, TV_RISE);
	}
	sb = addAccordPair(s, sb, T_SECONDARY);
	if (withFourEventSignal) {
		s->addEvent720(2 * 180.0f + prefix, fourEventWave, TV_FALL);
	}

	for (int i = 3; i <= 4; i++) {
		sb = addAccordPair(s, sb, T_SECONDARY);
		sb = addAccordPair(s, sb, T_SECONDARY);
		sb = addAccordPair(s, sb, T_SECONDARY);
		sb = addAccordPair(s, sb, T_SECONDARY);
		sb = addAccordPair(s, sb, T_SECONDARY);

		if (withFourEventSignal) {
			s->addEvent720(i * 180.0f + prefix - tdcWidth, fourEventWave, TV_RISE);
		}
		sb = addAccordPair(s, sb, T_SECONDARY);
		if (withFourEventSignal) {
			s->addEvent720(i * 180.0f + prefix, fourEventWave, TV_FALL);
		}
	}
	s->useOnlyPrimaryForSync = true;
}

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

void configureHondaAccordShifted(TriggerWaveform *s) {
	s->initialize(FOUR_STROKE_CAM_SENSOR);

	float sb = S24;

	// like this there is no issue
//	s->addEvent720(S24 + 0.001, T_PRIMARY, TV_RISE);
//	s->addEvent720(S24 + 0.1, T_SECONDARY, TV_RISE);

	s->addEvent720(S24 + 0.001, T_SECONDARY, TV_RISE);
	s->addEvent720(S24 + 0.1, T_PRIMARY, TV_RISE);



	sb += S24;
	s->addEvent720(sb, T_SECONDARY, TV_FALL);
	sb += S24;

	s->addEvent720(S24 + 22, T_PRIMARY, TV_FALL);


	for (int i = 0;i<23;i++) {
		sb = addAccordPair(s, sb, T_SECONDARY);
	}


	s->isSynchronizationNeeded = false;
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
