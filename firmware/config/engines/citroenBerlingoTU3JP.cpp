/**
 * @file	citroenBerlingoTU3JP.cpp
 *
 * CITROEN_TU3JP
 * set engine_type 15
 * Board pin out for Frankenstein
 *
 * @date Apr 15, 2014
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#include "engine_math.h"
#include "thermistors.h"
#include "citroenBerlingoTU3JP.h"
#include "custom_engine.h"
#include "settings.h"

#if IGN_LOAD_COUNT == DEFAULT_IGN_LOAD_COUNT
static const ignition_table_t default_tps_advance_table = {
{/*0  engineLoad=0.00*/   /*0 800.0*/+15.00, /*1 1213.0*/+16.33, /*2 1626.0*/+17.67, /*3 2040.0*/+19.00, /*4 2453.0*/+20.33, /*5 2866.0*/+21.67, /*6 3280.0*/+23.00, /*7 3693.0*/+24.33, /*8 4106.0*/+25.67, /*9 4520.0*/+27.00, /*10 4933.0*/+28.33, /*11 5346.0*/+29.67, /*12 5760.0*/+31.00, /*13 6173.0*/+32.33, /*14 6586.0*/+33.67, /*15 7000.0*/+35.00},
{/*1  engineLoad=6.66*/	  /*0 800.0*/+14.80, /*1 1213.0*/+16.12, /*2 1626.0*/+17.45, /*3 2040.0*/+18.77, /*4 2453.0*/+20.10, /*5 2866.0*/+21.42, /*6 3280.0*/+22.75, /*7 3693.0*/+24.07, /*8 4106.0*/+25.40, /*9 4520.0*/+26.72, /*10 4933.0*/+28.04, /*11 5346.0*/+29.37, /*12 5760.0*/+30.69, /*13 6173.0*/+32.02, /*14 6586.0*/+33.34, /*15 7000.0*/+34.67},
{/*2  engineLoad=13.33*/  /*0 800.0*/+14.60, /*1 1213.0*/+15.92, /*2 1626.0*/+17.23, /*3 2040.0*/+18.55, /*4 2453.0*/+19.86, /*5 2866.0*/+21.18, /*6 3280.0*/+22.49, /*7 3693.0*/+23.81, /*8 4106.0*/+25.12, /*9 4520.0*/+26.44, /*10 4933.0*/+27.76, /*11 5346.0*/+29.07, /*12 5760.0*/+30.39, /*13 6173.0*/+31.70, /*14 6586.0*/+33.02, /*15 7000.0*/+34.33},
{/*3  engineLoad=20.00*/  /*0 800.0*/+14.40, /*1 1213.0*/+15.71, /*2 1626.0*/+17.01, /*3 2040.0*/+18.32, /*4 2453.0*/+19.63, /*5 2866.0*/+20.93, /*6 3280.0*/+22.24, /*7 3693.0*/+23.55, /*8 4106.0*/+24.85, /*9 4520.0*/+26.16, /*10 4933.0*/+27.47, /*11 5346.0*/+28.77, /*12 5760.0*/+30.08, /*13 6173.0*/+31.39, /*14 6586.0*/+32.69, /*15 7000.0*/+34.00},
{/*4  engineLoad=26.66*/  /*0 800.0*/+14.20, /*1 1213.0*/+15.50, /*2 1626.0*/+16.80, /*3 2040.0*/+18.09, /*4 2453.0*/+19.39, /*5 2866.0*/+20.69, /*6 3280.0*/+21.99, /*7 3693.0*/+23.28, /*8 4106.0*/+24.58, /*9 4520.0*/+25.88, /*10 4933.0*/+27.18, /*11 5346.0*/+28.48, /*12 5760.0*/+29.77, /*13 6173.0*/+31.07, /*14 6586.0*/+32.37, /*15 7000.0*/+33.67},
{/*5  engineLoad=33.33*/  /*0 800.0*/+14.00, /*1 1213.0*/+15.29, /*2 1626.0*/+16.58, /*3 2040.0*/+17.87, /*4 2453.0*/+19.16, /*5 2866.0*/+20.44, /*6 3280.0*/+21.73, /*7 3693.0*/+23.02, /*8 4106.0*/+24.31, /*9 4520.0*/+25.60, /*10 4933.0*/+26.89, /*11 5346.0*/+28.18, /*12 5760.0*/+29.47, /*13 6173.0*/+30.76, /*14 6586.0*/+32.04, /*15 7000.0*/+33.33},
{/*6  engineLoad=40.00*/  /*0 800.0*/+13.80, /*1 1213.0*/+15.08, /*2 1626.0*/+16.36, /*3 2040.0*/+17.64, /*4 2453.0*/+18.92, /*5 2866.0*/+20.20, /*6 3280.0*/+21.48, /*7 3693.0*/+22.76, /*8 4106.0*/+24.04, /*9 4520.0*/+25.32, /*10 4933.0*/+26.60, /*11 5346.0*/+27.88, /*12 5760.0*/+29.16, /*13 6173.0*/+30.44, /*14 6586.0*/+31.72, /*15 7000.0*/+33.00},
{/*7  engineLoad=46.66*/  /*0 800.0*/+13.60, /*1 1213.0*/+14.87, /*2 1626.0*/+16.14, /*3 2040.0*/+17.41, /*4 2453.0*/+18.68, /*5 2866.0*/+19.96, /*6 3280.0*/+21.23, /*7 3693.0*/+22.50, /*8 4106.0*/+23.77, /*9 4520.0*/+25.04, /*10 4933.0*/+26.31, /*11 5346.0*/+27.58, /*12 5760.0*/+28.85, /*13 6173.0*/+30.12, /*14 6586.0*/+31.40, /*15 7000.0*/+32.67},
{/*8  engineLoad=53.33*/  /*0 800.0*/+13.40, /*1 1213.0*/+14.66, /*2 1626.0*/+15.92, /*3 2040.0*/+17.19, /*4 2453.0*/+18.45, /*5 2866.0*/+19.71, /*6 3280.0*/+20.97, /*7 3693.0*/+22.24, /*8 4106.0*/+23.50, /*9 4520.0*/+24.76, /*10 4933.0*/+26.02, /*11 5346.0*/+27.28, /*12 5760.0*/+28.55, /*13 6173.0*/+29.81, /*14 6586.0*/+31.07, /*15 7000.0*/+32.33},
{/*9  engineLoad=60.00*/  /*0 800.0*/+13.20, /*1 1213.0*/+14.45, /*2 1626.0*/+15.71, /*3 2040.0*/+16.96, /*4 2453.0*/+18.21, /*5 2866.0*/+19.47, /*6 3280.0*/+20.72, /*7 3693.0*/+21.97, /*8 4106.0*/+23.23, /*9 4520.0*/+24.48, /*10 4933.0*/+25.73, /*11 5346.0*/+26.99, /*12 5760.0*/+28.24, /*13 6173.0*/+29.49, /*14 6586.0*/+30.75, /*15 7000.0*/+32.00},
{/*10 engineLoad=66.66*/  /*0 800.0*/+13.00, /*1 1213.0*/+14.24, /*2 1626.0*/+15.49, /*3 2040.0*/+16.73, /*4 2453.0*/+17.98, /*5 2866.0*/+19.22, /*6 3280.0*/+20.47, /*7 3693.0*/+21.71, /*8 4106.0*/+22.96, /*9 4520.0*/+24.20, /*10 4933.0*/+25.44, /*11 5346.0*/+26.69, /*12 5760.0*/+27.93, /*13 6173.0*/+29.18, /*14 6586.0*/+30.42, /*15 7000.0*/+31.67},
{/*11 engineLoad=73.33*/  /*0 800.0*/+12.80, /*1 1213.0*/+14.04, /*2 1626.0*/+15.27, /*3 2040.0*/+16.51, /*4 2453.0*/+17.74, /*5 2866.0*/+18.98, /*6 3280.0*/+20.21, /*7 3693.0*/+21.45, /*8 4106.0*/+22.68, /*9 4520.0*/+23.92, /*10 4933.0*/+25.16, /*11 5346.0*/+26.39, /*12 5760.0*/+27.63, /*13 6173.0*/+28.86, /*14 6586.0*/+30.10, /*15 7000.0*/+31.33},
{/*12 engineLoad=80.00*/  /*0 800.0*/+12.60, /*1 1213.0*/+13.83, /*2 1626.0*/+15.05, /*3 2040.0*/+16.28, /*4 2453.0*/+17.51, /*5 2866.0*/+18.73, /*6 3280.0*/+19.96, /*7 3693.0*/+21.19, /*8 4106.0*/+22.41, /*9 4520.0*/+23.64, /*10 4933.0*/+24.87, /*11 5346.0*/+26.09, /*12 5760.0*/+27.32, /*13 6173.0*/+28.55, /*14 6586.0*/+29.77, /*15 7000.0*/+31.00},
{/*13 engineLoad=86.66*/  /*0 800.0*/+12.40, /*1 1213.0*/+13.62, /*2 1626.0*/+14.84, /*3 2040.0*/+16.05, /*4 2453.0*/+17.27, /*5 2866.0*/+18.49, /*6 3280.0*/+19.71, /*7 3693.0*/+20.92, /*8 4106.0*/+22.14, /*9 4520.0*/+23.36, /*10 4933.0*/+24.58, /*11 5346.0*/+25.80, /*12 5760.0*/+27.01, /*13 6173.0*/+28.23, /*14 6586.0*/+29.45, /*15 7000.0*/+30.67},
{/*14 engineLoad=93.33*/  /*0 800.0*/+12.20, /*1 1213.0*/+13.41, /*2 1626.0*/+14.62, /*3 2040.0*/+15.83, /*4 2453.0*/+17.04, /*5 2866.0*/+18.24, /*6 3280.0*/+19.45, /*7 3693.0*/+20.66, /*8 4106.0*/+21.87, /*9 4520.0*/+23.08, /*10 4933.0*/+24.29, /*11 5346.0*/+25.50, /*12 5760.0*/+26.71, /*13 6173.0*/+27.92, /*14 6586.0*/+29.12, /*15 7000.0*/+30.33},
{/*15 engineLoad=100.00*/ /*0 800.0*/+12.00, /*1 1213.0*/+13.20, /*2 1626.0*/+14.40, /*3 2040.0*/+15.60, /*4 2453.0*/+16.80, /*5 2866.0*/+18.00, /*6 3280.0*/+19.20, /*7 3693.0*/+20.40, /*8 4106.0*/+21.60, /*9 4520.0*/+22.80, /*10 4933.0*/+24.00, /*11 5346.0*/+25.20, /*12 5760.0*/+26.40, /*13 6173.0*/+27.60, /*14 6586.0*/+28.80, /*15 7000.0*/+30.00}
};
#endif

//static const float rpmSteps[16] = {400, 800, 1200, 1600, 2000, 2400, 2800, 3200, 3600, 4000, 4400, 4800, 5200, 5600, 6000, 6400};
//static const float mapSteps[16] = {25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100};

EXTERN_CONFIG;

void setCitroenBerlingoTU3JPConfiguration(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	setDefaultFrankensoConfiguration(PASS_CONFIG_PARAMETER_SIGNATURE);

	engineConfiguration->engineType = CITROEN_TU3JP;

	/**
	 * Base engine setting
	 */
	setOperationMode(engineConfiguration, FOUR_STROKE_CRANK_SENSOR);
	engineConfiguration->trigger.type = TT_TOOTHED_WHEEL_60_2;
	engineConfiguration->globalTriggerAngleOffset = 114;
	engineConfiguration->specs.cylindersCount = 4;
	engineConfiguration->specs.displacement = 1.360;
	engineConfiguration->specs.firingOrder = FO_1_3_4_2;
	engineConfiguration->ignitionMode = IM_WASTED_SPARK;
	engineConfiguration->injectionMode = IM_BATCH;
	engineConfiguration->crankingInjectionMode = IM_SIMULTANEOUS;
	engineConfiguration->rpmHardLimit = 5000;
	engineConfiguration->cranking.rpm = 600;

//	memcpy(config->ve2RpmBins, rpmSteps, sizeof(rpmSteps));

	/**
	 * Algorithm Alpha-N setting
	 */
	setAlgorithm(LM_ALPHA_N PASS_CONFIG_PARAMETER_SUFFIX);
	setTimingRpmBin(800, 7000 PASS_CONFIG_PARAMETER_SUFFIX);

	/**
	 * Outputs
	 */

	// Frankenstein lo-side output #1: PC14 Igniter 1-4
	// Frankenstein lo-side output #2: PC15 Igniter 2-3
	// Frankenstein lo-side output #3: PE6  Injector 1-4
	// Frankenstein lo-side output #4: PC13 Injector 2-3
	// Frankenstein lo-side output #5: PE4
	// Frankenstein lo-side output #6: PE5
	// Frankenstein lo-side output #7: PE2
	// Frankenstein lo-side output #8: PE3	Tachometer
	// Frankenstein lo-side output #9: PE0	Fan
	// Frankenstein lo-side output #10: PE1	MIL
	// Frankenstein lo-side output #11: PB8	Main relay
	// Frankenstein lo-side output #12: PB9	Fuel pump

	engineConfiguration->ignitionPins[0] = GPIOC_14;
	engineConfiguration->ignitionPins[1] = GPIO_UNASSIGNED;
	engineConfiguration->ignitionPins[2] = GPIOC_15;
	engineConfiguration->ignitionPins[3] = GPIO_UNASSIGNED;

	engineConfiguration->injector.flow = 137; //SIEMENS DEKA VAZ20734
	engineConfiguration->injectionPins[0] = GPIOE_6;
	engineConfiguration->injectionPins[1] = GPIOC_13;
	engineConfiguration->injectionPins[2] = GPIO_UNASSIGNED;
	engineConfiguration->injectionPins[3] = GPIO_UNASSIGNED;

	engineConfiguration->fanPin = GPIOE_0;
	engineConfiguration->fanPinMode = OM_DEFAULT;
	engineConfiguration->fanOffTemperature = 95;
	engineConfiguration->fanOnTemperature = 99;

	engineConfiguration->malfunctionIndicatorPin = GPIOE_1;
	engineConfiguration->malfunctionIndicatorPinMode = OM_DEFAULT;

	engineConfiguration->mainRelayPin = GPIOB_8;

	engineConfiguration->fuelPumpPin = GPIOB_9;
	engineConfiguration->fuelPumpPinMode = OM_DEFAULT;

	setFrankenso_01_LCD(engineConfiguration);


//	engineConfiguration->o2heaterPin = GPIOC_13;
//	engineConfiguration->logicAnalyzerPins[1] = GPIO_UNASSIGNED;

	/**
	 * Inputs
	 */

	// See https://docs.google.com/spreadsheet/ccc?key=0Arl1FeMZcfisdEdGdUlHdWh6cVBoSzFIbkxqa1QtZ3c
	// Frankenstein analog input #1: PA1		adc1	MAP
	// Frankenstein analog input #2: PA3		adc3	TPS
	// Frankenstein analog input #3: PC3		adc13	IAT
	// Frankenstein analog input #4: PC1		adc11	CLT
	// Frankenstein analog input #5: PA0		adc0	vBatt
	// Frankenstein analog input #6: PC2		adc12	WBO
	// Frankenstein analog input #7: PA4		adc4
	// Frankenstein analog input #8: PA2		adc2
	// Frankenstein analog input #9: PA6		adc6
	// Frankenstein analog input #10: PA7		adc7
	// Frankenstein analog input #11: PC4		adc14
	// Frankenstein analog input #12: PC5|PA8	adc15	Speed Sensor

	/**
	 * MAP <BOSCH 0 261 230 057>
	 */
	engineConfiguration->map.sensor.hwChannel = EFI_ADC_1;
	engineConfiguration->map.sensor.type = MT_CUSTOM;
	engineConfiguration->map.sensor.lowValue = 10;
	engineConfiguration->map.sensor.highValue = 105;
	engineConfiguration->mapHighValueVoltage = 5;
	/**
	 * TPS <MAGNETI MARELLI>
	 */
	engineConfiguration->tps1_1AdcChannel = EFI_ADC_3;
	engineConfiguration->tpsMax = 102; // convert 12to10 bit (ADC/4)
	engineConfiguration->tpsMin = 825; // convert 12to10 bit (ADC/4)
	/**
	 * IAT <OEM ECU>
	 */
	engineConfiguration->iat.adcChannel = EFI_ADC_13;
	engineConfiguration->iat.config = {-20.0, 23.0, 92.0, 15600.0, 2250.0, 240.0, 2660};
	/**
	* CLT <LADA Samara>
	*/
	engineConfiguration->clt.adcChannel = EFI_ADC_11;
	engineConfiguration->clt.config = {-20.0, 25.0, 100.0, 28680.0, 2796.0, 177.0, 2660};
	/**
	 * vBatt
	 */
	engineConfiguration->vbattAdcChannel = EFI_ADC_0;
	engineConfiguration->vbattDividerCoeff = ((float) (2.6 + 10.1)) / 2.6 * 2;
	/**
	* WBO Innovate LC-1
	*/
	engineConfiguration->afr.hwChannel = EFI_ADC_12;
	/**
	* Speed Sensor
	*/
	engineConfiguration->vehicleSpeedSensorInputPin = GPIOA_8;
	/**
	* Other
	*/
//	engineConfiguration->mafAdcChannel = GPIO_UNASSIGNED;

#if IGN_LOAD_COUNT == DEFAULT_IGN_LOAD_COUNT
	MEMCPY(config->ignitionTable, default_tps_advance_table);
#endif
}
