rem 
rem STM32F767 version of the firmware for Nucleo-F767ZI board
rem

cd ../../..
set PROJECT_BOARD=ME7_PNP
set PROJECT_CPU=ST_STM32F7
set EXTRA_PARAMS=-DDUMMY -DSTM32F767xx ^
 -DEFI_INJECTOR_PIN3=GPIO_UNASSIGNED ^
 -DEFI_COMMUNICATION_PIN=GPIOA_2 ^
 -DEFI_FATAL_ERROR_PIN=GPIOA_5 ^
 -DEFI_ENABLE_ASSERTS=FALSE ^
 -DCH_DBG_THREADS_PROFILING=FALSE
rem set DEBUG_LEVEL_OPT="-O2"
make -j4
rem TODO fix hex2dfu for F7
rem ..\misc\hex2dfu\HEX2DFU.exe build/rusefi.hex -out build/rusefi.dfu