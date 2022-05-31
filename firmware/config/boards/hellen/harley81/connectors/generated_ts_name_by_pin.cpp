//DO NOT EDIT MANUALLY, let automation work hard.

// auto-generated by PinoutLogic.java based on  config/boards/hellen/harley81/connectors/harley81.yaml
#include "pch.h"

// see comments at declaration in pin_repository.h
const char * getBoardSpecificPinName(brain_pin_e brainPin) {
	switch(brainPin) {
		case Gpio::C13: return "24 Front Coil";
		case Gpio::C7: return "auxWastegate";
		case Gpio::D14: return "4 Left Oil Fan";
		case Gpio::D15: return "1 Right Oil Cooling";
		case Gpio::D2: return "25 Front ACR";
		case Gpio::E5: return "43 Rear Coil";
		case Gpio::F11: return "51 - VSS";
		case Gpio::F6: return "11 Crank VR-";
		case Gpio::G11: return "8 Rear ACR";
		case Gpio::G14: return "61 Reverse Gear";
		case Gpio::G3: return "auxMain";
		case Gpio::G5: return "63 Cooling Pump";
		case Gpio::G7: return "6 Front Injector";
		case Gpio::G8: return "7 Rear Injector";
		default: return nullptr;
	}
	return nullptr;
}
