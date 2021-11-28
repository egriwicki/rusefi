/**
 * See also BinarySensorLog.java
 * See also mlq_file_format.txt
 */

#include "pch.h"

#include "binary_logging.h"
#include "log_field.h"
#include "crc.h"
#include "buffered_writer.h"

#define TIME_PRECISION 1000

// floating number of seconds with millisecond precision
static scaled_channel<uint32_t, TIME_PRECISION> packedTime;

// todo: we are at the edge of sdLogBuffer size and at the moment we have no code to make sure buffer does not overflow
// todo: make this logic smarter
static constexpr LogField fields[] = {
	{tsOutputChannels.RPMValue, GAUGE_NAME_RPM, "rpm", 0},
	{packedTime, GAUGE_NAME_TIME, "sec", 0},
	{tsOutputChannels.totalTriggerErrorCounter, GAUGE_NAME_TRG_ERR, "err", 0},
	{tsOutputChannels.vehicleSpeedKph, GAUGE_NAME_VVS, "kph", 0},
	{tsOutputChannels.internalMcuTemperature, GAUGE_NAME_CPU_TEMP, "C", 0},
	{tsOutputChannels.coolant, GAUGE_NAME_CLT, "C", 1},
	{tsOutputChannels.intake, GAUGE_NAME_IAT, "C", 1},
	{tsOutputChannels.auxTemp1, GAUGE_NAME_AUX_TEMP1, "C", 1},
	{tsOutputChannels.auxTemp2, GAUGE_NAME_AUX_TEMP2, "C", 1},
	{tsOutputChannels.throttlePedalPosition, GAUGE_NAME_TPS, "%", 2},
	{tsOutputChannels.TPS2Value, GAUGE_NAME_TPS2, "%", 2},
	{tsOutputChannels.throttlePedalPosition, GAUGE_NAME_THROTTLE_PEDAL, "%", 2},
	{tsOutputChannels.MAPValue, GAUGE_NAME_MAP, "kPa", 1},
	{tsOutputChannels.AFRValue, GAUGE_NAME_AFR, "afr", 2},
	{tsOutputChannels.AFRValue2, GAUGE_NAME_AFR2, "afr", 2},
	{tsOutputChannels.lambdaValue, GAUGE_NAME_LAMBDA, "", 3},
	{tsOutputChannels.lambdaValue2, GAUGE_NAME_LAMBDA2, "", 3},
	{tsOutputChannels.warningCounter, GAUGE_NAME_WARNING_COUNTER, "", 0},
	{tsOutputChannels.lastErrorCode, GAUGE_NAME_WARNING_LAST, "", 0},
	{tsOutputChannels.tuneCrc16, GAUGE_NAME_TUNE_CRC16, "", 0},
	{tsOutputChannels.engineMakeCodeNameCrc16, GAUGE_NAME_ENGINE_CRC16, "", 0},

	{tsOutputChannels.firmwareVersion, GAUGE_NAME_VERSION, "", 0},
	{tsOutputChannels.accelerationX, GAUGE_NAME_ACCEL_X, "", 2},
	{tsOutputChannels.accelerationY, GAUGE_NAME_ACCEL_Y, "", 2},
	{tsOutputChannels.accelerationZ, GAUGE_NAME_ACCEL_Z, "", 2},
	{tsOutputChannels.accelerationRoll, GAUGE_NAME_ACCEL_ROLL, "", 2},
	{tsOutputChannels.accelerationYaw, GAUGE_NAME_ACCEL_YAW, "", 2},

	{tsOutputChannels.debugIntField1, GAUGE_NAME_DEBUG_I1, "", 0},
	{tsOutputChannels.debugIntField2, GAUGE_NAME_DEBUG_I2, "", 0},
	{tsOutputChannels.debugIntField3, GAUGE_NAME_DEBUG_I3, "", 0},
	{tsOutputChannels.debugIntField4, GAUGE_NAME_DEBUG_I4, "", 0},
	{tsOutputChannels.debugIntField5, GAUGE_NAME_DEBUG_I5, "", 0},
	{tsOutputChannels.debugFloatField1, GAUGE_NAME_DEBUG_F1, "", 3},
	{tsOutputChannels.debugFloatField2, GAUGE_NAME_DEBUG_F2, "", 3},
	{tsOutputChannels.debugFloatField3, GAUGE_NAME_DEBUG_F3, "", 3},
	{tsOutputChannels.debugFloatField4, GAUGE_NAME_DEBUG_F4, "", 3},
	{tsOutputChannels.debugFloatField5, GAUGE_NAME_DEBUG_F5, "", 3},
	{tsOutputChannels.debugFloatField6, GAUGE_NAME_DEBUG_F6, "", 3},
	{tsOutputChannels.debugFloatField7, GAUGE_NAME_DEBUG_F7, "", 3},
	{tsOutputChannels.VBatt, GAUGE_NAME_VBAT, "v", 2},
	{tsOutputChannels.oilPressure, GAUGE_NAME_OIL_PRESSURE, GAUGE_NAME_FUEL_PRESSURE_HIGH_UNITS, 0},
	{tsOutputChannels.lowFuelPressure, GAUGE_NAME_FUEL_PRESSURE_LOW, GAUGE_NAME_FUEL_PRESSURE_LOW_UNITS, 0},
	{tsOutputChannels.highFuelPressure, GAUGE_NAME_FUEL_PRESSURE_HIGH, GAUGE_NAME_FUEL_PRESSURE_HIGH_UNITS, 0},
	{tsOutputChannels.vvtPositionB1I, GAUGE_NAME_VVT_B1I, "deg", 1},
	{tsOutputChannels.vvtPositionB1E, GAUGE_NAME_VVT_B1E, "deg", 1},
	{tsOutputChannels.vvtPositionB2I, GAUGE_NAME_VVT_B2I, "deg", 1},
	{tsOutputChannels.vvtPositionB2E, GAUGE_NAME_VVT_B2E, "deg", 1},
	{tsOutputChannels.vvtTargets[0], GAUGE_NAME_VVT_TARGET_B1I, "deg", 0},
	{tsOutputChannels.vvtTargets[1], GAUGE_NAME_VVT_TARGET_B1E, "deg", 0},
	{tsOutputChannels.vvtTargets[2], GAUGE_NAME_VVT_TARGET_B2I, "deg", 0},
	{tsOutputChannels.vvtTargets[3], GAUGE_NAME_VVT_TARGET_B2E, "deg", 0},
	{tsOutputChannels.wastegatePositionSensor, GAUGE_NAME_WG_POSITION, "%", 2},
	{tsOutputChannels.idlePositionSensor, GAUGE_NAME_IDLE_POSITION, "%", 2},
	{tsOutputChannels.chargeAirMass, GAUGE_NAME_AIR_MASS, "g", 3},
	{tsOutputChannels.currentTargetAfr, GAUGE_NAME_TARGET_AFR, "afr", 2},
	{tsOutputChannels.targetLambda, GAUGE_NAME_TARGET_LAMBDA, "", 3},
	{tsOutputChannels.baseFuel, GAUGE_NAME_FUEL_BASE, "ms", 3},
	{tsOutputChannels.fuelRunning, GAUGE_NAME_FUEL_RUNNING, "ms", 3},
	{tsOutputChannels.actualLastInjection, GAUGE_NAME_FUEL_LAST_INJECTION, "ms", 3},
	{tsOutputChannels.injectorDutyCycle, GAUGE_NAME_FUEL_INJ_DUTY, "%", 0},
	{tsOutputChannels.veValue, GAUGE_NAME_FUEL_VE, "%", 1},
	{tsOutputChannels.tCharge, GAUGE_NAME_TCHARGE, "C", 1},
	{tsOutputChannels.injectorLagMs, GAUGE_NAME_INJECTOR_LAG, "ms", 3},
	{tsOutputChannels.fuelPidCorrection[0], GAUGE_NAME_FUEL_TRIM, "%", 2},
	{tsOutputChannels.fuelPidCorrection[1], GAUGE_NAME_FUEL_TRIM_2, "%", 2},
	{tsOutputChannels.wallFuelCorrection, GAUGE_NAME_FUEL_WALL_CORRECTION, "ms", 3},
	{tsOutputChannels.tpsAccelFuel, GAUGE_NAME_FUEL_TPS_EXTRA, "ms", 3},
	{tsOutputChannels.ignitionAdvance, GAUGE_NAME_TIMING_ADVANCE, "deg", 1},
	{tsOutputChannels.sparkDwellValue, GAUGE_COIL_DWELL_TIME, "ms", 1},
	{tsOutputChannels.coilDutyCycle, GAUGE_NAME_DWELL_DUTY, "%", 0},
	{tsOutputChannels.idleAirValvePosition, GAUGE_NAME_IAC, "%", 1},
	{tsOutputChannels.etbTarget, GAUGE_NAME_ETB_TARGET, "%", 2},
	{tsOutputChannels.etb1DutyCycle, GAUGE_NAME_ETB_DUTY, "%", 1},
	{tsOutputChannels.etb1Error, GAUGE_NAME_ETB_ERROR, "%", 3},
	{tsOutputChannels.fuelTankLevel, GAUGE_NAME_FUEL_LEVEL, "%", 0},
	{tsOutputChannels.fuelingLoad, GAUGE_NAME_FUEL_LOAD, "%", 1},
	{tsOutputChannels.ignitionLoad, GAUGE_NAME_IGNITION_LOAD, "%", 1},
	{tsOutputChannels.massAirFlowValue, GAUGE_NAME_AIR_FLOW, "kg/h", 1},
	{tsOutputChannels.tcuDesiredGear, GAUGE_NAME_DESIRED_GEAR, "gear", 0},
	{tsOutputChannels.tcuCurrentGear, GAUGE_NAME_CURRENT_GEAR, "gear", 0},
	{tsOutputChannels.flexPercent, GAUGE_NAME_FLEX, "%", 1},
	{tsOutputChannels.fuelFlowRate, GAUGE_NAME_FUEL_FLOW, "g/s", 3},
	{tsOutputChannels.totalFuelConsumption, GAUGE_NAME_FUEL_CONSUMPTION, "g", 1},
	{tsOutputChannels.knockLevel, GAUGE_NAME_KNOCK_LEVEL, "dBv", 0},
	{tsOutputChannels.knock[0],  GAUGE_NAME_KNOCK_1,  "dBv", 0},
	{tsOutputChannels.knock[1],  GAUGE_NAME_KNOCK_2,  "dBv", 0},
	{tsOutputChannels.knock[2],  GAUGE_NAME_KNOCK_3,  "dBv", 0},
	{tsOutputChannels.knock[3],  GAUGE_NAME_KNOCK_4,  "dBv", 0},
	{tsOutputChannels.knock[4],  GAUGE_NAME_KNOCK_5,  "dBv", 0},
	{tsOutputChannels.knock[5],  GAUGE_NAME_KNOCK_6,  "dBv", 0},
	{tsOutputChannels.knock[6],  GAUGE_NAME_KNOCK_7,  "dBv", 0},
	{tsOutputChannels.knock[7],  GAUGE_NAME_KNOCK_8,  "dBv", 0},
	{tsOutputChannels.knock[8],  GAUGE_NAME_KNOCK_9,  "dBv", 0},
	{tsOutputChannels.knock[9],  GAUGE_NAME_KNOCK_10, "dBv", 0},
	{tsOutputChannels.knock[10], GAUGE_NAME_KNOCK_11, "dBv", 0},
	{tsOutputChannels.knock[11], GAUGE_NAME_KNOCK_12, "dBv", 0},
};

static constexpr uint16_t computeFieldsRecordLength() {
	uint16_t recLength = 0;
	for (size_t i = 0; i < efi::size(fields); i++) {
		recLength += fields[i].getSize();
	}

	return recLength;
}

static constexpr uint16_t recordLength = computeFieldsRecordLength();

void writeHeader(Writer& outBuffer) {
	char buffer[MLQ_HEADER_SIZE];
	// File format: MLVLG\0
	strncpy(buffer, "MLVLG", 6);

	// Format version = 01
	buffer[6] = 0;
	buffer[7] = 1;

	// Timestamp
	buffer[8] = 0;
	buffer[9] = 0;
	buffer[10] = 0;
	buffer[11] = 0;

	// Info data start
	buffer[12] = 0;
	buffer[13] = 0;

	size_t headerSize = MLQ_HEADER_SIZE + efi::size(fields) * 55;

	// Data begin index: begins immediately after the header
	buffer[14] = 0;
	buffer[15] = 0;
	buffer[16] = (headerSize >> 8) & 0xFF;
	buffer[17] = headerSize & 0xFF;

	// Record length - length of a single data record: sum size of all fields
	buffer[18] = recordLength >> 8;
	buffer[19] = recordLength & 0xFF;

	// Number of logger fields
	buffer[20] = 0;
	buffer[21] = efi::size(fields);

	outBuffer.write(buffer, MLQ_HEADER_SIZE);

	// Write the actual logger fields, offset 22
	for (size_t i = 0; i < efi::size(fields); i++) {
		fields[i].writeHeader(outBuffer);
	}
}

static uint8_t blockRollCounter = 0;

size_t writeBlock(char* buffer) {
	// Offset 0 = Block type, standard data block in this case
	buffer[0] = 0;

	// Offset 1 = rolling counter sequence number
	buffer[1] = blockRollCounter++;

	// Offset 2, size 2 = Timestamp at 10us resolution
	uint16_t timestamp = getTimeNowUs() / 10;
	buffer[2] = timestamp >> 8;
	buffer[3] = timestamp & 0xFF;

	packedTime = currentTimeMillis() * 1.0 / TIME_PRECISION;

	// Offset 4 = field data
	const char* dataBlockStart = buffer + 4;
	char* dataBlock = buffer + 4;
	for (size_t i = 0; i < efi::size(fields); i++) {
		size_t entrySize = fields[i].writeData(dataBlock);

		// Increment pointer to next entry
		dataBlock += entrySize;
	}

	size_t dataBlockSize = dataBlock - dataBlockStart;

	// "CRC" at the end is just the sum of all bytes
	uint8_t sum = 0;
	for (size_t i = 0; i < dataBlockSize; i++) {
		sum += dataBlockStart[i];
	}
	*dataBlock = sum;

	// Total size has 4 byte header + 1 byte checksum
	return dataBlockSize + 5;
}
