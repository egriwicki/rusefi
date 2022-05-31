//DO NOT EDIT MANUALLY, let automation work hard.

// auto-generated by PinoutLogic.java based on  config/boards/hellen/hellen88bmw/connectors/main.yaml
#include "pch.h"

// see comments at declaration in pin_repository.h
const char * getBoardSpecificPinName(brain_pin_e brainPin) {
	switch(brainPin) {
		case Gpio::A6: return "17 - CAM";
		case Gpio::B1: return "16 - CRANK";
		case Gpio::B8: return "24 - Coil 6";
		case Gpio::C13: return "50 - Coil 1";
		case Gpio::C9: return "29 - Idle";
		case Gpio::D10: return "5 - INJ_4";
		case Gpio::D11: return "31 - INJ_3";
		case Gpio::D12: return "2 - IdleC";
		case Gpio::D15: return "47 - Tach";
		case Gpio::D3: return "27 - MAIN";
		case Gpio::D9: return "3 - INJ_5";
		case Gpio::E2: return "25 - Coil 5";
		case Gpio::E3: return "23 - Coil 4";
		case Gpio::E4: return "52 - Coil 3";
		case Gpio::E5: return "51 - Coil 2";
		case Gpio::F12: return "4 - INJ_6";
		case Gpio::F13: return "7 - INJ_7/VVT";
		case Gpio::F14: return "35 - INJ_8";
		case Gpio::G14: return "1 - OUT_PUMP";
		case Gpio::G7: return "33 - INJ_1";
		case Gpio::G8: return "32 - INJ_2";
		default: return nullptr;
	}
	return nullptr;
}
