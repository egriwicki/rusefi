#!/bin/bash

export PROJECT_BOARD=hellen/harley81
export PROJECT_CPU=ARCH_STM32F4
export EXTRA_PARAMS="-DSHORT_BOARD_NAME=harley81"

cd ..
bash ../common_make.sh
