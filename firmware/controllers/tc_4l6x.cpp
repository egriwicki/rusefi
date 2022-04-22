#include "pch.h"

#include "tc_4l6x.h"

Gm4l6xTransmissionController gm4l6xTransmissionController;
static SimplePwm tccPwm("TCC Control"); 
static SimplePwm pcPwm("Pressure Control"); 
static SimplePwm shift32Pwm("3-2 Shift Control"); 

void Gm4l6xTransmissionController::init() {
    for (size_t i = 0; i < efi::size(engineConfiguration->tcu_solenoid); i++) {
        enginePins.tcuSolenoids[i].initPin("Transmission Solenoid", engineConfiguration->tcu_solenoid[i], &engineConfiguration->tcu_solenoid_mode[i]);
    }
		enginePins.tcuTccOnoffSolenoid.initPin("TCC On/Off Solenoid", engineConfiguration->tcu_tcc_onoff_solenoid, &engineConfiguration->tcu_tcc_onoff_solenoid_mode);
		enginePins.tcuTccPwmSolenoid.initPin("TCC PWM Solenoid", engineConfiguration->tcu_tcc_pwm_solenoid, &engineConfiguration->tcu_tcc_pwm_solenoid_mode);
		startSimplePwm(&tccPwm,
									 "TCC",
									 &engine->executor,
									 &enginePins.tcuTccPwmSolenoid,
									 engineConfiguration->tcu_tcc_pwm_solenoid_freq,
									 0);
		enginePins.tcuPcSolenoid.initPin("Pressure Control Solenoid", engineConfiguration->tcu_pc_solenoid_pin, &engineConfiguration->tcu_pc_solenoid_pin_mode);
		startSimplePwm(&pcPwm,
									 "Line Pressure",
									 &engine->executor,
									 &enginePins.tcuPcSolenoid,
									 engineConfiguration->tcu_pc_solenoid_freq,
									 0);
		enginePins.tcu32Solenoid.initPin("3-2 Shift Solenoid", engineConfiguration->tcu_32_solenoid_pin, &engineConfiguration->tcu_32_solenoid_pin_mode);
		startSimplePwm(&shift32Pwm,
									 "3-2 Solenoid",
									 &engine->executor,
									 &enginePins.tcu32Solenoid,
									 engineConfiguration->tcu_32_solenoid_freq,
									 0);
}

void Gm4l6xTransmissionController::update(gear_e gear) {
	setCurrentGear(gear);
	setTccState();
	setPcState(gear);

#if EFI_TUNER_STUDIO
	if (engineConfiguration->debugMode == DBG_TCU) {
		engine->outputChannels.debugIntField1 = config->tcuSolenoidTable[static_cast<int>(gear) + 1][0];
		engine->outputChannels.debugIntField2 = config->tcuSolenoidTable[static_cast<int>(gear) + 1][1];
		engine->outputChannels.debugIntField3 = config->tcuSolenoidTable[static_cast<int>(gear) + 1][2];
		engine->outputChannels.debugIntField4 = config->tcuSolenoidTable[static_cast<int>(gear) + 1][3];
		engine->outputChannels.debugIntField5 = config->tcuSolenoidTable[static_cast<int>(gear) + 1][4];
	}
#endif

	TransmissionControllerBase::update(gear);

	float time = isShiftCompleted();
	if (time != 0) {
		engine->outputChannels.lastShiftTime = time;
		// TODO set pressure control back to normal
	}
}

gear_e Gm4l6xTransmissionController::setCurrentGear(gear_e gear) {
	if (gear == getCurrentGear()) {
		return getCurrentGear();
	}
	currentGear = gear;
	enginePins.tcuTccOnoffSolenoid.setValue(0);
	for (size_t i = 0; i < efi::size(engineConfiguration->tcu_solenoid); i++) {
#if ! EFI_UNIT_TEST
		enginePins.tcuSolenoids[i].setValue(config->tcuSolenoidTable[static_cast<int>(gear) + 1][i]);
#endif
	}
	measureShiftTime(gear);
	return getCurrentGear();
}

void Gm4l6xTransmissionController::setTccState() {
	auto tps = Sensor::get(SensorType::DriverThrottleIntent);
	auto vss = Sensor::get(SensorType::VehicleSpeed);
	if (!tps.Valid || !vss.Valid) {
		return;
	}
	if (getCurrentGear() == GEAR_4) {
		int lockSpeed = interpolate2d(tps.Value, config->tcu_tccTpsBins, config->tcu_tccLockSpeed);
		int unlockSpeed = interpolate2d(tps.Value, config->tcu_tccTpsBins, config->tcu_tccUnlockSpeed);
		if (vss.Value > lockSpeed) {
			enginePins.tcuTccOnoffSolenoid.setValue(1);
		} else if (vss.Value < unlockSpeed) {
			enginePins.tcuTccOnoffSolenoid.setValue(1);
		}
	} else {
		enginePins.tcuTccOnoffSolenoid.setValue(0);
	}
}

void Gm4l6xTransmissionController::setPcState(gear_e gear) {
	if (gear != getCurrentGear()) {
		// TODO set special pressure for shift
	}
	uint8_t (*pcts)[sizeof(config->tcu_pcAirmassBins)/sizeof(config->tcu_pcAirmassBins[0])];
	switch (getCurrentGear()) {
	case REVERSE:
		pcts = &config->tcu_pcValsR;
		break;
	case NEUTRAL:
		pcts = &config->tcu_pcValsN;
		break;
	case GEAR_1:
		pcts = &config->tcu_pcValsR;
		break;
	case GEAR_2:
		pcts = &config->tcu_pcValsR;
		break;
	case GEAR_3:
		pcts = &config->tcu_pcValsR;
		break;
	case GEAR_4:
		pcts = &config->tcu_pcValsR;
		break;
	}
	int pct = interpolate2d(engine->engineState.sd.airMassInOneCylinder, config->tcu_pcAirmassBins, *pcts);
	pcPwm.setSimplePwmDutyCycle(pct*0.01);
}

Gm4l6xTransmissionController* getGm4l6xTransmissionController() {
	return &gm4l6xTransmissionController;
}
