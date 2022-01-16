#pragma once

#include "can_sensor.h"

class AemXSeriesWideband final : public CanSensorBase {
public:
	AemXSeriesWideband(uint8_t sensorIndex, SensorType type);

protected:
	void decodeFrame(const CANRxFrame& frame, efitick_t nowNt) override;

private:
	const uint8_t m_sensorIndex;
};
