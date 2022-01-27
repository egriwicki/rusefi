/*
 * @file test_fuelCut.cpp
 *
 * @date Mar 22, 2018
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#include "pch.h"

#include "event_queue.h"
#include "fsio_impl.h"

using ::testing::_;

TEST(fuelCut, coasting) {
	EngineTestHelper eth(TEST_ENGINE);
	EXPECT_CALL(eth.mockAirmass, getAirmass(_))
		.WillRepeatedly(Return(AirmassResult{0.1008f, 50.0f}));

	// configure coastingFuelCut
	engineConfiguration->coastingFuelCutEnabled = true;
	engineConfiguration->coastingFuelCutRpmLow = 1300;
	engineConfiguration->coastingFuelCutRpmHigh = 1500;
	engineConfiguration->coastingFuelCutTps = 2;
	engineConfiguration->coastingFuelCutClt = 30;
	engineConfiguration->coastingFuelCutMap = 100;
	// set cranking threshold
	engineConfiguration->cranking.rpm = 999;

	// basic engine setup
	setupSimpleTestEngineWithMafAndTT_ONE_trigger(&eth);

	Sensor::setMockValue(SensorType::Map, 0);

	// mock CLT - just above threshold ('hot engine')
	float hotClt = engineConfiguration->coastingFuelCutClt + 1;
	Sensor::setMockValue(SensorType::Clt, hotClt);
	// mock TPS - throttle is opened
	Sensor::setMockValue(SensorType::DriverThrottleIntent, 60);
	// set 'running' RPM - just above RpmHigh threshold
	Sensor::setMockValue(SensorType::Rpm, engineConfiguration->coastingFuelCutRpmHigh + 1);
	// 'advance' time (amount doesn't matter)
	eth.moveTimeForwardUs(1000);

	const float normalInjDuration = 1.5f;
	/*
	 * We need to pass through all rpm changes (high-mid-low-mid-high) because of state-machine
	 */

	// process
	eth.engine.periodicFastCallback();

	// Define some helpers for not-cut and cut
	#define EXPECT_NORMAL() EXPECT_FLOAT_EQ(normalInjDuration, engine->injectionDuration)
	#define EXPECT_CUT() EXPECT_FLOAT_EQ(0, engine->injectionDuration)

	// this is normal injection mode (the throttle is opened), no fuel cut-off
	EXPECT_NORMAL();

	// 'releasing' the throttle
	Sensor::setMockValue(SensorType::DriverThrottleIntent, 0);
	eth.engine.periodicFastCallback();

	// Fuel cut-off is enabled now
	EXPECT_CUT();

	// Now drop the CLT below threshold
	Sensor::setMockValue(SensorType::Clt, engineConfiguration->coastingFuelCutClt - 1);
	eth.engine.periodicFastCallback();

	// Fuel cut-off should be diactivated - the engine is 'cold'
	EXPECT_NORMAL();

	// restore CLT
	Sensor::setMockValue(SensorType::Clt, hotClt);
	// And set RPM - somewhere between RpmHigh and RpmLow threshold
	Sensor::setMockValue(SensorType::Rpm, (engineConfiguration->coastingFuelCutRpmHigh + engineConfiguration->coastingFuelCutRpmLow) / 2);
	eth.engine.periodicFastCallback();

	// Fuel cut-off is enabled - nothing should change
	EXPECT_NORMAL();

	// Now drop RPM just below RpmLow threshold
	Sensor::setMockValue(SensorType::Rpm, engineConfiguration->coastingFuelCutRpmLow - 1);
	eth.engine.periodicFastCallback();

	// Fuel cut-off is now disabled (the engine is idling)
	EXPECT_NORMAL();

	// Now change RPM just below RpmHigh threshold
	Sensor::setMockValue(SensorType::Rpm, engineConfiguration->coastingFuelCutRpmHigh - 1);
	eth.engine.periodicFastCallback();

	// Fuel cut-off is still disabled
	EXPECT_NORMAL();

	// Now set RPM just above RpmHigh threshold
	Sensor::setMockValue(SensorType::Rpm, engineConfiguration->coastingFuelCutRpmHigh + 1);
	eth.engine.periodicFastCallback();

	// Fuel cut-off is active again!
	EXPECT_CUT();

	// Configure vehicle speed thresholds
	engineConfiguration->coastingFuelCutVssHigh = 50;
	engineConfiguration->coastingFuelCutVssLow = 40;

	// High speed, should still be cut.
	Sensor::setMockValue(SensorType::VehicleSpeed, 55);
	eth.engine.periodicFastCallback();
	EXPECT_CUT();

	// Between thresholds, still cut.
	Sensor::setMockValue(SensorType::VehicleSpeed, 45);
	eth.engine.periodicFastCallback();
	EXPECT_CUT();

	// Below lower threshold, normal fuel resumes
	Sensor::setMockValue(SensorType::VehicleSpeed, 35);
	eth.engine.periodicFastCallback();
	EXPECT_NORMAL();

	// Between thresholds, still normal.
	Sensor::setMockValue(SensorType::VehicleSpeed, 45);
	eth.engine.periodicFastCallback();
	EXPECT_NORMAL();

	// Back above upper, cut again.
	Sensor::setMockValue(SensorType::VehicleSpeed, 55);
	eth.engine.periodicFastCallback();
	EXPECT_CUT();
}

