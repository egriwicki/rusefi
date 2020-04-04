/*
 * @file smart_gpio.h
 *
 * @date Apr 13, 2019
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#pragma once

/* TLE6240 pins go right after on chips */
#define TLE6240_PIN(n)		((brain_pin_e)((int)BRAIN_PIN_LAST_ONCHIP + 1 + (n)))
/* MC33972 pins go right after TLE6240 */
#define MC33972_PIN(n)		((brain_pin_e)((int)BRAIN_PIN_LAST_ONCHIP + 1 + 16 + (n)))

void initSmartGpio(void);
void startSmartCsPins();
void stopSmartCsPins();

