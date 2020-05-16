/*
 * @file global_execution_queue.h
 *
 *  Created on: Jan 9, 2019
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#pragma once

#include "scheduler.h"
#include "event_queue.h"

class TestExecutor : public ExecutorInterface {
public:
	void scheduleByTimestamp(scheduling_s *scheduling, efitimeus_t timeUs, action_s action) override;
	void scheduleByTimestampNt(scheduling_s *scheduling, efitick_t timeNt, action_s action) override;
	void scheduleForLater(scheduling_s *scheduling, int delayUs, action_s action) override;
	void clear();
	int executeAll(efitime_t now);
	int size();
	scheduling_s* getForUnitTest(int index);

	void setMockExecutor(ExecutorInterface* exec);
private:
	EventQueue schedulingQueue;
	ExecutorInterface* m_mockExecutor = nullptr;
};
