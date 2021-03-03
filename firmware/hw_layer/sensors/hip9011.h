/**
 * @file	hip9011.h
 * @brief	HIP9011/TPIC8101 driver
 *
 * @date Nov 27, 2013
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#pragma once

#define HIP_THREAD_PERIOD 100

void initHip9011(Logging *sharedLogger);
void startHip9001_pins();
void stopHip9001_pins();
void setHip9011FrankensoPinout(void);
#if HAL_USE_ADC
void hipAdcCallback(adcsample_t value);
#endif /* HAL_USE_ADC */
void setHipGain(float value);
void setHipBand(float value);
void setPrescalerAndSDO(int value);
void setKnockThresh(float value);
void setMaxKnockSubDeg(int value);

void intHoldCallback(trigger_event_e ckpEventType, uint32_t index, efitick_t edgeTimestamp DECLARE_ENGINE_PARAMETER_SUFFIX);
