#!/bin/bash

export PROJECT_BOARD=hellen/hellen-nb1
export PROJECT_CPU=ARCH_STM32F4
export EXTRA_PARAMS="-DSHORT_BOARD_NAME=hellenNA8_96"

cd ..
bash ../common_make.sh
