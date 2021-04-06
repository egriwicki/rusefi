
#pragma once

#include "can_sensor.h"

class EGTtoCAN final : public CanListener {
public:
	EGTtoCAN(uint8_t EGTIndex, SensorType type);

protected:
	void decodeFrame(const CANRxFrame& frame, efitick_t nowNt) override;
};
