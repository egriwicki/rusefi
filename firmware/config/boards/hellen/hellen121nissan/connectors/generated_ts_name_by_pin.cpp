// auto-generated by PinoutLogic.java

#include "pch.h"

const char * getBoardSpecificPinName(brain_pin_e brainPin) {
	switch(brainPin) {
		case GPIOA_6: return "14 - Cam Left";
		case GPIOA_7: return "33 - Cam Right";
		case GPIOB_1: return "13 - Crank";
		case GPIOB_8: return "79 - Coil 6";
		case GPIOC_13: return "62 - Coil 1";
		case GPIOD_10: return "41 - INJ_4";
		case GPIOD_11: return "22 - INJ_3";
		case GPIOD_12: return "113 Fuel Pump Relay";
		case GPIOD_3: return "29 - VIAS";
		case GPIOD_9: return "21 - INJ_5";
		case GPIOE_2: return "60 - Coil 5";
		case GPIOE_3: return "80 - Coil 4";
		case GPIOE_4: return "61 - Coil 3";
		case GPIOE_5: return "81 - Coil 2";
		case GPIOF_10: return "102 - Power Steering Input";
		case GPIOF_12: return "40 - INJ_6";
		case GPIOF_13: return "10 - VTC Left";
		case GPIOF_14: return "11 - VTC Right";
		case GPIOF_9: return "101 - Brake Input";
		case GPIOG_14: return "111 Main Relay";
		case GPIOG_5: return "104 ETB Relay";
		case GPIOG_7: return "23 - INJ_1";
		case GPIOG_8: return "42 - INJ_2";
		default: return nullptr;
	}
	return nullptr;
}
