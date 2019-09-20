#include "converter_sensor.h"
#include "rusefi_hw_enums.h"

class AdcSubscription {
public:
	static void SubscribeSensor(ConvertedSensor& sensor, adc_channel_e channel, float voltsPerAdcVolt = 0.0f);
	static void UpdateSubscribers();
};
