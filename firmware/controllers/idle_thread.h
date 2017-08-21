/**
 * @file	idle_thread.h
 * @brief	Idle Valve Control thread
 *
 * @date May 23, 2013
 * @author Andrey Belomutskiy, (c) 2012-2017
 */

#ifndef IDLE_THREAD_H_
#define IDLE_THREAD_H_

#include "engine.h"

percent_t getIdlePosition(void);
void setIdleValvePosition(int positionPercent);
void startIdleThread(Logging*sharedLogger);
void setDefaultIdleParameters(void);
void startIdleBench(void);
void setIdleDT(int value);
void setIdleOffset(float value);
void setIdlePFactor(float value);
void setIdleIFactor(float value);
void setIdleDFactor(float value);
void setIdleMode(idle_mode_e value);
void setTargetIdleRpm(int value);
void setIdleDT(int value);
void onConfigurationChangeIdleCallback(engine_configuration_s *previousConfiguration);

#endif /* IDLE_THREAD_H_ */
