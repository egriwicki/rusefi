/*
 * test_pid_auto.cpp
 *
 *  Created on: Sep 14, 2017
 * @author Andrey Belomutskiy, (c) 2012-2017
 */


#include "main.h"
#include "test_pid_auto.h"
#include "pid_auto_tune.h"

efitimems_t mockTimeMs = 0;

efitimems_t currentTimeMillis(void) {
	return mockTimeMs;
}

Logging logging;

static zigZagValue(int index) {
	int i = index % 20;
	if ( i <= 10)
		 return i * 10;
	return (20 - i) * 10;

}

void testPidAutoZigZag() {
	mockTimeMs = 0;

	PID_AutoTune at;

	at.outputStart = 50;

	at.input = zigZagValue(mockTimeMs);
	at.Runtime(&logging);

	mockTimeMs++;
	at.input = zigZagValue(mockTimeMs);
	at.Runtime(&logging);
	assertEqualsLM("min@1", 0, at.absMin);
	assertEqualsLM("max@1", 10, at.absMax);
	assertEqualsM("peakCount", 0, at.peakCount);

	for (;mockTimeMs<=11;mockTimeMs++) {
		at.input = zigZagValue(mockTimeMs);
		at.Runtime(&logging);

	}
	assertEqualsLM("min@11", 0, at.absMin);
	assertEqualsLM("max@11", 100, at.absMax);
	assertEqualsM("peakCount", 0, at.peakCount);

	for (;mockTimeMs<=21;mockTimeMs++) {
		at.input = zigZagValue(mockTimeMs);
		at.Runtime(&logging);
	}
	assertEqualsM("peakCount@21", 1, at.peakCount);

	for (;mockTimeMs<=41;mockTimeMs++) {
		at.input = zigZagValue(mockTimeMs);
		at.Runtime(&logging);
	}
	assertEqualsM("peakCount@41", 2, at.peakCount);
	assertEqualsM("Pu@41", 1, cisnan(at.Pu));

	for (;mockTimeMs<=60;mockTimeMs++) {
		at.input = zigZagValue(mockTimeMs);
		at.Runtime(&logging);
	}
	assertEqualsM("peakCount@60", 3, at.peakCount);
	assertEqualsM("Pu@60", 0.02, at.Pu);

}
