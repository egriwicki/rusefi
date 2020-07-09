// this section was generated automatically by rusEfi tool ConfigDefinition.jar based on (unknown script) integration/pid_state.txt Mon Jul 06 06:07:19 UTC 2020
// by class com.rusefi.output.CHeaderConsumer
// begin
#pragma once
#include "rusefi_types.h"
// start of pid_state_s
struct pid_state_s {
	/**
	 * offset 0
	 */
	float iTerm = (float)0;
	/**
	 * we are remembering this only for debugging purposes
	 * offset 4
	 */
	float dTerm = (float)0;
	/**
	 * only used for logging
	 * offset 8
	 */
	float target = (float)0;
	/**
	 * only used for logging
	 * offset 12
	 */
	float input = (float)0;
	/**
	 * only used for logging
	 * offset 16
	 */
	float output = (float)0;
	/**
	 * offset 20
	 */
	float errorAmplificationCoef = (float)0;
	/**
	 * offset 24
	 */
	float previousError = (float)0;
	/** total size 28*/
};

typedef struct pid_state_s pid_state_s;

// end
// this section was generated automatically by rusEfi tool ConfigDefinition.jar based on (unknown script) integration/pid_state.txt Mon Jul 06 06:07:19 UTC 2020
