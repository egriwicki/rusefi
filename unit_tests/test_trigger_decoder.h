/**
 * @file	test_trigger_decoder.h
 *
 * @date Dec 24, 2013
 * @author Andrey Belomutskiy, (c) 2012-2017
 */

#ifndef TEST_TRIGGER_DECODER_H_
#define TEST_TRIGGER_DECODER_H_

void testTriggerDecoder(void);
void testFuelSchedulerBug299smallAndMedium(void);
void testFuelSchedulerBug299smallAndLarge(void);
void testDifferentInjectionModes(void);
void testSparkReverseOrderBug319(void);
void testMissedSpark299(void);
void testRpmCalculator(void);
void testStartupFuelPumping(void);
void test1995FordInline6TriggerDecoder(void);
void testTriggerDecoder2(const char *msg, engine_type_e type, int synchPointIndex, float channel1duty, float channel2duty);


#endif /* TEST_TRIGGER_DECODER_H_ */
