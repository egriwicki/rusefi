/**
 * @file    alternator_controller.cpp
 * @brief   alternator controller - some newer vehicles control alternator with ECU
 *
 * @date Apr 6, 2014
 * @author Dmitry Sidin
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#include "pch.h"

#if EFI_ALTERNATOR_CONTROL
#include "alternator_controller.h"
#include "pid.h"
#include "local_version_holder.h"
#include "periodic_task.h"

#if defined(HAS_OS_ACCESS)
#error "Unexpected OS ACCESS HERE"
#endif /* HAS_OS_ACCESS */

static SimplePwm alternatorControl("alt");
static Pid alternatorPid(&persistentState.persistentConfiguration.engineConfiguration.alternatorControl);

static percent_t currentAltDuty;

static bool currentPlainOnOffState = false;
static bool shouldResetPid = false;

static void pidReset() {
	alternatorPid.reset();
}

void AlternatorController::onFastCallback() {
	if (!isBrainPinValid(engineConfiguration->alternatorControlPin)) {
		return;
	}

#if ! EFI_UNIT_TEST
	if (shouldResetPid) {
		pidReset();
		shouldResetPid = false;
	}
#endif

	// this block could be executed even in on/off alternator control mode
	// but at least we would reflect latest state
#if EFI_TUNER_STUDIO
	alternatorPid.postState(&engine->outputChannels.alternatorStatus);
#endif /* EFI_TUNER_STUDIO */

	// todo: migrate this to FSIO
	bool alternatorShouldBeEnabledAtCurrentRpm = GET_RPM() > engineConfiguration->cranking.rpm;

	if (!engineConfiguration->isAlternatorControlEnabled || !alternatorShouldBeEnabledAtCurrentRpm) {
		// we need to avoid accumulating iTerm while engine is not running
		pidReset();

		// Shut off output if not needed
		alternatorControl.setSimplePwmDutyCycle(0);

		return;
	}

	auto vBatt = Sensor::get(SensorType::BatteryVoltage);
	float targetVoltage = engineConfiguration->targetVBatt;

	// todo: I am not aware of a SINGLE person to use this onOffAlternatorLogic
	if (engineConfiguration->onOffAlternatorLogic) {
		if (!vBatt) {
			// Somehow battery voltage isn't valid, disable alternator control
			enginePins.alternatorPin.setValue(false);
		}

		float h = 0.1;
		bool newState = (vBatt.Value < targetVoltage - h) || (currentPlainOnOffState && vBatt.Value < targetVoltage);
		enginePins.alternatorPin.setValue(newState);
		currentPlainOnOffState = newState;
#if EFI_TUNER_STUDIO
			engine->outputChannels.alternatorOnOff = newState;
#endif /* EFI_TUNER_STUDIO */

		return;
	}

	if (!vBatt) {
		// Somehow battery voltage isn't valid, disable alternator control
		alternatorPid.reset();
		alternatorControl.setSimplePwmDutyCycle(0);
	} else {
		currentAltDuty = alternatorPid.getOutput(targetVoltage, vBatt.Value, FAST_CALLBACK_PERIOD_MS / 1000.0f);
		if (engineConfiguration->isVerboseAlternator) {
			efiPrintf("alt duty: %.2f/vbatt=%.2f/p=%.2f/i=%.2f/d=%.2f int=%.2f", currentAltDuty, vBatt.Value,
					alternatorPid.getP(), alternatorPid.getI(), alternatorPid.getD(), alternatorPid.getIntegration());
		}

		alternatorControl.setSimplePwmDutyCycle(PERCENT_TO_DUTY(currentAltDuty));
	}
}

void showAltInfo(void) {
	efiPrintf("alt=%s @%s t=%dms", boolToString(engineConfiguration->isAlternatorControlEnabled),
			hwPortname(engineConfiguration->alternatorControlPin),
			engineConfiguration->alternatorControl.periodMs);
	efiPrintf("p=%.2f/i=%.2f/d=%.2f offset=%.2f", engineConfiguration->alternatorControl.pFactor,
			0, 0, engineConfiguration->alternatorControl.offset); // todo: i & d
	efiPrintf("vbatt=%.2f/duty=%.2f/target=%.2f", Sensor::getOrZero(SensorType::BatteryVoltage), currentAltDuty,
			engineConfiguration->targetVBatt);
}

void setAltPFactor(float p) {
	engineConfiguration->alternatorControl.pFactor = p;
	efiPrintf("setAltPid: %.2f", p);
	pidReset();
	showAltInfo();
}

void onConfigurationChangeAlternatorCallback(engine_configuration_s *previousConfiguration) {
	shouldResetPid = !alternatorPid.isSame(&previousConfiguration->alternatorControl);
}

void initAlternatorCtrl() {
	addConsoleAction("altinfo", showAltInfo);
	if (!isBrainPinValid(engineConfiguration->alternatorControlPin))
		return;

	if (!engineConfiguration->onOffAlternatorLogic) {
		startSimplePwm(&alternatorControl,
				"Alternator control",
				&engine->executor,
				&enginePins.alternatorPin,
				engineConfiguration->alternatorPwmFrequency, 0);
	}
}

// todo: start invoking this method like 'startVvtControlPins'
void startAlternatorPin(void) {

}

void stopAlternatorPin(void) {
	// todo: implementation!
}

#endif /* EFI_ALTERNATOR_CONTROL */
