#!/bin/bash

export PROJECT_BOARD=proteus
export PROJECT_CPU=ARCH_STM32F7
export EXTRA_PARAMS="-DVR_HW_CHECK_MODE=TRUE -DHW_CHECK_MODE=TRUE -DHW_CHECK_SD=TRUE -DHW_CHECK_ALWAYS_STIMULATE=TRUE -DSHORT_BOARD_NAME=proteus_f7"

export DEFAULT_ENGINE_TYPE=-DDEFAULT_ENGINE_TYPE=PROTEUS_QC_TEST_BOARD

# export DEBUG_LEVEL_OPT="-O0 -ggdb -g"

bash ../common_make.sh
