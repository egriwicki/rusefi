#include "pch.h"

#include "init.h"
#include "frequency_sensor.h"
#include "flex_sensor.h"

// 0.01 means filter bandwidth of ~1hz with ~100hz sensor
static FrequencySensor flexSensor(SensorType::FuelEthanolPercent, MS2NT(500), 0.01f);
static FlexConverter converter;

// https://rusefi.com/forum/viewtopic.php?p=37452&sid=829804c90d5b2e1fecd1b900cf1b1811#p37452

void initFlexSensor() {
	auto pin = engineConfiguration->flexSensorPin;

	// Nothing to do if no sensor configured
	if (!isBrainPinValid(pin)) {
		return;
	}

	flexSensor.setFunction(converter);
	flexSensor.init(pin);
	flexSensor.Register();
}

void deInitFlexSensor() {
	flexSensor.deInit();
}
