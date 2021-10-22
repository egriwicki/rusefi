/*
 * @file mercedes.cpp
 *
 * @date: Sep 17, 2021
 * @author Andrey Belomutskiy, (c) 2012-2021
 */

#include "pch.h"
#include "custom_engine.h"

static void common(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	setHellenDefaultVrThresholds(PASS_CONFIG_PARAMETER_SIGNATURE);
	engineConfiguration->vrThreshold[0].pin = GPIOD_14;
}

void setHellenMercedes128_4_cyl(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
    common(PASS_CONFIG_PARAMETER_SIGNATURE);
}

// is this M104 or M112 or both?
void setHellenMercedes128_6_cyl(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
    common(PASS_CONFIG_PARAMETER_SIGNATURE);
	engineConfiguration->specs.cylindersCount = 6;
	// 1-4-2-5-3-6 M104
	engineConfiguration->specs.firingOrder = FO_1_4_3_6_2_5; // M112
}

// M113
void setHellenMercedes128_8_cyl(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
    common(PASS_CONFIG_PARAMETER_SIGNATURE);
	engineConfiguration->specs.cylindersCount = 8;
	engineConfiguration->specs.firingOrder = FO_1_5_4_2_6_3_7_8;
}
