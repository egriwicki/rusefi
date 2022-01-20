/**
 * @file boards/proteus/board_configuration.cpp
 *
 * @brief Configuration defaults for the Proteus board
 *
 * @author Matthew Kennedy, (c) 2019
 */

#include "pch.h"
#include "fsio_impl.h"
#include "proteus_meta.h"

static const brain_pin_e injPins[] = {
	PROTEUS_LS_1,
	PROTEUS_LS_2,
	PROTEUS_LS_3,
	PROTEUS_LS_4,
	PROTEUS_LS_5,
	PROTEUS_LS_6,
	PROTEUS_LS_7,
	PROTEUS_LS_8,
	PROTEUS_LS_9,
	PROTEUS_LS_10,
	PROTEUS_LS_11,
	PROTEUS_LS_12
};

static const brain_pin_e ignPins[] = {
	PROTEUS_IGN_1,
	PROTEUS_IGN_2,
	PROTEUS_IGN_3,
	PROTEUS_IGN_4,
	PROTEUS_IGN_5,
	PROTEUS_IGN_6,
	PROTEUS_IGN_7,
	PROTEUS_IGN_8,
	PROTEUS_IGN_9,
	PROTEUS_IGN_10,
	PROTEUS_IGN_11,
	PROTEUS_IGN_12,
};

static void setInjectorPins() {
	copyArray(engineConfiguration->injectionPins, injPins);
	engineConfiguration->injectionPinMode = OM_DEFAULT;
}

static void setIgnitionPins() {
	copyArray(engineConfiguration->ignitionPins, ignPins);
	engineConfiguration->ignitionPinMode = OM_DEFAULT;
}

void setSdCardConfigurationOverrides() {
}

static void setLedPins() {
	// PE3 is error LED, configured in board.mk
	engineConfiguration->communicationLedPin = GPIOE_4;
	engineConfiguration->runningLedPin = GPIOE_5;
	engineConfiguration->warningLedPin = GPIOE_6;
}

static void setupVbatt() {
	// 5.6k high side/10k low side = 1.56 ratio divider
	engineConfiguration->analogInputDividerCoefficient = 1.56f;

	// 82k high side/10k low side = 9.2
	engineConfiguration->vbattDividerCoeff = (92.0f / 10.0f);

	// Battery sense on PA7
	engineConfiguration->vbattAdcChannel = EFI_ADC_7;

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
	engineConfiguration->etbIo[0].controlPin = GPIOD_12;
	// DIR pin
	engineConfiguration->etbIo[0].directionPin1 = GPIOD_10;
	// Disable pin
	engineConfiguration->etbIo[0].disablePin = GPIOD_11;
	// Unused
	engineConfiguration->etbIo[0].directionPin2 = GPIO_UNASSIGNED;

	// Throttle #2
	// PWM pin
	engineConfiguration->etbIo[1].controlPin = GPIOD_13;
	// DIR pin
	engineConfiguration->etbIo[1].directionPin1 = GPIOD_9;
	// Disable pin
	engineConfiguration->etbIo[1].disablePin = GPIOD_8;
	// Unused
	engineConfiguration->etbIo[1].directionPin2 = GPIO_UNASSIGNED;

	// we only have pwm/dir, no dira/dirb
	engineConfiguration->etb_use_two_wires = false;
}

static void setupDefaultSensorInputs() {
	// trigger inputs
#if VR_HW_CHECK_MODE
	// set_trigger_input_pin 0 PE7
	engineConfiguration->triggerInputPins[0] = PROTEUS_VR_1;
	engineConfiguration->camInputs[0] = PROTEUS_VR_2;
#else
	// Digital channel 1 as default - others not set
	engineConfiguration->triggerInputPins[0] = PROTEUS_DIGITAL_1;
	engineConfiguration->camInputs[0] = GPIO_UNASSIGNED;
#endif

	engineConfiguration->triggerInputPins[1] = GPIO_UNASSIGNED;
	engineConfiguration->triggerInputPins[2] = GPIO_UNASSIGNED;


	engineConfiguration->clt.adcChannel = PROTEUS_IN_CLT;
	engineConfiguration->iat.adcChannel = PROTEUS_IN_IAT;
	engineConfiguration->tps1_1AdcChannel = PROTEUS_IN_TPS;
	engineConfiguration->map.sensor.hwChannel = PROTEUS_IN_MAP;

	// pin #28 WBO AFR "Analog Volt 10"
	engineConfiguration->afr.hwChannel = EFI_ADC_5;
}

static void setupSdCard() {

	engineConfiguration->sdCardSpiDevice = SPI_DEVICE_3;
	engineConfiguration->sdCardCsPin = GPIOD_2;

	engineConfiguration->is_enabled_spi_3 = true;
	engineConfiguration->spi3sckPin = GPIOC_10;
	engineConfiguration->spi3misoPin = GPIOC_11;
	engineConfiguration->spi3mosiPin = GPIOC_12;
}

void setBoardConfigOverrides() {
	setupSdCard();
	setLedPins();
	setupVbatt();

	engineConfiguration->clt.config.bias_resistor = 2700;
	engineConfiguration->iat.config.bias_resistor = 2700;

	engineConfiguration->canTxPin = GPIOD_1;
	engineConfiguration->canRxPin = GPIOD_0;

#if defined(STM32F4) || defined(STM32F7)
	engineConfiguration->can2RxPin = GPIOB_12;
	engineConfiguration->can2TxPin = GPIOB_13;
#endif

	engineConfiguration->lps25BaroSensorScl = GPIOB_10;
	engineConfiguration->lps25BaroSensorSda = GPIOB_11;
}

void setSerialConfigurationOverrides() {
	engineConfiguration->useSerialPort = false;
	engineConfiguration->binarySerialTxPin = GPIO_UNASSIGNED;
	engineConfiguration->binarySerialRxPin = GPIO_UNASSIGNED;
//	engineConfiguration->consoleSerialTxPin = GPIO_UNASSIGNED;
//	engineConfiguration->consoleSerialRxPin = GPIO_UNASSIGNED;
}


/**
 * @brief   Board-specific configuration defaults.
 *
 * See also setDefaultEngineConfiguration
 *
 * @todo    Add your board-specific code, if any.
 */
void setBoardDefaultConfiguration() {
	setInjectorPins();
	setIgnitionPins();
	setupEtb();

	engineConfiguration->isSdCardEnabled = true;

	// "required" hardware is done - set some reasonable defaults
	setupDefaultSensorInputs();

	// Some sensible defaults for other options
	setOperationMode(engineConfiguration, FOUR_STROKE_CRANK_SENSOR);
	engineConfiguration->trigger.type = TT_TOOTHED_WHEEL_60_2;
	engineConfiguration->useOnlyRisingEdgeForTrigger = true;
	setAlgorithm(LM_SPEED_DENSITY);

	engineConfiguration->specs.cylindersCount = 8;
	engineConfiguration->specs.firingOrder = FO_1_8_7_2_6_5_4_3;

	engineConfiguration->enableSoftwareKnock = true;

	engineConfiguration->ignitionMode = IM_INDIVIDUAL_COILS;
	engineConfiguration->crankingInjectionMode = IM_SIMULTANEOUS;
	engineConfiguration->injectionMode = IM_SIMULTANEOUS;

#if HW_PROTEUS & EFI_PROD_CODE
	engineConfiguration->mainRelayPin = PROTEUS_LS_13;
	engineConfiguration->fanPin = PROTEUS_LS_15;
	engineConfiguration->fuelPumpPin = PROTEUS_LS_16;
#endif // HW_PROTEUS

	// If we're running as hardware CI, borrow a few extra pins for that
#ifdef HARDWARE_CI
	engineConfiguration->triggerSimulatorPins[0] = GPIOG_3;
	engineConfiguration->triggerSimulatorPins[1] = GPIOG_2;
#endif
}

void boardPrepareForStop() {
	#ifdef STM32F7XX
	// enable EXTI on PD0 - CAN RX pin
	palSetPadMode(GPIOD, 0, PAL_MODE_INPUT); //Select Pin 0 on D Port - PD0, CAN RX as input
	palEnableLineEvent(PAL_LINE(GPIOD, 0), PAL_EVENT_MODE_RISING_EDGE); // Set PD0 to interrupt on rising edge
	#endif

	#ifdef STM32F4XX
	// enable EXTI on PA0 - The only WKUP pin F4 has.
	PWR->CR |= PWR_CR_CWUF; //Clear Wakeup Pin flag for PA0
	palSetPadMode(GPIOA, 0, PAL_MODE_INPUT); //Select Pin 0 on D Port - PA0, Wkup
	palEnableLineEvent(PAL_LINE(GPIOA, 0), PAL_EVENT_MODE_RISING_EDGE); // Set PA0 to interrupt on rising edge

	#endif
}
