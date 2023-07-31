/*
 * @file hyundai.cpp
 *
https://rusefi.com/docs/pinouts/hellen/hellen-hyundai-pb-mt/
 *
https://rusefi.com/docs/pinouts/hellen/hellen154hyundai/
 * @date Oct 11, 2021
 * @author Andrey Belomutskiy, (c) 2012-2021
 */

#include "pch.h"

#include "hyundai.h"
#include "proteus_meta.h"
#include "defaults.h"
#include "lua_lib.h"

static void set201xHyundai() {
	setPPSCalibration(0.73, 4.0, 0.34, 1.86);

    // note how these numbers are very flipped m111 defaults?
    setTPS1Calibration(98, 926, 891, 69);
    setEtbPID(8.8944, 70.2307, 0.1855);
   	// Some sensible defaults for other options
   	setAlgorithm(LM_SPEED_DENSITY);
}

void setHyundaiPb() {
    setInline4();
	engineConfiguration->displacement = 1.6;
	strcpy(engineConfiguration->engineMake, ENGINE_MAKE_Hyundai);
	strcpy(engineConfiguration->engineCode, "Gamma");

	engineConfiguration->trigger.type = trigger_type_e::TT_TOOTHED_WHEEL_60_2;
	engineConfiguration->globalTriggerAngleOffset = 90;

    engineConfiguration->etbIdleThrottleRange = 6.0;

    engineConfiguration->totalGearsCount = 5;
    engineConfiguration->gearRatio[0] = 12;
    engineConfiguration->gearRatio[1] = 6;
    engineConfiguration->gearRatio[2] = 4.4;
    engineConfiguration->gearRatio[3] = 2.4;
    engineConfiguration->gearRatio[4] = 1;

    // ETB buzzing is annoying :(
	engineConfiguration->disableEtbWhenEngineStopped = true;

	engineConfiguration->afr.hwChannel = EFI_ADC_NONE;
	engineConfiguration->enableAemXSeries = true;

	set201xHyundai();
	// Injectors flow 1214 cc/min at 100 bar pressure
	engineConfiguration->injector.flow = 1214;
	setGDIFueling();
	engineConfiguration->injectionMode = IM_SEQUENTIAL;
   	engineConfiguration->crankingInjectionMode = IM_SEQUENTIAL;

	engineConfiguration->map.sensor.type = MT_CUSTOM;
	engineConfiguration->map.sensor.lowValue = 20;
	engineConfiguration->mapLowValueVoltage = 0.79;
	engineConfiguration->map.sensor.highValue = 101.3;
	engineConfiguration->mapHighValueVoltage = 4;

    engineConfiguration->mc33_hpfp_i_peak = 10;
    engineConfiguration->mc33_hpfp_i_hold = 4;

	engineConfiguration->vvtMode[0] = VVT_SECOND_HALF;
	engineConfiguration->vvtMode[1] = VVT_SECOND_HALF;

    engineConfiguration->ignitionMode = IM_INDIVIDUAL_COILS;

    engineConfiguration->hpfpCamLobes = 4;
    engineConfiguration->rethrowHardFault = true;

   	engineConfiguration->highPressureFuel.v1 = 0.5; /* volts */;
   	engineConfiguration->highPressureFuel.value1 = 0;
   	engineConfiguration->highPressureFuel.v2 = 4.5; /* volts */;
   	// page 98, Fuel System > Engine Control System > Rail Pressure Sensor (RPS) > Specifications
   	engineConfiguration->highPressureFuel.value2 = 20'000;
	engineConfiguration->highPressureFuel.hwChannel = PROTEUS_IN_ANALOG_VOLT_4;

#if HW_PROTEUS
	setCommonNTCSensor(&engineConfiguration->clt, PROTEUS_DEFAULT_AT_PULLUP);
	setCommonNTCSensor(&engineConfiguration->iat, PROTEUS_DEFAULT_AT_PULLUP);

//    engineConfiguration->acRelayPin = PROTEUS_LS_6;
    engineConfiguration->acSwitch = PROTEUS_DIGITAL_5;

	engineConfiguration->fanPin = PROTEUS_LS_5;
    engineConfiguration->tachOutputPin = PROTEUS_IGN_12;
    engineConfiguration->hpfpValvePin = PROTEUS_LS_6;
    engineConfiguration->vvtPins[0] = PROTEUS_LS_15; // intake
    engineConfiguration->vvtPins[1] = PROTEUS_LS_16; // exhaust
#endif // HW_PROTEUS

#if HW_PROTEUS
	strncpy(config->luaScript, GET_BIT_RANGE_LSB TWO_BYTES_LSB PRINT_ARRAY SET_TWO_BYTES_LSB HYUNDAI_SUM_NIBBLES R"(

GDI4_BASE_ADDRESS = 0xBB20
GDI_CHANGE_ADDRESS = GDI4_BASE_ADDRESS + 0x10

GDI4_CAN_SET_TAG = 0x78
local data_set_settings = { GDI4_CAN_SET_TAG, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

FIXED_POINT = 128

setTickRate(100)

function onCanConfiguration3(bus, id, dlc, data)
--	print("Received configuration3 "..arrayToString(data))
	pumpPeak = getTwoBytesLSB(data, 6, 1 / 128)
--	print("GDI4 says PumpPeakCurrent ".. pumpPeak)
	setLuaGauge(1, pumpPeak)
end

canRxAdd(GDI4_BASE_ADDRESS + 3, onCanConfiguration3)

EMS_DCT11_128 = 0x80
EMS_DCT12_129 = 0x81
EMS_H12_399 = 0x18f
EMS6_608 = 0x260
EMS5_672 = 0x2a0
EMS11_790 = 0x316
EMS12_809 = 0x329
EMS9_898 = 0x382
EMS14_1349 = 0x545

counter = 0

payLoad128 =  { 0x00, 0x17, 0x70, 0x0F, 0x1B, 0x2C, 0x1B, 0x75 }
payLoad129 =  { 0x40, 0x84, 0x5F, 0x00, 0x00, 0x00, 0x00, 0x75 }
payLoad399 = {0x00, 0x30, 0x1d, 0x00, 0x00, 0x63, 0x00, 0x00}
payLoad608 = {0x05, 0x1d, 0x00, 0x30, 0x01, 0xa5, 0x7f, 0x31}
payLoad672 = {0xe0, 0x00, 0x5f, 0x98, 0x39, 0x12, 0x9e, 0x08}
payLoad809 = {0xd7, 0x7b, 0x7e, 0x0c, 0x11, 0x2c, 0x00, 0x10}
payLoad898 = {0x40, 0xfe, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x08}
payLoad1349 = { 0xCA, 0x16, 0x00, 0x8A, 0x75, 0xFF, 0x75, 0xFF }

speedSensor = Sensor.new("VehicleSpeed")
speedSensor : setTimeout(3000)

function onCluPacket(bus, id, dlc, data)
	speedKph = getBitRange(data, 8, 9) * 0.5
	print('onCAR_SPEED ' ..speedKph)
	speedSensor : set(speedKph)
end

canRxAdd(1, 1264, onCluPacket)

function onTick()
	local RPMread = math.floor(getSensor("RPM") * 4)
	local RPMhi = RPMread >> 8
	local RPMlo = RPMread & 0xff

    payLoad128[3] = RPMlo
    payLoad128[4] = RPMhi

    counter = (counter + 1) % 16

    check128 = hyundaiSumNibbles(payLoad128, counter)
    payLoad128[8] = check128 * 16 + counter
    txCan(1, EMS_DCT11_128, 0, payLoad128)

    check129 = hyundaiSumNibbles(payLoad129, counter)
    payLoad129[8] = check129 * 16 + counter
    txCan(1, EMS_DCT12_129, 0, payLoad129)

	canRPMpayload = { 0x05, 0x1B, RPMlo, RPMhi, 0x1B, 0x2C, 0x00, 0x7F }

	txCan(1, EMS11_790, 0, canRPMpayload)
	txCan(1, EMS14_1349, 0, payLoad1349)

	txCan(1, EMS_H12_399, 0, payLoad399)
	txCan(1, EMS6_608, 0, payLoad608)
	txCan(1, EMS5_672, 0, payLoad672)
	txCan(1, EMS12_809, 0, payLoad809)
	txCan(1, EMS9_898, 0, payLoad898)

	pumpPeakCurrent      = getCalibration("mc33_hpfp_i_peak")
	pumpHoldCurrent      = getCalibration("mc33_hpfp_i_hold")

	TholdOff = getCalibration("mc33_t_hold_off")
	THoldDuration = getCalibration("mc33_t_hold_tot")


	setTwoBytesLsb(data_set_settings, 1, TholdOff)
	setTwoBytesLsb(data_set_settings, 3, THoldDuration)
	setTwoBytesLsb(data_set_settings, 5, pumpPeakCurrent * FIXED_POINT)
	print('Will be sending ' ..arrayToString(data_set_settings))
	txCan(1, GDI_CHANGE_ADDRESS + 3, 1, data_set_settings)

	setTwoBytesLsb(data_set_settings, 1, pumpHoldCurrent * FIXED_POINT)
	setTwoBytesLsb(data_set_settings, 3, GDI4_BASE_ADDRESS)
	print('Will be sending ' ..arrayToString(data_set_settings))
	txCan(1, GDI_CHANGE_ADDRESS + 4, 1, data_set_settings)
end

)", efi::size(config->luaScript));
#endif // HW_PROTEUS

}

void setProteusHyundaiPb() {
    setHyundaiPb();
	engineConfiguration->triggerInputPins[0] = PROTEUS_VR_1;
	engineConfiguration->camInputs[0] = PROTEUS_DIGITAL_1;
	engineConfiguration->camInputs[1] = PROTEUS_DIGITAL_2; // currently intake, TODO: flip!

	setProteusEtbIO();

// something something input levels are not happy for digital input pin?
	engineConfiguration->starterControlPin = PROTEUS_LS_14;
	engineConfiguration->startStopButtonPin = PROTEUS_IN_AV_6_DIGITAL;
	engineConfiguration->startStopButtonMode = PI_DEFAULT;
}

static void commonGenesisCoupe() {
	set201xHyundai();


	engineConfiguration->displayLogicLevelsInEngineSniffer = true;

	engineConfiguration->enableSoftwareKnock = true;
	engineConfiguration->canNbcType = CAN_BUS_GENESIS_COUPE;

	engineConfiguration->injectorCompensationMode = ICM_FixedRailPressure;

	engineConfiguration->cylindersCount = 4;
	engineConfiguration->firingOrder = FO_1_3_4_2;
	engineConfiguration->displacement = 1.998;
	strcpy(engineConfiguration->engineMake, ENGINE_MAKE_Hyundai);
	strcpy(engineConfiguration->engineCode, "Theta II");
	engineConfiguration->globalTriggerAngleOffset = 90;

	engineConfiguration->ignitionMode = IM_INDIVIDUAL_COILS; // IM_WASTED_SPARK


	strncpy(config->luaScript, R"(

setTickRate(100)
t = Timer.new()
t : reset()

local data_0x329 = { 0x2C, 0x96, 0x80, 0x0E, 0x11, 0x2E, 0x00, 0x14 }
local slowCounter = 0
local slowRoll = 0

local slowRollTable = { 0x0C, 0x4F, 0x80, 0xE3 }

-- wakeup CAN messages
local data_0x382 = { 0x00, 0x3A, 0X44, 0x24, 0x00, 0x00, 0x00, 0x00 }
local data_0x0a0 = { 0x00, 0x68, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00 }
local data_0x0a1 = { 0x80, 0x80, 0x00, 0x00, 0xEC, 0x00, 0x00, 0x00 }
local data_0x18f = { 0xFA , 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
local data_0x545 = { 0xEC, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00 }
local data_0xA1 = { 0x80, 0x80, 0x00, 0x00, 0xEC, 0x00, 0x00, 0x00 }
local data_0xA0 = { 0x00, 0x68, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00 }
local data_0x260 = { 0x01, 0x1A, 0x1A, 0x10, 0x00, 0xAB, 0x93, 0x00 }
local data_0x2A0 = { 0x00, 0x00, 0x04, 0x00, 0x78, 0x05, 0x0D, 0x01 }

function onTick()

	-- keep alives
	txCan(1, 0x18f, 0, data_0x18f)
	txCan(1, 0x545, 0, data_0x545)
	-- more random can sends
	txCan(1, 0x545, 0, data_0x545)
	txCan(1, 0x382, 0, data_0x382)
	txCan(1, 0xA0, 0, data_0xA0)
	txCan(1, 0xA1, 0, data_0xA1)
	txCan(1, 0x260, 0, data_0x260)
	txCan(1, 0x2A0, 0, data_0x2A0)

	local RPMread = math.floor(getSensor("RPM") + 0.5) * 4

	local RPMhi = RPMread >> 8
	local RPMlo = RPMread & 0xff

	local CLTread = 50
	if getSensor("CLT") then
		CLTread = math.floor(getSensor("CLT") + 0.5)
	else
		CLTread = 50
	end

	local CLThi = CLTread
	local CLTlo = CLTread * 256

	canCLTpayloadNo = { 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	canCLTpayloadLo = { 0x00, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	canCLTpayloadHi = { 0x00, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	canCLTpayloadHi2 = { 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

	if slowCounter == 0 then
		-- cycle through 0, 40, 80, c0
		slowRoll = (slowRoll + 1) & 3
		data_0x329[1] = slowRollTable[slowRoll + 1]

		txCan(1, 0x329, 0, data_0x329)
		slowCounter = 30
	end
	slowCounter = slowCounter -1

	-- rpm fun stuff
	if t : getElapsedSeconds() < 1.5 then
		canRPMpayload = { 0x05, 0x1C, 0xFC, 0x7B, 0x1C, 0x2F, 0x00, 0x80 }
	else
		canRPMpayload = { 0x05, RPMlo, RPMhi, RPMhi, RPMlo, 0x2F, 0x00, 0x80 }
	end

	txCan(1, 0x316, 0, canRPMpayload)
	if CLTread < 60 then
		txCan(1, 0x608, 0, canCLTpayloadNo)
	elseif CLTread >= 60 and CLTread < 85 then
		txCan(1, 0x608, 0, canCLTpayloadLo)
	elseif CLTread >= 85 and CLTread < 110 then
		txCan(1, 0x608, 0, canCLTpayloadHi)
	else
		txCan(1, 0x608, 0, canCLTpayloadHi2)
	end
end


)", efi::size(config->luaScript));
}

void setGenesisCoupeBK1() {
	commonGenesisCoupe();
	// 2010-2012 315cc at 43.5psi
	// fuel system is fixed pressure 55psi
	engineConfiguration->injector.flow = 354.19; // https://www.google.com/search?q=315*sqrt%2855%2F43.5%29
	engineConfiguration->fuelReferencePressure = PSI2KPA(55);

}

void setGenesisCoupeBK2() {
	commonGenesisCoupe();
	// 2013+ 450cc at 43.5
	// fuel system is fixed pressure 85psi
	// flow rate P2 = flow rate P1 * sqrt(P2/P1)
	engineConfiguration->injector.flow = 629.03; // https://www.google.com/search?q=450*sqrt%2885%2F43.5%29
	engineConfiguration->fuelReferencePressure = PSI2KPA(85);

}
