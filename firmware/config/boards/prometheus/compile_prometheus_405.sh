#!/bin/bash

# STM32F405 version of the firmware for https://rusefi.com/forum/viewtopic.php?f=4&t=1215

cd ../../..
export PROJECT_BOARD=prometheus/f405
export EXTRA_PARAMS=-DSHORT_BOARD_NAME=prometheus_405


bash config/boards/common_make.sh

