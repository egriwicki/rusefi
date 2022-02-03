// auto-generated by PinoutLogic.java

#include "pch.h"

const char * getBoardSpecificPinName(brain_pin_e brainPin) {
	switch(brainPin) {
		case GPIOC_1: return "2M - Pressure Sensor";
		case GPIOB_1: return "3E - CRANK";
		case GPIOA_1: return "2N - Temperature Sensor";
		case GPIOD_9: return "3S - A/C Fan 94-95";
		case GPIOE_5: return "1H - Ignition 2 & 3";
		case GPIOF_3: return "2L - Flex Fuel";
		case GPIOI_0: return "Tachometer";
		case GPIOG_2: return "Fuel Pump Relay";
		case GPIOC_5: return "1Q AC Switch";
		case GPIOC_4: return "1V - Clutch Switch";
		case GPIOA_6: return "3G - CAM";
		case GPIOI_8: return "1G - Ignition 1 & 4";
		case GPIOG_9: return "1E - Check Engine Light";
		case GPIOG_8: return "3V - Injector 2";
		case GPIOG_7: return "3U - Injector 1";
		case GPIOD_10: return "3Z - Injector 4";
		case GPIOH_15: return "1J - AC Relay";
		case GPIOD_12: return "Radiator Fan";
		case GPIOF_11: return "1M - VSS";
		case GPIOF_12: return "2J - Boost Control";
		case GPIOD_11: return "3Y - Injector 3";
		case GPIOD_14: return "3W - IDLE";
		default: return nullptr;
	}
	return nullptr;
}
