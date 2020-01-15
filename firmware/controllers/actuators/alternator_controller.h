/**
 * @file    alternator_controller.h
 * @brief   alternator controller
 *
 * @date Apr 6, 2014
 * @author Dmitry Sidin
 * @author Andrey Belomutskiy, (c) 2012-2020
 *
 */
#ifndef ALTERNATORCONTROLLER_H_
#define ALTERNATORCONTROLLER_H_

#include "engine.h"
void initAlternatorCtrl(Logging *sharedLogger);
void setAltPFactor(float p);
void setAltIFactor(float p);
void setAltDFactor(float p);
void showAltInfo(void);
void setDefaultAlternatorParameters(DECLARE_CONFIG_PARAMETER_SIGNATURE);

void onConfigurationChangeAlternatorCallback(engine_configuration_s *previousConfiguration);

#endif /* ALTERNATORCONTROLLER_H_ */
