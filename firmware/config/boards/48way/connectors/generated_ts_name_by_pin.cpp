//DO NOT EDIT MANUALLY, let automation work hard.

// auto-generated by PinoutLogic.java based on  config/boards/48way/connectors/main.yaml
#include "pch.h"

// see comments at declaration in pin_repository.h
const char * getBoardSpecificPinName(brain_pin_e brainPin) {
	switch(brainPin) {
		case Gpio::A1: return "TPS Reference Input (A1)";
		case Gpio::B14: return "Ignition 4 Output (B14)";
		case Gpio::B15: return "Ignition 3 Output (B15)";
		case Gpio::C13: return "CrankShaft Input (C13)";
		case Gpio::C6: return "Idle1 Output (C6)";
		case Gpio::C7: return "Boost Controller Output (C7)";
		case Gpio::D10: return "Ignition 8 Output (D10)";
		case Gpio::D11: return "Ignition 7 Output (D11)";
		case Gpio::D12: return "Ignition 1 Output (D12)";
		case Gpio::D13: return "Ignition 2 Output (D13)";
		case Gpio::D14: return "HC2 Output (D14)";
		case Gpio::D15: return "HC1 Output (D15)";
		case Gpio::D8: return "Ignition 5 Output (D8)";
		case Gpio::D9: return "Ignition 6 Output (D9)";
		case Gpio::E1: return "Clutch Input (E1)";
		case Gpio::E10: return "Injector 6 Output (E10)";
		case Gpio::E11: return "Injector 5 Output (E11)";
		case Gpio::E12: return "Injector 4 Output (E12)";
		case Gpio::E13: return "Injector 3 Output (E13)";
		case Gpio::E14: return "Injector 2 Output (E14)";
		case Gpio::E15: return "Injector 1 Output (E15)";
		case Gpio::E2: return "Fan Relay (E2)";
		case Gpio::E3: return "Fuel Relay (E3)";
		case Gpio::E4: return "Launch Control (E4)";
		case Gpio::E5: return "Tachometer (E5)";
		case Gpio::E6: return "Camshaft Input (E6)";
		case Gpio::E8: return "Injector 8 Output (E8)";
		case Gpio::E9: return "Injector 7 Output (E9)";
		default: return nullptr;
	}
	return nullptr;
}
