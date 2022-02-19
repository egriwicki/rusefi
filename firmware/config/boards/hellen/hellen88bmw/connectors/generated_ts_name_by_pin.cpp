// auto-generated by PinoutLogic.java

#include "pch.h"

const char * getBoardSpecificPinName(brain_pin_e brainPin) {
	switch(brainPin) {
		case GPIOB_1: return "16 - CRANK";
		case GPIOD_9: return "3 - INJ_5";
		case GPIOC_9: return "29 - Idle";
		case GPIOB_8: return "24 - Coil 6";
		case GPIOE_5: return "51 - Coil 2";
		case GPIOE_4: return "52 - Coil 3";
		case GPIOE_3: return "23 - Coil 4";
		case GPIOA_6: return "17 - CAM";
		case GPIOE_2: return "25 - Coil 5";
		case GPIOD_3: return "27 - MAIN";
		case GPIOG_8: return "32 - INJ_2";
		case GPIOG_7: return "33 - INJ_1";
		case GPIOG_14: return "1 - OUT_PUMP";
		case GPIOC_13: return "50 - Coil 1";
		case GPIOD_10: return "5 - INJ_4";
		case GPIOF_13: return "7 - INJ_7/VVT";
		case GPIOF_14: return "35 - INJ_8";
		case GPIOD_12: return "2 - IdleC";
		case GPIOF_12: return "4 - INJ_6";
		case GPIOD_11: return "31 - INJ_3";
		case GPIOD_15: return "47 - Tach";
		default: return nullptr;
	}
	return nullptr;
}
