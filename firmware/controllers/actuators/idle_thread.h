/**
 * @file	idle_thread.h
 * @brief	Idle Valve Control thread
 *
 * @date May 23, 2013
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#pragma once

#include "engine_module.h"
#include "engine_ptr.h"
#include "rusefi_types.h"
#include "periodic_task.h"
#include "pid.h"

struct IIdleController {
	enum class Phase : uint8_t {
		Cranking,	// Below cranking threshold
		Idling,		// Below idle RPM, off throttle
		Coasting,	// Off throttle but above idle RPM
		CrankToIdleTaper, // Taper between cranking and idling
		Running,	// On throttle
	};

	virtual Phase determinePhase(int rpm, int targetRpm, SensorResult tps, float vss, float crankingTaperFraction) const = 0;
	virtual int getTargetRpm(float clt) const = 0;
	virtual float getCrankingOpenLoop(float clt) const = 0;
	virtual float getRunningOpenLoop(float clt, SensorResult tps) const = 0;
	virtual float getOpenLoop(Phase phase, float clt, SensorResult tps, float crankingTaperFraction) const = 0;
	virtual float getClosedLoop(Phase phase, float tps, int rpm, int target) = 0;
	virtual float getCrankingTaperFraction() const = 0;
};

class IdleController : public IIdleController, public EnginePtr, public EngineModule {
public:
	typedef IIdleController interface_t;

	void init(pid_s* idlePidConfig);

	float getIdlePosition();

	// TARGET DETERMINATION
	int getTargetRpm(float clt) const override;

	// PHASE DETERMINATION: what is the driver trying to do right now?
	Phase determinePhase(int rpm, int targetRpm, SensorResult tps, float vss, float crankingTaperFraction) const override;
	float getCrankingTaperFraction() const override;

	// OPEN LOOP CORRECTIONS
	float getCrankingOpenLoop(float clt) const override;
	float getRunningOpenLoop(float clt, SensorResult tps) const override;
	float getOpenLoop(Phase phase, float clt, SensorResult tps, float crankingTaperFraction) const override;

	float getIdleTimingAdjustment(int rpm);
	float getIdleTimingAdjustment(int rpm, int targetRpm, Phase phase);

	// CLOSED LOOP CORRECTION
	float getClosedLoop(IIdleController::Phase phase, float tpsPos, int rpm, int targetRpm) override;

	void onConfigurationChange(engine_configuration_s const * previousConfig) final;
	void onSlowCallback() final;

	// Allow querying state from outside
	bool isIdlingOrTaper() {
		return m_lastPhase == Phase::Idling || (CONFIG(useSeparateIdleTablesForCrankingTaper) && m_lastPhase == Phase::CrankToIdleTaper);
	}

private:
	// These are stored by getIdlePosition() and used by getIdleTimingAdjustment()
	Phase m_lastPhase = Phase::Cranking;
	int m_lastTargetRpm = 0;

	// This is stored by getClosedLoop and used in case we want to "do nothing"
	float m_lastAutomaticPosition = 0;

	Pid m_timingPid;
};

percent_t getIdlePosition();

void applyIACposition(percent_t position);
void setManualIdleValvePosition(int positionPercent);

void startIdleThread();
void setDefaultIdleParameters();
void startIdleBench(void);
void setIdleOffset(float value);
void setIdlePFactor(float value);
void setIdleIFactor(float value);
void setIdleDFactor(float value);
void setIdleMode(idle_mode_e value);
void setTargetIdleRpm(int value);
Pid * getIdlePid();
void startPedalPins();
void stopPedalPins();
