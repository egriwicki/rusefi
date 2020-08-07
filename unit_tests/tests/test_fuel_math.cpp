#include "engine_test_helper.h"
#include "fuel_math.h"
#include "alphan_airmass.h"
#include "maf_airmass.h"
#include "mocks.h"

#include "gtest/gtest.h"

using ::testing::StrictMock;
using ::testing::FloatNear;

TEST(FuelMath, getStandardAirCharge) {
	WITH_ENGINE_TEST_HELPER(TEST_ENGINE);

	// Miata 1839cc 4cyl
	CONFIG(specs.displacement) = 1.839f;
	CONFIG(specs.cylindersCount) = 4;

	EXPECT_FLOAT_EQ(0.5535934f, getStandardAirCharge(PASS_ENGINE_PARAMETER_SIGNATURE));

	// LS 5.3 liter v8
	CONFIG(specs.displacement) = 5.327f;
	CONFIG(specs.cylindersCount) = 8;

	EXPECT_FLOAT_EQ(0.80179232f, getStandardAirCharge(PASS_ENGINE_PARAMETER_SIGNATURE));

	// Chainsaw - single cylinder 32cc
	CONFIG(specs.displacement) = 0.032f;
	CONFIG(specs.cylindersCount) = 1;
	EXPECT_FLOAT_EQ(0.038531788f, getStandardAirCharge(PASS_ENGINE_PARAMETER_SIGNATURE));

	// Leopard 1 47.666 liter v12
	CONFIG(specs.displacement) = 47.666f;
	CONFIG(specs.cylindersCount) = 12;

	EXPECT_FLOAT_EQ(4.782959f, getStandardAirCharge(PASS_ENGINE_PARAMETER_SIGNATURE));
}

TEST(AirmassModes, AlphaNNormal) {
	WITH_ENGINE_TEST_HELPER(TEST_ENGINE);
	// 4 cylinder 4 liter = easy math
	engineConfiguration->specs.displacement = 4.0f;
	engineConfiguration->specs.cylindersCount = 4;

	StrictMock<MockVp3d> veTable;

	EXPECT_CALL(veTable, getValue(1200, FloatNear(0.71f, EPS4D)))
		.WillOnce(Return(35.0f));

	AlphaNAirmass dut(veTable);
	INJECT_ENGINE_REFERENCE(&dut);

	Sensor::setMockValue(SensorType::Tps1, 0.71f);

	// Mass of 1 liter of air * VE
	float expectedAirmass = 1.2047f * 0.35f;

	auto result = dut.getAirmass(1200);
	EXPECT_NEAR(result.CylinderAirmass, expectedAirmass, EPS4D);
	EXPECT_NEAR(result.EngineLoadPercent, 0.71f, EPS4D);
}

TEST(AirmassModes, AlphaNFailedTps) {
	WITH_ENGINE_TEST_HELPER(TEST_ENGINE);

	// Shouldn't get called
	StrictMock<MockVp3d> veTable;

	AlphaNAirmass dut(veTable);
	INJECT_ENGINE_REFERENCE(&dut);

	// explicitly reset the sensor
	Sensor::resetMockValue(SensorType::Tps1);
	// Ensure that it's actually failed
	ASSERT_FALSE(Sensor::get(SensorType::Tps1).Valid);

	auto result = dut.getAirmass(1200);
	EXPECT_EQ(result.CylinderAirmass, 0);
}

TEST(misc, MafNormal) {
	WITH_ENGINE_TEST_HELPER(FORD_ASPIRE_1996);
	engineConfiguration->fuelAlgorithm = LM_REAL_MAF;
	engineConfiguration->injector.flow = 200;
	setAfrMap(config->afrTable, 13);

	MockVp3d veTable;
	// Ensure that the correct cell is read from the VE table
	EXPECT_CALL(veTable, getValue(6000, FloatNear(70.9814f, EPS4D)))
		.WillOnce(Return(75.0f));

	MafAirmass dut(veTable);
	INJECT_ENGINE_REFERENCE(&dut);

	auto airmass = dut.getAirmassImpl(200, 6000);

	// Check results
	EXPECT_NEAR(0.277777f * 0.75f, airmass.CylinderAirmass, EPS4D);
	EXPECT_NEAR(70.9814f, airmass.EngineLoadPercent, EPS4D);
}
