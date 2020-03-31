
#pragma once

#include "stored_value_sensor.h"
#include "scaled_channel.h"
#include "hal.h"

class CanSensorBase : public StoredValueSensor {
public:
	CanSensorBase(uint32_t eid, SensorType type, efitick_t timeout)
		: StoredValueSensor(type, timeout)
		, m_eid(eid)
	{
	}

	void showInfo(Logging* logger, const char* sensorName) const override;

	CanSensorBase* processFrame(const CANRxFrame& frame, efitick_t nowNt) {
		if (frame.EID == m_eid) {
			decodeFrame(frame, nowNt);
		}

		return m_next;
	}

	void setNext(CanSensorBase* next) {
		m_next = next;
	}

protected:
	virtual void decodeFrame(const CANRxFrame& frame, efitick_t nowNt) = 0;

private:
	CanSensorBase* m_next = nullptr;
	const uint32_t m_eid;
};

template <typename TStorage, int TScale>
class CanSensor : public CanSensorBase {
public:
	CanSensor(uint32_t eid, uint8_t offset, SensorType type, efitick_t timeout)
		: CanSensorBase(eid, type, timeout)
		, m_offset(offset)
	{
	}

	void decodeFrame(const CANRxFrame& frame, efitick_t nowNt) override {
		float value = *reinterpret_cast<const scaled_channel<TStorage, TScale>*>(&frame.data8[m_offset]);
		setValidValue(value, nowNt);
	}

private:
	const uint8_t m_offset = 2;
};
