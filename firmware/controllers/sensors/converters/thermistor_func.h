#pragma once

#include "engine_configuration_generated_structures.h"
#include "sensor_converter_func.h"

class ThermistorFunc final : public SensorConverter {
public:
	SensorResult convert(float ohms) const override;

	void configure(thermistor_conf_s &cfg);

private:
	// Steinhart-Hart coefficients
	float m_a;
	float m_b;
	float m_c;
};
