//DO NOT EDIT MANUALLY, let automation work hard.

// auto-generated by PinoutLogic.java based on config/boards/hellen/hellen121nissan/connectors/121nissanAUX.yaml
// auto-generated by PinoutLogic.java based on config/boards/hellen/hellen121nissan/connectors/main.yaml
#include "pch.h"

// see comments at declaration in pin_repository.h
const char * getBoardSpecificPinName(brain_pin_e brainPin) {
	switch(brainPin) {
		case Gpio::A6: return "14 - Cam Left";
		case Gpio::A7: return "33 - Cam Right";
		case Gpio::B1: return "13 - Crank";
		case Gpio::B8: return "79 - Coil 6";
		case Gpio::C13: return "62 - Coil 1";
		case Gpio::C6: return "8 Aux Low 1";
		case Gpio::C7: return "3 Aux Low 2";
		case Gpio::C8: return "2 Aux Low 3";
		case Gpio::D10: return "41 - INJ_4";
		case Gpio::D11: return "22 - INJ_3";
		case Gpio::D12: return "113 Fuel Pump Relay";
		case Gpio::D3: return "29 - VIAS";
		case Gpio::D9: return "21 - INJ_5";
		case Gpio::E2: return "60 - Coil 5";
		case Gpio::E3: return "80 - Coil 4";
		case Gpio::E4: return "61 - Coil 3";
		case Gpio::E5: return "81 - Coil 2";
		case Gpio::F10: return "102 - Power Steering Input";
		case Gpio::F12: return "40 - INJ_6";
		case Gpio::F13: return "10 - VTC Left";
		case Gpio::F14: return "11 - VTC Right";
		case Gpio::F9: return "101 - Brake Input";
		case Gpio::G14: return "111 Main Relay";
		case Gpio::G5: return "104 ETB Relay";
		case Gpio::G7: return "23 - INJ_1";
		case Gpio::G8: return "42 - INJ_2";
		default: return nullptr;
	}
	return nullptr;
}
