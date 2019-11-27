rem 
rem STM32F405 version of the firmware for https://rusefi.com/forum/viewtopic.php?f=4&t=1215
rem

cd ../../..
set PROJECT_BOARD=prometheus
set PROMETHEUS_BOARD=405
set EXTRA_PARAMS=-DDUMMY -DSTM32F405xx -DEFI_ENABLE_ASSERTS=FALSE ^
 -DFIRMWARE_ID=\"prometheus405\" ^
 -DCH_DBG_ENABLE_ASSERTS=FALSE -DCH_DBG_ENABLE_STACK_CHECK=FALSE -DCH_DBG_FILL_THREADS=FALSE -DCH_DBG_THREADS_PROFILING=FALSE
set DEBUG_LEVEL_OPT="-O2"
set USE_BOOTLOADER=yes

call config/boards/common_make.bat

call config/boards/clean_env_variables.bat