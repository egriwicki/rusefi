// this https://en.wikipedia.org/wiki/Reverse_Polish_notation is generated automatically
// from controllers/system_fsio.txt
// on 2017-06-25_01_22_09
//
//
// in this file we define system FSIO expressions
//
// system_fsio.txt is input for compile_fsio_file tool, see gen_system_fsio.bat
//
// see http://rusefi.com/wiki/index.php?title=Manual:Flexible_Logic
//
// Jan 19, 2017
// Andrey Belomutskiy, (c) 2012-2017
//

// Human-readable: (fan and (coolant > fan_off_setting)) | (coolant > fan_on_setting)
#define FAN_CONTROL_LOGIC "fan coolant fan_off_setting > and coolant fan_on_setting > |"

// Human-readable: (time_since_boot < startup_fuel_pump_duration) | (rpm > 0)
#define FUEL_PUMP_LOGIC "time_since_boot startup_fuel_pump_duration < rpm 0 > |"

// Human-readable: vbatt < 14.5
#define ALTERNATOR_LOGIC "vbatt 14.5 <"

// Human-readable: ac_on_switch
#define AC_RELAY_LOGIC "ac_on_switch"
// Combined RPM, CLT and VBATT warning light

// Human-readable: (rpm > fsio_setting(2)) | ((coolant > fsio_setting(3)) | (vbatt < fsio_setting(4)))
#define COMBINED_WARNING_LIGHT "rpm 2 fsio_setting > coolant 3 fsio_setting > vbatt 4 fsio_setting < | |"
//needed by EFI_MAIN_RELAY_CONTROL

// Human-readable: (time_since_boot < 2) | (vbatt > 5) | in_shutdown
#define MAIN_RELAY_LOGIC "time_since_boot 2 < vbatt 5 > | in_shutdown |"
// could be used for simple variable intake geometry setups or warning light or starter block

// Human-readable: rpm > fsio_setting(1)
#define RPM_ABOVE_USER_SETTING_1 "rpm 1 fsio_setting >"
// could be used for simple variable intake geometry setups or warning light or starter block

// Human-readable: rpm > fsio_setting(2)
#define RPM_ABOVE_USER_SETTING_2 "rpm 2 fsio_setting >"

// Human-readable: rpm < fsio_setting(1)
#define RPM_BELOW_USER_SETTING_1 "rpm 1 fsio_setting <"
// starter block using configurable parameter

// Human-readable: rpm < cranking_rpm
#define STARTER_BLOCK "rpm cranking_rpm <"
