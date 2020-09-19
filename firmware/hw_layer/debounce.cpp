/**
 * @file        debounce.cpp
 * @brief       Generic button debounce class
 *
 * @date Aug 31, 2020
 * @author David Holdeman, (c) 2020
 */
#include "debounce.h"
#include "pin_repository.h"
#include "engine_configuration.h"
#include "hardware.h"

ButtonDebounce* ButtonDebounce::s_firstDebounce = nullptr;

/**
We need to have a separate init function because we do not have the pin or mode in the context in which the class is originally created
*/
void ButtonDebounce::init (efitimems_t threshold, brain_pin_e *pin, pin_input_mode_e *mode) {
   // we need to keep track of whether we have already been initialized due to the way unit tests run.
    if (!initialized) {
	// Link us to the list that is used to track ButtonDebounce instances, so that when the configuration changes,
	//  they can be looped through and updated.
        nextDebounce = s_firstDebounce;
        s_firstDebounce = this;
    }
    m_threshold = MS2NT(threshold);
    timeLast = 0;
    m_pin = pin;
    active_pin = *pin;
    m_mode = mode;
    active_mode = *mode;
#ifndef EFI_UNIT_TEST
    // getInputMode converts from pin_input_mode_e to iomode_t
    efiSetPadMode("Button", active_pin, getInputMode(active_mode));
#endif
    initialized = true;
}

void ButtonDebounce::stopConfigurationList () {
    ButtonDebounce *listItem = s_firstDebounce;
    while (listItem != nullptr) {
        listItem->stopConfiguration();
        if (listItem->nextDebounce != nullptr) {
            listItem = listItem->nextDebounce;
        } else {
            break;
        }
    }
}

void ButtonDebounce::startConfigurationList () {
    ButtonDebounce *listItem = s_firstDebounce;
    while (listItem != nullptr) {
        listItem->startConfiguration();
        if (listItem->nextDebounce != nullptr) {
            listItem = listItem->nextDebounce;
        } else {
            break;
        }
    }
}

void ButtonDebounce::stopConfiguration () {
    // If the configuration has changed
#ifndef EFI_ACTIVE_CONFIGURATION_IN_FLASH
    if (*m_pin != active_pin || *m_mode != active_mode) {
#else
    if (*m_pin != active_pin || *m_mode != active_mode || (isActiveConfigurationVoid && (*m_pin != 0 || *m_mode != 0))) {
#endif /* EFI_ACTIVE_CONFIGURATION_IN_FLASH */
#ifndef EFI_UNIT_TEST
        brain_pin_markUnused(active_pin);
#endif /* EFI_UNIT_TEST */
    }
}

void ButtonDebounce::startConfiguration () {
#ifndef EFI_UNIT_TEST
    efiSetPadMode("Button", *m_pin, getInputMode(*m_mode));
#endif
    active_pin = *m_pin;
    active_mode = *m_mode;
}

/**
@returns true if the button is pressed, and will not return true again within the set timeout
*/
bool ButtonDebounce::readPinEvent() {
    storedValue = false;
    return readPinState();
}

bool ButtonDebounce::readPinState() {
    if (!m_pin) {
        return false;
    }
    efitick_t timeNow = getTimeNowNt();
    // If it's been less than the threshold since we were last called
    if ((timeNow - timeLast) < m_threshold) {
        return storedValue;
    }
    // storedValue is a class variable, so it needs to be reset.
    // We don't actually need it to be a class variable in this method,
    //  but when a method is implemented to actually get the pin's state,
    //  for example to implement long button presses, it will be needed.
    storedValue = false;
#if EFI_PROD_CODE || EFI_UNIT_TEST
    storedValue = efiReadPin(active_pin);
#endif
#if EFI_PROD_CODE
    // Invert
    if (getInputMode(active_mode) == PAL_MODE_INPUT_PULLUP) {
        storedValue = !storedValue;
    }
#endif
    if (storedValue) {
        timeLast = timeNow;
    }
    return storedValue;
}
