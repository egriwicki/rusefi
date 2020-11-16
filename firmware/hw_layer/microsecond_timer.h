/**
 * @file	microsecond_timer.h
 *
 * @date Apr 14, 2014
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

void initMicrosecondTimer(void);
void setHardwareSchedulerTimer(efitick_t nowNt, efitick_t setTimeNt);

#define TOO_FAR_INTO_FUTURE_US (10 * US_PER_SECOND)
#define TOO_FAR_INTO_FUTURE_NT US2NT(TOO_FAR_INTO_FUTURE_US)

#ifdef __cplusplus
}
#endif /* __cplusplus */
