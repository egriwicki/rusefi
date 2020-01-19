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
void setHardwareUsTimer(int32_t deltaTimeUs);

#ifdef __cplusplus
}
#endif /* __cplusplus */

