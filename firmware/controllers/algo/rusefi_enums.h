/**
 * @file	rusefi_enums.h
 * @brief	Fundamental rusEfi enumerable types live here
 *
 * @note this file should probably not include any other files
 *
 * @date Jan 14, 2014
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#pragma once

#include "efifeatures.h"
#include "obd_error_codes.h"
#include "live_data_ids.h"
#include "engine_types.h"
// we do not want to start the search for header from current folder so we use brackets here
// https://stackoverflow.com/questions/21593/what-is-the-difference-between-include-filename-and-include-filename
#include <rusefi_hw_enums.h>

#define PERCENT_MULT 100.0f
#define PERCENT_DIV 0.01f

typedef enum {
	ADC_OFF = 0,
	ADC_SLOW = 1,
	ADC_FAST = 2,

	Force_4_bytes_size_adc_channel_mode = ENUM_32_BITS,
} adc_channel_mode_e;

typedef enum {
	TV_FALL = 0,
	TV_RISE = 1
} trigger_value_e;

// see also PWM_PHASE_MAX_WAVE_PER_PWM
// todo: better names?
typedef enum {
	T_PRIMARY = 0,
	T_SECONDARY = 1,
	// todo: I really do not want to call this 'tertiary'. maybe we should rename all of these?
	T_CHANNEL_3 = 2,
	T_NONE = 15
} trigger_wheel_e;

// see also 'HW_EVENT_TYPES'
typedef enum {
	SHAFT_PRIMARY_FALLING = 0,
	SHAFT_PRIMARY_RISING = 1,
	SHAFT_SECONDARY_FALLING = 2,
	SHAFT_SECONDARY_RISING = 3,
	SHAFT_3RD_FALLING = 4,
	SHAFT_3RD_RISING = 5,
} trigger_event_e;

typedef enum  __attribute__ ((__packed__)) {
	/**
	 * This mode is useful for troubleshooting and research - events are logged but no effects on phase synchronization
	 */
	VVT_INACTIVE = 0,

	/**
	 * Single-tooth cam sensor mode where TDC and cam signal happen in opposite 360 degree of 720 degree engine cycle
	 */
	VVT_SECOND_HALF = 1,
	/**
	 * Toyota 2JZ has three cam tooth. We pick one of these three tooth to synchronize based on the expected angle position of the event
	 */
	VVT_2JZ = 2,
	/**
	 * Mazda NB2 has three cam tooth. We synchronize based on gap ratio.
	 * @see TT_VVT_MIATA_NB2
	 */
	VVT_MIATA_NB2 = 3,

	/**
	 * Single-tooth cam sensor mode where TDC and cam signal happen in the same 360 degree of 720 degree engine cycle
	 */
	VVT_FIRST_HALF = 4,
	/**
	 * @see TT_VVT_BOSCH_QUICK_START
	 */
	VVT_BOSCH_QUICK_START = 5,

	/**
	 * 1.8l Toyota 1ZZ-FE https://rusefi.com/forum/viewtopic.php?f=3&t=1735
	 * 4 minus one
	 * looks abandoned and unfinished?
	 */
	VVT_TOYOTA_4_1 = 6,

	VVT_FORD_ST170 = 7,

	VVT_BARRA_3_PLUS_1 = 8,

	VVT_NISSAN_VQ = 9,

	/**
	 * 4 plus one
	 */
	VVT_HONDA_K = 10,

	VVT_NISSAN_MR = 11,

	VVT_12 = 12,

	VVT_MAP_V_TWIN_ANOTHER = 13,

} vvt_mode_e;

/**
 * This enum is used to select your desired Engine Load calculation algorithm
 */
typedef enum {
	/**
	 * Speed Density algorithm - Engine Load is a function of MAP, VE and target AFR
	 * http://articles.sae.org/8539/
	 */
	LM_SPEED_DENSITY = 3,

	/**
	 * MAF with a known kg/hour function
	 */
	LM_REAL_MAF = 4,

	LM_ALPHA_N = 5,

	LM_LUA = 6,

	// This mode is for unit testing only, so that tests don't have to rely on a particular real airmass mode
	LM_MOCK = 100,

	Force_4_bytes_size_engine_load_mode = ENUM_32_BITS,
} engine_load_mode_e;

typedef enum {
	DM_NONE = 0,
	DM_HD44780 = 1,
	DM_HD44780_OVER_PCF8574 = 2,

	Force_4_bytes_size_display_mode = ENUM_32_BITS,

} display_mode_e;

typedef enum  __attribute__ ((__packed__)) {
	TL_AUTO = 0,
	TL_SEMI_AUTO = 1,
	TL_MANUAL = 2,
	TL_HALL = 3,

} tle8888_mode_e;


typedef enum {
	LF_NATIVE = 0,
	/**
	 * http://www.efianalytics.com/MegaLogViewer/
	 * log example: http://svn.code.sf.net/p/rusefi/code/trunk/misc/ms_logs/
	 */
	LM_MLV = 1,

	Force_4_bytes_size_log_format = ENUM_32_BITS,
} log_format_e;

typedef enum {
	/**
	 * In auto mode we currently have some pid-like-but-not really PID logic which is trying
	 * to get idle RPM to desired value by dynamically adjusting idle valve position.
	 * TODO: convert to PID
	 */
	IM_AUTO = 0,
	/**
	 * Manual idle control is extremely simple: user just specifies desired idle valve position
	 * which could be adjusted according to current CLT
	 */
	IM_MANUAL = 1,
	Force_4_bytes_size_idle_mode = ENUM_32_BITS,
} idle_mode_e;

typedef enum __attribute__ ((__packed__)) {
	/**
	 * GND for logical OFF, VCC for logical ON
	 */
	OM_DEFAULT = 0,
	/**
	 * GND for logical ON, VCC for logical OFF
	 */
	OM_INVERTED = 1,
	/**
	 * logical OFF is floating, logical ON is GND
	 */
	OM_OPENDRAIN = 2,
	OM_OPENDRAIN_INVERTED = 3
} pin_output_mode_e;

typedef enum __attribute__ ((__packed__)) {
	PI_DEFAULT = 0,
	PI_PULLUP = 1,
	PI_PULLDOWN = 2
} pin_input_mode_e;

#define CRANK_MODE_MULTIPLIER 2.0f

/**
 * @see getCycleDuration
 * @see getEngineCycle
 */
// todo: better enum name
typedef enum {
	OM_NONE = 0,
	/**
	 * 720 degree engine cycle but trigger is defined using a 360 cycle which is when repeated.
	 * For historical reasons we have a pretty weird approach where one crank trigger revolution is
	 * defined as if it's stretched to 720 degress. See CRANK_MODE_MULTIPLIER
	 */
	FOUR_STROKE_CRANK_SENSOR = 1,
	/**
	 * 720 degree engine and trigger cycle
	 */
	FOUR_STROKE_CAM_SENSOR = 2,
	/**
	 * 360 degree cycle
	 */
	TWO_STROKE = 3,

	/**
	 * 720 degree engine cycle but trigger is defined using a 180 cycle which is when repeated three more times
	 * In other words, same pattern is repeated on the crank wheel twice.
	 */
	FOUR_STROKE_SYMMETRICAL_CRANK_SENSOR = 4,

	/**
	 * Same pattern repeated three times on crank wheel. Crazy, I know!
	 */
	FOUR_STROKE_THREE_TIMES_CRANK_SENSOR = 5,

	Force_4_bytes_size_operation_mode_e = ENUM_32_BITS,
} operation_mode_e;

/**
 * @brief Ignition Mode
 */
typedef enum {
	/**
	 * in this mode only SPARKOUT_1_OUTPUT is used
	 */
	IM_ONE_COIL = 0,
	/**
	 * in this mode we use as many coils as we have cylinders
	 */
	IM_INDIVIDUAL_COILS = 1,
	IM_WASTED_SPARK = 2,

	/**
	 * some v12 engines line BMW M70 and M73 run two distributors, one for each bank of cylinders
	 */
	IM_TWO_COILS = 3,

	Force_4_bytes_size_ignition_mode = ENUM_32_BITS,
} ignition_mode_e;

/**
 * @see getNumberOfInjections
 */
typedef enum {
	/**
	 * each cylinder has it's own injector but they all works in parallel
	 */
	IM_SIMULTANEOUS = 0,
	/**
	 * each cylinder has it's own injector, each injector is wired separately
	 */
	IM_SEQUENTIAL = 1,
	/**
	 * each cylinder has it's own injector but these injectors work in pairs. Injectors could be wired in pairs or separately.
	 * Each pair is fired once per engine cycle
	 * todo: we might want to implement one additional mode where each pair of injectors is floating twice per engine cycle.
	 * todo: this could reduce phase offset from injection to stroke but would not work great for large injectors
	 */
	IM_BATCH = 2,
	/**
	 * only one injector located in throttle body
	 */
	IM_SINGLE_POINT = 3,


	Force_4_bytes_size_injection_mode = ENUM_32_BITS,
} injection_mode_e;

/**
 * @brief Ignition Mode while cranking
 */
typedef enum {
	CIM_DEFAULT = 0,
	CIM_FIXED_ANGLE = 1,

	// todo: make this a one byte enum
	Force_4_bytes_size_cranking_ignition_mode = ENUM_32_BITS,
} cranking_ignition_mode_e;

typedef enum __attribute__ ((__packed__)) {
	UART_NONE = 0,
	UART_DEVICE_1 = 1,
	UART_DEVICE_2 = 2,
	UART_DEVICE_3 = 3,
	UART_DEVICE_4 = 4,
} uart_device_e;

typedef enum __attribute__ ((__packed__)) {
	_5MHz,
	_2_5MHz,
	_1_25MHz,
	_150KHz
} spi_speed_e;


/**
 * See spi3mosiPin
 * See spi2MisoMode
 */
typedef enum __attribute__ ((__packed__)) {
	SPI_NONE = 0,
	SPI_DEVICE_1 = 1,
	SPI_DEVICE_2 = 2,
	SPI_DEVICE_3 = 3,
	SPI_DEVICE_4 = 4,
} spi_device_e;

typedef enum {
	BMW_e46 = 0,
	W202 = 1,
	Force_4_bytes_size_can_vss_nbc_e = ENUM_32_BITS,
} can_vss_nbc_e;

/**
 * inertia measurement unit, yawn accelerometer
 * By the way both kinds of BOSCH use Housing : TE 1-967640-1, pins 144969-1 seal 967056-1 plug 967067-2
 */
typedef enum  __attribute__ ((__packed__)) {
	IMU_NONE = 0,
	IMU_VAG = 1,
	/**
	 * f037000002
	 * https://github.com/rusefi/rusefi_documentation/blob/master/OEM-Docs/Bosch/Data%20Sheet_68903691_Acceleration_Sensor_MM5.10.pdf
	 */
	IMU_MM5_10 = 2,
	IMU_TYPE_3 = 3,
	IMU_TYPE_4 = 4,
} imu_type_e;

typedef enum {
	ES_BPSX_D1 = 0,
	/**
	 * same as innovate LC2
	 * 0v->7.35afr, 5v->22.39
	 */
	ES_Innovate_MTX_L = 1,
	/**
	 * Same as AEM
	 * 0v->10.0afr
	 * 5v->20.0afr
	 */
	ES_14Point7_Free = 2,

	ES_NarrowBand = 3,

	ES_PLX = 4,

	ES_Custom = 5,

	ES_AEM = 6,

	Force_4_bytes_size_ego_sensor = ENUM_32_BITS,
} ego_sensor_e;

typedef brain_pin_e output_pin_e;

typedef enum {
	MT_CUSTOM = 0,
	MT_DENSO183 = 1,
	/**
	 * 20 to 250 kPa (2.9 to 36.3 psi) 0.2 to 4.9 V OUTPUT
	 */
	MT_MPX4250 = 2,
	MT_HONDA3BAR = 3,
	MT_DODGE_NEON_2003 = 4,
	/**
	 * 22012AA090
	 */
	MT_SUBY_DENSO = 5,
	/**
	 * 16040749
	 */
	MT_GM_3_BAR = 6,

	/**
	 * 20 to 105 kPa (2.9 to 15.2 psi) 0.3 to 4.9 V Output
	 */
	MT_MPX4100 = 7,

	/**
	 * http://rusefi.com/forum/viewtopic.php?f=3&t=906&p=18976#p18976
	 * Toyota 89420-02010
	 */
	MT_TOYOTA_89420_02010 = 8,

	/**
	 * 20 to 250 kPa (2.9 to 36.3 psi) 0.25 to 4.875 OUTPUT
	 * More precise calibration data for new NXP sensor revisions MPX4250A and MPXA4250A.
	 * For an old Freescale MPX4250D use "MT_MPX4250".
	 * See https://www.nxp.com/docs/en/data-sheet/MPX4250A.pdf
	 */
	MT_MPX4250A = 9, 
	

	/**
	 * Bosch 2.5 Bar TMap Map Sensor with IAT
	 * 20 kPa at 0.40V, 250 kPa at 4.65V
	 * 4 pin:
	 *    Pin 1 : Sensor Ground
	 *    Pin 2 : Temp Signal
	 *    Pin 3 : 5v
	 *    Pin 4 : Map Signal
	 * Volkswagen Passat B6
	 */

	MT_BOSCH_2_5 = 10,

	MT_MAZDA_1_BAR = 11,

	MT_GM_2_BAR = 12,

	MT_GM_1_BAR = 13,

	MT_MPXH6400 = 14,

	Force_4_bytes_size_cranking_map_type = ENUM_32_BITS,
} air_pressure_sensor_type_e;

typedef enum {
	SC_OFF = 0,
	/**
	 * You would use this value if you want to see a detailed graph of your trigger events
	 */
	SC_TRIGGER = 1,
	// unused 2
	SC_RPM_ACCEL = 3,
	SC_DETAILED_RPM = 4,
	SC_AUX_FAST1 = 5,

	Internal_ForceMyEnumIntSize_sensor_chart = ENUM_32_BITS,
} sensor_chart_e;

typedef enum {
	REVERSE = -1,
	NEUTRAL = 0,
	GEAR_1 = 1,
	GEAR_2 = 2,
	GEAR_3 = 3,
	GEAR_4 = 4,

} gear_e;

typedef enum {
	CUSTOM = 0,
	Bosch0280218037 = 1,
	Bosch0280218004 = 2,
	DensoTODO = 3,
	Internal_ForceMyEnumIntSize_maf_sensor = ENUM_32_BITS,
} maf_sensor_type_e;

typedef enum {
	/**
	 * This is the default mode in which ECU controls timing dynamically
	 */
	TM_DYNAMIC = 0,
	/**
	 * Fixed timing is useful while you are playing with a timing gun - you need to have fixed
	 * timing if you want to install your distributor at some specific angle
	 */
	TM_FIXED = 1,

	Internal_ForceMyEnumIntSize_timing_mode = ENUM_32_BITS,
} timing_mode_e;

typedef enum {
    CS_OPEN = 0,
    CS_CLOSED = 1,
    CS_SWIRL_TUMBLE = 2,

	Internal_ForceMyEnumIntSize_chamber_stype = ENUM_32_BITS,
} chamber_style_e;

/**
 * Net Body Computer types
 */
typedef enum {
	CAN_BUS_NBC_NONE = 0,
	CAN_BUS_NBC_FIAT = 1,
	CAN_BUS_NBC_VAG = 2,
	CAN_BUS_MAZDA_RX8 = 3,
	CAN_BUS_NBC_BMW = 4,
	CAN_BUS_W202_C180 = 5,
    CAN_BUS_BMW_E90 = 6,
	CAN_BUS_Haltech = 7,
	CAN_BUS_MQB = 8,
	CAN_BUS_NISSAN_VQ = 9,
	CAN_BUS_GENESIS_COUPE = 10,
	CAN_BUS_HONDA_K = 11,
	CAN_AIM_DASH = 12,

	Internal_ForceMyEnumIntSize_can_nbc = ENUM_32_BITS,
} can_nbc_e;

typedef enum {
	TCHARGE_MODE_RPM_TPS = 0,
	TCHARGE_MODE_AIR_INTERP = 1,
	Force_4bytes_size_tChargeMode_e = ENUM_32_BITS,
} tChargeMode_e;

// peak type
typedef enum {
  MINIMUM = -1,
  NOT_A_PEAK = 0,
  MAXIMUM = 1
} PidAutoTune_Peak;

// auto tuner state
typedef enum {
  AUTOTUNER_OFF = 0,
  STEADY_STATE_AT_BASELINE = 1,
  STEADY_STATE_AFTER_STEP_UP = 2,
  RELAY_STEP_UP = 4,
  RELAY_STEP_DOWN = 8,
  CONVERGED = 16,
  FAILED = 128
} PidAutoTune_AutoTunerState;

typedef enum {
	INIT = 0,
	TPS_THRESHOLD = 1,
	RPM_DEAD_ZONE = 2,
	PID_VALUE = 4,
	PID_UPPER = 16,
	BLIP = 64,
	/**
	 * Live Docs reads 4 byte value so we want 4 byte enum
	 */
	Force_4bytes_size_idle_state_e = ENUM_32_BITS,
} idle_state_e;

typedef enum {
	OPEN_LOOP = 0,
	CLOSED_LOOP = 1,
	Force_4bytes_size_boostType_e = ENUM_32_BITS,
} boostType_e;

typedef enum {
	SWITCH_INPUT_LAUNCH = 0,
	CLUTCH_INPUT_LAUNCH = 1,
	ALWAYS_ACTIVE_LAUNCH = 2,
	Force_4bytes_size_launchActivationMode_e = ENUM_32_BITS,
} launchActivationMode_e;

typedef enum {
	SWITCH_INPUT_ANTILAG = 0,
	ALWAYS_ON_ANTILAG = 1,
	Force_4bytes_size_antiLagActivationMode_e = ENUM_32_BITS,
} antiLagActivationMode_e;

typedef enum __attribute__ ((__packed__)) {
	GPPWM_Tps = 0,
	GPPWM_Map = 1,
	GPPWM_Clt = 2,
	GPPWM_Iat = 3,
	GPPWM_FuelLoad = 4,
	GPPWM_IgnLoad = 5,
	GPPWM_AuxTemp1 = 6,
	GPPWM_AuxTemp2 = 7,
	GPPWM_Zero = 8,
	GPPWM_AccelPedal = 9,
	GPPWM_Vbatt = 10,
} gppwm_channel_e;

typedef enum __attribute__ ((__packed__)) {
	B100KBPS = 0, // 100kbps
	B250KBPS = 1, // 250kbps
	B500KBPS = 2, // 500kbps
	B1MBPS = 3, // 1Mbps
} can_baudrate_e;

typedef enum __attribute__ ((__packed__)) {
	GPPWM_GreaterThan = 0,
	GPPWM_LessThan = 1,
} gppwm_compare_mode_e;

typedef enum __attribute__ ((__packed__)) {
	VE_None = 0,
	VE_MAP = 1,
	VE_TPS = 2,
} ve_override_e;

typedef enum __attribute__ ((__packed__)) {
	AFR_None = 0,
	AFR_MAP = 1,
	AFR_Tps = 2,
	AFR_AccPedal = 3,
	AFR_CylFilling = 4,
} load_override_e;

typedef enum __attribute__ ((__packed__)) {
// todo: rename to HB_None?
	ETB_None = 0,
	ETB_Throttle1 = 1,
	ETB_Throttle2 = 2,
	ETB_IdleValve = 3,
	ETB_Wastegate = 4,
// todo: rename to dc_function_e? rename to hbrg_function_e?
} etb_function_e;

typedef enum __attribute__ ((__packed__)) {
	STEPPER_FULL = 0,
	STEPPER_HALF = 2,
	STEPPER_FOURTH = 4,
	STEPPER_EIGHTH = 8,
} stepper_num_micro_steps_e;

typedef enum __attribute__ ((__packed__)) {
	IPT_Low = 0,
	IPT_High = 1,
} injector_pressure_type_e;

typedef enum __attribute__ ((__packed__)) {
	ICM_None = 0,
	ICM_FixedRailPressure = 1,
	ICM_SensedRailPressure = 2,
} injector_compensation_mode_e;

typedef enum __attribute__ ((__packed__)) {
	INJ_None = 0,
	INJ_PolynomialAdder = 1,
} InjectorNonlinearMode;

typedef enum __attribute__ ((__packed__)) {
    HPFP_CAM_NONE = 0,
    HPFP_CAM_IN1 = 1,
    HPFP_CAM_EX1 = 2,
    HPFP_CAM_IN2 = 3,
    HPFP_CAM_EX2 = 4,
} hpfp_cam_e;
