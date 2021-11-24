// this section was generated automatically by rusEFI tool ConfigDefinition.jar based on (unknown script) controllers/actuators/boost_control.txt Tue Nov 23 21:00:21 UTC 2021
// by class com.rusefi.output.CHeaderConsumer
// begin
#pragma once
#include "rusefi_types.h"
// start of boost_control_s
struct boost_control_s {
	/**
	offset 0 bit 0 */
	bool isTpsValid : 1 {};
	/**
	offset 0 bit 1 */
	bool m_shouldResetPid : 1 {};
	/**
	offset 0 bit 2 */
	bool isBelowClosedLoopThreshold : 1 {};
	/**
	offset 0 bit 3 */
	bool unusedBit_3_3 : 1 {};
	/**
	offset 0 bit 4 */
	bool unusedBit_3_4 : 1 {};
	/**
	offset 0 bit 5 */
	bool unusedBit_3_5 : 1 {};
	/**
	offset 0 bit 6 */
	bool unusedBit_3_6 : 1 {};
	/**
	offset 0 bit 7 */
	bool unusedBit_3_7 : 1 {};
	/**
	offset 0 bit 8 */
	bool unusedBit_3_8 : 1 {};
	/**
	offset 0 bit 9 */
	bool unusedBit_3_9 : 1 {};
	/**
	offset 0 bit 10 */
	bool unusedBit_3_10 : 1 {};
	/**
	offset 0 bit 11 */
	bool unusedBit_3_11 : 1 {};
	/**
	offset 0 bit 12 */
	bool unusedBit_3_12 : 1 {};
	/**
	offset 0 bit 13 */
	bool unusedBit_3_13 : 1 {};
	/**
	offset 0 bit 14 */
	bool unusedBit_3_14 : 1 {};
	/**
	offset 0 bit 15 */
	bool unusedBit_3_15 : 1 {};
	/**
	offset 0 bit 16 */
	bool unusedBit_3_16 : 1 {};
	/**
	offset 0 bit 17 */
	bool unusedBit_3_17 : 1 {};
	/**
	offset 0 bit 18 */
	bool unusedBit_3_18 : 1 {};
	/**
	offset 0 bit 19 */
	bool unusedBit_3_19 : 1 {};
	/**
	offset 0 bit 20 */
	bool unusedBit_3_20 : 1 {};
	/**
	offset 0 bit 21 */
	bool unusedBit_3_21 : 1 {};
	/**
	offset 0 bit 22 */
	bool unusedBit_3_22 : 1 {};
	/**
	offset 0 bit 23 */
	bool unusedBit_3_23 : 1 {};
	/**
	offset 0 bit 24 */
	bool unusedBit_3_24 : 1 {};
	/**
	offset 0 bit 25 */
	bool unusedBit_3_25 : 1 {};
	/**
	offset 0 bit 26 */
	bool unusedBit_3_26 : 1 {};
	/**
	offset 0 bit 27 */
	bool unusedBit_3_27 : 1 {};
	/**
	offset 0 bit 28 */
	bool unusedBit_3_28 : 1 {};
	/**
	offset 0 bit 29 */
	bool unusedBit_3_29 : 1 {};
	/**
	offset 0 bit 30 */
	bool unusedBit_3_30 : 1 {};
	/**
	offset 0 bit 31 */
	bool unusedBit_3_31 : 1 {};
	/**
	 * offset 4
	 */
	float openLoopPart = (float)0;
	/**
	 * offset 8
	 */
	float closedLoopPart = (float)0;
	/** total size 12*/
};

// end
// this section was generated automatically by rusEFI tool ConfigDefinition.jar based on (unknown script) controllers/actuators/boost_control.txt Tue Nov 23 21:00:21 UTC 2021
