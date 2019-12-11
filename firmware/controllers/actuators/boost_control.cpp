#include "global.h"

#if EFI_BOOST_CONTROL

#if EFI_TUNER_STUDIO
#include "tunerstudio_configuration.h"
#endif /* EFI_TUNER_STUDIO */
#include "engine.h"
#include "boost_control.h"
#include "tps.h"
#include "map.h"
#include "io_pins.h"
#include "engine_configuration.h"
#include "pwm_generator_logic.h"
#include "pid.h"
#include "engine_controller.h"
#include "periodic_task.h"
#include "pin_repository.h"
#include "pwm_generator.h"
#include "pid_auto_tune.h"
#include "local_version_holder.h"
#define NO_PIN_PERIOD 500

#if defined(HAS_OS_ACCESS)
#error "Unexpected OS ACCESS HERE"
#endif

EXTERN_ENGINE;

static Logging *logger;
static boost_Map3D_t boostMap("boostmap", 1);
static SimplePwm boostPwmControl("boost");
static pid_s *boostPidS = &persistentState.persistentConfiguration.engineConfiguration.boostPid;
static Pid boostControlPid(boostPidS);


static bool shouldResetPid = false;

#if EFI_TUNER_STUDIO
extern TunerStudioOutputChannels tsOutputChannels;
#endif /* EFI_TUNER_STUDIO */


static void pidReset(void) {
	boostControlPid.reset();
}


class BoostControl: public PeriodicTimerController {
public:
OutputPin boostPin;

	int getPeriodMs()
	 override {

		return CONFIG(boostControlPin) == GPIO_UNASSIGNED ? NO_PIN_PERIOD : GET_PERIOD_LIMITED(&engineConfiguration->boostPid);
		//return GET_PERIOD_LIMITED(&engineConfiguration->boostPid);
	}

	void PeriodicTask() override {
		if (shouldResetPid) {
					pidReset();
					shouldResetPid = false;
				}

#if EFI_TUNER_STUDIO
		boostControlPid.postState(&tsOutputChannels);
#endif /* EFI_TUNER_STUDIO */

   float rpm = GET_RPM_VALUE;
   float tps = getTPS(PASS_ENGINE_PARAMETER_SIGNATURE);


   if (engineConfiguration->isBoostControlEnabled == true ) {
   if (engineConfiguration->boostType == OPEN_LOOP) {
		//Open loop
	   percent_t boostPwm = boostMap.getValue(rpm / RPM_1_BYTE_PACKING_MULT, tps / TPS_1_BYTE_PACKING_MULT)* LOAD_1_BYTE_PACKING_MULT;
			boostPwmControl.setSimplePwmDutyCycle(PERCENT_TO_DUTY(boostPwm));


	if (engineConfiguration->debugMode == DBG_BOOST) {
	#if EFI_TUNER_STUDIO
	tsOutputChannels.debugFloatField1 = boostPwm;
	#endif /* EFI_TUNER_STUDIO */
				}

		}

		else if (engineConfiguration->boostType == CLOSED_LOOP) {

			float mapValue = getMap(PASS_ENGINE_PARAMETER_SIGNATURE);

			float targetBoost = boostMap.getValue(rpm / RPM_1_BYTE_PACKING_MULT, tps / TPS_1_BYTE_PACKING_MULT)* LOAD_1_BYTE_PACKING_MULT;
			percent_t boostPwm = (boostControlPid.getOutput(targetBoost, mapValue) * 0.1);
			boostPwmControl.setSimplePwmDutyCycle(PERCENT_TO_DUTY(boostPwm));

			if (engineConfiguration->debugMode == DBG_BOOST) {
			#if EFI_TUNER_STUDIO
			tsOutputChannels.debugFloatField1 = boostPwm;
			#endif /* EFI_TUNER_STUDIO */
		  }
		}

	}

  }

};

static BoostControl BoostController;

void setBoostPFactor(float value) {
	engineConfiguration->boostPid.pFactor = value;
	boostControlPid.reset();

}

void setBoostIFactor(float value) {
	engineConfiguration->boostPid.iFactor = value;
	boostControlPid.reset();
}


void setBoostDFactor(float value) {
	engineConfiguration->boostPid.dFactor = value;
	boostControlPid.reset();
}

void setDefaultBoostParameters(DECLARE_CONFIG_PARAMETER_SIGNATURE) {


	engineConfiguration->isBoostControlEnabled = true;
	engineConfiguration->boostPwmFrequency = 55;
	engineConfiguration->boostPid.offset = 0;
	engineConfiguration->boostPid.pFactor = 30;
	engineConfiguration->boostPid.periodMs = 100;
	engineConfiguration->boostPid.maxValue = 99;
	engineConfiguration->boostControlPin = GPIOG_4;
	engineConfiguration->boostControlPinMode = OM_DEFAULT;

	setLinearCurve(config->boostRpmBins, 0, 8000 / RPM_1_BYTE_PACKING_MULT, 1);
	setLinearCurve(config->boostTpsBins, 0, 100 / TPS_1_BYTE_PACKING_MULT, 1);
	for (int loadIndex = 0;loadIndex<BOOST_TPS_COUNT;loadIndex++) {
			for (int rpmIndex = 0;rpmIndex<BOOST_RPM_COUNT;rpmIndex++) {
				config->boostTable[loadIndex][rpmIndex] = config->boostTpsBins[loadIndex] * TPS_1_BYTE_PACKING_MULT / LOAD_1_BYTE_PACKING_MULT;
			}
		}
}

static void turnBoostPidOn() {
	if (CONFIG(boostControlPin) == GPIO_UNASSIGNED){
		return;
	}
	startSimplePwmExt(&boostPwmControl, "Boost", &engine->executor,
			CONFIG(boostControlPin), &enginePins.boostPin,
			engineConfiguration->boostPwmFrequency, 0.5,
			(pwm_gen_callback*) applyPinState);
}

void startBoostPin(void) {

	turnBoostPidOn();

}
void stopBoostPin(void) {
	brain_pin_markUnused(activeConfiguration.boostControlPin);
}

void onConfigurationChangeBoostCallback(engine_configuration_s *previousConfiguration) {
	shouldResetPid = !boostControlPid.isSame(&previousConfiguration->boostPid);
}

void initBoostCtrl(Logging *sharedLogger DECLARE_ENGINE_PARAMETER_SUFFIX){
	logger = sharedLogger;
	boostMap.init(config->boostTable, config->boostTpsBins, config->boostRpmBins);
	boostControlPid.reset();
	startBoostPin();
	BoostController.Start();

}

#endif
