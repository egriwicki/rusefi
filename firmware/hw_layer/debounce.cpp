#include "debounce.h"
#include "io_pins.h"

ButtonDebounce::ButtonDebounce (int t, brain_pin_e p, iomode_t m) {
    threshold = t;
    timeLast = 0;
    pin = p;
    mode = m;
    efiSetPadMode("Button", p, m);
}

bool ButtonDebounce::readPinEvent() {
    efitimems_t timeNow = currentTimeMillis();
    if ((timeNow - timeLast) < threshold) {
        return false;
    }
    timeLast = timeNow;
    readValue = false;
    readValue = efiReadPin(pin);
    if (mode != PAL_MODE_INPUT_PULLDOWN) {
        return !readValue;
    }
    return readValue;
}
