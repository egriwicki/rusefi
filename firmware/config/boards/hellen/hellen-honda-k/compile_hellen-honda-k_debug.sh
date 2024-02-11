#!/bin/bash

export EXTRA_PARAMS="\
 -DEFI_HPFP=FALSE \
 -DEFI_ALTERNATOR_CONTROL=FALSE \
 -DEFI_LOGIC_ANALYZER=FALSE \
 -DEFI_TOOTH_LOGGER=FALSE \
 -DEFI_LUA=FALSE \
 -DRAMDISK_INVALID"
export DEBUG_LEVEL_OPT="-O0 -ggdb -g"
export INCLUDE_ELF=yes
cd ..
bash ../common_make.sh config/boards/hellen/hellen-honda-k/meta-info.env
