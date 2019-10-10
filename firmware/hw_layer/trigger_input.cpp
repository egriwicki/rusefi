/**
 * @file	trigger_input.cpp
 * @brief	Position sensor hardware layer (ICU and PAL drivers)
 *
 * todo: VVT implementation is a nasty copy-paste :(
 *
 * see digital_input_hw.cp
 *
 * @date Dec 30, 2012
 * @author Andrey Belomutskiy, (c) 2012-2018
 */

#include "global.h"

volatile int icuWidthCallbackCounter = 0;
volatile int icuWidthPeriodCounter = 0;
bool hwTriggerInputEnabled = true; // this is useful at least for real hardware integration testing

#if EFI_SHAFT_POSITION_INPUT && (HAL_TRIGGER_USE_PAL == TRUE) && (HAL_USE_COMP == FALSE)

#include "trigger_input.h"
#include "digital_input_exti.h"
#include "pin_repository.h"
#include "trigger_structure.h"
#include "trigger_central.h"
#include "engine_configuration.h"

#define TRIGGER_SUPPORTED_CHANNELS 2

extern bool hasFirmwareErrorFlag;

EXTERN_ENGINE
;
static Logging *logger;

#if EFI_PROD_CODE

/* static variables for PAL implementation */
static ioline_t primary_line;

static void shaft_callback(void *arg) {
	ioline_t pal_line = (ioline_t)arg;
	// todo: support for 3rd trigger input channel
	// todo: start using real event time from HW event, not just software timer?
	if (hasFirmwareErrorFlag)
		return;

	bool isPrimary = pal_line == primary_line;
	if (!isPrimary && !TRIGGER_SHAPE(needSecondTriggerInput)) {
		return;
	}

	bool rise = (palReadLine(pal_line) == PAL_HIGH);
	trigger_event_e signal;
	// todo: add support for 3rd channel
	if (rise) {
		signal = isPrimary ?
					(engineConfiguration->invertPrimaryTriggerSignal ? SHAFT_PRIMARY_FALLING : SHAFT_PRIMARY_RISING) :
					(engineConfiguration->invertSecondaryTriggerSignal ? SHAFT_SECONDARY_FALLING : SHAFT_SECONDARY_RISING);
	} else {
		signal = isPrimary ?
					(engineConfiguration->invertPrimaryTriggerSignal ? SHAFT_PRIMARY_RISING : SHAFT_PRIMARY_FALLING) :
					(engineConfiguration->invertSecondaryTriggerSignal ? SHAFT_SECONDARY_RISING : SHAFT_SECONDARY_FALLING);
	}

	hwHandleShaftSignal(signal);
}

static void cam_callback(void *arg) {
	ioline_t pal_line = (ioline_t)arg;

	bool rise = (palReadLine(pal_line) == PAL_HIGH);

	if (rise) {
		hwHandleVvtCamSignal(TV_RISE);
	} else {
		hwHandleVvtCamSignal(TV_FALL);
	}
}

static int turnOnTriggerInputPin(const char *msg, brain_pin_e brainPin, bool is_shaft) {
	scheduleMsg(logger, "turnOnTriggerInputPin(PAL) %s %s", msg, hwPortname(brainPin));

	/* TODO:
	 * * do not set to both edges if we need only one
	 * * simplify callback in case of one edge */
	ioline_t pal_line = PAL_LINE(getHwPort("trg", brainPin), getHwPin("trg", brainPin));
	return efiExtiEnablePin(msg, brainPin, PAL_EVENT_MODE_BOTH_EDGES, is_shaft ? shaft_callback : cam_callback, (void *)pal_line);
}

static void turnOffTriggerInputPin(brain_pin_e brainPin) {
	efiExtiDisablePin(brainPin);
}

static void setPrimaryChannel(brain_pin_e brainPin) {
	primary_line = PAL_LINE(getHwPort("trg", brainPin), getHwPin("trg", brainPin));
}

#endif /* EFI_PROD_CODE */

/*==========================================================================*/
/* Exported functions.														*/
/*==========================================================================*/

void turnOnTriggerInputPins(Logging *sharedLogger) {
	logger = sharedLogger;

	applyNewTriggerInputPins();
}

void stopTriggerInputPins(void) {
#if EFI_PROD_CODE
	for (int i = 0; i < TRIGGER_SUPPORTED_CHANNELS; i++) {
		if (CONFIGB(triggerInputPins)[i]
				!= activeConfiguration.bc.triggerInputPins[i]) {
			turnOffTriggerInputPin(activeConfiguration.bc.triggerInputPins[i]);
		}
	}
	for (int i = 0; i < CAM_INPUTS_COUNT; i++) {
		if (engineConfiguration->camInputs[i] != activeConfiguration.camInputs[i]) {
			turnOffTriggerInputPin(activeConfiguration.camInputs[i]);
		}
	}
#endif /* EFI_PROD_CODE */
}

void startTriggerInputPins(void) {
#if EFI_PROD_CODE
	for (int i = 0; i < TRIGGER_SUPPORTED_CHANNELS; i++) {
		if (CONFIGB(triggerInputPins)[i]
				!= activeConfiguration.bc.triggerInputPins[i]) {
			const char * msg = (i == 0 ? "trigger#1" : (i == 1 ? "trigger#2" : "trigger#3"));
			turnOnTriggerInputPin(msg, CONFIGB(triggerInputPins)[i], true);
		}
	}

	for (int i = 0; i < CAM_INPUTS_COUNT; i++) {
		if (engineConfiguration->camInputs[i] != activeConfiguration.camInputs[i]) {
			turnOnTriggerInputPin("cam", engineConfiguration->camInputs[i], false);
		}
	}

	setPrimaryChannel(CONFIGB(triggerInputPins)[0]);
#endif /* EFI_PROD_CODE */
}

void applyNewTriggerInputPins(void) {
	// first we will turn off all the changed pins
	stopTriggerInputPins();
	// then we will enable all the changed pins
	startTriggerInputPins();
}

#endif /* (EFI_SHAFT_POSITION_INPUT && (HAL_TRIGGER_USE_PAL == TRUE) && (HAL_USE_COMP == FALSE)) */
