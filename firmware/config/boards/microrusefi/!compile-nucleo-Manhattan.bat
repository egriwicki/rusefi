
cd ../../..

set PROJECT_BOARD=microrusefi
set PROJECT_CPU=ARCH_STM32F7

set VAR_DEF_ENGINE_TYPE = -DDEFAULT_ENGINE_TYPE=engine_type_e::BMW_M73_F
set LED_CRITICAL_ERROR_BRAIN_PIN = -DLED_CRITICAL_ERROR_BRAIN_PIN=Gpio::B14

set EXTRA_PARAMS=-DDUMMY ^
 -DEFI_COMMUNICATION_PIN=Gpio::B7

call config/boards/common_make.bat

