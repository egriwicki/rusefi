#include "pch.h"

#include "limp_manager.h"

void LimpManager::updateState(int rpm, efitick_t nowNt) {
	Clearable allowFuel = engineConfiguration->isInjectionEnabled;
	Clearable allowSpark = engineConfiguration->isIgnitionEnabled;

	// User-configured hard RPM limit
	if (rpm > engineConfiguration->rpmHardLimit) {
		if (engineConfiguration->cutFuelOnHardLimit) {
			allowFuel.clear();
		}

		if (engineConfiguration->cutSparkOnHardLimit) {
			allowSpark.clear();
		}
	}

	// Force fuel limiting on the fault rev limit
	if (rpm > m_faultRevLimit) {
		allowFuel.clear();
	}

	// Limit fuel only on boost pressure (limiting spark bends valves)
	if (engineConfiguration->boostCutPressure != 0) {
		if (Sensor::getOrZero(SensorType::Map) > engineConfiguration->boostCutPressure) {
			allowFuel.clear();
		}
	}

	if (engine->rpmCalculator.isRunning()) {
		uint16_t minOilPressure = engineConfiguration->minOilPressureAfterStart;

		// Only check if the setting is enabled
		if (minOilPressure > 0) {
			// Has it been long enough we should have pressure?
			bool isTimedOut = engine->rpmCalculator.getTimeSinceEngineStart(nowNt) > 5.0f;

			// Only check before timed out
			if (!isTimedOut) {
				auto oilp = Sensor::get(SensorType::OilPressure);

				if (oilp) {
					// We had oil pressure! Set the flag.
					if (oilp.Value > minOilPressure) {
						m_hadOilPressureAfterStart = true;
					}
				}
			}

			// If time is up, the sensor works, and no pressure, kill the engine.
			if (isTimedOut && !m_hadOilPressureAfterStart) {
				allowFuel.clear();
			}
		}
	} else {
		// reset state in case of stalled engine
		m_hadOilPressureAfterStart = false;
	}

	if (engine->needToStopEngine(nowNt)) {
		/**
		 * todo: we need explicit clarification on why do we cut fuel but do not cut spark here!
		 */
		allowFuel.clear();
	}

	if (!engine->isMainRelayEnabled()) {
/*
todo AndreiKA this change breaks 22 unit tests?
		allowFuel.clear();
		allowSpark.clear();
*/
	}

	m_transientAllowInjection = allowFuel;
	m_transientAllowIgnition = allowSpark;
}

void LimpManager::etbProblem() {
	m_allowEtb.clear();
	setFaultRevLimit(1500);
}

void LimpManager::fatalError() {
	m_allowEtb.clear();
	m_allowIgnition.clear();
	m_allowInjection.clear();
	m_allowTriggerInput.clear();

	setFaultRevLimit(0);
}

void LimpManager::setFaultRevLimit(int limit) {
	// Only allow decreasing the limit
	// aka uses the limit of the worst fault to yet occur
	m_faultRevLimit = minI(m_faultRevLimit, limit);
}

bool LimpManager::allowElectronicThrottle() const {
	return m_allowEtb;
}

bool LimpManager::allowTriggerInput() const {
	return m_allowTriggerInput;
}

bool LimpManager::allowInjection() const {
	return m_transientAllowInjection && m_allowInjection;
}

bool LimpManager::allowIgnition() const {
	return m_transientAllowIgnition && m_allowIgnition;
}
