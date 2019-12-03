/**
 * @file boards/proteus/board_configuration.cpp
 *
 * @brief Configuration defaults for the Proteus board
 *
 * @author Matthew Kennedy, (c) 2019
 */

#include "global.h"
#include "engine.h"
#include "engine_math.h"
#include "allsensors.h"
#include "fsio_impl.h"
#include "engine_configuration.h"

EXTERN_ENGINE;

static void setInjectorPins() {
	boardConfiguration->injectionPins[0] = GPIOD_7;
	boardConfiguration->injectionPins[1] = GPIOG_9;
	boardConfiguration->injectionPins[2] = GPIOG_10;
	boardConfiguration->injectionPins[3] = GPIOG_11;
	boardConfiguration->injectionPins[4] = GPIOG_12;
	boardConfiguration->injectionPins[5] = GPIOG_13;
	boardConfiguration->injectionPins[6] = GPIOG_14;
	boardConfiguration->injectionPins[7] = GPIOB_4;
	boardConfiguration->injectionPins[8] = GPIOB_5;
	boardConfiguration->injectionPins[9] = GPIOB_6;
	boardConfiguration->injectionPins[10] = GPIOB_7;
	boardConfiguration->injectionPins[11] = GPIOB_8;

	boardConfiguration->injectionPinMode = OM_DEFAULT;
}

static void setIgnitionPins() {
	boardConfiguration->ignitionPins[0] = GPIOD_4;
	boardConfiguration->ignitionPins[1] = GPIOD_3;
	boardConfiguration->ignitionPins[2] = GPIOC_9;
	boardConfiguration->ignitionPins[3] = GPIOC_8;
	boardConfiguration->ignitionPins[4] = GPIOC_7;
	boardConfiguration->ignitionPins[5] = GPIOG_8;
	boardConfiguration->ignitionPins[6] = GPIOG_7;
	boardConfiguration->ignitionPins[7] = GPIOG_6;
	boardConfiguration->ignitionPins[8] = GPIOG_5;
	boardConfiguration->ignitionPins[9] = GPIOG_4;
	boardConfiguration->ignitionPins[10] = GPIOG_3;
	boardConfiguration->ignitionPins[11] = GPIOG_2;

	boardConfiguration->ignitionPinMode = OM_DEFAULT;
}

static void setLedPins() {
	engineConfiguration->communicationLedPin = GPIOE_4;
	engineConfiguration->runningLedPin = GPIOE_5;
	boardConfiguration->triggerErrorPin = GPIOE_6;
}

static void setupVbatt() {
	// 6.8k high side/10k low side = 1.6667 ratio divider
	engineConfiguration->analogInputDividerCoefficient = 2.5f / 1.5f;

	// 47k high side/10k low side = 4.7
	engineConfiguration->vbattDividerCoeff = (57.0f / 10.0f);
	//engineConfiguration->vbattAdcChannel = TODO;

	engineConfiguration->adcVcc = 3.3f;
}

static void setupEtb() {
	// TLE9201 driver
	// This chip has three control pins:
	// DIR - sets direction of the motor
	// PWM - pwm control (enable high, coast low)
	// DIS - disables motor (enable low)

	// Throttle #1
	// PWM pin
	engineConfiguration->etbIo[0].controlPin1 = GPIOD_12;
	// DIR pin
	engineConfiguration->etbIo[0].directionPin1 = GPIOD_10;
	// Unused
	engineConfiguration->etbIo[0].directionPin2 = GPIO_UNASSIGNED;

	// Throttle #2
	// PWM pin
	engineConfiguration->etbIo[0].controlPin1 = GPIOD_13;
	// DIR pin
	engineConfiguration->etbIo[0].directionPin1 = GPIOD_9;
	// Unused
	engineConfiguration->etbIo[0].directionPin2 = GPIO_UNASSIGNED;

#if EFI_FSIO
	// disable ETB by default
	setFsio(7, GPIOD_8, "1" PASS_CONFIG_PARAMETER_SUFFIX);
	setFsio(8, GPIOD_11, "1" PASS_CONFIG_PARAMETER_SUFFIX);
#endif /* EFI_FSIO */

	// we only have pwm/dir, no dira/dirb
	engineConfiguration->etb_use_two_wires = false;
	engineConfiguration->etbFreq = 800;
}

static void setupDefaultSensorInputs() {
	// trigger inputs
	// VR channel 1 as default - others not set
	boardConfiguration->triggerInputPins[0] = GPIOC_6;
	boardConfiguration->triggerInputPins[1] = GPIO_UNASSIGNED;
	boardConfiguration->triggerInputPins[2] = GPIO_UNASSIGNED;
	engineConfiguration->camInputs[0] = GPIO_UNASSIGNED;

	// clt = Analog Temp 1 = PC4
	engineConfiguration->clt.adcChannel = EFI_ADC_14;
	engineConfiguration->clt.config.bias_resistor = 2700;

	// iat = Analog Temp 2 = PC5
	engineConfiguration->iat.adcChannel = EFI_ADC_15;
	engineConfiguration->iat.config.bias_resistor = 2700;

	// TODO: more sensors
}

void setPinConfigurationOverrides(void) {
}

void setSerialConfigurationOverrides(void) {
	boardConfiguration->useSerialPort = false;
	engineConfiguration->binarySerialTxPin = GPIO_UNASSIGNED;
	engineConfiguration->binarySerialRxPin = GPIO_UNASSIGNED;
	engineConfiguration->consoleSerialTxPin = GPIO_UNASSIGNED;
	engineConfiguration->consoleSerialRxPin = GPIO_UNASSIGNED;
}


/**
 * @brief   Board-specific configuration code overrides.
 *
 * See also setDefaultEngineConfiguration
 *
 * @todo    Add your board-specific code, if any.
 */
void setBoardConfigurationOverrides(void) {
	setInjectorPins();
	setIgnitionPins();
	setLedPins();
	setupVbatt();
	setupEtb();

	// "required" hardware is done - set some reasonable defaults
	setupDefaultSensorInputs();

	// Some sensible defaults for other options
	setOperationMode(engineConfiguration, FOUR_STROKE_CRANK_SENSOR);
	engineConfiguration->trigger.type = TT_TOOTHED_WHEEL_60_2;
	engineConfiguration->useOnlyRisingEdgeForTrigger = true;
	setAlgorithm(LM_SPEED_DENSITY PASS_CONFIG_PARAMETER_SUFFIX);

	engineConfiguration->specs.cylindersCount = 8;
	engineConfiguration->specs.firingOrder = FO_1_8_7_2_6_5_4_3;

	engineConfiguration->ignitionMode = IM_INDIVIDUAL_COILS;
	engineConfiguration->crankingInjectionMode = IM_SIMULTANEOUS;
	engineConfiguration->injectionMode = IM_SIMULTANEOUS;
}

void setAdcChannelOverrides(void) {
}
