/**
 * @file	trigger_renix.cpp
 *
 * https://en.wikipedia.org/wiki/Renix
 * Has something to do with AMC/Jeep
 *
 * @date Apr 18, 2020
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#include "trigger_renix.h"

// TT_RENIX_44_2_2
void initializeRenix44_2_2(TriggerWaveform *s) {
	s->initialize(FOUR_STROKE_SYMMETRICAL_CRANK_SENSOR);

	// 44-2-2 is symmetrical so we only need to define one half
	float tooth = 180.0 / 22; // hint: tooth = 8.181818 degrees

	float currentAngle = 0;
	for (int i = 0;i < 20;i++) {
		s->addEventAngle(currentAngle + tooth / 2, T_PRIMARY, TV_RISE);
		s->addEventAngle(currentAngle + tooth, T_PRIMARY, TV_FALL);
		currentAngle += tooth;
	}

	s->addEventAngle(currentAngle + tooth, T_PRIMARY, TV_RISE);

	// float math error accumulates at this point so we have to spell out 180
	s->addEventAngle(180, T_PRIMARY, TV_FALL);

}
