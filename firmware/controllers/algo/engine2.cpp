/*
 * engine2.cpp
 *
 * @date Jan 5, 2019
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

// todo: move this code to more proper locations

#include "pch.h"

#include "os_access.h"
#include "speed_density.h"
#include "fuel_math.h"
#include "advance_map.h"
#include "aux_valves.h"
#include "closed_loop_fuel.h"
#include "launch_control.h"
#include "injector_model.h"

#if EFI_PROD_CODE
#include "svnversion.h"
#endif

#if ! EFI_UNIT_TEST
#include "status_loop.h"
#endif

WarningCodeState::WarningCodeState() {
	clear();
}

void WarningCodeState::clear() {
	warningCounter = 0;
	lastErrorCode = 0;
	timeOfPreviousWarning = DEEP_IN_THE_PAST_SECONDS;
	recentWarnings.clear();
}

void WarningCodeState::addWarningCode(obd_code_e code) {
	warningCounter++;
	lastErrorCode = code;
	if (!recentWarnings.contains(code)) {
		chibios_rt::CriticalSectionLocker csl;

		// We don't bother double checking
		recentWarnings.add((int)code);
	}
}

/**
 * @param forIndicator if we want to retrieving value for TS indicator, this case a minimal period is applued
 */
bool WarningCodeState::isWarningNow(efitimesec_t now, bool forIndicator) const {
	int period = forIndicator ? maxI(3, engineConfiguration->warningPeriod) : engineConfiguration->warningPeriod;
	return absI(now - timeOfPreviousWarning) < period;
}

void FuelConsumptionState::consumeFuel(float grams, efitick_t nowNt) {
	m_consumedGrams += grams;

	float elapsedSecond = m_timer.getElapsedSecondsAndReset(nowNt);

	// If it's been a long time since last injection, ignore this pulse
	if (elapsedSecond > 0.2f) {
		m_rate = 0;
	} else {
		m_rate = grams / elapsedSecond;
	}
}

float FuelConsumptionState::getConsumedGrams() const {
	return m_consumedGrams;
}

float FuelConsumptionState::getConsumptionGramPerSecond() const {
	return m_rate;
}

EngineState::EngineState() {
	timeSinceLastTChargeK = getTimeNowNt();
}

void EngineState::updateSlowSensors() {
}

void EngineState::periodicFastCallback() {
	ScopePerf perf(PE::EngineStatePeriodicFastCallback);

#if EFI_ENGINE_CONTROL
	if (!engine->slowCallBackWasInvoked) {
		warning(CUSTOM_SLOW_NOT_INVOKED, "Slow not invoked yet");
	}
	efitick_t nowNt = getTimeNowNt();
	if (engine->rpmCalculator.isCranking()) {
		crankingTime = nowNt;
		timeSinceCranking = 0.0f;
	} else {
		timeSinceCranking = nowNt - crankingTime;
	}
	recalculateAuxValveTiming();

	int rpm = Sensor::getOrZero(SensorType::Rpm);
	sparkDwell = engine->ignitionState.getSparkDwell(rpm);
	dwellAngle = cisnan(rpm) ? NAN :  sparkDwell / getOneDegreeTimeMs(rpm);

	// todo: move this into slow callback, no reason for IAT corr to be here
	running.intakeTemperatureCoefficient = getIatFuelCorrection();
	// todo: move this into slow callback, no reason for CLT corr to be here
	running.coolantTemperatureCoefficient = getCltFuelCorrection();

	engine->module<DfcoController>()->update();

	// post-cranking fuel enrichment.
	// for compatibility reasons, apply only if the factor is greater than unity (only allow adding fuel)
	if (engineConfiguration->postCrankingFactor > 1.0f) {
		// convert to microsecs and then to seconds
		running.timeSinceCrankingInSecs = NT2US(timeSinceCranking) / US_PER_SECOND_F;
		// use interpolation for correction taper
		running.postCrankingFuelCorrection = interpolateClamped(0.0f, engineConfiguration->postCrankingFactor,
			engineConfiguration->postCrankingDurationSec, 1.0f, running.timeSinceCrankingInSecs);
	} else {
		running.postCrankingFuelCorrection = 1.0f;
	}

	cltTimingCorrection = getCltTimingCorrection();

	baroCorrection = getBaroCorrection();

	auto tps = Sensor::get(SensorType::Tps1);
	updateTChargeK(rpm, tps.value_or(0));

	float injectionMass = getInjectionMass(rpm);
	auto clResult = fuelClosedLoopCorrection();

	// Store the pre-wall wetting injection duration for scheduling purposes only, not the actual injection duration
	engine->injectionDuration = engine->module<InjectorModel>()->getInjectionDuration(injectionMass);

	float fuelLoad = getFuelingLoad();
	injectionOffset = getInjectionOffset(rpm, fuelLoad);

	float ignitionLoad = getIgnitionLoad();
	float advance = getAdvance(rpm, ignitionLoad) * engine->ignitionState.luaTimingMult + engine->ignitionState.luaTimingAdd;

	// compute per-bank fueling
	for (size_t i = 0; i < STFT_BANK_COUNT; i++) {
		float corr = clResult.banks[i];
		engine->stftCorrection[i] = corr;
	}

	// Now apply that to per-cylinder fueling and timing
	for (size_t i = 0; i < engineConfiguration->specs.cylindersCount; i++) {
		uint8_t bankIndex = engineConfiguration->cylinderBankSelect[i];
		auto bankTrim =engine->stftCorrection[bankIndex];
		auto cylinderTrim = getCylinderFuelTrim(i, rpm, fuelLoad);

		// Apply both per-bank and per-cylinder trims
		engine->injectionMass[i] = injectionMass * bankTrim * cylinderTrim;

		timingAdvance[i] = advance + getCylinderIgnitionTrim(i, rpm, ignitionLoad);
	}

	// TODO: calculate me from a table!
	trailingSparkAngle = engineConfiguration->trailingSparkAngle;

	multispark.count = getMultiSparkCount(rpm);

#if EFI_LAUNCH_CONTROL
	engine->launchController.update();
#endif //EFI_LAUNCH_CONTROL

	engine->limpManager.updateState(rpm, nowNt);

#endif // EFI_ENGINE_CONTROL
}

void EngineState::updateTChargeK(int rpm, float tps) {
#if EFI_ENGINE_CONTROL
	float newTCharge = getTCharge(rpm, tps);
	// convert to microsecs and then to seconds
	efitick_t curTime = getTimeNowNt();
	float secsPassed = (float)NT2US(curTime - timeSinceLastTChargeK) / US_PER_SECOND_F;
	if (!cisnan(newTCharge)) {
		// control the rate of change or just fill with the initial value
		sd.tCharge = (sd.tChargeK == 0) ? newTCharge : limitRateOfChange(newTCharge, sd.tCharge, engineConfiguration->tChargeAirIncrLimit, engineConfiguration->tChargeAirDecrLimit, secsPassed);
		sd.tChargeK = convertCelsiusToKelvin(sd.tCharge);
		timeSinceLastTChargeK = curTime;
	}
#endif
}

#if EFI_SIMULATOR
#define VCS_VERSION "123"
#endif

void TriggerConfiguration::update() {
	UseOnlyRisingEdgeForTrigger = isUseOnlyRisingEdgeForTrigger();
	VerboseTriggerSynchDetails = isVerboseTriggerSynchDetails();
	TriggerType = getType();
}

bool PrimaryTriggerConfiguration::isUseOnlyRisingEdgeForTrigger() const {
	return engineConfiguration->useOnlyRisingEdgeForTrigger;
}

trigger_type_e PrimaryTriggerConfiguration::getType() const {
	return engineConfiguration->trigger.type;
}

bool PrimaryTriggerConfiguration::isVerboseTriggerSynchDetails() const {
	return engineConfiguration->verboseTriggerSynchDetails;
}

bool VvtTriggerConfiguration::isUseOnlyRisingEdgeForTrigger() const {
	return engineConfiguration->vvtCamSensorUseRise;
}

trigger_type_e VvtTriggerConfiguration::getType() const {
	// Convert from VVT type to trigger type
	return getVvtTriggerType(engineConfiguration->vvtMode[index]);
}

bool VvtTriggerConfiguration::isVerboseTriggerSynchDetails() const {
	return engineConfiguration->verboseVVTDecoding;
}
