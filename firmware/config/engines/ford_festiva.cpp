/*
 * @file ford_festiva.cpp
 *
 * set engine_type 14
 *
 * http://rusefi.com/wiki/index.php?title=Vehicle:Mazda_Protege_1993
 * http://rusefi.com/forum/viewtopic.php?f=3&t=537
 * http://rusefi.com/forum/viewtopic.php?f=3&t=822
 *
 * @date Jan 3, 2016
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#include "ford_festiva.h"
#include "thermistors.h"
#include "engine_math.h"
#include "maf.h"
#include "ego.h"
#include "fsio_impl.h"
#include "mazda_miata.h"
#include "custom_engine.h"

#if IGN_LOAD_COUNT == DEFAULT_IGN_LOAD_COUNT
static const ignition_table_t racingFestivaIgnitionTable = {
		/* Generated by TS2C on Mon Feb 08 13:59:57 EST 2016*/
		{/* 0 10.000	*//* 0 800.0*/13.000,	/* 1 1213.3281*/12.900,	/* 2 1626.6562*/10.500,	/* 3 2040.0*/10.500,	/* 4 2453.3438*/10.500,	/* 5 2866.6562*/10.500,	/* 6 3280.0*/11.500,	/* 7 3693.3438*/11.500,	/* 8 4106.6875*/11.500,	/* 9 4520.0*/12.000,	/* 10 4933.3125*/12.000,	/* 11 5346.6875*/12.000,	/* 12 5760.0*/12.000,	/* 13 6173.3125*/12.000,	/* 14 6586.6875*/12.000,	/* 15 7000.0*/12.000,	},
		{/* 1 16.000	*//* 0 800.0*/13.000,	/* 1 1213.3281*/12.900,	/* 2 1626.6562*/10.500,	/* 3 2040.0*/10.500,	/* 4 2453.3438*/10.500,	/* 5 2866.6562*/10.500,	/* 6 3280.0*/11.500,	/* 7 3693.3438*/11.500,	/* 8 4106.6875*/11.500,	/* 9 4520.0*/12.000,	/* 10 4933.3125*/12.000,	/* 11 5346.6875*/12.000,	/* 12 5760.0*/12.000,	/* 13 6173.3125*/12.000,	/* 14 6586.6875*/12.000,	/* 15 7000.0*/12.000,	},
		{/* 2 22.000	*//* 0 800.0*/14.600,	/* 1 1213.3281*/14.000,	/* 2 1626.6562*/14.900,	/* 3 2040.0*/12.400,	/* 4 2453.3438*/12.300,	/* 5 2866.6562*/12.300,	/* 6 3280.0*/12.200,	/* 7 3693.3438*/12.200,	/* 8 4106.6875*/12.100,	/* 9 4520.0*/12.000,	/* 10 4933.3125*/12.000,	/* 11 5346.6875*/12.000,	/* 12 5760.0*/12.000,	/* 13 6173.3125*/12.000,	/* 14 6586.6875*/12.000,	/* 15 7000.0*/12.000,	},
		{/* 3 28.000	*//* 0 800.0*/14.600,	/* 1 1213.3281*/14.000,	/* 2 1626.6562*/14.900,	/* 3 2040.0*/12.400,	/* 4 2453.3438*/12.300,	/* 5 2866.6562*/12.300,	/* 6 3280.0*/12.200,	/* 7 3693.3438*/12.200,	/* 8 4106.6875*/12.100,	/* 9 4520.0*/12.000,	/* 10 4933.3125*/12.000,	/* 11 5346.6875*/12.000,	/* 12 5760.0*/12.000,	/* 13 6173.3125*/12.000,	/* 14 6586.6875*/12.000,	/* 15 7000.0*/12.000,	},
		{/* 4 31.000	*//* 0 800.0*/15.200,	/* 1 1213.3281*/15.400,	/* 2 1626.6562*/15.400,	/* 3 2040.0*/15.400,	/* 4 2453.3438*/15.400,	/* 5 2866.6562*/16.800,	/* 6 3280.0*/16.800,	/* 7 3693.3438*/23.000,	/* 8 4106.6875*/23.000,	/* 9 4520.0*/26.000,	/* 10 4933.3125*/26.000,	/* 11 5346.6875*/26.000,	/* 12 5760.0*/26.000,	/* 13 6173.3125*/26.000,	/* 14 6586.6875*/26.000,	/* 15 7000.0*/26.000,	},
		{/* 5 35.000	*//* 0 800.0*/15.200,	/* 1 1213.3281*/15.400,	/* 2 1626.6562*/15.400,	/* 3 2040.0*/15.400,	/* 4 2453.3438*/15.400,	/* 5 2866.6562*/16.800,	/* 6 3280.0*/16.800,	/* 7 3693.3438*/23.000,	/* 8 4106.6875*/23.000,	/* 9 4520.0*/26.000,	/* 10 4933.3125*/26.000,	/* 11 5346.6875*/26.000,	/* 12 5760.0*/26.000,	/* 13 6173.3125*/26.000,	/* 14 6586.6875*/26.000,	/* 15 7000.0*/26.000,	},
		{/* 6 38.000	*//* 0 800.0*/16.700,	/* 1 1213.3281*/16.700,	/* 2 1626.6562*/17.000,	/* 3 2040.0*/19.000,	/* 4 2453.3438*/19.000,	/* 5 2866.6562*/21.500,	/* 6 3280.0*/21.500,	/* 7 3693.3438*/25.800,	/* 8 4106.6875*/25.800,	/* 9 4520.0*/26.000,	/* 10 4933.3125*/26.000,	/* 11 5346.6875*/26.000,	/* 12 5760.0*/26.000,	/* 13 6173.3125*/26.000,	/* 14 6586.6875*/26.000,	/* 15 7000.0*/26.000,	},
		{/* 7 41.000	*//* 0 800.0*/16.700,	/* 1 1213.3281*/16.700,	/* 2 1626.6562*/17.000,	/* 3 2040.0*/19.000,	/* 4 2453.3438*/19.000,	/* 5 2866.6562*/21.500,	/* 6 3280.0*/21.500,	/* 7 3693.3438*/25.800,	/* 8 4106.6875*/25.800,	/* 9 4520.0*/26.000,	/* 10 4933.3125*/26.000,	/* 11 5346.6875*/26.000,	/* 12 5760.0*/26.000,	/* 13 6173.3125*/26.000,	/* 14 6586.6875*/26.000,	/* 15 7000.0*/26.000,	},
		{/* 8 48.000	*//* 0 800.0*/16.900,	/* 1 1213.3281*/17.000,	/* 2 1626.6562*/17.000,	/* 3 2040.0*/19.700,	/* 4 2453.3438*/19.700,	/* 5 2866.6562*/22.200,	/* 6 3280.0*/22.500,	/* 7 3693.3438*/22.500,	/* 8 4106.6875*/30.000,	/* 9 4520.0*/30.000,	/* 10 4933.3125*/30.000,	/* 11 5346.6875*/30.000,	/* 12 5760.0*/30.000,	/* 13 6173.3125*/30.000,	/* 14 6586.6875*/30.000,	/* 15 7000.0*/30.000,	},
		{/* 9 52.000	*//* 0 800.0*/17.000,	/* 1 1213.3281*/17.100,	/* 2 1626.6562*/17.000,	/* 3 2040.0*/20.100,	/* 4 2453.3438*/20.100,	/* 5 2866.6562*/22.600,	/* 6 3280.0*/23.000,	/* 7 3693.3438*/23.000,	/* 8 4106.6875*/30.000,	/* 9 4520.0*/30.000,	/* 10 4933.3125*/30.000,	/* 11 5346.6875*/30.000,	/* 12 5760.0*/30.000,	/* 13 6173.3125*/30.000,	/* 14 6586.6875*/30.000,	/* 15 7000.0*/30.000,	},
		{/* 10 59.000	*//* 0 800.0*/17.200,	/* 1 1213.3281*/17.400,	/* 2 1626.6562*/17.000,	/* 3 2040.0*/21.000,	/* 4 2453.3438*/21.000,	/* 5 2866.6562*/23.400,	/* 6 3280.0*/23.900,	/* 7 3693.3438*/23.900,	/* 8 4106.6875*/30.000,	/* 9 4520.0*/30.000,	/* 10 4933.3125*/30.000,	/* 11 5346.6875*/30.000,	/* 12 5760.0*/30.000,	/* 13 6173.3125*/30.000,	/* 14 6586.6875*/30.000,	/* 15 7000.0*/30.000,	},
		{/* 11 65.000	*//* 0 800.0*/16.200,	/* 1 1213.3281*/16.400,	/* 2 1626.6562*/16.400,	/* 3 2040.0*/21.000,	/* 4 2453.3438*/21.000,	/* 5 2866.6562*/23.800,	/* 6 3280.0*/24.000,	/* 7 3693.3438*/24.000,	/* 8 4106.6875*/30.000,	/* 9 4520.0*/30.000,	/* 10 4933.3125*/30.000,	/* 11 5346.6875*/30.000,	/* 12 5760.0*/30.000,	/* 13 6173.3125*/30.000,	/* 14 6586.6875*/30.000,	/* 15 7000.0*/30.000,	},
		{/* 12 74.000	*//* 0 800.0*/15.300,	/* 1 1213.3281*/15.900,	/* 2 1626.6562*/15.900,	/* 3 2040.0*/19.500,	/* 4 2453.3438*/19.100,	/* 5 2866.6562*/23.100,	/* 6 3280.0*/23.900,	/* 7 3693.3438*/23.900,	/* 8 4106.6875*/29.200,	/* 9 4520.0*/28.600,	/* 10 4933.3125*/30.000,	/* 11 5346.6875*/30.000,	/* 12 5760.0*/30.000,	/* 13 6173.3125*/30.000,	/* 14 6586.6875*/30.000,	/* 15 7000.0*/30.000,	},
		{/* 13 85.000	*//* 0 800.0*/20.000,	/* 1 1213.3281*/15.900,	/* 2 1626.6562*/15.900,	/* 3 2040.0*/19.500,	/* 4 2453.3438*/19.500,	/* 5 2866.6562*/24.800,	/* 6 3280.0*/26.000,	/* 7 3693.3438*/26.000,	/* 8 4106.6875*/26.900,	/* 9 4520.0*/27.500,	/* 10 4933.3125*/27.600,	/* 11 5346.6875*/28.100,	/* 12 5760.0*/28.000,	/* 13 6173.3125*/28.700,	/* 14 6586.6875*/28.500,	/* 15 7000.0*/30.000,	},
		{/* 14 94.000	*//* 0 800.0*/20.000,	/* 1 1213.3281*/16.000,	/* 2 1626.6562*/16.000,	/* 3 2040.0*/19.500,	/* 4 2453.3438*/19.700,	/* 5 2866.6562*/25.000,	/* 6 3280.0*/26.200,	/* 7 3693.3438*/26.200,	/* 8 4106.6875*/26.900,	/* 9 4520.0*/27.500,	/* 10 4933.3125*/27.600,	/* 11 5346.6875*/28.100,	/* 12 5760.0*/28.000,	/* 13 6173.3125*/28.700,	/* 14 6586.6875*/28.900,	/* 15 7000.0*/30.200,	},
		{/* 15 101.000	*//* 0 800.0*/20.000,	/* 1 1213.3281*/16.200,	/* 2 1626.6562*/16.200,	/* 3 2040.0*/19.500,	/* 4 2453.3438*/19.500,	/* 5 2866.6562*/23.100,	/* 6 3280.0*/24.000,	/* 7 3693.3438*/24.000,	/* 8 4106.6875*/26.900,	/* 9 4520.0*/27.500,	/* 10 4933.3125*/27.600,	/* 11 5346.6875*/28.100,	/* 12 5760.0*/28.000,	/* 13 6173.3125*/30.000,	/* 14 6586.6875*/30.900,	/* 15 7000.0*/30.200,	},
};
#endif

static const fuel_table_t racingFestivaVeTable = {
		/* Generated by TS2C on Mon Feb 08 13:58:49 EST 2016*/
		{/* 0 10.000	*//* 0 800.0*/110.000,	/* 1 1200.0*/105.384,	/* 2 1600.0*/94.188,	/* 3 2000.0*/104.591,	/* 4 2400.0*/108.557,	/* 5 2800.0*/102.816,	/* 6 3200.0*/104.492,	/* 7 3600.0*/106.760,	/* 8 4100.0*/130.328,	/* 9 4500.0*/130.600,	/* 10 4900.0*/110.748,	/* 11 5300.0*/110.622,	/* 12 5700.0*/110.000,	/* 13 6100.0*/110.000,	/* 14 6500.0*/110.000,	/* 15 7000.0*/110.000,	},
		{/* 1 20.000	*//* 0 800.0*/92.912,	/* 1 1200.0*/88.219,	/* 2 1600.0*/87.050,	/* 3 2000.0*/94.294,	/* 4 2400.0*/100.350,	/* 5 2800.0*/101.178,	/* 6 3200.0*/99.942,	/* 7 3600.0*/108.697,	/* 8 4100.0*/110.794,	/* 9 4500.0*/107.359,	/* 10 4900.0*/107.412,	/* 11 5300.0*/110.000,	/* 12 5700.0*/110.000,	/* 13 6100.0*/110.000,	/* 14 6500.0*/110.000,	/* 15 7000.0*/110.000,	},
		{/* 2 30.000	*//* 0 800.0*/93.691,	/* 1 1200.0*/90.029,	/* 2 1600.0*/94.296,	/* 3 2000.0*/98.011,	/* 4 2400.0*/104.727,	/* 5 2800.0*/111.653,	/* 6 3200.0*/113.626,	/* 7 3600.0*/117.779,	/* 8 4100.0*/119.690,	/* 9 4500.0*/118.623,	/* 10 4900.0*/114.264,	/* 11 5300.0*/115.000,	/* 12 5700.0*/115.000,	/* 13 6100.0*/115.000,	/* 14 6500.0*/115.000,	/* 15 7000.0*/115.000,	},
		{/* 3 40.000	*//* 0 800.0*/99.005,	/* 1 1200.0*/100.483,	/* 2 1600.0*/102.497,	/* 3 2000.0*/107.411,	/* 4 2400.0*/110.811,	/* 5 2800.0*/116.596,	/* 6 3200.0*/116.402,	/* 7 3600.0*/117.253,	/* 8 4100.0*/121.555,	/* 9 4500.0*/121.311,	/* 10 4900.0*/115.000,	/* 11 5300.0*/115.000,	/* 12 5700.0*/115.000,	/* 13 6100.0*/115.000,	/* 14 6500.0*/115.000,	/* 15 7000.0*/115.000,	},
		{/* 4 50.000	*//* 0 800.0*/106.376,	/* 1 1200.0*/112.563,	/* 2 1600.0*/103.710,	/* 3 2000.0*/106.785,	/* 4 2400.0*/111.187,	/* 5 2800.0*/121.104,	/* 6 3200.0*/120.854,	/* 7 3600.0*/122.625,	/* 8 4100.0*/125.261,	/* 9 4500.0*/120.702,	/* 10 4900.0*/115.000,	/* 11 5300.0*/115.000,	/* 12 5700.0*/115.000,	/* 13 6100.0*/115.000,	/* 14 6500.0*/115.000,	/* 15 7000.0*/115.000,	},
		{/* 5 60.000	*//* 0 800.0*/108.366,	/* 1 1200.0*/107.741,	/* 2 1600.0*/105.713,	/* 3 2000.0*/107.654,	/* 4 2400.0*/113.228,	/* 5 2800.0*/124.786,	/* 6 3200.0*/121.577,	/* 7 3600.0*/119.707,	/* 8 4100.0*/121.255,	/* 9 4500.0*/120.000,	/* 10 4900.0*/115.000,	/* 11 5300.0*/115.000,	/* 12 5700.0*/115.000,	/* 13 6100.0*/115.000,	/* 14 6500.0*/115.000,	/* 15 7000.0*/115.000,	},
		{/* 6 70.000	*//* 0 800.0*/106.030,	/* 1 1200.0*/108.577,	/* 2 1600.0*/112.935,	/* 3 2000.0*/113.187,	/* 4 2400.0*/113.910,	/* 5 2800.0*/109.719,	/* 6 3200.0*/97.090,	/* 7 3600.0*/120.000,	/* 8 4100.0*/120.000,	/* 9 4500.0*/120.000,	/* 10 4900.0*/120.000,	/* 11 5300.0*/120.000,	/* 12 5700.0*/120.000,	/* 13 6100.0*/120.000,	/* 14 6500.0*/120.000,	/* 15 7000.0*/120.000,	},
		{/* 7 80.000	*//* 0 800.0*/109.383,	/* 1 1200.0*/107.073,	/* 2 1600.0*/107.386,	/* 3 2000.0*/113.634,	/* 4 2400.0*/113.413,	/* 5 2800.0*/120.000,	/* 6 3200.0*/120.000,	/* 7 3600.0*/120.000,	/* 8 4100.0*/120.000,	/* 9 4500.0*/120.000,	/* 10 4900.0*/120.000,	/* 11 5300.0*/120.000,	/* 12 5700.0*/120.000,	/* 13 6100.0*/120.000,	/* 14 6500.0*/120.000,	/* 15 7000.0*/120.000,	},
		{/* 8 90.000	*//* 0 800.0*/107.380,	/* 1 1200.0*/109.401,	/* 2 1600.0*/109.114,	/* 3 2000.0*/115.732,	/* 4 2400.0*/110.736,	/* 5 2800.0*/120.000,	/* 6 3200.0*/120.000,	/* 7 3600.0*/120.000,	/* 8 4100.0*/120.000,	/* 9 4500.0*/120.611,	/* 10 4900.0*/120.000,	/* 11 5300.0*/120.000,	/* 12 5700.0*/120.000,	/* 13 6100.0*/120.000,	/* 14 6500.0*/120.000,	/* 15 7000.0*/120.000,	},
		{/* 9 100.000	*//* 0 800.0*/97.878,	/* 1 1200.0*/100.000,	/* 2 1600.0*/100.000,	/* 3 2000.0*/104.241,	/* 4 2400.0*/100.000,	/* 5 2800.0*/120.000,	/* 6 3200.0*/120.000,	/* 7 3600.0*/120.000,	/* 8 4100.0*/120.711,	/* 9 4500.0*/121.226,	/* 10 4900.0*/121.317,	/* 11 5300.0*/120.000,	/* 12 5700.0*/120.000,	/* 13 6100.0*/120.000,	/* 14 6500.0*/120.000,	/* 15 7000.0*/120.000,	},
		{/* 10 110.000	*//* 0 800.0*/100.000,	/* 1 1200.0*/100.000,	/* 2 1600.0*/100.000,	/* 3 2000.0*/100.000,	/* 4 2400.0*/100.000,	/* 5 2800.0*/120.000,	/* 6 3200.0*/120.000,	/* 7 3600.0*/120.000,	/* 8 4100.0*/120.000,	/* 9 4500.0*/120.000,	/* 10 4900.0*/120.000,	/* 11 5300.0*/120.000,	/* 12 5700.0*/120.000,	/* 13 6100.0*/120.000,	/* 14 6500.0*/120.000,	/* 15 7000.0*/120.000,	},
		{/* 11 120.000	*//* 0 800.0*/100.000,	/* 1 1200.0*/100.000,	/* 2 1600.0*/100.000,	/* 3 2000.0*/100.000,	/* 4 2400.0*/100.000,	/* 5 2800.0*/120.000,	/* 6 3200.0*/120.000,	/* 7 3600.0*/120.000,	/* 8 4100.0*/120.000,	/* 9 4500.0*/120.000,	/* 10 4900.0*/120.000,	/* 11 5300.0*/120.000,	/* 12 5700.0*/120.000,	/* 13 6100.0*/120.000,	/* 14 6500.0*/120.000,	/* 15 7000.0*/120.000,	},
		{/* 12 130.000	*//* 0 800.0*/100.000,	/* 1 1200.0*/100.000,	/* 2 1600.0*/100.000,	/* 3 2000.0*/100.000,	/* 4 2400.0*/100.000,	/* 5 2800.0*/120.000,	/* 6 3200.0*/120.000,	/* 7 3600.0*/120.000,	/* 8 4100.0*/120.000,	/* 9 4500.0*/120.000,	/* 10 4900.0*/120.000,	/* 11 5300.0*/120.000,	/* 12 5700.0*/120.000,	/* 13 6100.0*/120.000,	/* 14 6500.0*/120.000,	/* 15 7000.0*/120.000,	},
		{/* 13 140.000	*//* 0 800.0*/100.000,	/* 1 1200.0*/100.000,	/* 2 1600.0*/100.000,	/* 3 2000.0*/100.000,	/* 4 2400.0*/100.000,	/* 5 2800.0*/120.000,	/* 6 3200.0*/120.000,	/* 7 3600.0*/120.000,	/* 8 4100.0*/120.000,	/* 9 4500.0*/120.000,	/* 10 4900.0*/120.000,	/* 11 5300.0*/120.000,	/* 12 5700.0*/120.000,	/* 13 6100.0*/120.000,	/* 14 6500.0*/120.000,	/* 15 7000.0*/120.000,	},
		{/* 14 150.000	*//* 0 800.0*/100.000,	/* 1 1200.0*/100.000,	/* 2 1600.0*/100.000,	/* 3 2000.0*/100.000,	/* 4 2400.0*/100.000,	/* 5 2800.0*/120.000,	/* 6 3200.0*/120.000,	/* 7 3600.0*/120.000,	/* 8 4100.0*/120.000,	/* 9 4500.0*/120.000,	/* 10 4900.0*/120.000,	/* 11 5300.0*/120.000,	/* 12 5700.0*/120.000,	/* 13 6100.0*/120.000,	/* 14 6500.0*/120.000,	/* 15 7000.0*/120.000,	},
		{/* 15 160.000	*//* 0 800.0*/100.000,	/* 1 1200.0*/100.000,	/* 2 1600.0*/100.000,	/* 3 2000.0*/100.000,	/* 4 2400.0*/100.000,	/* 5 2800.0*/120.000,	/* 6 3200.0*/120.000,	/* 7 3600.0*/120.000,	/* 8 4100.0*/120.000,	/* 9 4500.0*/120.000,	/* 10 4900.0*/120.000,	/* 11 5300.0*/120.000,	/* 12 5700.0*/120.000,	/* 13 6100.0*/120.000,	/* 14 6500.0*/120.000,	/* 15 7000.0*/120.000,	},
};

EXTERN_CONFIG;

/**
 * pin 1I/W9 - extra +5v
 * set engine_type 14
 */
void setFordEscortGt(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	common079721_2351(PASS_CONFIG_PARAMETER_SIGNATURE);

	engineConfiguration->trigger.type = TT_MAZDA_DOHC_1_4;

	setFrankenso_01_LCD(engineConfiguration);
	setFrankenso0_1_joystick(engineConfiguration);

	setDensoTODO(config);

	engineConfiguration->globalFuelCorrection = 0.75;
	engineConfiguration->specs.displacement = 1.839;
	setAlgorithm(LM_SPEED_DENSITY PASS_CONFIG_PARAMETER_SUFFIX);

	static const float veRpmBins[] = 
	{
		800,
		1200,
		1600,
		2000,
		2400,
		2800,
		3200,
		3600,
		4100,
		4500,
		4900,
		5300,
		5700,
		6100,
		6500,
		7000
	};

	copyArray(config->veRpmBins, veRpmBins);


	MEMCPY(config->veTable, racingFestivaVeTable);

//	engineConfiguration->triggerInputPins[0] = GPIOC_6; // 2G YEL/BLU
//	engineConfiguration->triggerInputPins[1] = GPIOA_5; // 2E White CKP

	// in case of SOHC distributor we only have one signal
//	engineConfiguration->triggerInputPins[0] = GPIOA_5; // 2E White CKP
//	engineConfiguration->triggerInputPins[1] = GPIO_UNASSIGNED;

	// in case of DOHC distributor we have two signals
	engineConfiguration->triggerInputPins[0] = GPIOC_6;
	engineConfiguration->triggerInputPins[1] = GPIOA_5; // 2E White CKP

	// Denso 195500-2180
	engineConfiguration->injector.flow = 265;

	engineConfiguration->isFastAdcEnabled = true;
	engineConfiguration->map.sensor.type = MT_DENSO183;
	/**
	 * pin PA4: jumper W47<>W47 - ecu plug 3I
	 */
	engineConfiguration->map.sensor.hwChannel = EFI_ADC_4;

	setEgoSensor(ES_Innovate_MTX_L PASS_CONFIG_PARAMETER_SUFFIX);
	engineConfiguration->afr.hwChannel = EFI_ADC_2; // Frankenso analog #5 // PA2

	// set_idle_position 10
	engineConfiguration->manIdlePosition = 10;
	engineConfiguration->crankingIACposition = 65;

	setWholeIatCorrTimingTable(0 PASS_CONFIG_PARAMETER_SUFFIX);


	// set global_trigger_offset_angle -37
	engineConfiguration->globalTriggerAngleOffset = -37;
	// set ignition_offset 0
	engineConfiguration->ignitionOffset = 0;
	// set injection_offset 0
	engineConfiguration->extraInjectionOffset = 0;

	// todo: change to 15?
	// set cranking_timing_angle 3
	engineConfiguration->crankingTimingAngle = 3;
	engineConfiguration->crankingChargeAngle = 70;

	setLinearCurve(config->ignitionLoadBins, 20, 105, 5);
	setWholeTimingTable_d(10 PASS_CONFIG_PARAMETER_SUFFIX);
	setAfrMap(config->afrTable, 13.5);

	setSingleCoilDwell(PASS_CONFIG_PARAMETER_SIGNATURE);
	engineConfiguration->ignitionMode = IM_ONE_COIL;

	engineConfiguration->triggerSimulatorPinModes[0] = OM_OPENDRAIN;
	engineConfiguration->triggerSimulatorPinModes[1] = OM_OPENDRAIN;

	// individual coils
	// W6  PC9
	// W8  PC7
	// W12 PE8
	// W13 PE12

	engineConfiguration->ignitionPins[0] = GPIOC_9;
	engineConfiguration->ignitionPins[1] = GPIOC_7;
	engineConfiguration->ignitionPins[2] = GPIOE_8;
	engineConfiguration->ignitionPins[3] = GPIOE_12;
	engineConfiguration->ignitionPinMode = OM_DEFAULT;

	engineConfiguration->ignitionMode = IM_INDIVIDUAL_COILS;

	engineConfiguration->dizzySparkOutputPin = GPIOE_14; // Frankenso high side - pin 1G;

	/**
	 * Outputs
	 */
	// Frankenso low out #1: PE6
	// Frankenso low out #2: PE5 MIL
	// Frankenso low out #3:
	// Frankenso low out #4:
	// Frankenso low out #5: PE3 VICS solenoid
	// Frankenso low out #6: PE4
	// Frankenso low out #7: PE0<>PD5
	// Frankenso low out #8: PE2 INJ
	// Frankenso low out #9: PB9 IDLE
	// Frankenso low out #10: PE1<>PD3 INJ 1&3
	// Frankenso low out #11: PB8
	// Frankenso low out #12: PB7

	engineConfiguration->injectionPins[0] = GPIOD_3;
	engineConfiguration->injectionPins[1] = GPIOE_2;


	// 40% idle is good default
	engineConfiguration->idle.solenoidFrequency = 300;
	engineConfiguration->idle.solenoidPin = GPIOB_9;

	engineConfiguration->malfunctionIndicatorPin = GPIOE_5;
	engineConfiguration->malfunctionIndicatorPinMode = OM_DEFAULT;

	engineConfiguration->tunerStudioSerialSpeed = 19200;

	commonFrankensoAnalogInputs(engineConfiguration);
	setCommonNTCSensor(&engineConfiguration->clt, 2700);
	setCommonNTCSensor(&engineConfiguration->iat, 2700);

	// we have a 1999 Auto Miata TB mounted on this car
	engineConfiguration->tpsMin = 630; // convert 12to10 bit (ADC/4)
	engineConfiguration->tpsMax = 115; // convert 12to10 bit (ADC/4)

	engineConfiguration->tps1_1AdcChannel = EFI_ADC_3;
//	engineConfiguration->map.sensor.hwChannel = EFI_ADC_4;
	engineConfiguration->mafAdcChannel = EFI_ADC_0;
	engineConfiguration->clt.adcChannel = EFI_ADC_12;
	engineConfiguration->iat.adcChannel = EFI_ADC_11;

	// todo: 8.2 or 10k?
	engineConfiguration->vbattDividerCoeff = ((float) (10 + 33)) / 10 * 2;

	// VICS solenoid
#if EFI_FSIO
	/**
	 * to test
	 * set_fsio_setting 1 5000
	 */
	engineConfiguration->fsio_setting[0] = 5000;
	// set_fsio_expression 1 "rpm > fsio_setting(1)"
	setFsioExt(0, GPIOE_3, RPM_ABOVE_USER_SETTING_1, 150 PASS_CONFIG_PARAMETER_SUFFIX);

	// warning light
	/**
	 * to test
	 * set_fsio_setting 2 1800
	 * set_fsio_setting 3 95
	 * set_fsio_setting 4 14
	 *
	 * set_fsio_expression 2 "rpm > fsio_setting(2)"
	 * set_rpn_expression 1 "rpm 0 fsio_setting > coolant 1 fsio_setting > | vbatt 2 fsio_setting < |"
	 * eval "rpm 0 fsio_setting > coolant 1 fsio_setting > | vbatt 2 fsio_setting < |"
	 */
	engineConfiguration->fsio_setting[1] = 6200; // RPM threshold
	engineConfiguration->fsio_setting[2] = 90; // CLT threshold
	engineConfiguration->fsio_setting[3] = 13.5; // voltage threshold

//	setFsio(1, GPIOC_13, "rpm 2 fsio_setting > coolant 3 fsio_setting > | vbatt 4 fsio_setting < |" PASS_CONFIG_PARAMETER_SUFFIX);
	setFsio(1, GPIOD_7, RPM_ABOVE_USER_SETTING_2 PASS_CONFIG_PARAMETER_SUFFIX);
#endif /* EFI_FSIO */

	static const float ignitionRpmBins[] =
	{
		800,
		1200,
		1600,
		2000,
		2400,
		2800,
		3200,
		3600,
		4100,
		4500,
		4900,
		5300,
		5700,
		6100,
		6500,
		7000
	};

	copyArray(config->ignitionRpmBins, ignitionRpmBins);

#if IGN_LOAD_COUNT == DEFAULT_IGN_LOAD_COUNT
	MEMCPY(config->ignitionTable, racingFestivaIgnitionTable);
#endif

	engineConfiguration->tpsAccelEnrichmentThreshold = 40;

	engineConfiguration->engineLoadAccelEnrichmentThreshold = 5.0;
	engineConfiguration->engineLoadAccelEnrichmentMultiplier = 1;

	engineConfiguration->isSdCardEnabled = true;

//	engineConfiguration->useFSIO16ForTimingAdjustment = true;
//  we wanted to have a timinig table adjustment switch here
//	engineConfiguration->fsioAdc[0] = EFI_ADC_12; // PC2

	strcpy(config->fsioFormulas[15], ANALOG_CONDITION);

	// end of Ford Escort GT config
}

