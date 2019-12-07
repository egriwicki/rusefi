/*
 * boost_control.h
 *
 *  Created on: 18. aug. 2019
 *      Author: Ola
 */
#pragma once

#include "engine.h"
#include "periodic_task.h"



void turnBoostControlOn(void);

void startBoostPin(void);
void stopBoostPin(void);
void initBoostCtrl(Logging *sharedLogger);
void setBoostPFactor(float p);
void setBoostIFactor(float i);
void setBoostDFactor(float d);
void setDefaultBoostParameters(DECLARE_CONFIG_PARAMETER_SIGNATURE);
void showBoostInfo(void);
void unregisterBoostPin();
void onConfigurationChangeBoostCallback(engine_configuration_s *previousConfiguration);



