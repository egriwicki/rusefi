/**
 * @file    main_trigger_callback.cpp
 * @brief   Main logic is here!
 *
 * See http://rusefi.com/docs/html/
 *
 * @date Feb 7, 2013
 * @author Andrey Belomutskiy, (c) 2012-2020
 *
 * This file is part of rusEfi - see http://rusefi.com
 *
 * rusEfi is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * rusEfi is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "pch.h"

#include "os_access.h"

#if EFI_PRINTF_FUEL_DETAILS
	bool printFuelDebug = false;
#endif // EFI_PRINTF_FUEL_DETAILS

#if EFI_ENGINE_CONTROL && EFI_SHAFT_POSITION_INPUT

#include "main_trigger_callback.h"
#include "trigger_central.h"
#include "spark_logic.h"
#include "advance_map.h"
#include "cyclic_buffer.h"
#include "fuel_math.h"
#include "cdm_ion_sense.h"
#include "tooth_logger.h"
#include "local_version_holder.h"
#include "event_queue.h"
#include "injector_model.h"
#if EFI_LAUNCH_CONTROL
#include "launch_control.h"
#endif

#include "backup_ram.h"

void startSimultaneousInjection(void*) {
	efitick_t nowNt = getTimeNowNt();
	for (size_t i = 0; i < engineConfiguration->specs.cylindersCount; i++) {
		enginePins.injectors[i].open(nowNt);
	}
}

static void endSimultaneousInjectionOnlyTogglePins() {
	efitick_t nowNt = getTimeNowNt();
	for (size_t i = 0; i < engineConfiguration->specs.cylindersCount; i++) {
		enginePins.injectors[i].close(nowNt);
	}
}

void endSimultaneousInjection(InjectionEvent *event) {
	event->isScheduled = false;
	endSimultaneousInjectionOnlyTogglePins();
	engine->injectionEvents.addFuelEventsForCylinder(event->ownIndex);
}

void InjectorOutputPin::open(efitick_t nowNt) {
	// per-output counter for error detection
	overlappingCounter++;
	// global counter for logging
	engine->engineState.fuelInjectionCounter++;

#if FUEL_MATH_EXTREME_LOGGING
	if (printFuelDebug) {
		printf("InjectorOutputPin::open %s %d now=%0.1fms\r\n", name, overlappingCounter, (int)getTimeNowUs() / 1000.0);
	}
#endif /* FUEL_MATH_EXTREME_LOGGING */

	if (overlappingCounter > 1) {
//		/**
//		 * #299
//		 * this is another kind of overlap which happens in case of a small duty cycle after a large duty cycle
//		 */
#if FUEL_MATH_EXTREME_LOGGING
		if (printFuelDebug) {
			printf("overlapping, no need to touch pin %s %d\r\n", name, (int)getTimeNowUs());
		}
#endif /* FUEL_MATH_EXTREME_LOGGING */
	} else {
#if EFI_TOOTH_LOGGER
	LogTriggerInjectorState(nowNt, true);
#endif // EFI_TOOTH_LOGGER
		setHigh();
	}
}

void turnInjectionPinHigh(InjectionEvent *event) {
	efitick_t nowNt = getTimeNowNt();
	for (int i = 0;i < MAX_WIRES_COUNT;i++) {
		InjectorOutputPin *output = event->outputs[i];

		if (output) {
			output->open(nowNt);
		}
	}
}

void InjectorOutputPin::close(efitick_t nowNt) {
#if FUEL_MATH_EXTREME_LOGGING
	if (printFuelDebug) {
		printf("InjectorOutputPin::close %s %d %d\r\n", name, overlappingCounter, (int)getTimeNowUs());
	}
#endif /* FUEL_MATH_EXTREME_LOGGING */

	overlappingCounter--;
	if (overlappingCounter > 0) {
#if FUEL_MATH_EXTREME_LOGGING
		if (printFuelDebug) {
			printf("was overlapping, no need to touch pin %s %d\r\n", name, (int)getTimeNowUs());
		}
#endif /* FUEL_MATH_EXTREME_LOGGING */
	} else {
#if EFI_TOOTH_LOGGER
	LogTriggerInjectorState(nowNt, false);
#endif // EFI_TOOTH_LOGGER
		setLow();
	}

	// Don't allow negative overlap count
	if (overlappingCounter < 0) {
		overlappingCounter = 0;
	}
}

void turnInjectionPinLow(InjectionEvent *event) {
	efitick_t nowNt = getTimeNowNt();

	engine->outputChannels.mostRecentTimeBetweenIgnitionEvents = nowNt - engine->mostRecentIgnitionEvent;
	engine->mostRecentIgnitionEvent = nowNt;

	event->isScheduled = false;
	for (int i = 0;i<MAX_WIRES_COUNT;i++) {
		InjectorOutputPin *output = event->outputs[i];
		if (output) {
			output->close(nowNt);
		}
	}
	engine->injectionEvents.addFuelEventsForCylinder(event->ownIndex);
}

void InjectionEvent::onTriggerTooth(int rpm, efitick_t nowNt, float currentPhase, float nextPhase) {
	auto eventAngle = injectionStartAngle;

	// Determine whether our angle is going to happen before (or near) the next tooth
	if (!isPhaseInRange(eventAngle, currentPhase, nextPhase)) {
		return;
	}

	// Select fuel mass from the correct cylinder
	auto injectionMassGrams = engine->injectionMass[this->cylinderNumber];

	// Perform wall wetting adjustment on fuel mass, not duration, so that
	// it's correct during fuel pressure (injector flow) or battery voltage (deadtime) transients
	injectionMassGrams = wallFuel.adjust(injectionMassGrams);
	const floatms_t injectionDuration = engine->module<InjectorModel>()->getInjectionDuration(injectionMassGrams);

#if EFI_PRINTF_FUEL_DETAILS
	if (printFuelDebug) {
		printf("fuel injectionDuration=%.2fms adjusted=%.2fms\n",
		  engine->engineState.injectionDuration,
		  injectionDuration);
	}
#endif /*EFI_PRINTF_FUEL_DETAILS */

	bool isCranking = engine->rpmCalculator.isCranking();
	/**
	 * todo: pre-calculate 'numberOfInjections'
	 * see also injectorDutyCycle
	 */
	int numberOfInjections = isCranking ? getNumberOfInjections(engineConfiguration->crankingInjectionMode) : getNumberOfInjections(engineConfiguration->injectionMode);
	if (injectionDuration * numberOfInjections > getEngineCycleDuration(rpm)) {
		warning(CUSTOM_TOO_LONG_FUEL_INJECTION, "Too long fuel injection %.2fms", injectionDuration);
	}

	engine->engineState.fuelConsumption.consumeFuel(injectionMassGrams * numberOfInjections, nowNt);

	if (this->cylinderNumber == 0) {
		engine->outputChannels.actualLastInjection = injectionDuration;
	}

	if (cisnan(injectionDuration)) {
		warning(CUSTOM_OBD_NAN_INJECTION, "NaN injection pulse");
		return;
	}
	if (injectionDuration < 0) {
		warning(CUSTOM_OBD_NEG_INJECTION, "Negative injection pulse %.2f", injectionDuration);
		return;
	}

	// If somebody commanded an impossibly short injection, do nothing.
	// Durations under 50us-ish aren't safe for the scheduler
	// as their order may be swapped, resulting in a stuck open injector
	// see https://github.com/rusefi/rusefi/pull/596 for more details
	if (injectionDuration < 0.050f)
	{
		return;
	}

	floatus_t durationUs = MS2US(injectionDuration);


	// we are ignoring low RPM in order not to handle "engine was stopped to engine now running" transition
/*
 * Wall Wetting would totally skip fuel on sudden deceleration a
	if (rpm > 2 * engineConfiguration->cranking.rpm) {
		const char *outputName = outputs[0]->name;
		if (engine->prevOutputName == outputName
				&& engineConfiguration->injectionMode != IM_SIMULTANEOUS
				&& engineConfiguration->injectionMode != IM_SINGLE_POINT) {
			warning(CUSTOM_OBD_SKIPPED_FUEL, "looks like skipped fuel event revCounter=%d %s", getRevolutionCounter(), outputName);
		}
		engine->prevOutputName = outputName;
	}
*/

#if EFI_PRINTF_FUEL_DETAILS
	if (printFuelDebug) {
		InjectorOutputPin *output = outputs[0];
		printf("handleFuelInjectionEvent fuelout %s injection_duration %dus engineCycleDuration=%.1fms\t\n", output->name, (int)durationUs,
				(int)MS2US(getCrankshaftRevolutionTimeMs(Sensor::getOrZero(SensorType::Rpm))) / 1000.0);
	}
#endif /*EFI_PRINTF_FUEL_DETAILS */

	if (isScheduled) {
#if EFI_PRINTF_FUEL_DETAILS
		if (printFuelDebug) {
			InjectorOutputPin *output = outputs[0];
			printf("handleFuelInjectionEvent still used %s now=%.1fms\r\n", output->name, (int)getTimeNowUs() / 1000.0);
		}
#endif /*EFI_PRINTF_FUEL_DETAILS */
		return; // this InjectionEvent is still needed for an extremely long injection scheduled previously
	}

	isScheduled = true;

	action_s startAction, endAction;
	// We use different callbacks based on whether we're running sequential mode or not - everything else is the same
	if (isSimultaneous) {
		startAction = startSimultaneousInjection;
		endAction = { &endSimultaneousInjection, this };
	} else {
		// sequential or batch
		startAction = { &turnInjectionPinHigh, this };
		endAction = { &turnInjectionPinLow, this };
	}

	float angleFromNow = eventAngle - currentPhase;
	if (angleFromNow < 0) {
		angleFromNow += engine->engineCycle;
	}

	efitick_t startTime = scheduleByAngle(&signalTimerUp, nowNt, angleFromNow, startAction);
	efitick_t turnOffTime = startTime + US2NT((int)durationUs);
	engine->executor.scheduleByTimestampNt("inj", &endOfInjectionEvent, turnOffTime, endAction);

#if EFI_UNIT_TEST
		printf("scheduling injection angle=%.2f/delay=%.2f injectionDuration=%.2f\r\n", angleFromNow, NT2US(startTime - nowNt), injectionDuration);
#endif
#if EFI_DEFAILED_LOGGING
	efiPrintf("handleFuel pin=%s eventIndex %d duration=%.2fms %d", outputs[0]->name,
			injEventIndex,
			injectionDuration,
			getRevolutionCounter());
	efiPrintf("handleFuel pin=%s delay=%.2f %d", outputs[0]->name, NT2US(startTime - nowNt),
			getRevolutionCounter());
#endif /* EFI_DEFAILED_LOGGING */
}

static void handleFuel(const bool limitedFuel, uint32_t trgEventIndex, int rpm, efitick_t nowNt, float currentPhase, float nextPhase) {
	ScopePerf perf(PE::HandleFuel);
	
	efiAssertVoid(CUSTOM_STACK_6627, getCurrentRemainingStack() > 128, "lowstck#3");
	efiAssertVoid(CUSTOM_ERR_6628, trgEventIndex < engine->engineCycleEventCount, "handleFuel/event index");

	if (trgEventIndex == 0) {
		engine->tpsAccelEnrichment.onEngineCycleTps();
	}

	if (limitedFuel) {
		return;
	}

	/**
	 * Injection events are defined by addFuelEvents() according to selected
	 * fueling strategy
	 */
	FuelSchedule *fs = &engine->injectionEvents;
	if (!fs->isReady) {
		fs->addFuelEvents();
	}

#if FUEL_MATH_EXTREME_LOGGING
	if (printFuelDebug) {
		efiPrintf("handleFuel ind=%d %d", trgEventIndex, getRevolutionCounter());
	}
#endif /* FUEL_MATH_EXTREME_LOGGING */

	fs->onTriggerTooth(rpm, nowNt, currentPhase, nextPhase);
}

/**
 * This is the main trigger event handler.
 * Both injection and ignition are controlled from this method.
 */
void mainTriggerCallback(uint32_t trgEventIndex, efitick_t edgeTimestamp, angle_t currentPhase, angle_t nextPhase) {
	ScopePerf perf(PE::MainTriggerCallback);

#if ! HW_CHECK_MODE
	if (hasFirmwareError()) {
		/**
		 * In case on a major error we should not process any more events.
		 * TODO: add 'pin shutdown' invocation somewhere - coils might be still open here!
		 */
		return;
	}
#endif // HW_CHECK_MODE

	int rpm = engine->rpmCalculator.getCachedRpm();
	if (rpm == 0) {
		// this happens while we just start cranking

		// todo: check for 'trigger->is_synchnonized?'
		// TODO: add 'pin shutdown' invocation somewhere - coils might be still open here!
		return;
	}
	if (rpm == NOISY_RPM) {
		warning(OBD_Crankshaft_Position_Sensor_A_Circuit_Malfunction, "noisy trigger");
		// TODO: add 'pin shutdown' invocation somewhere - coils might be still open here!
		return;
	}

	LimpState limitedFuelState = engine->limpManager.allowInjection();
	engine->outputChannels.fuelCutReason = (int8_t)limitedFuelState.reason;
	bool limitedFuel = !limitedFuelState.value;
	
	if (trgEventIndex == 0) {
		if (HAVE_CAM_INPUT()) {
			engine->triggerCentral.validateCamVvtCounters();
		}

		if (engine->triggerCentral.checkIfTriggerConfigChanged()) {
			engine->ignitionEvents.isReady = false; // we need to rebuild complete ignition schedule
			engine->injectionEvents.isReady = false;
			// moved 'triggerIndexByAngle' into trigger initialization (why was it invoked from here if it's only about trigger shape & optimization?)
			// see updateTriggerWaveform() -> prepareOutputSignals()

			// we need this to apply new 'triggerIndexByAngle' values
			engine->periodicFastCallback();
		}
	}

	/**
	 * For fuel we schedule start of injection based on trigger angle, and then inject for
	 * specified duration of time
	 */
	handleFuel(limitedFuel, trgEventIndex, rpm, edgeTimestamp, currentPhase, nextPhase);

	engine->module<TriggerScheduler>()->scheduleEventsUntilNextTriggerTooth(
		rpm, trgEventIndex, edgeTimestamp);

	/**
	 * For spark we schedule both start of coil charge and actual spark based on trigger angle
	 */
	onTriggerEventSparkLogic(trgEventIndex, rpm, edgeTimestamp, currentPhase, nextPhase);
}

// Check if the engine is not stopped or cylinder cleanup is activated
static bool isPrimeInjectionPulseSkipped() {
	if (!engine->rpmCalculator.isStopped())
		return true;
	return engineConfiguration->isCylinderCleanupEnabled && (Sensor::getOrZero(SensorType::Tps1) > CLEANUP_MODE_TPS);
}

/**
 * Prime injection pulse
 */
void PrimeController::onIgnitionStateChanged(bool ignitionOn) {
	if (!ignitionOn) {
		// don't prime on ignition-off
		return;
	}

	// First, we need a protection against 'fake' ignition switch on and off (i.e. no engine started), to avoid repeated prime pulses.
	// So we check and update the ignition switch counter in non-volatile backup-RAM
#if EFI_PROD_CODE
	uint32_t ignSwitchCounter = backupRamLoad(BACKUP_IGNITION_SWITCH_COUNTER);
#else /* EFI_PROD_CODE */
	uint32_t ignSwitchCounter = 0;
#endif /* EFI_PROD_CODE */

	// if we're just toying with the ignition switch, give it another chance eventually...
	if (ignSwitchCounter > 10)
		ignSwitchCounter = 0;
	// If we're going to skip this pulse, then save the counter as 0.
	// That's because we'll definitely need the prime pulse next time (either due to the cylinder cleanup or the engine spinning)
	if (isPrimeInjectionPulseSkipped())
		ignSwitchCounter = -1;
	// start prime injection if this is a 'fresh start'
	if (ignSwitchCounter == 0) {
		auto primeDelayMs = engineConfiguration->primingDelay * 1000;

		auto startTime = getTimeNowNt() + MS2NT(primeDelayMs);
		engine->executor.scheduleByTimestampNt("prime", &m_start, startTime, { PrimeController::onPrimeStartAdapter, this});
	} else {
		efiPrintf("Skipped priming pulse since ignSwitchCounter = %d", ignSwitchCounter);
	}
#if EFI_PROD_CODE
	// we'll reset it later when the engine starts
	backupRamSave(BACKUP_IGNITION_SWITCH_COUNTER, ignSwitchCounter + 1);
#endif /* EFI_PROD_CODE */
}

void PrimeController::onPrimeStart() {
	auto durationMs = getPrimeDuration();

	// Don't prime a zero-duration pulse
	if (durationMs <= 0) {
		efiPrintf("Skipped zero-duration priming pulse.");
		return;
	}

	efiPrintf("Firing priming pulse of %.2f ms", durationMs);

	auto endTime = getTimeNowNt() + MS2NT(durationMs);

	// Open all injectors, schedule closing later
	m_isPriming = true;
	startSimultaneousInjection();
	engine->executor.scheduleByTimestampNt("prime", &m_end, endTime, { onPrimeEndAdapter, this });
}

void PrimeController::onPrimeEnd() {
	endSimultaneousInjectionOnlyTogglePins();

	m_isPriming = false;
}

floatms_t PrimeController::getPrimeDuration() const {
	auto clt = Sensor::get(SensorType::Clt);

	// If the coolant sensor is dead, skip the prime. The engine will still start fine, but may take a little longer.
	if (!clt) {
		return 0;
	}

	auto primeMass = 
		0.001f *	// convert milligram to gram
		interpolate2d(clt.Value, engineConfiguration->primeBins, engineConfiguration->primeValues);

	return engine->module<InjectorModel>()->getInjectionDuration(primeMass);
}

void updatePrimeInjectionPulseState() {
	static bool counterWasReset = false;
	if (counterWasReset)
		return;

	if (!engine->rpmCalculator.isStopped()) {
#if EFI_PROD_CODE
		backupRamSave(BACKUP_IGNITION_SWITCH_COUNTER, 0);
#endif /* EFI_PROD_CODE */
		counterWasReset = true;
	}
}

#endif /* EFI_ENGINE_CONTROL */
