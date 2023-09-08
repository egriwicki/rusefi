/**
 * @file boards/hellen/hellen-honda-k/board_configuration.cpp
 *
 * See https://rusefi.com/s/XX
 *
 * @author Andrey Belomutskiy, (c) 2012-2023
 */

#include "pch.h"
#include "defaults.h"
#include "hellen_meta.h"
#include "honda_k_dbc.h"

static void setInjectorPins() {
	engineConfiguration->injectionPins[0] = Gpio::H144_LS_1;
	engineConfiguration->injectionPins[1] = Gpio::H144_LS_2;
	engineConfiguration->injectionPins[2] = Gpio::H144_LS_3;
	engineConfiguration->injectionPins[3] = Gpio::H144_LS_4;
}

static void setIgnitionPins() {
	engineConfiguration->ignitionPins[0] = Gpio::H144_IGN_1;
	engineConfiguration->ignitionPins[1] = Gpio::H144_IGN_2;
	engineConfiguration->ignitionPins[2] = Gpio::H144_IGN_3;
	engineConfiguration->ignitionPins[3] = Gpio::H144_IGN_4;
}

static void setupVbatt() {
	// 4.7k high side/4.7k low side = 2.0 ratio divider
	engineConfiguration->analogInputDividerCoefficient = 2.0f;

	// set vbatt_divider 5.835
	// 33k / 6.8k
	engineConfiguration->vbattDividerCoeff = (33 + 6.8) / 6.8; // 5.835

	// pin input +12 from Main Relay
	engineConfiguration->vbattAdcChannel = EFI_ADC_5; // 4T

	engineConfiguration->adcVcc = 3.29f;
}

static void setupDefaultSensorInputs() {
    engineConfiguration->vehicleSpeedSensorInputPin = Gpio::H144_IN_VSS;

	engineConfiguration->tps1_1AdcChannel = H144_IN_TPS;

	engineConfiguration->mafAdcChannel = EFI_ADC_NONE;
	engineConfiguration->map.sensor.hwChannel = H144_IN_MAP1;

	engineConfiguration->afr.hwChannel = EFI_ADC_NONE;

	engineConfiguration->clt.adcChannel = H144_IN_CLT;

	engineConfiguration->iat.adcChannel = H144_IN_IAT;
}

void onBoardStandBy() {
    efiPrintf("K: onBoardStandBy");
    hellenBoardStandBy();
}

#include "hellen_leds_144.cpp"

void setBoardConfigOverrides() {
	setHellenMegaEnPin();
	setupVbatt();

	setHellenSdCardSpi1();
	configureHellenMegaAccCS2Pin();
	configureHellenCanTerminator();

	engineConfiguration->clt.config.bias_resistor = 4700;
	engineConfiguration->iat.config.bias_resistor = 4700;

	engineConfiguration->triggerInputPins[0] = Gpio::H144_IN_RES1;
	engineConfiguration->camInputs[0] = Gpio::H144_IN_RES3;
	// ex or in?
	engineConfiguration->camInputs[1] = Gpio::H144_IN_RES2;


//    //ETB1
//    // PWM pin
//    engineConfiguration->etbIo[0].controlPin = Gpio::H144_OUT_PWM2;
//    // DIR pin
//	engineConfiguration->etbIo[0].directionPin1 = Gpio::H144_OUT_PWM3;
//   	// Disable pin
//   	engineConfiguration->etbIo[0].disablePin = Gpio::H144_OUT_IO12;
//   	// Unused
// 	engineConfiguration->etbIo[0].directionPin2 = Gpio::Unassigned;

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
	setHondaK();

    // not override since sometimes we have isues?
	setHellenMMbaro();

	engineConfiguration->displayLogicLevelsInEngineSniffer = true;

	engineConfiguration->globalTriggerAngleOffset = 663;

	engineConfiguration->enableSoftwareKnock = true;

	engineConfiguration->canTxPin = H176_CAN_TX;
	engineConfiguration->canRxPin = H176_CAN_RX;

    engineConfiguration->vvtPins[0] = Gpio::H144_OUT_PWM4;

    gppwm_channel *vtsControl = &engineConfiguration->gppwm[0];
    // vtsControl->pin = Gpio::H144_OUT_IO6;

	engineConfiguration->fuelPumpPin = Gpio::H144_OUT_IO13;;
	engineConfiguration->idle.solenoidPin = Gpio::H144_LS_6;
	engineConfiguration->fanPin = Gpio::H144_OUT_IO12;
	engineConfiguration->mainRelayPin = Gpio::H144_OUT_IO3;
	engineConfiguration->malfunctionIndicatorPin = Gpio::H144_OUT_IO7;

	// how come this is not denso 183?!
	engineConfiguration->map.sensor.type = MT_CUSTOM;
	engineConfiguration->map.sensor.lowValue = 11.4;
	engineConfiguration->mapLowValueVoltage = 0.5;
	engineConfiguration->map.sensor.highValue = 170.7;
	engineConfiguration->mapHighValueVoltage = 4.8;

    engineConfiguration->enableKline = true;
    engineConfiguration->kLineBaudRate = 9600;
	engineConfiguration->hondaK = true;
	engineConfiguration->verboseKLine = true;

	engineConfiguration->brakePedalPin = Gpio::H144_IN_CAM;
	engineConfiguration->acRelayPin = Gpio::H144_LS_5;
    engineConfiguration->tachOutputPin = Gpio::H144_OUT_IO10;
    // A/C switch on k-line

	// "required" hardware is done - set some reasonable defaults
	setupDefaultSensorInputs();
	engineConfiguration->enableVerboseCanTx = true;

	engineConfiguration->etbFunctions[0] = DC_None;
	engineConfiguration->etbFunctions[1] = DC_Wastegate;

	// Some sensible defaults for other options
	setCrankOperationMode();

	setAlgorithm(LM_SPEED_DENSITY);

	engineConfiguration->injectorCompensationMode = ICM_FixedRailPressure;

	setCommonNTCSensor(&engineConfiguration->clt, HELLEN_DEFAULT_AT_PULLUP);
	setCommonNTCSensor(&engineConfiguration->iat, HELLEN_DEFAULT_AT_PULLUP);

    setTPS1Calibration(100, 650);
	hellenWbo();
}

static Gpio OUTPUTS[] = {
		Gpio::H144_LS_1, // 1 inj 1
		Gpio::H144_LS_2,
		Gpio::H144_LS_3,
		Gpio::H144_LS_4, // 4
	Gpio::H144_LS_5, // 5 A/C relay
	Gpio::H144_LS_6, // 6 idle air
	Gpio::H144_LS_7, // 7 intake runner
	Gpio::H144_LS_8, // 8 Lockout Solenoid
	Gpio::H144_OUT_IO12, // 9 Radiator Relay Output
	Gpio::H144_OUT_PWM6, // 10 B21 EVAP
	Gpio::H144_OUT_PWM5, // 11 B23 VTC VVT
	Gpio::H144_OUT_IO3, // E7 12 main relay
};

int getBoardMetaOutputsCount() {
    return efi::size(OUTPUTS);
}

Gpio* getBoardMetaOutputs() {
    return OUTPUTS;
}
