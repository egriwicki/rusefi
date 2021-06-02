#include "defaults.h"
#include "engine_configuration.h"

EXTERN_CONFIG;

void setDefaultBaseEngine(DECLARE_CONFIG_PARAMETER_SIGNATURE) {
	// Base Engine Settings
	engineConfiguration->specs.cylindersCount = 4;
	engineConfiguration->specs.displacement = 2;
	engineConfiguration->specs.firingOrder = FO_1_3_4_2;

	CONFIG(compressionRatio) = 9;

	// Limits and Fallbacks
	engineConfiguration->rpmHardLimit = 7000;
	engineConfiguration->cutFuelOnHardLimit = true;
	engineConfiguration->cutSparkOnHardLimit = true;

	engineConfiguration->failedMapFallback = 60;
	engineConfiguration->enableMapEstimationTableFallback = false;

	// Trigger
	engineConfiguration->trigger.type = TT_TOOTHED_WHEEL_60_2;
	setOperationMode(engineConfiguration, FOUR_STROKE_CAM_SENSOR);
	engineConfiguration->useOnlyRisingEdgeForTrigger = false;

	engineConfiguration->globalTriggerAngleOffset = 0;

	// Default this to on - if you want to diagnose, turn it off.
	engineConfiguration->silentTriggerError = true;

	// Advanced Trigger

	// Battery and alternator
	engineConfiguration->vbattDividerCoeff = ((float) (15 + 65)) / 15;

#if EFI_ALTERNATOR_CONTROL
	setDefaultAlternatorParameters(PASS_CONFIG_PARAMETER_SIGNATURE);
#endif /* EFI_ALTERNATOR_CONTROL */

	// Fuel pump
	engineConfiguration->startUpFuelPumpDuration = 4;

	// Fan
	engineConfiguration->fanOnTemperature = 95;
	engineConfiguration->fanOffTemperature = 91;

	// Tachometer
	// 50% duty cycle is the default for tach signal
	CONFIG(tachPulseDurationAsDutyCycle) = true;
	CONFIG(tachPulseDuractionMs) = 0.5;
	CONFIG(tachPulsePerRev) = 1;

	// Check engine light
#if EFI_PROD_CODE
	engineConfiguration->warningPeriod = 10;
#else
	engineConfiguration->warningPeriod = 0;
#endif /* EFI_PROD_CODE */
}
