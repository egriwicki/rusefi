#include "ac_control.h"
#include "engine.h"

#include "deadband.h"
#include "efi_gpio.h"
#include "sensor.h"

EXTERN_ENGINE;

static Deadband<200> maxRpmDeadband;
static Deadband<5> maxCltDeadband;
static Deadband<5> maxTpsDeadband;

static bool getAcState(DECLARE_ENGINE_PARAMETER_SIGNATURE) {
	auto rpm = Sensor::get(SensorType::Rpm).value_or(0);

	// Engine too slow, disable
	if (rpm < 500) {
		return false;
	}

	// Engine too fast, disable
	auto maxRpm = CONFIG(maxAcRpm);
	if (maxRpm != 0) {
		if (maxRpmDeadband.gt(rpm, maxRpm)) {
			return false;
		}
	}

	auto clt = Sensor::get(SensorType::Clt);

	// No AC with failed CLT
	if (!clt) {
		return false;
	}

	// Engine too hot, disable
	auto maxClt = CONFIG(maxAcClt);
	if (maxClt != 0) {
		if (maxCltDeadband.gt(maxClt, clt.Value)) {
			return false;
		}
	}

	// TPS too high, disable
	auto maxTps = CONFIG(maxAcTps);
	if (maxTps != 0) {
		auto tps = Sensor::get(SensorType::Tps1).value_or(0);

		if (maxTpsDeadband.gt(maxTps, tps)) {
			return false;
		}
	}

	// All conditions allow AC, simply pass thru switch
	return ENGINE(acSwitchState);
}

bool updateAc(DECLARE_ENGINE_PARAMETER_SIGNATURE) {
	bool isEnabled = getAcState(PASS_ENGINE_PARAMETER_SIGNATURE);

	enginePins.acRelay.setValue(isEnabled);

	return isEnabled;
}
