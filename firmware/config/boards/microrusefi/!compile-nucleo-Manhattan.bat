
cd ../../..

set LDSCRIPT = config/boards/NUCLEO_F767/STM32F76xxI.ld

set PROJECT_BOARD=microrusefi
set PROJECT_CPU=ARCH_STM32F7

set DEFAULT_ENGINE_TYPE = -DDEFAULT_ENGINE_TYPE=BMW_M73_F
set EFI_FATAL_ERROR_PIN = -DEFI_FATAL_ERROR_PIN=GPIOB_14

set EXTRA_PARAMS=-DDUMMY ^
 -DEFI_COMMUNICATION_PIN=GPIOB_7

rem -DDEFAULT_ENGINE_TYPE=MRE_BOARD_TEST

call config/boards/common_make.bat

