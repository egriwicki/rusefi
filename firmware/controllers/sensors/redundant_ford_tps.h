#pragma once

#include "sensor.h"

class RedundantFordTps final : public Sensor
{
public:
	RedundantFordTps(
		SensorType outputType,
		SensorType firstSensor,
		SensorType secondSensor
	);

	void configure(float maxDifference);

	SensorResult get() const override;

	bool isRedundant() const override {
		return true;
	}

	void showInfo(Logging* logger, const char* sensorName) const override;

private:
	// The two sensors we interpret to form one redundant sensor
	const SensorType m_first;
	const SensorType m_second;

	// How far apart do we allow the sensors to be before reporting an issue?
	float m_maxDifference = 0;
};
