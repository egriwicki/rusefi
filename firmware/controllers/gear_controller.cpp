#include "gear_controller.h"
#include "tunerstudio_outputs.h"
#include "adc_inputs.h"
#include "engine_configuration.h"

EXTERN_CONFIG

void GearControllerBase::init(DECLARE_ENGINE_PARAMETER_SIGNATURE) {
    INJECT_ENGINE_REFERENCE(&transmissionController);
    transmissionController.init();
}

void GearControllerBase::update() {
    // We are responsible for telling the transmission controller
    //  what gear we want.
    transmissionController.update(getDesiredGear());
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

int GearControllerBase::getInputRole() {
    if (CONFIG(tcuInputMode)) {
        return getInputRoleD();
    } else {
        return getInputRoleA();
    }
}

int GearControllerBase::getInputRoleD() {
    bool states[TCU_INPUT_COUNT];
    for (int i = 0; i < TCU_INPUT_COUNT; i++) {
        states[i] = tcuDigitalInputs[i].readPinState();
    }
}

int GearControllerBase::getInputRoleA() {
    int levels[TCU_INPUT_COUNT];
    for (int i = 0; i < TCU_INPUT_COUNT; i++) {
        levels[i] = adcToVolts(getAdcValue("tcuinput", tcuAnalogInputs[i]));
    }
}

brain_pin_e GearControllerBase::getDigitalInputPin(int input) {
    switch (input) {
        case 1:
            return CONFIG(tcuInputPin1D)
        case 2:
            return CONFIG(tcuInputPin2D)
        case 3:
            return CONFIG(tcuInputPin3D)
        case 4:
            return CONFIG(tcuInputPin4D)
        case 5:
            return CONFIG(tcuInputPin5D)
        case 6:
            return CONFIG(tcuInputPin6D)
        case 7:
            return CONFIG(tcuInputPin7D)
        case 8:
            return CONFIG(tcuInputPin8D)
    }
}

adc_channel_e GearControllerBase::getAnalogInputPin(int input) {
    switch (input) {
        case 1:
            return CONFIG(tcuInputPin1A)
        case 2:
            return CONFIG(tcuInputPin2A)
        case 3:
            return CONFIG(tcuInputPin3A)
        case 4:
            return CONFIG(tcuInputPin4A)
        case 5:
            return CONFIG(tcuInputPin5A)
        case 6:
            return CONFIG(tcuInputPin6A)
        case 7:
            return CONFIG(tcuInputPin7A)
        case 8:
            return CONFIG(tcuInputPin8A)
    }
}

void GearControllerBase::postState() {
#if EFI_TUNER_STUDIO
    tsOutputChannels.tcuDesiredGear = getDesiredGear();
#endif
}

