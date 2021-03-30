/*
 * @file HIP9011_logic.cpp
 *
 *  Created on: Jan 3, 2019
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#include "global.h"
#include "engine.h"
#include "hip9011_logic.h"

/*==========================================================================*/
/* Local definitions.														*/
/*==========================================================================*/

#define DESIRED_OUTPUT_VALUE	5.0f

/*==========================================================================*/
/* Local variables and types.												*/
/*==========================================================================*/

/**
 * These are HIP9011 magic values - integrator time constants in uS
 */
const int integratorValues[INT_LOOKUP_SIZE] = {
/* 00 */ 40,  45,  50,  55,  60,  65,  70,  75,
/* 08 */ 80,  90, 100, 110, 120, 130, 140, 150,
/* 16 */160, 180, 200, 220, 240, 260, 280, 300,
/* 24 */320, 360, 400, 440, 480, 520, 560, 600
};

const float gainLookupInReverseOrder[GAIN_LOOKUP_SIZE] = {
/* 00 */0.111, 0.118, 0.125, 0.129, 0.133, 0.138, 0.143, 0.148,
/* 08 */0.154, 0.160, 0.167, 0.174, 0.182, 0.190, 0.200, 0.211,
/* 16 */0.222, 0.236, 0.250, 0.258, 0.267, 0.276, 0.286, 0.296,
/* 24 */0.308, 0.320, 0.333, 0.348, 0.364, 0.381, 0.400, 0.421,
/* 32 */0.444, 0.471, 0.500, 0.548, 0.567, 0.586, 0.607, 0.630,
/* 40 */0.654, 0.680, 0.708, 0.739, 0.773, 0.810, 0.850, 0.895,
/* 48 */0.944, 1.000, 1.063, 1.143, 1.185, 1.231, 1.280, 1.333,
/* 56 */1.391, 1.455, 1.523, 1.600, 1.684, 1.778, 1.882, 2.000
};

const float bandFreqLookup[BAND_LOOKUP_SIZE] = {
/* 00 */ 1.22,  1.26,  1.31,  1.35,  1.40,  1.45,  1.51,  1.57,
/* 08 */ 1.63,  1.71,  1.78,  1.87,  1.96,  2.07,  2.18,  2.31,
/* 16 */ 2.46,  2.54,  2.62,  2.71,  2.81,  2.92,  3.03,  3.15,
/* 24 */ 3.28,  3.43,  3.59,  3.76,  3.95,  4.16,  4.39,  4.66,
/* 32 */ 4.95,  5.12,  5.29,  5.48,  5.68,  5.90,  6.12,  6.37,
/* 40 */ 6.64,  6.94,  7.27,  7.63,  8.02,  8.46,  8.95,  9.50,
/* 48 */10.12, 10.46, 10.83, 11.22, 11.65, 12.10, 12.60, 13.14,
/* 56 */13.72, 14.36, 15.07, 15.84, 16.71, 17.67, 18.76, 19.98
};

/*==========================================================================*/
/* Forward declarations														*/
/*==========================================================================*/

EXTERN_ENGINE;

/*==========================================================================*/
/* Exported.																*/
/*==========================================================================*/

HIP9011::HIP9011(Hip9011HardwareInterface *hardware) {
	this->hw = hardware;
}

int HIP9011::sendCommand(uint8_t cmd) {
	return hw->sendSyncCommand(cmd, NULL);
}

/**
 * @return frequency band we are interested in
 */
float HIP9011::getBand(DEFINE_HIP_PARAMS) {
	return GET_CONFIG_VALUE(knockBandCustom) == 0 ?
			HIP9011_BAND(GET_CONFIG_VALUE(cylinderBore)) :
			GET_CONFIG_VALUE(knockBandCustom);
}

int HIP9011::getBandIndex(DEFINE_HIP_PARAMS) {
	float freq = getBand(FORWARD_HIP_PARAMS);
	return findIndexMsg("freq", bandFreqLookup, BAND_LOOKUP_SIZE, freq);
}

int HIP9011::getGainIndex(DEFINE_HIP_PARAMS) {
	int i = GAIN_LOOKUP_SIZE - 1 - findIndexMsg("fGain", gainLookupInReverseOrder, GAIN_LOOKUP_SIZE, GET_CONFIG_VALUE(hip9011Gain));
	// GAIN_LOOKUP_SIZE is returned for index which is too low
	return i == GAIN_LOOKUP_SIZE ? GAIN_LOOKUP_SIZE - 1 : i;
}

/**
 * 'TC is typically TINT/(2*Pi*VOUT)'
 * Knock Sensor Training TPIC8101, page 24
 */
float HIP9011::getRpmByAngleWindowAndTimeUs(int timeUs, float angleWindowWidth) {
	/**
	 * TINT = TC * 2 * PI * VOUT
	 */
	float integrationTimeUs = timeUs * 2 * PIF * DESIRED_OUTPUT_VALUE;
	/**
	 * rpm = 60 seconds / time
	 * '60000000' because revolutions per MINUTE in uS conversion
	 */
	float windowWidthMult = angleWindowWidth / 360.0f;
	return 60000000.0f / integrationTimeUs * windowWidthMult;
}

/**
 *
 * We know the set of possible integration times, we know the knock detection window width
 */
void HIP9011::prepareRpmLookup(void) {
	/**
	 * out binary search method needs increasing order thus the reverse order here
	 */
	for (int i = 0; i < INT_LOOKUP_SIZE; i++) {
		rpmLookup[i] = getRpmByAngleWindowAndTimeUs(integratorValues[INT_LOOKUP_SIZE - i - 1], angleWindowWidth);
	}
}

int HIP9011::getIntegrationIndexByRpm(float rpm) {
	int i = findIndexMsg("getIbR", rpmLookup, INT_LOOKUP_SIZE, (rpm));
	return i == -1 ? INT_LOOKUP_SIZE - 1 : INT_LOOKUP_SIZE - i - 1;
}

void HIP9011::setAngleWindowWidth(DEFINE_HIP_PARAMS) {
	float new_angleWindowWidth =
		GET_CONFIG_VALUE(knockDetectionWindowEnd) -
		GET_CONFIG_VALUE(knockDetectionWindowStart);
	if (new_angleWindowWidth < 0) {
#if EFI_PROD_CODE
		warning(CUSTOM_KNOCK_WINDOW, "invalid knock window");
#endif
		new_angleWindowWidth = 0;
	}
	// float '==' is totally appropriate here
	if (angleWindowWidth == new_angleWindowWidth)
		return; // exit if value has not change
	angleWindowWidth = new_angleWindowWidth;
	prepareRpmLookup();
}

void HIP9011::handleSettings(int rpm DEFINE_PARAM_SUFFIX(DEFINE_HIP_PARAMS)) {
	int ret;

	setAngleWindowWidth(FORWARD_HIP_PARAMS);

	int new_prescaler = GET_CONFIG_VALUE(hip9011PrescalerAndSDO);
	int new_integratorIdx = getIntegrationIndexByRpm(rpm);
	int new_gainIdx = getGainIndex(FORWARD_HIP_PARAMS);
	int new_bandIdx = getBandIndex(FORWARD_HIP_PARAMS);

	if (gainIdx != new_gainIdx) {
		ret = sendCommand(SET_GAIN_CMD(new_gainIdx));
		if (ret == 0)
			gainIdx = new_gainIdx;
	}
	if (intergratorIdx != new_integratorIdx) {
		ret = sendCommand(SET_INTEGRATOR_CMD(new_integratorIdx));
		if (ret == 0)
			intergratorIdx = new_integratorIdx;
	}
	if (bandIdx != new_bandIdx) {
		ret = sendCommand(SET_BAND_PASS_CMD(new_bandIdx));
		if (ret == 0)
			bandIdx = new_bandIdx;
	}
	if (prescaler != new_prescaler) {
		ret = sendCommand(SET_PRESCALER_CMD(new_prescaler));
		if (ret == 0)
			prescaler = new_prescaler;
	}
}
