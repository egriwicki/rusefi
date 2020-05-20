/*
 * @file	mazda_miata_vvt.cpp
 *
 * Miata NB2, also known as MX-5 Mk2.5
 *
 * MAZDA_MIATA_2003
 * set engine_type 47
 *
 * coil1/4          (p1 +5 VP)    GPIOE_14
 * coil2/2          (p1 +5 VP)    GPIOC_9
 * tachometer +5 VP (p3 +12 VP)   GPIOE_8
 * alternator +5 VP (p3 +12 VP)   GPIOE_10
 * ETB PWM                        GPIOE_6 inverted low-side with pull-up
 * ETB dir1                       GPIOE_12
 * ETB dir2                       GPIOC_7
 *
 * COP ion #1                     GPIOD_8
 * COP ion #3                     GPIOD_9
 *
 * @date Oct 4, 2016
 * @author Andrey Belomutskiy, (c) 2012-2020
 * http://rusefi.com/forum/viewtopic.php?f=3&t=1095
 *
 *
 * See also TT_MAZDA_MIATA_VVT_TEST for trigger simulation
 *
 * Based on http://rusefi.com/wiki/index.php?title=Manual:Hardware_Frankenso_board#Default_Pinout
 *
 * board #70 - red car, hunchback compatible
 * set engine_type 55
 *
 * Crank   primary trigger        PA5 (3E in Miata board)       white
 * Cam     vvt input              PC6 (3G in Miata board)       blue
 * Wideband input                 PA3 (3J in Miata board)
 *
 * coil1/4          (p1 +5 VP)    GPIOE_14
 * coil2/2          (p1 +5 VP)    GPIOC_7
 *
 * tachometer +5 VP (p3 +12 VP)   GPIOE_8
 * alternator +5 VP (p3 +12 VP)   GPIOE_10
 *
 * VVT solenoid on aux PID#1      GPIOE_3
 * warning light                  GPIOE_6
 *
 *
 * idle solenoid                  PC13 on middle harness plug. diodes seem to be in the harness
 */

#include "mazda_miata_vvt.h"
#include "custom_engine.h"
#include "fsio_impl.h"
#include "ego.h"
#include "thermistors.h"

EXTERN_CONFIG;

static const float injectorLagBins[VBAT_INJECTOR_CURVE_SIZE] = {
        6.0,         8.0,        10.0,        11.0,
        12.0,        13.0,  14.0,        15.0
};

static const float injectorLagCorrection[VBAT_INJECTOR_CURVE_SIZE] = {
        4.0 ,        3.0 ,        2.0 ,        1.7,
        1.5 ,        1.35,        1.25 ,        1.20
};

static const float vvt18fsioRpmBins[FSIO_TABLE_8] =
{700.0, 1000.0, 2000.0, 3000.0, 3500.0, 4500.0, 5500.0, 6500.0}
;

static const float vvt18fsioLoadBins[FSIO_TABLE_8] =
{30.0, 40.0, 50.0, 60.0, 70.0, 75.0, 82.0, 85.0}
;

float const fsio_table_vvt_target[FSIO_TABLE_8][FSIO_TABLE_8] = {
		/* Generated by TS2C on Mon Feb 13 19:11:32 EST 2017*/
		{/* 0 30.000	*//* 0 700.0*/1.000,	/* 1 1000.0*/3.000,	/* 2 2000.0*/10.000,	/* 3 3000.0*/20.000,	/* 4 3500.0*/27.000,	/* 5 4500.0*/28.000,	/* 6 5500.0*/11.000,	/* 7 6500.0*/5.000,	},
		{/* 1 40.000	*//* 0 700.0*/3.000,	/* 1 1000.0*/10.000,	/* 2 2000.0*/19.000,	/* 3 3000.0*/26.000,	/* 4 3500.0*/30.000,	/* 5 4500.0*/28.000,	/* 6 5500.0*/11.000,	/* 7 6500.0*/5.000,	},
		{/* 2 50.000	*//* 0 700.0*/7.000,	/* 1 1000.0*/16.000,	/* 2 2000.0*/24.000,	/* 3 3000.0*/28.000,	/* 4 3500.0*/30.000,	/* 5 4500.0*/28.000,	/* 6 5500.0*/11.000,	/* 7 6500.0*/5.000,	},
		{/* 3 60.000	*//* 0 700.0*/11.000,	/* 1 1000.0*/20.000,	/* 2 2000.0*/27.000,	/* 3 3000.0*/28.000,	/* 4 3500.0*/30.000,	/* 5 4500.0*/28.000,	/* 6 5500.0*/11.000,	/* 7 6500.0*/5.000,	},
		{/* 4 70.000	*//* 0 700.0*/13.000,	/* 1 1000.0*/24.000,	/* 2 2000.0*/31.000,	/* 3 3000.0*/28.000,	/* 4 3500.0*/30.000,	/* 5 4500.0*/28.000,	/* 6 5500.0*/11.000,	/* 7 6500.0*/5.000,	},
		{/* 5 75.000	*//* 0 700.0*/15.000,	/* 1 1000.0*/27.000,	/* 2 2000.0*/33.000,	/* 3 3000.0*/28.000,	/* 4 3500.0*/30.000,	/* 5 4500.0*/28.000,	/* 6 5500.0*/11.000,	/* 7 6500.0*/5.000,	},
		{/* 6 82.000	*//* 0 700.0*/17.000,	/* 1 1000.0*/28.000,	/* 2 2000.0*/33.000,	/* 3 3000.0*/28.000,	/* 4 3500.0*/30.000,	/* 5 4500.0*/28.000,	/* 6 5500.0*/11.000,	/* 7 6500.0*/5.000,	},
		{/* 7 85.000	*//* 0 700.0*/17.000,	/* 1 1000.0*/28.000,	/* 2 2000.0*/33.000,	/* 3 3000.0*/28.000,	/* 4 3500.0*/30.000,	/* 5 4500.0*/28.000,	/* 6 5500.0*/11.000,	/* 7 6500.0*/5.000,	},
};

static const float tpsTspSame[TPS_TPS_ACCEL_TABLE] =
{0.0, 10.0, 20.0, 35.0, 50.0, 65.0, 80.0, 100.0};

float const tpsTpsTable[TPS_TPS_ACCEL_TABLE][TPS_TPS_ACCEL_TABLE] = {
/* Generated by TS2C on Tue Apr 18 21:29:16 EDT 2017*/
{/* 0 0.000	*//* 0 0.0*/0.000,	/* 1 10.0*/0.000,	/* 2 20.0*/0.000,	/* 3 35.0*/0.000,	/* 4 50.0*/0.000,	/* 5 65.0*/0.000,	/* 6 80.0*/0.000,	/* 7 100.0*/0.000,	},
{/* 1 10.000	*//* 0 0.0*/10.000,	/* 1 10.0*/0.000,	/* 2 20.0*/0.000,	/* 3 35.0*/0.000,	/* 4 50.0*/0.000,	/* 5 65.0*/0.000,	/* 6 80.0*/0.000,	/* 7 100.0*/0.000,	},
{/* 2 20.000	*//* 0 0.0*/17.000,	/* 1 10.0*/10.000,	/* 2 20.0*/0.000,	/* 3 35.0*/0.000,	/* 4 50.0*/0.000,	/* 5 65.0*/0.000,	/* 6 80.0*/0.000,	/* 7 100.0*/0.000,	},
{/* 3 35.000	*//* 0 0.0*/23.000,	/* 1 10.0*/17.000,	/* 2 20.0*/10.000,	/* 3 35.0*/0.000,	/* 4 50.0*/0.000,	/* 5 65.0*/0.000,	/* 6 80.0*/0.000,	/* 7 100.0*/0.000,	},
{/* 4 50.000	*//* 0 0.0*/28.000,	/* 1 10.0*/23.000,	/* 2 20.0*/17.000,	/* 3 35.0*/10.000,	/* 4 50.0*/0.000,	/* 5 65.0*/0.000,	/* 6 80.0*/0.000,	/* 7 100.0*/0.000,	},
{/* 5 65.000	*//* 0 0.0*/32.000,	/* 1 10.0*/28.000,	/* 2 20.0*/23.000,	/* 3 35.0*/17.000,	/* 4 50.0*/10.000,	/* 5 65.0*/0.000,	/* 6 80.0*/0.000,	/* 7 100.0*/0.000,	},
{/* 6 80.000	*//* 0 0.0*/35.000,	/* 1 10.0*/32.000,	/* 2 20.0*/28.000,	/* 3 35.0*/23.000,	/* 4 50.0*/17.000,	/* 5 65.0*/10.000,	/* 6 80.0*/0.000,	/* 7 100.0*/0.000,	},
{/* 7 100.000	*//* 0 0.0*/37.000,	/* 1 10.0*/35.000,	/* 2 20.0*/32.000,	/* 3 35.0*/28.000,	/* 4 50.0*/23.000,	/* 5 65.0*/17.000,	/* 6 80.0*/10.000,	/* 7 100.0*/0.000,	},
};


const float mazda_miata_nb2_RpmBins[FUEL_RPM_COUNT] = {700.0, 820.0, 950.0, 1100.0,
		1300.0, 1550.0, 1800.0, 2150.0,
		2500.0, 3000.0, 3500.0, 4150.0,
		4900.0, 5800.0, 6800.0, 8000.0}
;

const float mazda_miata_nb2_LoadBins[FUEL_LOAD_COUNT] = {20.0, 25.0, 30.0, 35.0,
		40.0, 46.0, 54.0, 63.0,
		73.0, 85.0, 99.0, 116.0,
		135.0, 158.0, 185.0, 220.0}
;

static const  float ignition18vvtRpmBins[FUEL_RPM_COUNT] = {
		700.0, 		         850.0 ,		         943.0 ,
		         1112.0 ,		         1310.0 ,		         1545.0 ,
		         1821.0, 		         2146.0, 		         2530.0,
		         2982.0, 		         3515.0 ,		         4144.0 ,
		         4884.0 ,		         5757.0 ,		         6787.0, 		         8000.0};

static const float ignition18vvtLoadBins[FUEL_LOAD_COUNT] = {
		25.0 ,		         29.10009765625 ,		         34.0 ,		         39.60009765625 ,
		         46.2001953125 ,		         53.89990234375 ,		         62.7998046875 ,
				 73.2001953125 ,		         85.400390625 ,		         99.5 ,		         116.0 ,
		         135.30078125 ,		         157.69921875 ,		         183.900390625 ,		         214.400390625 ,
		         250.0};

static const fuel_table_t mapBased18vvtVeTable_NB_fuel_rail = {
		/* Generated by TS2C on Tue Apr 18 21:46:03 EDT 2017*/
		{/* 0 20.000	*//* 0 700.0*/35.685,	/* 1 820.0*/36.982,	/* 2 950.0*/37.200,	/* 3 1100.0*/35.790,	/* 4 1300.0*/36.438,	/* 5 1550.0*/37.272,	/* 6 1800.0*/33.458,	/* 7 2150.0*/31.345,	/* 8 2500.0*/25.367,	/* 9 3000.0*/24.573,	/* 10 3500.0*/24.560,	/* 11 4150.0*/25.705,	/* 12 4900.0*/26.587,	/* 13 5800.0*/29.273,	/* 14 6800.0*/33.646,	/* 15 8000.0*/36.896,	},
		{/* 1 25.000	*//* 0 700.0*/35.535,	/* 1 820.0*/37.952,	/* 2 950.0*/38.225,	/* 3 1100.0*/37.100,	/* 4 1300.0*/36.868,	/* 5 1550.0*/37.290,	/* 6 1800.0*/41.785,	/* 7 2150.0*/39.520,	/* 8 2500.0*/40.022,	/* 9 3000.0*/37.265,	/* 10 3500.0*/35.565,	/* 11 4150.0*/36.272,	/* 12 4900.0*/37.039,	/* 13 5800.0*/35.710,	/* 14 6800.0*/38.212,	/* 15 8000.0*/40.621,	},
		{/* 2 30.000	*//* 0 700.0*/37.748,	/* 1 820.0*/40.882,	/* 2 950.0*/39.945,	/* 3 1100.0*/37.788,	/* 4 1300.0*/38.893,	/* 5 1550.0*/41.615,	/* 6 1800.0*/45.105,	/* 7 2150.0*/47.712,	/* 8 2500.0*/54.600,	/* 9 3000.0*/48.670,	/* 10 3500.0*/47.853,	/* 11 4150.0*/55.655,	/* 12 4900.0*/55.498,	/* 13 5800.0*/49.273,	/* 14 6800.0*/50.331,	/* 15 8000.0*/51.444,	},
		{/* 3 35.000	*//* 0 700.0*/39.675,	/* 1 820.0*/44.125,	/* 2 950.0*/42.123,	/* 3 1100.0*/40.857,	/* 4 1300.0*/45.520,	/* 5 1550.0*/48.538,	/* 6 1800.0*/48.993,	/* 7 2150.0*/52.555,	/* 8 2500.0*/56.085,	/* 9 3000.0*/53.197,	/* 10 3500.0*/52.382,	/* 11 4150.0*/58.470,	/* 12 4900.0*/62.063,	/* 13 5800.0*/57.389,	/* 14 6800.0*/58.854,	/* 15 8000.0*/58.917,	},
		{/* 4 40.000	*//* 0 700.0*/45.470,	/* 1 820.0*/56.098,	/* 2 950.0*/49.000,	/* 3 1100.0*/45.075,	/* 4 1300.0*/54.000,	/* 5 1550.0*/53.248,	/* 6 1800.0*/55.022,	/* 7 2150.0*/54.513,	/* 8 2500.0*/57.425,	/* 9 3000.0*/55.402,	/* 10 3500.0*/57.045,	/* 11 4150.0*/59.667,	/* 12 4900.0*/62.945,	/* 13 5800.0*/59.550,	/* 14 6800.0*/63.474,	/* 15 8000.0*/62.896,	},
		{/* 5 46.000	*//* 0 700.0*/54.595,	/* 1 820.0*/61.655,	/* 2 950.0*/56.473,	/* 3 1100.0*/52.265,	/* 4 1300.0*/53.317,	/* 5 1550.0*/58.608,	/* 6 1800.0*/57.620,	/* 7 2150.0*/59.022,	/* 8 2500.0*/58.695,	/* 9 3000.0*/58.413,	/* 10 3500.0*/60.002,	/* 11 4150.0*/64.660,	/* 12 4900.0*/66.688,	/* 13 5800.0*/64.357,	/* 14 6800.0*/65.124,	/* 15 8000.0*/63.864,	},
		{/* 6 54.000	*//* 0 700.0*/60.197,	/* 1 820.0*/67.792,	/* 2 950.0*/66.027,	/* 3 1100.0*/60.100,	/* 4 1300.0*/59.155,	/* 5 1550.0*/59.607,	/* 6 1800.0*/61.960,	/* 7 2150.0*/63.623,	/* 8 2500.0*/63.118,	/* 9 3000.0*/60.965,	/* 10 3500.0*/62.195,	/* 11 4150.0*/69.045,	/* 12 4900.0*/71.032,	/* 13 5800.0*/67.183,	/* 14 6800.0*/65.605,	/* 15 8000.0*/63.877,	},
		{/* 7 63.000	*//* 0 700.0*/65.607,	/* 1 820.0*/70.670,	/* 2 950.0*/71.193,	/* 3 1100.0*/67.927,	/* 4 1300.0*/62.265,	/* 5 1550.0*/61.768,	/* 6 1800.0*/65.190,	/* 7 2150.0*/63.987,	/* 8 2500.0*/63.945,	/* 9 3000.0*/64.695,	/* 10 3500.0*/66.277,	/* 11 4150.0*/69.593,	/* 12 4900.0*/73.335,	/* 13 5800.0*/71.135,	/* 14 6800.0*/67.308,	/* 15 8000.0*/65.571,	},
		{/* 8 73.000	*//* 0 700.0*/70.760,	/* 1 820.0*/74.892,	/* 2 950.0*/73.700,	/* 3 1100.0*/75.399,	/* 4 1300.0*/71.755,	/* 5 1550.0*/66.492,	/* 6 1800.0*/66.218,	/* 7 2150.0*/65.995,	/* 8 2500.0*/67.402,	/* 9 3000.0*/69.588,	/* 10 3500.0*/68.407,	/* 11 4150.0*/72.383,	/* 12 4900.0*/76.445,	/* 13 5800.0*/75.327,	/* 14 6800.0*/66.716,	/* 15 8000.0*/65.095,	},
		{/* 9 85.000	*//* 0 700.0*/71.825,	/* 1 820.0*/75.935,	/* 2 950.0*/76.701,	/* 3 1100.0*/74.133,	/* 4 1300.0*/73.258,	/* 5 1550.0*/72.668,	/* 6 1800.0*/71.280,	/* 7 2150.0*/70.157,	/* 8 2500.0*/72.560,	/* 9 3000.0*/72.457,	/* 10 3500.0*/74.947,	/* 11 4150.0*/76.565,	/* 12 4900.0*/78.225,	/* 13 5800.0*/76.108,	/* 14 6800.0*/68.455,	/* 15 8000.0*/64.913,	},
		{/* 10 99.000	*//* 0 700.0*/75.315,	/* 1 820.0*/76.670,	/* 2 950.0*/78.845,	/* 3 1100.0*/76.120,	/* 4 1300.0*/73.420,	/* 5 1550.0*/74.563,	/* 6 1800.0*/74.585,	/* 7 2150.0*/74.285,	/* 8 2500.0*/77.522,	/* 9 3000.0*/76.202,	/* 10 3500.0*/77.563,	/* 11 4150.0*/76.135,	/* 12 4900.0*/77.720,	/* 13 5800.0*/76.672,	/* 14 6800.0*/69.815,	/* 15 8000.0*/65.180,	},
		{/* 11 116.000	*//* 0 700.0*/80.000,	/* 1 820.0*/80.000,	/* 2 950.0*/80.000,	/* 3 1100.0*/80.000,	/* 4 1300.0*/80.000,	/* 5 1550.0*/80.000,	/* 6 1800.0*/80.000,	/* 7 2150.0*/80.000,	/* 8 2500.0*/80.000,	/* 9 3000.0*/80.000,	/* 10 3500.0*/80.000,	/* 11 4150.0*/80.000,	/* 12 4900.0*/80.000,	/* 13 5800.0*/80.000,	/* 14 6800.0*/80.000,	/* 15 8000.0*/80.000,	},
		{/* 12 135.000	*//* 0 700.0*/80.000,	/* 1 820.0*/80.000,	/* 2 950.0*/80.000,	/* 3 1100.0*/80.000,	/* 4 1300.0*/80.000,	/* 5 1550.0*/80.000,	/* 6 1800.0*/80.000,	/* 7 2150.0*/80.000,	/* 8 2500.0*/80.000,	/* 9 3000.0*/80.000,	/* 10 3500.0*/80.000,	/* 11 4150.0*/80.000,	/* 12 4900.0*/80.000,	/* 13 5800.0*/80.000,	/* 14 6800.0*/80.000,	/* 15 8000.0*/80.000,	},
		{/* 13 158.000	*//* 0 700.0*/80.000,	/* 1 820.0*/80.000,	/* 2 950.0*/80.000,	/* 3 1100.0*/80.000,	/* 4 1300.0*/80.000,	/* 5 1550.0*/80.000,	/* 6 1800.0*/80.000,	/* 7 2150.0*/80.000,	/* 8 2500.0*/80.000,	/* 9 3000.0*/80.000,	/* 10 3500.0*/80.000,	/* 11 4150.0*/80.000,	/* 12 4900.0*/80.000,	/* 13 5800.0*/80.000,	/* 14 6800.0*/80.000,	/* 15 8000.0*/80.000,	},
		{/* 14 185.000	*//* 0 700.0*/80.000,	/* 1 820.0*/80.000,	/* 2 950.0*/80.000,	/* 3 1100.0*/80.000,	/* 4 1300.0*/80.000,	/* 5 1550.0*/80.000,	/* 6 1800.0*/80.000,	/* 7 2150.0*/80.000,	/* 8 2500.0*/80.000,	/* 9 3000.0*/80.000,	/* 10 3500.0*/80.000,	/* 11 4150.0*/80.000,	/* 12 4900.0*/80.000,	/* 13 5800.0*/80.000,	/* 14 6800.0*/80.000,	/* 15 8000.0*/80.000,	},
		{/* 15 220.000	*//* 0 700.0*/80.000,	/* 1 820.0*/80.000,	/* 2 950.0*/80.000,	/* 3 1100.0*/80.000,	/* 4 1300.0*/80.000,	/* 5 1550.0*/80.000,	/* 6 1800.0*/80.000,	/* 7 2150.0*/80.000,	/* 8 2500.0*/80.000,	/* 9 3000.0*/80.000,	/* 10 3500.0*/80.000,	/* 11 4150.0*/80.000,	/* 12 4900.0*/80.000,	/* 13 5800.0*/80.000,	/* 14 6800.0*/80.000,	/* 15 8000.0*/80.000,	},
};

#if IGN_LOAD_COUNT == DEFAULT_IGN_LOAD_COUNT
static const ignition_table_t mapBased18vvtTimingTable = {
		/* Generated by TS2C on Tue Apr 18 21:43:57 EDT 2017*/
		{/* 0 25.000	*//* 0 700.0*/14.800,	/* 1 850.0*/13.000,	/* 2 943.0*/13.000,	/* 3 1112.0*/16.000,	/* 4 1310.0*/21.500,	/* 5 1545.0*/25.100,	/* 6 1821.0*/28.300,	/* 7 2146.0*/31.460,	/* 8 2530.0*/34.370,	/* 9 2982.0*/36.598,	/* 10 3515.0*/38.698,	/* 11 4144.0*/39.728,	/* 12 4884.0*/40.346,	/* 13 5757.0*/40.758,	/* 14 6787.0*/40.964,	/* 15 8000.0*/41.170,	},
		{/* 1 29.100	*//* 0 700.0*/14.800,	/* 1 850.0*/13.000,	/* 2 943.0*/13.000,	/* 3 1112.0*/16.100,	/* 4 1310.0*/21.500,	/* 5 1545.0*/25.000,	/* 6 1821.0*/28.200,	/* 7 2146.0*/31.270,	/* 8 2530.0*/34.190,	/* 9 2982.0*/36.210,	/* 10 3515.0*/38.286,	/* 11 4144.0*/39.316,	/* 12 4884.0*/40.037,	/* 13 5757.0*/40.552,	/* 14 6787.0*/40.758,	/* 15 8000.0*/40.964,	},
		{/* 2 34.000	*//* 0 700.0*/14.800,	/* 1 850.0*/13.000,	/* 2 943.0*/13.000,	/* 3 1112.0*/16.830,	/* 4 1310.0*/21.600,	/* 5 1545.0*/24.800,	/* 6 1821.0*/27.800,	/* 7 2146.0*/30.790,	/* 8 2530.0*/33.540,	/* 9 2982.0*/35.888,	/* 10 3515.0*/37.771,	/* 11 4144.0*/38.801,	/* 12 4884.0*/39.522,	/* 13 5757.0*/40.140,	/* 14 6787.0*/40.346,	/* 15 8000.0*/40.552,	},
		{/* 3 39.600	*//* 0 700.0*/15.200,	/* 1 850.0*/13.436,	/* 2 943.0*/13.368,	/* 3 1112.0*/17.520,	/* 4 1310.0*/21.700,	/* 5 1545.0*/24.600,	/* 6 1821.0*/27.400,	/* 7 2146.0*/30.140,	/* 8 2530.0*/33.010,	/* 9 2982.0*/35.072,	/* 10 3515.0*/36.947,	/* 11 4144.0*/38.183,	/* 12 4884.0*/38.904,	/* 13 5757.0*/39.522,	/* 14 6787.0*/39.728,	/* 15 8000.0*/39.934,	},
		{/* 4 46.200	*//* 0 700.0*/15.500,	/* 1 850.0*/13.949,	/* 2 943.0*/13.801,	/* 3 1112.0*/18.450,	/* 4 1310.0*/21.800,	/* 5 1545.0*/24.400,	/* 6 1821.0*/26.900,	/* 7 2146.0*/29.490,	/* 8 2530.0*/32.270,	/* 9 2982.0*/33.944,	/* 10 3515.0*/36.123,	/* 11 4144.0*/37.359,	/* 12 4884.0*/38.183,	/* 13 5757.0*/38.698,	/* 14 6787.0*/38.904,	/* 15 8000.0*/39.213,	},
		{/* 5 53.900	*//* 0 700.0*/15.700,	/* 1 850.0*/14.549,	/* 2 943.0*/14.306,	/* 3 1112.0*/18.970,	/* 4 1310.0*/21.900,	/* 5 1545.0*/24.000,	/* 6 1821.0*/26.200,	/* 7 2146.0*/28.770,	/* 8 2530.0*/30.950,	/* 9 2982.0*/32.926,	/* 10 3515.0*/34.990,	/* 11 4144.0*/36.226,	/* 12 4884.0*/37.050,	/* 13 5757.0*/37.771,	/* 14 6787.0*/38.080,	/* 15 8000.0*/38.492,	},
		{/* 6 62.800	*//* 0 700.0*/15.900,	/* 1 850.0*/15.241,	/* 2 943.0*/14.891,	/* 3 1112.0*/19.520,	/* 4 1310.0*/21.700,	/* 5 1545.0*/23.600,	/* 6 1821.0*/25.600,	/* 7 2146.0*/27.700,	/* 8 2530.0*/29.650,	/* 9 2982.0*/31.702,	/* 10 3515.0*/33.436,	/* 11 4144.0*/34.558,	/* 12 4884.0*/35.476,	/* 13 5757.0*/36.190,	/* 14 6787.0*/36.598,	/* 15 8000.0*/37.210,	},
		{/* 7 73.200	*//* 0 700.0*/16.200,	/* 1 850.0*/16.051,	/* 2 943.0*/15.573,	/* 3 1112.0*/19.700,	/* 4 1310.0*/21.400,	/* 5 1545.0*/23.000,	/* 6 1821.0*/24.800,	/* 7 2146.0*/26.440,	/* 8 2530.0*/28.270,	/* 9 2982.0*/30.020,	/* 10 3515.0*/31.400,	/* 11 4144.0*/32.600,	/* 12 4884.0*/33.300,	/* 13 5757.0*/34.100,	/* 14 6787.0*/34.700,	/* 15 8000.0*/35.300,	},
		{/* 8 85.400	*//* 0 700.0*/16.830,	/* 1 850.0*/17.000,	/* 2 943.0*/16.375,	/* 3 1112.0*/19.600,	/* 4 1310.0*/20.800,	/* 5 1545.0*/22.300,	/* 6 1821.0*/23.700,	/* 7 2146.0*/24.950,	/* 8 2530.0*/26.710,	/* 9 2982.0*/28.000,	/* 10 3515.0*/29.600,	/* 11 4144.0*/31.000,	/* 12 4884.0*/31.900,	/* 13 5757.0*/32.600,	/* 14 6787.0*/33.200,	/* 15 8000.0*/33.900,	},
		{/* 9 99.500	*//* 0 700.0*/16.200,	/* 1 850.0*/16.800,	/* 2 943.0*/17.300,	/* 3 1112.0*/18.000,	/* 4 1310.0*/19.000,	/* 5 1545.0*/20.200,	/* 6 1821.0*/21.300,	/* 7 2146.0*/22.100,	/* 8 2530.0*/23.400,	/* 9 2982.0*/25.200,	/* 10 3515.0*/26.700,	/* 11 4144.0*/28.000,	/* 12 4884.0*/28.900,	/* 13 5757.0*/29.800,	/* 14 6787.0*/30.700,	/* 15 8000.0*/31.400,	},
		{/* 10 116.000	*//* 0 700.0*/15.300,	/* 1 850.0*/15.900,	/* 2 943.0*/16.300,	/* 3 1112.0*/16.800,	/* 4 1310.0*/17.400,	/* 5 1545.0*/18.300,	/* 6 1821.0*/19.300,	/* 7 2146.0*/20.000,	/* 8 2530.0*/21.500,	/* 9 2982.0*/23.000,	/* 10 3515.0*/24.500,	/* 11 4144.0*/25.900,	/* 12 4884.0*/26.900,	/* 13 5757.0*/27.700,	/* 14 6787.0*/28.700,	/* 15 8000.0*/29.500,	},
		{/* 11 135.301	*//* 0 700.0*/13.600,	/* 1 850.0*/13.900,	/* 2 943.0*/14.100,	/* 3 1112.0*/14.600,	/* 4 1310.0*/15.100,	/* 5 1545.0*/15.900,	/* 6 1821.0*/17.000,	/* 7 2146.0*/17.700,	/* 8 2530.0*/19.100,	/* 9 2982.0*/20.600,	/* 10 3515.0*/22.100,	/* 11 4144.0*/23.300,	/* 12 4884.0*/24.500,	/* 13 5757.0*/25.500,	/* 14 6787.0*/26.500,	/* 15 8000.0*/27.300,	},
		{/* 12 157.699	*//* 0 700.0*/11.200,	/* 1 850.0*/11.600,	/* 2 943.0*/11.800,	/* 3 1112.0*/12.200,	/* 4 1310.0*/12.500,	/* 5 1545.0*/13.200,	/* 6 1821.0*/14.200,	/* 7 2146.0*/15.100,	/* 8 2530.0*/16.500,	/* 9 2982.0*/17.700,	/* 10 3515.0*/19.200,	/* 11 4144.0*/20.600,	/* 12 4884.0*/21.700,	/* 13 5757.0*/22.800,	/* 14 6787.0*/24.000,	/* 15 8000.0*/25.000,	},
		{/* 13 183.900	*//* 0 700.0*/8.300,	/* 1 850.0*/8.700,	/* 2 943.0*/9.000,	/* 3 1112.0*/9.300,	/* 4 1310.0*/9.800,	/* 5 1545.0*/10.400,	/* 6 1821.0*/11.300,	/* 7 2146.0*/12.100,	/* 8 2530.0*/13.100,	/* 9 2982.0*/14.500,	/* 10 3515.0*/16.000,	/* 11 4144.0*/17.200,	/* 12 4884.0*/18.500,	/* 13 5757.0*/19.700,	/* 14 6787.0*/21.000,	/* 15 8000.0*/22.100,	},
		{/* 14 214.400	*//* 0 700.0*/5.000,	/* 1 850.0*/5.300,	/* 2 943.0*/5.600,	/* 3 1112.0*/5.900,	/* 4 1310.0*/6.400,	/* 5 1545.0*/7.000,	/* 6 1821.0*/7.600,	/* 7 2146.0*/8.400,	/* 8 2530.0*/9.600,	/* 9 2982.0*/10.800,	/* 10 3515.0*/12.100,	/* 11 4144.0*/13.400,	/* 12 4884.0*/14.800,	/* 13 5757.0*/16.200,	/* 14 6787.0*/17.400,	/* 15 8000.0*/18.700,	},
		{/* 15 250.000	*//* 0 700.0*/1.100,	/* 1 850.0*/1.400,	/* 2 943.0*/1.600,	/* 3 1112.0*/2.000,	/* 4 1310.0*/2.500,	/* 5 1545.0*/3.000,	/* 6 1821.0*/3.700,	/* 7 2146.0*/4.400,	/* 8 2530.0*/5.400,	/* 9 2982.0*/6.600,	/* 10 3515.0*/7.600,	/* 11 4144.0*/9.000,	/* 12 4884.0*/10.500,	/* 13 5757.0*/12.000,	/* 14 6787.0*/13.400,	/* 15 8000.0*/14.900,	},
};
#endif


/*
#define MAF_TRANSFER_SIZE 8

static const float mafTransferVolts[MAF_TRANSFER_SIZE] = {1.365,
		1.569,
		2.028,
		2.35,
		2.611,
		2.959,
		3.499,
		4.011,
};


according to internet this should be the Miata NB transfer function but in reality it seems off
this could be related to us not using proper signal conditioning hardware
static const float mafTransferKgH[MAF_TRANSFER_SIZE] = {
		0,
		3.9456,
		18.7308,
		45.4788,
		82.278,
		154.4328,
		329.8104,
		594.2772
};


*/

#define MAF_TRANSFER_SIZE 10

// this transfer function somehow works with 1K pull-down
static const float mafTransferVolts[MAF_TRANSFER_SIZE] = {
		0.50,
		0.87,
		1.07,
		1.53,
		1.85,
		2.11,
		2.46,
		3.00,
		3.51,
		4.50
};

static const float mafTransferKgH[MAF_TRANSFER_SIZE] = {
		0.00,
		0.00,
		1.00,
		3.00,
		8.00,
		19.00,
		45.00,
		100.00,
		175.00,
		350.00
};


static void setMAFTransferFunction(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	memcpy(config->mafDecoding, mafTransferKgH, sizeof(mafTransferKgH));
	memcpy(config->mafDecodingBins, mafTransferVolts, sizeof(mafTransferVolts));
	for (int i = MAF_TRANSFER_SIZE;i<MAF_DECODING_COUNT;i++) {
		config->mafDecodingBins[i] = config->mafDecodingBins[MAF_TRANSFER_SIZE - 1] + i * 0.01;
		config->mafDecoding[i] = config->mafDecoding[MAF_TRANSFER_SIZE - 1];
	}
}

void setMazdaMiataNbTpsTps(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	memcpy(config->tpsTpsAccelFromRpmBins, tpsTspSame, sizeof(tpsTspSame));
	memcpy(config->tpsTpsAccelFromRpmBins, tpsTspSame, sizeof(tpsTspSame));
	// todo: there should be a better way?
	for (int loadIndex = 0; loadIndex < TPS_TPS_ACCEL_TABLE; loadIndex++) {
			for (int rpmIndex = 0; rpmIndex < TPS_TPS_ACCEL_TABLE; rpmIndex++) {
				config->tpsTpsAccelTable[loadIndex][rpmIndex] = tpsTpsTable[loadIndex][rpmIndex];
			}
		}
}

void setMazdaMiataNbInjectorLag(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	memcpy(engineConfiguration->injector.battLagCorr, injectorLagCorrection, sizeof(injectorLagCorrection));
	memcpy(engineConfiguration->injector.battLagCorrBins, injectorLagBins, sizeof(injectorLagBins));
}

void setMazdaNB2VVTSettings(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	memcpy(config->fsioTable1RpmBins, vvt18fsioRpmBins, sizeof(vvt18fsioRpmBins));
	memcpy(config->fsioTable1LoadBins, vvt18fsioLoadBins, sizeof(vvt18fsioLoadBins));
	// todo: there should be a better way?
	for (int loadIndex = 0; loadIndex < FSIO_TABLE_8; loadIndex++) {
		for (int rpmIndex = 0; rpmIndex < FSIO_TABLE_8; rpmIndex++) {
			config->fsioTable1[loadIndex][rpmIndex] = fsio_table_vvt_target[loadIndex][rpmIndex];
		}
	}

	engineConfiguration->auxPidFrequency[0] = 300; // VVT solenoid control

	// VVT closed loop
	engineConfiguration->auxPid[0].pFactor = 2;
	engineConfiguration->auxPid[0].iFactor = 0.005;
	engineConfiguration->auxPid[0].dFactor = 0;
	engineConfiguration->auxPid[0].offset = 33;
	engineConfiguration->auxPid[0].minValue = 24;
	engineConfiguration->auxPid[0].maxValue = 44;

	engineConfiguration->activateAuxPid1 = true; // todo: remove this field?
}

static void setMazdaMiataEngineNB2Defaults(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	engineConfiguration->displayLogicLevelsInEngineSniffer = true;
	engineConfiguration->useOnlyRisingEdgeForTrigger = true;
	engineConfiguration->trigger.type = TT_MIATA_VVT;

	setOperationMode(engineConfiguration, FOUR_STROKE_SYMMETRICAL_CRANK_SENSOR);
	engineConfiguration->specs.displacement = 1.839;
	strcpy(CONFIG(engineMake), ENGINE_MAKE_MAZDA);
	strcpy(CONFIG(engineCode), "NB2");

	engineConfiguration->map.sensor.type = MT_GM_3_BAR;
	setEgoSensor(ES_Innovate_MTX_L PASS_CONFIG_PARAMETER_SUFFIX);

	setCommonNTCSensor(&engineConfiguration->clt, 2700);
	setCommonNTCSensor(&engineConfiguration->iat, 2700);
	setMAFTransferFunction(PASS_CONFIG_PARAMETER_SIGNATURE);

	// set idle_position 35
	engineConfiguration->manIdlePosition = 35;

	engineConfiguration->specs.cylindersCount = 4;
	engineConfiguration->specs.firingOrder = FO_1_3_4_2;

	engineConfiguration->injectionMode = IM_SEQUENTIAL;

	engineConfiguration->ignitionMode = IM_WASTED_SPARK;
	/**
	 * http://miataturbo.wikidot.com/fuel-injectors
	 * 01-05 (purple) - #195500-4060
	 *
	 * NB2 Miata has an absolute pressure fuel system - NB2 fuel rail regulator has no vacuum line.
	 *
	 * Theoretically we shall have injectorFlow(MAP) curve, practically VE gets artificially high as MAP increases and
	 * accounts for flow change.
	 *
	 * Wall wetting AE could be an argument for honest injectorFlow(MAP)
	 */
	engineConfiguration->injector.flow = 265;
	engineConfiguration->fuelRailPressure = 400; // 400 kPa, 58 psi
	engineConfiguration->absoluteFuelPressure = true;

	engineConfiguration->crankingIACposition = 90;

	CONFIG(isAlternatorControlEnabled) = true;
	// enable altdebug
	engineConfiguration->targetVBatt = 13.8;
	engineConfiguration->alternatorControl.offset = 40;
	engineConfiguration->alternatorControl.pFactor = 14;
	engineConfiguration->alternatorControl.iFactor = 0.1;
	engineConfiguration->alternatorControl.dFactor = 0;
	engineConfiguration->alternatorControl.periodMs = 10;


	engineConfiguration->vvtCamSensorUseRise = true;
	// set vvt_mode 3
	engineConfiguration->vvtMode = MIATA_NB2;
	engineConfiguration->vvtOffset = 98; // 2003 red car value

	memcpy(config->veRpmBins, mazda_miata_nb2_RpmBins, sizeof(mazda_miata_nb2_RpmBins));
	memcpy(config->veLoadBins, mazda_miata_nb2_LoadBins, sizeof(mazda_miata_nb2_LoadBins));
	copyFuelTable(mapBased18vvtVeTable_NB_fuel_rail, config->veTable);

	memcpy(config->ignitionRpmBins, ignition18vvtRpmBins, sizeof(ignition18vvtRpmBins));
	memcpy(config->ignitionLoadBins, ignition18vvtLoadBins, sizeof(ignition18vvtLoadBins));
#if IGN_LOAD_COUNT == DEFAULT_IGN_LOAD_COUNT
	copyTimingTable(mapBased18vvtTimingTable, config->ignitionTable);
#endif

	setMazdaNB2VVTSettings(PASS_CONFIG_PARAMETER_SIGNATURE);

	// enable cylinder_cleanup
	engineConfiguration->isCylinderCleanupEnabled = true;

	setMazdaMiataNbTpsTps(PASS_CONFIG_PARAMETER_SIGNATURE);

	// set_whole_ve_map 80
	setMazdaMiataNbInjectorLag(PASS_CONFIG_PARAMETER_SIGNATURE);

	CONFIG(debugTriggerSync) = GPIOD_3;

//	engineConfiguration->debugMode = DBG_IDLE_CONTROL;
	engineConfiguration->debugMode = DBG_TRIGGER_COUNTERS;

	//set idle_offset 30
	engineConfiguration->idleRpmPid.offset = 30;
	engineConfiguration->idleRpmPid.pFactor = 0.07;
	engineConfiguration->idleRpmPid.iFactor = 0.0001;
	engineConfiguration->idleRpmPid.dFactor = 5;
	engineConfiguration->idleRpmPid.periodMs = 10;
} // end of setMazdaMiataEngineNB2Defaults

// MAZDA_MIATA_2003
void setMazdaMiata2003EngineConfiguration(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	setFrankensoConfiguration(PASS_CONFIG_PARAMETER_SIGNATURE);

	setMazdaMiataEngineNB2Defaults(PASS_CONFIG_PARAMETER_SIGNATURE);

//	engineConfiguration->triggerInputPins[0] = GPIOA_8; // custom Frankenso wiring in order to use SPI1 for accelerometer
	engineConfiguration->triggerInputPins[0] = GPIOA_5; // board still not modified
	engineConfiguration->triggerInputPins[1] = GPIO_UNASSIGNED;
	engineConfiguration->camInputs[0] = GPIOC_6;

//	engineConfiguration->is_enabled_spi_1 = true;

	engineConfiguration->twoWireBatchInjection = true; // this is needed for #492 testing

	engineConfiguration->alternatorControlPin = GPIOE_10;
	engineConfiguration->alternatorControlPinMode = OM_OPENDRAIN;

//	engineConfiguration->vehicleSpeedSensorInputPin = GPIOA_8;

	engineConfiguration->auxPidPins[0] = GPIOE_3; // VVT solenoid control
	//	/**
	//	 * set_fsio_setting 1 0.55
	//	 */
	engineConfiguration->fsio_setting[0] = 0.0;
//	setFsioExt(0, GPIOE_3, "0 fsio_setting", 400 PASS_CONFIG_PARAMETER_SUFFIX);



	// high-side driver with +12v VP jumper
	engineConfiguration->dizzySparkOutputPin = GPIOE_8; // tachometer

	// set global_trigger_offset_angle 0
	engineConfiguration->globalTriggerAngleOffset = 0;

	// enable trigger_details
	engineConfiguration->verboseTriggerSynchDetails = false;

	// set cranking_timing_angle 10
	engineConfiguration->crankingTimingAngle = 10;
	// set cranking_fuel 4
	engineConfiguration->cranking.baseFuel = 4; // this value for return-less NB miata fuel system, higher pressure

/**
 * Saab attempt
 * Saab  coil on #1 PD8 extra blue wire
 * Miata coil on #2 PC9  - orange ECU wire "2&3"
 * Saab  coil on #3 PD9 extra white wire
 * Miata coil on #4 PE14 - white ECU wire "1&4"
 */

	engineConfiguration->ignitionPins[0] = GPIOE_14;
	engineConfiguration->ignitionPins[1] = GPIO_UNASSIGNED;
	engineConfiguration->ignitionPins[2] = GPIOC_9;
	engineConfiguration->ignitionPins[3] = GPIO_UNASSIGNED;

	// set tps_min 90
	engineConfiguration->tpsMin = 100; // convert 12to10 bit (ADC/4)
	// set tps_max 540
	engineConfiguration->tpsMax = 650; // convert 12to10 bit (ADC/4)


	engineConfiguration->malfunctionIndicatorPin = GPIOD_5;


//	engineConfiguration->malfunctionIndicatorPin = GPIOD_9;
//	engineConfiguration->malfunctionIndicatorPinMode = OM_INVERTED;

	// todo: blue jumper wire - what is it?!
	// Frankenso analog #6 pin 3R, W56 (5th lower row pin from the end) top <> W45 bottom jumper, not OEM


	// see setFrankensoConfiguration
	// map.sensor.hwChannel = EFI_ADC_0; W53

	/**
	 * PA4 Wideband O2 Sensor
	 */
	// todo: re-wire the board to use "Frankenso analog #7 pin 3J, W48 top <>W48 bottom jumper, not OEM"
	//engineConfiguration->afr.hwChannel = EFI_ADC_3; // PA3
	engineConfiguration->afr.hwChannel = EFI_ADC_4;

	//
	/**
	 * Combined RPM, CLT and VBATT warning light
	 *
	 * to test
	 * set_fsio_setting 2 1800
	 * set_fsio_setting 3 65
	 * set_fsio_setting 4 15
	 */
	engineConfiguration->fsio_setting[1] = 6500; // #2 RPM threshold
	engineConfiguration->fsio_setting[2] = 105; // #3 CLT threshold
	engineConfiguration->fsio_setting[3] = 12.0; // #4 voltage threshold

//	setFsio(1, GPIOE_6, COMBINED_WARNING_LIGHT PASS_CONFIG_PARAMETER_SUFFIX);

	// enable auto_idle
	// enable verbose_idle
	engineConfiguration->isVerboseIAC = false;
	// set idle_p 0.05
	// set idle_i 0
	// set idle_d 0
	// set debug_mode 3
	// set idle_rpm 1700
	// see setDefaultIdleParameters

	engineConfiguration->adcVcc = 3.3f;
	engineConfiguration->vbattDividerCoeff = 8.80f;

	// by the way NB2 MAF internal diameter is about 2.5 inches / 63mm
	// 1K pull-down to read current from this MAF
	engineConfiguration->mafAdcChannel = EFI_ADC_6; // PA6 W46 <> W46

	engineConfiguration->throttlePedalUpVoltage = 0.65f;


	// TLE7209 two-wire ETB control
	// PWM
	CONFIG(etb_use_two_wires) = true;

	engineConfiguration->etbIo[0].controlPin1 = GPIO_UNASSIGNED;

	//
	engineConfiguration->etbIo[0].directionPin1 = GPIOE_12; // orange
	//
	engineConfiguration->etbIo[0].directionPin2 = GPIOC_7; // white/blue

	// set_analog_input_pin tps PC3
	engineConfiguration->tps1_1AdcChannel = EFI_ADC_13; // PC3 blue

	engineConfiguration->idleMode = IM_AUTO;
	// set_analog_input_pin pps PA2
/* a step back - Frankenso does not use ETB
	engineConfiguration->throttlePedalPositionAdcChannel = EFI_ADC_2;
*/

	engineConfiguration->idleRpmPid.offset = 0;
	engineConfiguration->idleRpmPid.pFactor = 0.2;
	engineConfiguration->idleRpmPid.iFactor = 0.0001;
	engineConfiguration->idleRpmPid.dFactor = 5;
	engineConfiguration->idleRpmPid.periodMs = 10;

	engineConfiguration->isFasterEngineSpinUpEnabled = true;

	//set etb_p 12
	engineConfiguration->etb.pFactor = 12; // a bit lower p-factor seems to work better on TLE9201? MRE?
	engineConfiguration->etb.iFactor = 	0;
	engineConfiguration->etb.dFactor = 0;
	engineConfiguration->etb.offset = 0;
	engineConfiguration->etb.minValue = -60;
	engineConfiguration->etb.maxValue = 50;

	engineConfiguration->cranking.baseFuel = 1;
	config->crankingFuelCoef[0] = 28; // base cranking fuel adjustment coefficient
	config->crankingFuelBins[0] = -20; // temperature in C
	config->crankingFuelCoef[1] = 22;
	config->crankingFuelBins[1] = -10;
	config->crankingFuelCoef[2] = 18;
	config->crankingFuelBins[2] = 5;
	config->crankingFuelCoef[3] = 15;
	config->crankingFuelBins[3] = 30;

	config->crankingFuelCoef[4] = 10;
	config->crankingFuelBins[4] = 35;
	config->crankingFuelCoef[5] = 10;
	config->crankingFuelBins[5] = 50;
	config->crankingFuelCoef[6] = 10;
	config->crankingFuelBins[6] = 65;
	config->crankingFuelCoef[7] = 10;
	config->crankingFuelBins[7] = 90;

}

/**
 * red car setting with default 1991/1995 miata harness
 * board #70 - closer to default miata NA6 harness
 *
 */
void setMazdaMiata2003EngineConfigurationBoardTest(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	setMazdaMiata2003EngineConfiguration(PASS_CONFIG_PARAMETER_SIGNATURE);

	engineConfiguration->ignitionPins[2] = GPIOC_7;

	// Frankenso analog #7 pin 3J, W48 top <>W48 bottom jumper, not OEM. Make sure 500K pull-down on Frankenso
	engineConfiguration->afr.hwChannel = EFI_ADC_3; // PA3

	engineConfiguration->mafAdcChannel = EFI_ADC_4; // PA4 - W47 top <>W47
}

static void setMiataNB2_MRE_common(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
#if (BOARD_TLE8888_COUNT > 0)
	setMazdaMiataEngineNB2Defaults(PASS_CONFIG_PARAMETER_SIGNATURE);

	// MRE has a special main relay control low side pin - rusEfi firmware is totally not involved with main relay control
	//
	// fuelPumpPin output is inherited from boards/microrusefi/board_configuration.cpp
	// fanPin output is inherited from boards/microrusefi/board_configuration.cpp
	//
	// crank trigger input is inherited from boards/microrusefi/board_configuration.cpp
	// map.sensor.hwChannel input is inherited from boards/microrusefi/board_configuration.cpp
	// tps1_1AdcChannel input is inherited from boards/microrusefi/board_configuration.cpp
	// afr.hwChannel input is inherited from boards/microrusefi/board_configuration.cpp

	engineConfiguration->ignitionPins[1] = GPIO_UNASSIGNED;
	engineConfiguration->ignitionPins[3] = GPIO_UNASSIGNED;

	engineConfiguration->camInputs[0] = GPIOA_5;
	/**
	 * By default "auto detection mode for VR sensor signals" is used
	 * We know that for short & strange Hall (?) signals like Miata NB2 crank sensor this does not work well above certain RPM.
	 */
	engineConfiguration->tle8888mode = TL_MANUAL;

	// GPIOD_6: "13 - GP Out 6" - selected to +12v
	engineConfiguration->alternatorControlPin = GPIOD_6;
	// GPIOD_7: "14 - GP Out 5" - selected to +12v
	engineConfiguration->dizzySparkOutputPin = GPIOD_7; // tachometer

	// set cranking_fuel 9
	engineConfiguration->cranking.baseFuel = 9; // higher value for return system NA conversion since lower fuel pressure

	engineConfiguration->useConstantDwellDuringCranking = true;
	engineConfiguration->ignitionDwellForCrankingMs = 8;

	engineConfiguration->vvtOffset = 97;


	//   # TLE8888 high current low side: VVT1 IN10 / OUT6
	// GPIOE_9:  "7 - Lowside 1"
	engineConfiguration->auxPidPins[0] = GPIOE_9; // VVT solenoid control

	// TLE8888_PIN_23: "33 - GP Out 3"
	engineConfiguration->malfunctionIndicatorPin = TLE8888_PIN_23;


	// todo: alternator warn
	// ?

	// todo: AC fan
	// TLE8888_PIN_24: "43 - GP Out 4"

	engineConfiguration->isFasterEngineSpinUpEnabled = true;

	// set_analog_input_pin pps PA7
	// EFI_ADC_7: "31 - AN volt 3" - PA7
	engineConfiguration->throttlePedalPositionAdcChannel = EFI_ADC_7;

	// set tps_min 90
	engineConfiguration->tpsMin = 90;

	// set tps_max 540
	engineConfiguration->tpsMax = 870;

	engineConfiguration->idleMode = IM_AUTO;

	// set vbatt_divider 11
	// 0.3#4 has wrong R139 as well?
	// 56k high side/10k low side multiplied by above analogInputDividerCoefficient = 11
	engineConfiguration->vbattDividerCoeff = (66.0f / 10.0f) * engineConfiguration->analogInputDividerCoefficient;
#endif /* BOARD_TLE8888_COUNT */
}


/**
 * Pretty much OEM 2003 Miata with ETB
 * set engine_type 13
 */
void setMiataNB2_MRE_ETB(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	setMiataNB2_MRE_common(PASS_CONFIG_PARAMETER_SIGNATURE);

	CONFIG(useETBforIdleControl) = true;

#if EFI_FSIO
	// enable ETB
	// set_rpn_expression 8 "0"
	setFsio(7, GPIOC_8, "0" PASS_CONFIG_PARAMETER_SUFFIX);
#endif /* EFI_FSIO */

	//set idle_offset 0
	engineConfiguration->idleRpmPid.offset = 0;
	engineConfiguration->idleRpmPid.pFactor = 0.2;
	engineConfiguration->idleRpmPid.iFactor = 0.0001;
	engineConfiguration->idleRpmPid.dFactor = 5;
	engineConfiguration->idleRpmPid.periodMs = 10;

	CONFIG(useETBforIdleControl) = true;
	engineConfiguration->throttlePedalUpVoltage = 1;
	// WAT? that's an interesting value, how come it's above 5v?
	engineConfiguration->throttlePedalWOTVoltage = 5.47;

	engineConfiguration->etb.pFactor = 12; // a bit lower p-factor seems to work better on TLE9201? MRE?
	engineConfiguration->etb.iFactor = 	0;
	engineConfiguration->etb.dFactor = 0;
	engineConfiguration->etb.offset = 0;
}

/**
 * set engine_type 11
 */
void setMiataNB2_MRE_MTB(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	setMiataNB2_MRE_common(PASS_CONFIG_PARAMETER_SIGNATURE);

	// somehow MRE72 adapter 0.2 has TPS routed to pin 26?
	engineConfiguration->tps1_1AdcChannel = EFI_ADC_6; // PA6


	// 1K pull-down to read current from this MAF
	engineConfiguration->mafAdcChannel = EFI_ADC_13; // J30 AV5


}
