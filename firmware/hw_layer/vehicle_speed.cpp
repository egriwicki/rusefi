/**
 * @file vehicle_speed.cpp
 *
 * @date Dec 26, 2014
 * @author Andrey Belomutskiy, (c) 2012-2017
 */

#include "vehicle_speed.h"

#if EFI_VEHICLE_SPEED || defined(__DOXYGEN__)

#include "engine.h"
#include "digital_input_hw.h"
#include "pin_repository.h"

EXTERN_ENGINE
;

static Logging *logger;

static efitick_t lastSignalTimeNt = 0;
static efitick_t vssDiff = 0;
static uint16_t tickOdoDistance;

#define DEFAULT_MOCK_SPEED -1
static float mockVehicleSpeed = DEFAULT_MOCK_SPEED; // in kilometers per hour

void setMockVehicleSpeed(float speedKPH) {
	mockVehicleSpeed = speedKPH;
}

/**
 * @return vehicle speed, in kilometers per hour
 */
float getVehicleSpeed(void) {
	if (mockVehicleSpeed != DEFAULT_MOCK_SPEED)
		return mockVehicleSpeed;
	if (!hasVehicleSpeedSensor())
		return 0;
	efitick_t nowNt = getTimeNowNt();
	if (nowNt - lastSignalTimeNt > US2NT(US_PER_SECOND_LL))
		return 0; // previous signal time is too long ago - we are stopped

	return engineConfiguration->vehicleSpeedCoef * US2NT(US_PER_SECOND_LL) / vssDiff;
}

static void vsAnaWidthCallback(void) {
	engine->engineState.vssDebugEventCounter++;
	efitick_t nowNt = getTimeNowNt();
	vssDiff = nowNt - lastSignalTimeNt;
	engine->engineState.mazdaOdoCounter = engine->engineState.mazdaOdoCounter + tickOdoDistance;
}

static void speedInfo(void) {
	scheduleMsg(logger, "VSS input at %s",
			hwPortname(boardConfiguration->vehicleSpeedSensorInputPin));

	scheduleMsg(logger, "c=%f eventCounter=%d speed=%f",
			engineConfiguration->vehicleSpeedCoef,
			engine->engineState.vssDebugEventCounter,
			getVehicleSpeed());
	scheduleMsg(logger, "vss diff %d", vssDiff);

}

bool hasVehicleSpeedSensor() {
	return boardConfiguration->vehicleSpeedSensorInputPin != GPIO_UNASSIGNED;
}

void stopVSSPins(void) {
	stopWaveAnalyzerDriver("VSS", activeConfiguration.bc.vehicleSpeedSensorInputPin);
}

void startVSSPins(void) {
	// todo
}

void initVehicleSpeed(Logging *l) {
	logger = l;
	addConsoleAction("speedinfo", speedInfo);
	if (!hasVehicleSpeedSensor())
		return;
	digital_input_s* vehicleSpeedInput = initWaveAnalyzerDriver("VSS", boardConfiguration->vehicleSpeedSensorInputPin);
	startInputDriver(vehicleSpeedInput, true);
	tickOdoDistance =  (uint16_t)((engineConfiguration->vehicleSpeedCoef*0.277*2.58)*256.0); //2.58 - magic coeff for RX8 cluster
	vehicleSpeedInput->widthListeners.registerCallback((VoidInt) vsAnaWidthCallback, NULL);
}
#else  /* EFI_VEHICLE_SPEED */

float getVehicleSpeed(void) {
	// no VSS support
	return 0;
}
#endif /* EFI_VEHICLE_SPEED */
