/**
 * @file frequency_sensor.cpp
 */

#include "pch.h"

#include "frequency_sensor.h"

#include "digital_input_exti.h"

// Callback adapter since we can't pass a member function to a callback
static void freqSensorExtiCallback(void* arg) {
	auto inst = reinterpret_cast<FrequencySensor*>(arg);
	inst->onEdge(getTimeNowNt());
}

void FrequencySensor::init(const char* const msg) {
#if EFI_PROD_CODE
	// todo: refactor https://github.com/rusefi/rusefi/issues/2123
	efiExtiEnablePin(msg, m_pin, 
		PAL_EVENT_MODE_FALLING_EDGE,
		freqSensorExtiCallback, reinterpret_cast<void*>(this));
#endif // EFI_PROD_CODE
}

void FrequencySensor::deInit() {
#if EFI_PROD_CODE
	efiExtiDisablePin(m_pin);
#endif

	m_pin = GPIO_UNASSIGNED;
}

void FrequencySensor::onEdge(efitick_t nowNt) {
	float frequency = 1 / m_edgeTimer.getElapsedSecondsAndReset(nowNt);

	postRawValue(frequency, nowNt);
}

brain_pin_e FrequencySensor::getPin(void)
{
	return m_pin;
}
