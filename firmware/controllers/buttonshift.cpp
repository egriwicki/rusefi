#include "buttonshift.h"

void ButtonShiftController::update() {
    bool upPinState = debounceUp.readPin();
    bool downPinState = debounceUp.readPin();
    gear_e gear = getDesiredGear();
    if (upPinState) {
        switch (gear) {
            case GEAR_1:
                setDesiredGear(GEAR_2);
                break;
            case GEAR_2:
                setDesiredGear(GEAR_3);
                break;
            case GEAR_3:
                setDesiredGear(GEAR_4);
                break;
            default:
                break;
        }
    } else if (downPinState) {
        switch (gear) {
            case GEAR_2:
                setDesiredGear(GEAR_1);
                break;
            case GEAR_3:
                setDesiredGear(GEAR_2);
                break;
            case GEAR_4:
                setDesiredGear(GEAR_3);
                break;
            default:
                break;
        }
    }
    postState(&tsOutputChannels);
}
