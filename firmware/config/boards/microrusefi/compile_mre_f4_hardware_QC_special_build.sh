#!/bin/bash

export EXTRA_PARAMS="-DDUMMY \
 -DHW_CHECK_MODE=TRUE \
 -DANALOG_HW_CHECK_MODE=TRUE \
 -DEFI_HPFP=FALSE \
 -DEFI_ALTERNATOR_CONTROL=FALSE \
 -DEFI_LOGIC_ANALYZER=FALSE \
 -DEFI_TOOTH_LOGGER=FALSE \
 -DEFI_LUA=FALSE \
 -DEFI_MALFUNCTION_INDICATOR=FALSE \
 -DEFI_AUX_PID=FALSE \
 -DEFI_MAX_31855=FALSE \
 -DEFI_ENGINE_SNIFFER=FALSE \
 -DEFI_STORAGE_INT_FLASH=FALSE \
 -DEFI_LAUNCH_CONTROL=FALSE \
 -DEFI_ANTILAG_SYSTEM=FALSE \
 -DHW_CHECK_ALWAYS_STIMULATE=TRUE \
 -DRAMDISK_INVALID"

export VAR_DEF_ENGINE_TYPE=-DDEFAULT_ENGINE_TYPE=MRE_BOARD_NEW_TEST

# this QC configuration is used to assert our status with debug info
export DEBUG_LEVEL_OPT="-O0 -ggdb -g"

bash ../common_make.sh microrusefi ARCH_STM32F4
