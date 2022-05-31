//DO NOT EDIT MANUALLY, let automation work hard.

// auto-generated by PinoutLogic.java based on  config/boards/hellen/alphax-2chan/connectors/main.yaml
#include "pch.h"

// see comments at declaration in pin_repository.h
const char * getBoardSpecificPinName(brain_pin_e brainPin) {
	switch(brainPin) {
		case Gpio::A6: return "C7 - CAM Hall/Digital";
		case Gpio::B1: return "C2/C3 Crank Sensor VR";
		case Gpio::C13: return "A6 - Ignition A";
		case Gpio::C6: return "A3 - Fuel Pump";
		case Gpio::C7: return "A4 - Tachometer";
		case Gpio::C8: return "C1 - Fan Relay";
		case Gpio::D13: return "C8 - Idle";
		case Gpio::E12: return "C3 - Crank+ (hall)";
		case Gpio::E14: return "C2 - Crank- (hall)";
		case Gpio::E5: return "A7 - Ignition B";
		case Gpio::F11: return "C4 - 2Step/Digital";
		case Gpio::G7: return "A8 - Injector A";
		case Gpio::G8: return "B8 - Injector B";
		default: return nullptr;
	}
	return nullptr;
}
