#!/bin/bash

echo "Compiling for Cypress FM4 S6E2CxAH"
# TODO: somehow this -DDUMMY is helping us to not mess up the parameters, why?!
# https://github.com/rusefi/rusefi/issues/684
export EXTRA_PARAMS="-DDUMMY -D__USE_CMSIS\
 -DEFI_ENABLE_ASSERTS=FALSE -DCH_DBG_ENABLE_CHECKS=FALSE -DCH_DBG_ENABLE_ASSERTS=FALSE -DCH_DBG_ENABLE_STACK_CHECK=FALSE -DCH_DBG_FILL_THREADS=FALSE -DCH_DBG_THREADS_PROFILING=FALSE\
 -DDEFAULT_ENGINE_TYPE=MINIMAL_PINS"

export BUILDDIR="build"
export PROJECT_BOARD="hellen/cypress"
export PROJECT_CPU="cypress"
export USE_FATFS="no"
export USE_BOOTLOADER="no"
export DEBUG_LEVEL_OPT="-O2"

cd ../../..

bash config/boards/common_make.sh
