/*
 * engine2.cpp
 *
 * @date Jan 5, 2019
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

// todo: move this code to more proper locations

#include "engine.h"
#include "thermistors.h"
#include "speed_density.h"
#include "allsensors.h"
#include "fuel_math.h"
#include "engine_math.h"
#include "advance_map.h"
#include "aux_valves.h"
#include "perf_trace.h"
#include "closed_loop_fuel.h"
#include "sensor.h"

#if EFI_PROD_CODE
#include "svnversion.h"
#endif

#if ! EFI_UNIT_TEST
#include "status_loop.h"
#endif

extern fuel_Map3D_t veMap;
extern afr_Map3D_t afrMap;

EXTERN_ENGINE;

// this does not look exactly right
extern LoggingWithStorage engineLogger;

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
		recentWarnings.add((int)code);
	}
}

/**
 * @param forIndicator if we want to retrieving value for TS indicator, this case a minimal period is applued
 */
bool WarningCodeState::isWarningNow(efitimesec_t now, bool forIndicator DECLARE_ENGINE_PARAMETER_SUFFIX) const {
	int period = forIndicator ? maxI(3, engineConfiguration->warningPeriod) : engineConfiguration->warningPeriod;
	return absI(now - timeOfPreviousWarning) < period;
}

MockAdcState::MockAdcState() {
	memset(hasMockAdc, 0, sizeof(hasMockAdc));
}

#if EFI_ENABLE_MOCK_ADC
void MockAdcState::setMockVoltage(int hwChannel, float voltage DECLARE_ENGINE_PARAMETER_SUFFIX) {
	efiAssertVoid(OBD_PCM_Processor_Fault, hwChannel >= 0 && hwChannel < MOCK_ADC_SIZE, "hwChannel out of bounds");
	scheduleMsg(&engineLogger, "fake voltage: channel %d value %.2f", hwChannel, voltage);

	fakeAdcValues[hwChannel] = voltsToAdc(voltage);
	hasMockAdc[hwChannel] = true;
}
#endif /* EFI_ENABLE_MOCK_ADC */

FuelConsumptionState::FuelConsumptionState() {
	accumulatedSecondPrevNt = accumulatedMinutePrevNt = getTimeNowNt();
}

void FuelConsumptionState::addData(float durationMs) {
	if (durationMs > 0.0f) {
		perSecondAccumulator += durationMs;
		perMinuteAccumulator += durationMs;
	}
}

void FuelConsumptionState::update(efitick_t nowNt DECLARE_ENGINE_PARAMETER_SUFFIX) {
	efitick_t deltaNt = nowNt - accumulatedSecondPrevNt;
	if (deltaNt >= NT_PER_SECOND) {
		perSecondConsumption = getFuelRate(perSecondAccumulator, deltaNt PASS_ENGINE_PARAMETER_SUFFIX);
		perSecondAccumulator = 0;
		accumulatedSecondPrevNt = nowNt;
	}

	deltaNt = nowNt - accumulatedMinutePrevNt;
	if (deltaNt >= NT_PER_SECOND * 60) {
		perMinuteConsumption = getFuelRate(perMinuteAccumulator, deltaNt PASS_ENGINE_PARAMETER_SUFFIX);
		perMinuteAccumulator = 0;
		accumulatedMinutePrevNt = nowNt;
	}
}

TransmissionState::TransmissionState() {
}

EngineState::EngineState() {
	timeSinceLastTChargeK = getTimeNowNt();

#if ! EFI_PROD_CODE
	memset(mockPinStates, 0, sizeof(mockPinStates));
#endif /* EFI_PROD_CODE */
}

void EngineState::updateSlowSensors(DECLARE_ENGINE_PARAMETER_SIGNATURE) {
	// this feeds rusEfi console Live Data
	engine->engineState.isCrankingState = ENGINE(rpmCalculator).isCranking();
}

void EngineState::periodicFastCallback(DECLARE_ENGINE_PARAMETER_SIGNATURE) {
	ScopePerf perf(PE::EngineStatePeriodicFastCallback);

#if EFI_ENGINE_CONTROL
	if (!engine->slowCallBackWasInvoked) {
		warning(CUSTOM_SLOW_NOT_INVOKED, "Slow not invoked yet");
	}
	efitick_t nowNt = getTimeNowNt();
	if (ENGINE(rpmCalculator).isCranking()) {
		crankingTime = nowNt;
		timeSinceCranking = 0.0f;
	} else {
		timeSinceCranking = nowNt - crankingTime;
	}
	updateAuxValves(PASS_ENGINE_PARAMETER_SIGNATURE);

	int rpm = ENGINE(rpmCalculator).getRpm();
	sparkDwell = getSparkDwell(rpm PASS_ENGINE_PARAMETER_SUFFIX);
	dwellAngle = cisnan(rpm) ? NAN :  sparkDwell / getOneDegreeTimeMs(rpm);

	// todo: move this into slow callback, no reason for IAT corr to be here
	running.intakeTemperatureCoefficient = getIatFuelCorrection(PASS_ENGINE_PARAMETER_SIGNATURE);
	// todo: move this into slow callback, no reason for CLT corr to be here
	running.coolantTemperatureCoefficient = getCltFuelCorrection(PASS_ENGINE_PARAMETER_SIGNATURE);

	running.pidCorrection = fuelClosedLoopCorrection(PASS_ENGINE_PARAMETER_SIGNATURE);

	// update fuel consumption states
	fuelConsumption.update(nowNt PASS_ENGINE_PARAMETER_SUFFIX);

	// Fuel cut-off isn't just 0 or 1, it can be tapered
	fuelCutoffCorrection = getFuelCutOffCorrection(nowNt, rpm PASS_ENGINE_PARAMETER_SUFFIX);

	// post-cranking fuel enrichment.
	// for compatibility reasons, apply only if the factor is greater than zero (0.01 margin used)
	if (engineConfiguration->postCrankingFactor > 0.01f) {
		// convert to microsecs and then to seconds
		running.timeSinceCrankingInSecs = NT2US(timeSinceCranking) / 1000000.0f;
		// use interpolation for correction taper
		running.postCrankingFuelCorrection = interpolateClamped(0.0f, engineConfiguration->postCrankingFactor,
			engineConfiguration->postCrankingDurationSec, 1.0f, running.timeSinceCrankingInSecs);
	} else {
		running.postCrankingFuelCorrection = 1.0f;
	}

	cltTimingCorrection = getCltTimingCorrection(PASS_ENGINE_PARAMETER_SIGNATURE);

	engineNoiseHipLevel = interpolate2d("knock", rpm, engineConfiguration->knockNoiseRpmBins,
					engineConfiguration->knockNoise);

	baroCorrection = getBaroCorrection(PASS_ENGINE_PARAMETER_SIGNATURE);

	ENGINE(injectionDuration) = getInjectionDuration(rpm PASS_ENGINE_PARAMETER_SUFFIX);

	float fuelLoad = getFuelingLoad(PASS_ENGINE_PARAMETER_SIGNATURE);
	injectionOffset = getInjectionOffset(rpm, fuelLoad PASS_ENGINE_PARAMETER_SUFFIX);

	float ignitionLoad = getIgnitionLoad(PASS_ENGINE_PARAMETER_SIGNATURE);
	timingAdvance = getAdvance(rpm, ignitionLoad PASS_ENGINE_PARAMETER_SUFFIX);
	multispark.count = getMultiSparkCount(rpm PASS_ENGINE_PARAMETER_SUFFIX);
#endif // EFI_ENGINE_CONTROL
}

void EngineState::updateTChargeK(int rpm, float tps DECLARE_ENGINE_PARAMETER_SUFFIX) {
#if EFI_ENGINE_CONTROL
	float newTCharge = getTCharge(rpm, tps PASS_ENGINE_PARAMETER_SUFFIX);
	// convert to microsecs and then to seconds
	efitick_t curTime = getTimeNowNt();
	float secsPassed = (float)NT2US(curTime - timeSinceLastTChargeK) / 1000000.0f;
	if (!cisnan(newTCharge)) {
		// control the rate of change or just fill with the initial value
		sd.tCharge = (sd.tChargeK == 0) ? newTCharge : limitRateOfChange(newTCharge, sd.tCharge, CONFIG(tChargeAirIncrLimit), CONFIG(tChargeAirDecrLimit), secsPassed);
		sd.tChargeK = convertCelsiusToKelvin(sd.tCharge);
		timeSinceLastTChargeK = curTime;
	}
#endif
}

SensorsState::SensorsState() {
}

int MockAdcState::getMockAdcValue(int hwChannel) const {
	efiAssert(OBD_PCM_Processor_Fault, hwChannel >= 0 && hwChannel < MOCK_ADC_SIZE, "hwChannel out of bounds", -1);
	return fakeAdcValues[hwChannel];
}

StartupFuelPumping::StartupFuelPumping() {
	isTpsAbove50 = false;
	pumpsCounter = 0;
}

void StartupFuelPumping::setPumpsCounter(int newValue) {
	if (pumpsCounter != newValue) {
		pumpsCounter = newValue;

		if (pumpsCounter == PUMPS_TO_PRIME) {
			scheduleMsg(&engineLogger, "let's squirt prime pulse %.2f", pumpsCounter);
			pumpsCounter = 0;
		} else {
			scheduleMsg(&engineLogger, "setPumpsCounter %d", pumpsCounter);
		}
	}
}

void StartupFuelPumping::update(DECLARE_ENGINE_PARAMETER_SIGNATURE) {
	if (GET_RPM() == 0) {
		bool isTpsAbove50 = Sensor::get(SensorType::DriverThrottleIntent).value_or(0) >= 50;

		if (this->isTpsAbove50 != isTpsAbove50) {
			setPumpsCounter(pumpsCounter + 1);
		}

	} else {
		/**
		 * Engine is not stopped - not priming pumping mode
		 */
		setPumpsCounter(0);
		isTpsAbove50 = false;
	}
}

#if EFI_SIMULATOR
#define VCS_VERSION "123"
#endif

void printCurrentState(Logging *logging, int seconds, const char *engineTypeName, const char *firmwareBuildId) {
	logging->appendPrintf("%s%s%d@%s%s %s %d%s", PROTOCOL_VERSION_TAG, DELIMETER,
			getRusEfiVersion(), VCS_VERSION,
			firmwareBuildId,
			engineTypeName,
			seconds,
			DELIMETER);
}

PrimaryTriggerConfiguration::PrimaryTriggerConfiguration(Engine *engine) {
	this->engine = engine;
}

bool PrimaryTriggerConfiguration::isUseOnlyRisingEdgeForTrigger() const {
	return engine->engineConfigurationPtr->useOnlyRisingEdgeForTrigger;
}

debug_mode_e PrimaryTriggerConfiguration::getDebugMode() const {
	return engine->engineConfigurationPtr->debugMode;
}

trigger_type_e PrimaryTriggerConfiguration::getType() const {
	return engine->engineConfigurationPtr->trigger.type;
}

bool PrimaryTriggerConfiguration::isSilentTriggerError() const {
	return engine->engineConfigurationPtr->silentTriggerError;
}

const char * PrimaryTriggerConfiguration::getPrintPrefix() const {
	return "TRG ";
}

bool PrimaryTriggerConfiguration::isVerboseTriggerSynchDetails() const {
	return engine->engineConfigurationPtr->verboseTriggerSynchDetails;
}

VvtTriggerConfiguration::VvtTriggerConfiguration(Engine *engine) {
	this->engine = engine;
}

bool VvtTriggerConfiguration::isUseOnlyRisingEdgeForTrigger() const {
	return engine->engineConfigurationPtr->vvtCamSensorUseRise;
}

const char * VvtTriggerConfiguration::getPrintPrefix() const {
	return "VVT ";
}

debug_mode_e VvtTriggerConfiguration::getDebugMode() const {
	return engine->engineConfigurationPtr->debugMode;
}

trigger_type_e VvtTriggerConfiguration::getType() const {
	return engine->triggerCentral.vvtTriggerType;
}

bool VvtTriggerConfiguration::isSilentTriggerError() const {
	return engine->engineConfigurationPtr->silentTriggerError;
}

bool VvtTriggerConfiguration::isVerboseTriggerSynchDetails() const {
	return engine->engineConfigurationPtr->verboseVVTDecoding;
}
