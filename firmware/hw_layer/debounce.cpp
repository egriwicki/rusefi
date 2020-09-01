/**
 * @file        debounce.cpp
 * @brief       Generic button debounce class
 *
 * @date Aug 31, 2020
 * @author David Holdeman, (c) 2020
 */
#include "debounce.h"

void ButtonDebounce::init (int t, engine_configuration_s p, engine_configuration_s m) {
    threshold = MS2NT(t);
    timeLast = 0;
    pin = CONFIG(p);
#ifdef PAL_MODE_INPUT_PULLDOWN
    // getInputMode converts from pin_input_mode_e to iomode_t
    mode = getInputMode(CONFIG(m));
    efiSetPadMode("Button", p, mode);
#endif
}

bool ButtonDebounce::readPinEvent() {
    if (!pin) {
        return false;
    }
    efitick_t timeNow = getTimeNowNt();
    // If it's been less than the threshold since we were last called
    if ((timeNow - timeLast) < threshold) {
        return false;
    }
    // readValue is a class variable, so it needs to be reset.
    // We don't actually need it to be a class variable in this method,
    //  but when a method is implemented to actually get the pin's state,
    //  for example to implement long button presses, it will be needed.
    readValue = false;
#ifdef PAL_MODE_INPUT_PULLDOWN
    readValue = efiReadPin(pin);
    // Invert
    if (mode != PAL_MODE_INPUT_PULLDOWN) {
        readValue = !readValue;
    }
#endif
    if (readValue) {
        timeLast = timeNow;
    }
    return readValue;
}
