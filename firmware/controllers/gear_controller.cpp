#include "pch.h"

#include "gear_controller.h"
#include "simple_tcu.h"

void GearControllerBase::init() {
	switch (engineConfiguration->transmissionController) {
	case TC_SIMPLE_TRANSMISSION_CONTROLLER :
		transmissionController = &simpleTransmissionController;
		break;
	default :
		return;
	}
	transmissionController->init();
}

void GearControllerBase::update() {
    // We are responsible for telling the transmission controller
    //  what gear we want.
    transmissionController->update(getDesiredGear());
    // Post state to TS
    postState();
}

gear_e GearControllerBase::getDesiredGear() const {
    return desiredGear;
}

gear_e GearControllerBase::setDesiredGear(gear_e gear) {
    desiredGear = gear;
    return getDesiredGear();
}

void GearControllerBase::postState() {
#if EFI_TUNER_STUDIO
    engine->outputChannels.tcuDesiredGear = getDesiredGear();
#endif
}

void initGearController() {
	switch (engineConfiguration->gearController) {
	case GC_BUTTON_SHIFT :
		engine->gearController = &buttonShiftController;
		break;
	default :
		return;
	}
	engine->gearController.init();
}
