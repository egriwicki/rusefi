#!/usr/bin/env bash

export EXTRA_PARAMS="\
 -DEFI_HPFP=FALSE \
 -DEFI_ALTERNATOR_CONTROL=FALSE \
 -DEFI_LOGIC_ANALYZER=FALSE \
 -DEFI_TOOTH_LOGGER=FALSE \
 -DEFI_LUA=FALSE \
 -DRAMDISK_INVALID"
export DEBUG_LEVEL_OPT="-O0 -ggdb -g"
export INCLUDE_ELF=yes
export PROJECT_BOARD="hellen-honda-k"
export PROJECT_CPU="ARCH_STM32F4"
export SHORT_BOARD_NAME=hellen-honda-k
