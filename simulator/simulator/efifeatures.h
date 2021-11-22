/*
 * efifeatures.h
 *
 * @date Mar 7, 2014
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#pragma once

// see SIM_SD1_PORT and SIM_SD2_PORT
#define TS_PRIMARY_SERIAL SD1
#define TS_SECONDARY_SERIAL SD2

#define EFI_ENABLE_ASSERTS TRUE
#define EFI_LAUNCH_CONTROL TRUE
#define EFI_ENABLE_MOCK_ADC TRUE

#define ENABLE_PERF_TRACE FALSE

#define EFI_PRINTF_FUEL_DETAILS FALSE
#define EFI_ENABLE_CRITICAL_ENGINE_STOP TRUE
#define EFI_ENABLE_ENGINE_WARNING TRUE

#define HW_CHECK_MODE FALSE

#define SC_BUFFER_SIZE 4000

#define EFI_ACTIVE_CONFIGURATION_IN_FLASH FALSE

#define EFI_BOOST_CONTROL FALSE

#define EFI_CANBUS_SLAVE FALSE

#define EFI_BOSCH_YAW FALSE

#define EFI_AUX_SERIAL FALSE

#define EFI_CLOCK_LOCKS FALSE

#define EFI_SERVO FALSE

#define EFI_LCD FALSE

#define TS_UART_MODE FALSE

#define EFI_USB_SERIAL FALSE

#define EFI_CDM_INTEGRATION FALSE

#define EFI_MC33816 FALSE
#define EFI_HPFP TRUE

#define EFI_BLUETOOTH_SETUP FALSE

#define EFI_GPIO_HARDWARE FALSE

#define BOARD_TLE6240_COUNT         0
#define BOARD_MC33972_COUNT			0
#define BOARD_TLE8888_COUNT 	0

#define EFI_FSIO TRUE

#define EFI_TEXT_LOGGING TRUE

#define EFI_MEMS FALSE

#ifndef EFI_TOOTH_LOGGER
#define EFI_TOOTH_LOGGER TRUE
#endif

#define CONSOLE_MAX_ACTIONS 256

#define EFI_USE_UART_DMA FALSE

#define EFI_MAP_AVERAGING TRUE
#define EFI_ALTERNATOR_CONTROL TRUE

#define EFI_ENGINE_AUDI_AAN FALSE
#define EFI_ENGINE_SNOW_BLOWER FALSE

/**
 * simulator works via self-stimulation which works via trigger emulation
 */
#define EFI_EMULATE_POSITION_SENSORS TRUE

#define EFI_SIGNAL_EXECUTOR_SLEEP TRUE
#define EFI_INTERNAL_ADC FALSE
#define EFI_EGT FALSE

#define EFI_SHAFT_POSITION_INPUT TRUE
#define EFI_UART_ECHO_TEST_MODE FALSE

#define EFI_ENGINE_CONTROL TRUE
#define EFI_IDLE_CONTROL TRUE

#define EFI_IDLE_PID_CIC TRUE
#define EFI_MAIN_RELAY_CONTROL FALSE
#define EFI_HIP_9011 TRUE
#define EFI_CJ125 TRUE
#define EFI_CAN_SUPPORT FALSE
#define EFI_MAX_31855 FALSE

#define EFI_ELECTRONIC_THROTTLE_BODY TRUE
#define EFI_AUX_PID TRUE
#define EFI_ICU_INPUTS FALSE

#ifndef HAL_TRIGGER_USE_PAL
#define HAL_TRIGGER_USE_PAL FALSE
#endif /* HAL_TRIGGER_USE_PAL */

#ifndef HAL_TRIGGER_USE_ADC
#define HAL_TRIGGER_USE_ADC FALSE
#endif /* HAL_TRIGGER_USE_ADC */

#define EFI_UART_GPS FALSE
#define EFI_HAS_RESET FALSE
#define EXTREME_TERM_LOGGING FALSE
#define DEBUG_FUEL FALSE
#define DEBUG_WAVE FALSE

#define EFI_DEFAILED_LOGGING FALSE
#define EFI_ENGINE_SNIFFER TRUE

#define FUEL_MATH_EXTREME_LOGGING FALSE
#define EFI_ANALOG_SENSORS TRUE
#define EFI_INTERNAL_FLASH FALSE
#define EFI_RTC FALSE
#define EFI_MALFUNCTION_INDICATOR FALSE
#define EFI_HD44780_LCD FALSE
#define EFI_LOGIC_ANALYZER FALSE
#define EFI_PWM_TESTER FALSE
#define TRIGGER_EXTREME_LOGGING FALSE
#define SPARK_EXTREME_LOGGING FALSE
#define DEBUG_PWM FALSE
#define EFI_SIGNAL_EXECUTOR_ONE_TIMER FALSE
#define EFI_TUNER_STUDIO_VERBOSE FALSE
#define EFI_FILE_LOGGING TRUE
#define EFI_WARNING_LED FALSE
#define EFI_VEHICLE_SPEED FALSE

#define EFI_SENSOR_CHART TRUE
#define EFI_HISTOGRAMS FALSE

#define EFI_TUNER_STUDIO TRUE

#define EFI_BOARD_TEST FALSE
#define EFI_JOYSTICK FALSE

#define EFI_LUA TRUE
#define LUA_USER_HEAP 100000
