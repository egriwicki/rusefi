#include "pch.h"
#include "init.h"
#include "electronic_throttle_impl.h"
#include "live_data.h"

static EtbController * initEtbIntegratedTest() {
	etbPidReset(); // ETB controlles are global shared instances :(

	engineConfiguration->tps1_1AdcChannel = EFI_ADC_3;
	engineConfiguration->tps1_2AdcChannel = EFI_ADC_3;

	engineConfiguration->throttlePedalPositionAdcChannel = EFI_ADC_3;
	engineConfiguration->throttlePedalPositionSecondAdcChannel = EFI_ADC_3;


	Sensor::setMockValue(SensorType::Tps1, 25.0f, true);

	initTps();
	doInitElectronicThrottle();
	return (EtbController*)engine->etbControllers[0];
}

TEST(etb, integrated) {
	EngineTestHelper eth(TEST_ENGINE); // we have a destructor so cannot move EngineTestHelper into utility method
	EtbController *etb = initEtbIntegratedTest();

	Sensor::setMockValue(SensorType::AcceleratorPedalPrimary, 40);
	Sensor::setMockValue(SensorType::AcceleratorPedalSecondary, 40);

	etb->update();

	ASSERT_EQ(engine->outputChannels.etbTarget, 40);
	ASSERT_EQ(etb->prevOutput, 100);
	ASSERT_EQ(etb->etbDutyAverage, 50);

	Sensor::setMockValue(SensorType::AcceleratorPedal, 10, true);
	etb->update();
	ASSERT_EQ(etb->etbDutyAverage, -25);
	ASSERT_EQ(etb->etbDutyRateOfChange, -75);

	float destination;
	int offset = ELECTRONIC_THROTTLE_BASE_ADDRESS + offsetof(electronic_throttle_s, etbDutyRateOfChange);
	copyRange((uint8_t*)&destination, getLiveDataFragments(), offset, sizeof(destination));
	ASSERT_EQ(destination, -75);
}

extern int timeNowUs;

TEST(etb, intermittentTps) {
	EngineTestHelper eth(TEST_ENGINE); // we have a destructor so cannot move EngineTestHelper into utility method
	EtbController *etb = initEtbIntegratedTest();

	// Tell the sensor checker that the ignition is on
	engine->module<SensorChecker>()->onIgnitionStateChanged(true);
	engine->module<SensorChecker>()->onSlowCallback();
	timeNowUs += 10e6;
	engine->module<SensorChecker>()->onSlowCallback();

	ASSERT_TRUE(engine->module<SensorChecker>()->analogSensorsShouldWork());

	ASSERT_FALSE(isTps1Error());
	ASSERT_FALSE(isTps2Error());
	ASSERT_TRUE(isPedalError());

	etb->update();

	EXPECT_EQ(0, etb->etbInputErrorCounter);

	int badCount = 0;

	// Do some bad/good/bad/good cycles, make sure count keeps up
	for (size_t i = 0; i < 10; i++) {
		Sensor::setInvalidMockValue(SensorType::Tps1);
		ASSERT_TRUE(isTps1Error());
		etb->update();

		badCount++;
		EXPECT_EQ(badCount, etb->etbInputErrorCounter);

		Sensor::setMockValue(SensorType::Tps1, 20);
		ASSERT_FALSE(isTps1Error());
		etb->update();
	}
}
