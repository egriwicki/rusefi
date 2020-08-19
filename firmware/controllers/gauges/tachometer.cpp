/*
 * @file tachometer.cpp
 * @brief This is about driving external analog tachometers
 *
 * This implementation produces one pulse per engine cycle
 *
 * todo: these is a bit of duplication with dizzySparkOutputPin
 *
 * @date Aug 18, 2015
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#include "tachometer.h"
#include "pwm_generator_logic.h"

EXTERN_ENGINE;

static SimplePwm tachControl("tach"); 
static float tachFreq;  
static float duty;   

#if EFI_UNIT_TEST
float getTachFreq() {
	return tachFreq;
}

float getTachDuty() {
	return duty;
}
#endif

static bool tachHasInit = false;

void tachSignalCallback(DECLARE_ENGINE_PARAMETER_SIGNATURE) {
	// Only do anything if tach enabled
	if (!tachHasInit) {
		return;
	}

	// How many tach pulse periods do we have?
	int periods = CONFIG(tachPulsePerRev);

	if (periods == 0 || periods > 10){
		firmwareError(CUSTOM_ERR_6709, "Invalid tachometer pulse per rev: %d", periods);
		return;
	}

	// What is the angle per tach output period?
	float cycleTimeMs = 60000.0f / GET_RPM();
	float periodTimeMs = cycleTimeMs / periods;
	tachFreq = 1000.0f / periodTimeMs;
	
	if (CONFIG(tachPulseDurationAsDutyCycle)) {
		// Simple case - duty explicitly set
		duty = CONFIG(tachPulseDuractionMs);
	} else {
		// Constant high-time mode - compute the correct duty cycle
		duty = CONFIG(tachPulseDuractionMs) / periodTimeMs;
	}

	// In case Freq is under 1Hz, we stop pwm to avoid warnings!
	if (tachFreq < 1) {
		tachFreq = NAN;
	}
	
	tachControl.setSimplePwmDutyCycle(duty);	
	tachControl.setFrequency(tachFreq);
}

void initTachometer(DECLARE_ENGINE_PARAMETER_SIGNATURE) {
	if (CONFIG(tachOutputPin) == GPIO_UNASSIGNED) {
		return;
	}

	startSimplePwmExt(&tachControl,
				"Tachometer",
				&engine->executor,
				CONFIG(tachOutputPin),
				&enginePins.tachOut,
				NAN, 0.1f);

	tachHasInit = true;
}
