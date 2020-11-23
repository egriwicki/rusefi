#include "global.h"

#if EFI_PROD_CODE && HAL_USE_PWM

#include "port_microsecond_timer.h"

#define SCHEDULER_PWM_DEVICE PWMD5

void portSetHardwareSchedulerTimer(efitick_t nowNt, efitick_t setTimeNt) {
	pwm_lld_enable_channel(&SCHEDULER_PWM_DEVICE, 0, setTimeNt);
	pwmEnableChannelNotificationI(&SCHEDULER_PWM_DEVICE, 0);
}

static void hwTimerCallback(PWMDriver*) {
	pwmDisableChannelNotificationI(&SCHEDULER_PWM_DEVICE, 0);
	portMicrosecondTimerCallback();
}

static constexpr PWMConfig timerConfig = {
	SCHEDULER_TIMER_FREQ,
	(uint32_t)-1,	// timer period = 2^32 counts
	nullptr,		// No update callback
	{
		{PWM_OUTPUT_DISABLED, hwTimerCallback},	// Channel 0 = timer callback, others unused
		{PWM_OUTPUT_DISABLED, nullptr},
		{PWM_OUTPUT_DISABLED, nullptr},
		{PWM_OUTPUT_DISABLED, nullptr}
	},
	0,	// CR1
	0	// CR2
};

void portInitMicrosecondTimer() {
	pwmStart(&SCHEDULER_PWM_DEVICE, &timerConfig);

	// ChibiOS doesn't let you configure timers in output compare mode, only PWM mode.
	// We want to be able to set the compare register without waiting for an update event
	// (which would take 358 seconds at 12mhz timer speed), so we have to use normal upcounting
	// output compare mode instead.
	SCHEDULER_TIMER_DEVICE->CCMR1 = 0x00006810;
}

uint32_t getTimeNowLowerNt() {
	// Using the same timer for measurement and scheduling improves
	// precision and reduces jitter.
	return TIM5->CNT;
}

#endif // EFI_PROD_CODE
