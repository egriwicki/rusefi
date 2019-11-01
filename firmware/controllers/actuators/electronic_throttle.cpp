/**
 * @file	electronic_throttle.cpp
 * @brief	Electronic Throttle driver
 *
 * todo: make this more universal if/when we get other hardware options
 *
 * Sep 2019 two-wire TLE9201 official driving around the block! https://www.youtube.com/watch?v=1vCeICQnbzI
 * May 2019 two-wire TLE7209 now behaves same as three-wire VNH2SP30 "eBay red board" on BOSCH 0280750009
 * Apr 2019 two-wire TLE7209 support added
 * Mar 2019 best results so far achieved with three-wire H-bridges like VNH2SP30 on BOSCH 0280750009
 * Jan 2019 actually driven around the block but still need some work.
 * Jan 2017 status:
 * Electronic throttle body with it's spring is definitely not linear - both P and I factors of PID are required to get any results
 *  PID implementation tested on a bench only
 *  it is believed that more than just PID would be needed, as is this is probably
 *  not usable on a real vehicle. Needs to be tested :)
 *
 *
 *
 *  ETB is controlled according to pedal position input (pedal position sensor is a potentiometer)
 *    pedal 0% means pedal not pressed / idle
 *    pedal 100% means pedal all the way down
 *  (not TPS - not the one you can calibrate in TunerStudio)
 *
 *  At the moment we only control opening motor - while relying on ETB spring to move throttle butterfly
 *  back. Throttle position sensor inside ETB is used for closed-loop PID control of ETB.
 *
 * See also pid.cpp
 *
 * Relevant console commands:
 *
 * ETB_BENCH_ENGINE
 * set engine_type 58
 *
 * enable verbose_etb
 * disable verbose_etb
 * ethinfo
 * set mock_pedal_position X
 *
 *
 * set debug_mode 17
 * for PID outputs
 *
 * set etb_p X
 * set etb_i X
 * set etb_d X
 * set etb_o X
 *
 * set_etb_duty X
 *
 * http://rusefi.com/forum/viewtopic.php?f=5&t=592
 *
 * @date Dec 7, 2013
 * @author Andrey Belomutskiy, (c) 2012-2018
 *
 * This file is part of rusEfi - see http://rusefi.com
 *
 * rusEfi is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * rusEfi is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "global.h"

#if EFI_ELECTRONIC_THROTTLE_BODY

#include "electronic_throttle.h"
#include "tps.h"
#include "io_pins.h"
#include "engine_configuration.h"
#include "pwm_generator_logic.h"
#include "pid.h"
#include "engine_controller.h"
#include "periodic_task.h"
#include "pin_repository.h"
#include "pwm_generator.h"
#include "dc_motor.h"
#include "pid_auto_tune.h"

#if defined(HAS_OS_ACCESS)
#error "Unexpected OS ACCESS HERE"
#endif

#define ETB_MAX_COUNT 2

static bool shouldResetPid = false;

static pid_s tuneWorkingPidSettings;
static Pid tuneWorkingPid(&tuneWorkingPidSettings);
static PID_AutoTune autoTune;

static LoggingWithStorage logger("ETB");
static pedal2tps_t pedal2tpsMap("Pedal2Tps", 1);

EXTERN_ENGINE;

static bool startupPositionError = false;

#define STARTUP_NEUTRAL_POSITION_ERROR_THRESHOLD 5

class EtbControl {
private:
	OutputPin m_pinEnable;
	OutputPin m_pinDir1;
	OutputPin m_pinDir2;

	SimplePwm m_pwmEnable;
	SimplePwm m_pwmDir1;
	SimplePwm m_pwmDir2;

	SimplePwm etbPwmUp;

public:
	DECLARE_ENGINE_PTR;

	EtbControl() : etbPwmUp("etbUp"), dcMotor(&m_pwmEnable, &m_pwmDir1, &m_pwmDir2) {}

	TwoPinDcMotor dcMotor;
	
	void setFrequency(int frequency) {
		m_pwmEnable.setFrequency(frequency);
		m_pwmDir1.setFrequency(frequency);
		m_pwmDir2.setFrequency(frequency);
	}

	void start(bool useTwoWires, 
			brain_pin_e pinEnable,
			// since we have pointer magic here we cannot simply have value parameter
			pin_output_mode_e *pinEnableMode,
			brain_pin_e pinDir1,
			brain_pin_e pinDir2) {
		dcMotor.SetType(useTwoWires ? TwoPinDcMotor::ControlType::PwmDirectionPins : TwoPinDcMotor::ControlType::PwmEnablePin);

		m_pinEnable.initPin("ETB Enable", pinEnable, pinEnableMode);
		m_pinDir1.initPin("ETB Dir 1", pinDir1);
		m_pinDir2.initPin("ETB Dir 2", pinDir2);

		// Clamp to >100hz
		int freq = maxI(100, engineConfiguration->etbFreq);


// no need to complicate event queue with ETB PWM in unit tests
#if ! EFI_UNIT_TEST
		startSimplePwm(&m_pwmEnable, "ETB Enable",
				&engine->executor,
				&m_pinEnable,
				freq,
				0,
				(pwm_gen_callback*)applyPinState);

		startSimplePwm(&m_pwmDir1, "ETB Dir 1",
				&engine->executor,
				&m_pinDir1,
				freq,
				0,
				(pwm_gen_callback*)applyPinState);

		startSimplePwm(&m_pwmDir2, "ETB Dir 2",
				&engine->executor,
				&m_pinDir2,
				freq,
				0,
				(pwm_gen_callback*)applyPinState);
#endif /* EFI_UNIT_TEST */
	}
};

static EtbControl etb1;

extern percent_t mockPedalPosition;

Pid etbPid;

static percent_t directPwmValue = NAN;
static percent_t currentEtbDuty;

#define ETB_DUTY_LIMIT 0.9
// this macro clamps both positive and negative percentages from about -100% to 100%
#define ETB_PERCENT_TO_DUTY(X) (maxF(minF((X * 0.01), ETB_DUTY_LIMIT - 0.01), 0.01 - ETB_DUTY_LIMIT))


	int EtbController::getPeriodMs() {
		return GET_PERIOD_LIMITED(&engineConfiguration->etb);
	}

	void EtbController::PeriodicTask() {
		// set debug_mode 17
		if (engineConfiguration->debugMode == DBG_ELECTRONIC_THROTTLE_PID) {
#if EFI_TUNER_STUDIO
			etbPid.postState(&tsOutputChannels);
			tsOutputChannels.debugIntField5 = engine->engineState.etbFeedForward;
#endif /* EFI_TUNER_STUDIO */
		} else if (engineConfiguration->debugMode == DBG_ELECTRONIC_THROTTLE_EXTRA) {
#if EFI_TUNER_STUDIO
			// set debug_mode 29
			tsOutputChannels.debugFloatField1 = directPwmValue;
#endif /* EFI_TUNER_STUDIO */
		}

		if (startupPositionError) {
			etb1.dcMotor.Set(0);
			return;
		}

		if (shouldResetPid) {
			etbPid.reset();
			shouldResetPid = false;
		}

		if (!cisnan(directPwmValue)) {
			etb1.dcMotor.Set(directPwmValue);
			return;
		}

		if (boardConfiguration->pauseEtbControl) {
			etb1.dcMotor.Set(0);
			return;
		}

		percent_t actualThrottlePosition = getTPS(PASS_ENGINE_PARAMETER_SIGNATURE);

		if (engine->etbAutoTune) {
			autoTune.input = actualThrottlePosition;
			bool result = autoTune.Runtime(&logger);

			tuneWorkingPid.updateFactors(autoTune.output, 0, 0);

			float value = tuneWorkingPid.getOutput(50, actualThrottlePosition);
			scheduleMsg(&logger, "AT input=%f output=%f PID=%f", autoTune.input,
					autoTune.output,
					value);
			scheduleMsg(&logger, "AT PID=%f", value);
			etb1.dcMotor.Set(ETB_PERCENT_TO_DUTY(value));

			if (result) {
				scheduleMsg(&logger, "GREAT NEWS! %f/%f/%f", autoTune.GetKp(), autoTune.GetKi(), autoTune.GetKd());
			}

			return;
		}


		percent_t pedalPosition = getPedalPosition(PASS_ENGINE_PARAMETER_SIGNATURE);

		int rpm = GET_RPM();
		engine->engineState.targetFromTable = pedal2tpsMap.getValue(rpm / RPM_1_BYTE_PACKING_MULT, pedalPosition);
		percent_t etbIdleAddition = CONFIGB(useETBforIdleControl) ? engine->engineState.idle.etbIdleAddition : 0;
		percent_t targetPosition = engine->engineState.targetFromTable + etbIdleAddition;

		if (engineConfiguration->debugMode == DBG_ETB_LOGIC) {
#if EFI_TUNER_STUDIO
			tsOutputChannels.debugFloatField1 = engine->engineState.targetFromTable;
			tsOutputChannels.debugFloatField2 = engine->engineState.idle.etbIdleAddition;
#endif /* EFI_TUNER_STUDIO */
		}

		engine->engineState.etbFeedForward = interpolate2d("etbb", targetPosition, engineConfiguration->etbBiasBins, engineConfiguration->etbBiasValues);

		etbPid.iTermMin = engineConfiguration->etb_iTermMin;
		etbPid.iTermMax = engineConfiguration->etb_iTermMax;

		currentEtbDuty = engine->engineState.etbFeedForward +
				etbPid.getOutput(targetPosition, actualThrottlePosition);

		etb1.dcMotor.Set(ETB_PERCENT_TO_DUTY(currentEtbDuty));

		if (engineConfiguration->isVerboseETB) {
			etbPid.showPidStatus(&logger, "ETB");
		}

		DISPLAY_STATE(Engine)
DISPLAY(DISPLAY_IF(hasEtbPedalPositionSensor))
		DISPLAY_TEXT(Electronic_Throttle);
		DISPLAY_SENSOR(TPS)
		DISPLAY_TEXT(eol);

		DISPLAY_TEXT(Pedal);
		DISPLAY_SENSOR(PPS);
		DISPLAY(DISPLAY_CONFIG(throttlePedalPositionAdcChannel));
		DISPLAY_TEXT(eol);

		DISPLAY_TEXT(Feed_forward);
		DISPLAY(DISPLAY_FIELD(etbFeedForward));
		DISPLAY_TEXT(eol);

		DISPLAY_STATE(ETB_pid)
		DISPLAY_TEXT(input);
		DISPLAY(DISPLAY_FIELD(input));
		DISPLAY_TEXT(Output);
		DISPLAY(DISPLAY_FIELD(output));
		DISPLAY_TEXT(iTerm);
		DISPLAY(DISPLAY_FIELD(iTerm));
		DISPLAY_TEXT(eol);
		DISPLAY(DISPLAY_FIELD(errorAmplificationCoef));
		DISPLAY(DISPLAY_FIELD(previousError));
		DISPLAY_TEXT(eol);

		DISPLAY_TEXT(Settings);
		DISPLAY(DISPLAY_CONFIG(ETB_PFACTOR));
		DISPLAY(DISPLAY_CONFIG(ETB_IFACTOR));
		DISPLAY(DISPLAY_CONFIG(ETB_DFACTOR));
		DISPLAY_TEXT(eol);
		DISPLAY(DISPLAY_CONFIG(ETB_OFFSET));
		DISPLAY(DISPLAY_CONFIG(ETB_PERIODMS));
		DISPLAY_TEXT(eol);
		DISPLAY(DISPLAY_CONFIG(ETB_MINVALUE));
		DISPLAY(DISPLAY_CONFIG(ETB_MAXVALUE));
/* DISPLAY_ELSE */
		DISPLAY_TEXT(No_Pedal_Sensor);
/* DISPLAY_ENDIF */
#if EFI_TUNER_STUDIO
		// 312
		tsOutputChannels.etbTarget = targetPosition;
		// 316
		tsOutputChannels.etb1DutyCycle = currentEtbDuty;
		// 320
		// Error is positive if the throttle needs to open further
		tsOutputChannels.etb1Error = targetPosition - actualThrottlePosition;
#endif /* EFI_TUNER_STUDIO */
	}

EtbController etbController;

/**
 * set_etb_duty X
 * manual duty cycle control without PID. Percent value from 0 to 100
 */
void setThrottleDutyCycle(percent_t level) {
	scheduleMsg(&logger, "setting ETB duty=%f%%", level);
	if (cisnan(level)) {
		directPwmValue = NAN;
		return;
	}

	float dc = ETB_PERCENT_TO_DUTY(level);
	directPwmValue = dc;
	etb1.dcMotor.Set(dc);
	scheduleMsg(&logger, "duty ETB duty=%f", dc);
}

#if EFI_PROD_CODE
static void showEthInfo(void) {
	static char pinNameBuffer[16];

	scheduleMsg(&logger, "etbAutoTune=%d",
			engine->etbAutoTune);

	scheduleMsg(&logger, "throttlePedal=%.2f %.2f/%.2f @%s",
			getPedalPosition(PASS_ENGINE_PARAMETER_SIGNATURE),
			engineConfiguration->throttlePedalUpVoltage,
			engineConfiguration->throttlePedalWOTVoltage,
			getPinNameByAdcChannel("tPedal", engineConfiguration->throttlePedalPositionAdcChannel, pinNameBuffer));

	scheduleMsg(&logger, "TPS=%.2f", getTPS(PASS_ENGINE_PARAMETER_SIGNATURE));
	scheduleMsg(&logger, "dir=%d DC=%f", etb1.dcMotor.isOpenDirection(), etb1.dcMotor.Get());

	scheduleMsg(&logger, "etbControlPin1=%s duty=%.2f freq=%d",
			hwPortname(CONFIGB(etb1.controlPin1)),
			currentEtbDuty,
			engineConfiguration->etbFreq);
	scheduleMsg(&logger, "dir1=%s", hwPortname(CONFIGB(etb1.directionPin1)));
	scheduleMsg(&logger, "dir2=%s", hwPortname(CONFIGB(etb1.directionPin2)));
	etbPid.showPidStatus(&logger, "ETB");
}

/**
 * set etb_p X
 */
void setEtbPFactor(float value) {
	engineConfiguration->etb.pFactor = value;
	etbPid.reset();
	showEthInfo();
}

static void setEtbFrequency(int frequency) {
	engineConfiguration->etbFreq = frequency;

	etb1.setFrequency(frequency);
}

static void etbReset() {
	scheduleMsg(&logger, "etbReset");
	
	etb1.dcMotor.Set(0);
	etbPid.reset();

	mockPedalPosition = MOCK_UNDEFINED;
}

/**
 * set etb_i X
 */
void setEtbIFactor(float value) {
	engineConfiguration->etb.iFactor = value;
	etbPid.reset();
	showEthInfo();
}

/**
 * set etb_d X
 */
void setEtbDFactor(float value) {
	engineConfiguration->etb.dFactor = value;
	etbPid.reset();
	showEthInfo();
}

/**
 * set etb_o X
 */
void setEtbOffset(int value) {
	engineConfiguration->etb.offset = value;
	etbPid.reset();
	showEthInfo();
}
#endif /* EFI_PROD_CODE */

void setBoschVNH2SP30Curve(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	engineConfiguration->etbBiasBins[0] = 0;
	engineConfiguration->etbBiasBins[1] = 1;
	engineConfiguration->etbBiasBins[2] = 5;
	/**
	 * This specific throttle has default position of about 7% open
	 */
	engineConfiguration->etbBiasBins[3] = 7;
	engineConfiguration->etbBiasBins[4] = 14;
	engineConfiguration->etbBiasBins[5] = 65;
	engineConfiguration->etbBiasBins[6] = 66;
	engineConfiguration->etbBiasBins[7] = 100;

	/**
	 * Some negative bias for below-default position
	 */
	engineConfiguration->etbBiasValues[0] = -15;
	engineConfiguration->etbBiasValues[1] = -15;
	engineConfiguration->etbBiasValues[2] = -10;
	/**
	 * Zero bias for index which corresponds to default throttle position, when no current is applied
	 * This specific throttle has default position of about 7% open
	 */
	engineConfiguration->etbBiasValues[3] = 0;
	engineConfiguration->etbBiasValues[4] = 19;
	engineConfiguration->etbBiasValues[5] = 20;
	engineConfiguration->etbBiasValues[6] = 26;
	engineConfiguration->etbBiasValues[7] = 28;
}

void setDefaultEtbParameters(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	CONFIG(etbIdleThrottleRange) = 5;

	setLinearCurveAny<uint8_t>(config->pedalToTpsPedalBins, PEDAL_TO_TPS_SIZE, /*from*/0, /*to*/100, 1);
	setLinearCurveAny<uint8_t>(config->pedalToTpsRpmBins, PEDAL_TO_TPS_SIZE, /*from*/0, /*to*/8000 / RPM_1_BYTE_PACKING_MULT, 1);

	for (int pedalIndex = 0;pedalIndex<PEDAL_TO_TPS_SIZE;pedalIndex++) {
		for (int rpmIndex = 0;rpmIndex<PEDAL_TO_TPS_SIZE;rpmIndex++) {
			config->pedalToTpsTable[pedalIndex][rpmIndex] = config->pedalToTpsPedalBins[pedalIndex];
		}
	}


	engineConfiguration->throttlePedalUpVoltage = 0; // that's voltage, not ADC like with TPS
	engineConfiguration->throttlePedalWOTVoltage = 6; // that's voltage, not ADC like with TPS

	engineConfiguration->etb.pFactor = 1;
	engineConfiguration->etb.iFactor = 0.05;
	engineConfiguration->etb.dFactor = 0.0;
	engineConfiguration->etb.periodMs = (1000 / DEFAULT_ETB_LOOP_FREQUENCY);
	engineConfiguration->etbFreq = DEFAULT_ETB_PWM_FREQUENCY;
	engineConfiguration->etb_iTermMin = -300;
	engineConfiguration->etb_iTermMax = 300;

	// values are above 100% since we have feedforward part of the total summation
	engineConfiguration->etb.minValue = -200;
	engineConfiguration->etb.maxValue = 200;
}

static bool isEtbPinsChanged(etb_io *current, etb_io *active) {
	return 	current->controlPin1 != active->controlPin1 ||
			current->controlPinMode != active->controlPinMode ||
			current->directionPin1 != active->directionPin1 ||
			current->directionPin2 != active->directionPin2;
}

#if EFI_PROD_CODE
bool isETBRestartNeeded(void) {
	/**
	 * We do not want any interruption in HW pin while adjusting other properties
	 */
	return isEtbPinsChanged(&engineConfiguration->bc.etb1, &activeConfiguration.bc.etb1);
}

void stopETBPins(void) {
	brain_pin_markUnused(activeConfiguration.bc.etb1.controlPin1);
	brain_pin_markUnused(activeConfiguration.bc.etb1.directionPin1);
	brain_pin_markUnused(activeConfiguration.bc.etb1.directionPin2);
}
#endif /* EFI_PROD_CODE */

void onConfigurationChangeElectronicThrottleCallback(engine_configuration_s *previousConfiguration) {
	shouldResetPid = !etbPid.isSame(&previousConfiguration->etb);
}

void startETBPins(DECLARE_ENGINE_PARAMETER_SIGNATURE) {

	// controlPinMode is a strange feature - it's simply because I am short on 5v I/O on Frankenso with Miata NB2 test mule
	etb1.start(
			CONFIG(etb1_use_two_wires),
			CONFIGB(etb1.controlPin1),
			&CONFIGB(etb1.controlPinMode),
			CONFIGB(etb1.directionPin1),
			CONFIGB(etb1.directionPin2)
			);
}

static void setTempOutput(float value) {
	autoTune.output = value;
}

/**
 * set_etbat_step X
 */
static void setAutoStep(float value) {
	autoTune.reset();
	autoTune.SetOutputStep(value);
}

static void setAutoPeriod(int period) {
	tuneWorkingPidSettings.periodMs = period;
	autoTune.reset();
}

static void setAutoOffset(int offset) {
	tuneWorkingPidSettings.offset = offset;
	autoTune.reset();
}

void setDefaultEtbBiasCurve(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	engineConfiguration->etbBiasBins[0] = 0;
	engineConfiguration->etbBiasBins[1] = 1;
	engineConfiguration->etbBiasBins[2] = 2;
	/**
	 * This specific throttle has default position of about 4% open
	 */
	engineConfiguration->etbBiasBins[3] = 4;
	engineConfiguration->etbBiasBins[4] = 7;
	engineConfiguration->etbBiasBins[5] = 98;
	engineConfiguration->etbBiasBins[6] = 99;
	engineConfiguration->etbBiasBins[7] = 100;

	/**
	 * Some negative bias for below-default position
	 */
	engineConfiguration->etbBiasValues[0] = -20;
	engineConfiguration->etbBiasValues[1] = -18;
	engineConfiguration->etbBiasValues[2] = -17;
	/**
	 * Zero bias for index which corresponds to default throttle position, when no current is applied
	 * This specific throttle has default position of about 4% open
	 */
	engineConfiguration->etbBiasValues[3] = 0;
	engineConfiguration->etbBiasValues[4] = 20;
	engineConfiguration->etbBiasValues[5] = 21;
	engineConfiguration->etbBiasValues[6] = 22;
	engineConfiguration->etbBiasValues[7] = 25;
}

void unregisterEtbPins() {

}

void initElectronicThrottle(DECLARE_ENGINE_PARAMETER_SIGNATURE) {
#if EFI_PROD_CODE
	addConsoleAction("ethinfo", showEthInfo);
	addConsoleAction("etbreset", etbReset);
	addConsoleActionI("etb_freq", setEtbFrequency);
#endif /* EFI_PROD_CODE */

	etbPid.initPidClass(&engineConfiguration->etb);

	INJECT_ENGINE_REFERENCE(etb1);
	INJECT_ENGINE_REFERENCE(etbController);

	pedal2tpsMap.init(config->pedalToTpsTable, config->pedalToTpsPedalBins, config->pedalToTpsRpmBins);

	engine->engineState.hasEtbPedalPositionSensor = hasPedalPositionSensor(PASS_ENGINE_PARAMETER_SIGNATURE);
	if (!engine->engineState.hasEtbPedalPositionSensor) {
		return;
	}
#if 0
	// not alive code
	autoTune.SetOutputStep(0.1);
#endif

#if 0 && ! EFI_UNIT_TEST
	percent_t startupThrottlePosition = getTPS(PASS_ENGINE_PARAMETER_SIGNATURE);
	if (absF(startupThrottlePosition - engineConfiguration->etbNeutralPosition) > STARTUP_NEUTRAL_POSITION_ERROR_THRESHOLD) {
		/**
		 * Unexpected electronic throttle start-up position is worth a fatal error
		 */
		firmwareError(OBD_Throttle_Actuator_Control_Range_Performance_Bank_1, "startup ETB position %.2f not %d",
				startupThrottlePosition,
				engineConfiguration->etbNeutralPosition);
		startupPositionError = true;
	}
#endif /* EFI_UNIT_TEST */

	startETBPins(PASS_ENGINE_PARAMETER_SIGNATURE);

#if EFI_PROD_CODE
	if (engineConfiguration->etbCalibrationOnStart) {
		etb1.dcMotor.Set(70);
		chThdSleep(600);
		grabTPSIsWideOpen();
		etb1.dcMotor.Set(-70);
		chThdSleep(600);
		grabTPSIsClosed();
	}

	// manual duty cycle control without PID. Percent value from 0 to 100
	addConsoleActionNANF(CMD_ETB_DUTY, setThrottleDutyCycle);
#endif

#if EFI_PROD_CODE && 0
	tuneWorkingPidSettings.pFactor = 1;
	tuneWorkingPidSettings.iFactor = 0;
	tuneWorkingPidSettings.dFactor = 0;
//	tuneWorkingPidSettings.offset = 10; // todo: not hard-coded value
	//todo tuneWorkingPidSettings.periodMs = 10;
	tuneWorkingPidSettings.minValue = 0;
	tuneWorkingPidSettings.maxValue = 100;
	tuneWorkingPidSettings.periodMs = 100;

	// this is useful once you do "enable etb_auto"
	addConsoleActionF("set_etbat_output", setTempOutput);
	addConsoleActionF("set_etbat_step", setAutoStep);
	addConsoleActionI("set_etbat_period", setAutoPeriod);
	addConsoleActionI("set_etbat_offset", setAutoOffset);
#endif /* EFI_PROD_CODE */


	etbPid.reset();

	etbController.Start();
}

#endif /* EFI_ELECTRONIC_THROTTLE_BODY */

