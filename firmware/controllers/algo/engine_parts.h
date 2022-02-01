/*
 * @file engine_parts.h
 *
 * @date May 27, 2019
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#pragma once

#include "cyclic_buffer.h"
#include "timer.h"

#define MOCK_ADC_SIZE 26

class MockAdcState {
public:
	MockAdcState();
	bool hasMockAdc[MOCK_ADC_SIZE];
	int fakeAdcValues[MOCK_ADC_SIZE];

	void setMockVoltage(int hwChannel, float voltage);
	int getMockAdcValue(int hwChannel) const;
};

class Accelerometer {
public:
	float x = 0; // G value
	float y = 0;
	float z = 0;
	float yaw = 0;
	float roll = 0;
};

class SensorsState {
public:
	SensorsState();

	Accelerometer accelerometer;
};

class FuelConsumptionState {
public:
	void consumeFuel(float grams, efitick_t nowNt);

	float getConsumedGrams() const;
	float getConsumptionGramPerSecond() const;

private:
	float m_consumedGrams = 0;
	float m_rate = 0;

	Timer m_timer;
};

class TransmissionState {
public:
	gear_e gearSelectorPosition;
};

typedef cyclic_buffer<int, 8> warningBuffer_t;

class WarningCodeState {
public:
	WarningCodeState();
	void addWarningCode(obd_code_e code);
	bool isWarningNow(efitimesec_t now, bool forIndicator) const;
	void clear();
	int warningCounter;
	int lastErrorCode;
	efitimesec_t timeOfPreviousWarning;
	// todo: we need a way to post multiple recent warnings into TS
	warningBuffer_t recentWarnings;
};

/**
 * 6 crossing over 50% TPS means pressing and releasing three times
 * TODO: looks like this code is not finished / not used?
 */
#define PUMPS_TO_PRIME 6

class StartupFuelPumping {
public:
	StartupFuelPumping();
	void update();
	bool isTpsAbove50;
	int pumpsCounter;
private:
	void setPumpsCounter(int newValue);
};

struct multispark_state
{
	efitick_t delay = 0;
	efitick_t dwell = 0;
	uint8_t count = 0;
};
