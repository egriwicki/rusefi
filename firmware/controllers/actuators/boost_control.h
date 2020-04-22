/*
 * boost_control.h
 *
 *  Created on: 18. aug. 2019
 *      Author: Ola Ruud
 */
#pragma once

#include "engine.h"
#include "periodic_task.h"
#include "closed_loop_controller.h"
#include "pid.h"

class SimplePwm;

class BoostController : public ClosedLoopController<float, percent_t>, public PeriodicTimerController {
public:
	DECLARE_ENGINE_PTR;

	void init(SimplePwm* pmw, const ValueProvider3D* openLoopMap, const ValueProvider3D* closedLoopTargetMap, pid_s* pidParams);

	// PeriodicTimerController implementation
	int getPeriodMs() override;
	void PeriodicTask() override;
	void reset();

	// Called when the configuration may have changed.  Controller will
	// reset if necessary.
	void onConfigurationChange(pid_s* previousConfiguration);

	// Helpers for individual parts of boost control
	expected<float> observePlant() const override;
	expected<float> getSetpoint() const override;

	expected<percent_t> getOpenLoop(float target) const override;
	expected<percent_t> getClosedLoop(float setpoint, float target) override;

	void setOutput(expected<percent_t> outputValue) override;

private:
	bool m_shouldResetPid = false;
	Pid m_pid;

	const ValueProvider3D* m_openLoopMap = nullptr;
	const ValueProvider3D* m_closedLoopTargetMap = nullptr;
	SimplePwm* m_pwm = nullptr;
};

void startBoostPin();
void stopBoostPin();
void initBoostCtrl(Logging *sharedLogger DECLARE_ENGINE_PARAMETER_SUFFIX);
void setDefaultBoostParameters(DECLARE_CONFIG_PARAMETER_SIGNATURE);
void onConfigurationChangeBoostCallback(engine_configuration_s *previousConfiguration);
