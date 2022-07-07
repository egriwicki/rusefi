/**
 *  @file thread_controller.h
 * 
 *  @date Jan 5, 2019
 *  @author Matthew Kennedy, (c) 2019
 */

#pragma once

#include "global.h"

/**
 * @brief A base class for a controller that requires its own thread.
 * 
 * Inherit from ThreadController.  Implement ThreadTask with the logic required for your thread.
 * The template parameter specifies the size of the stack used for the thread.  (because we have to
 * allocate the stack at compile time, it has to be a template parameter instead of a normal parameter)
 */
template <int TStackSize>
class ThreadController : public chibios_rt::BaseStaticThread<TStackSize>
{
private:
    const tprio_t m_prio;

protected:
    /**
     * Override this function to implement your controller's thread's behavior.
     */
    virtual void ThreadTask() = 0;

	const char* const m_name;

public:
    ThreadController(const char* name, tprio_t priority)
		: m_prio(priority)
		, m_name(name)
    {
    }

    /**
     * @brief Start the thread.
     */
    void Start()
    {
		this->start(m_prio);
    }

	void main() override {
		this->setName(m_name);

		ThreadTask();
	}
};
