#!/bin/sh

echo "Compiling for Kinetis MKE16F512"
# TODO: somehow this -DDUMMY is helping us to not mess up the parameters, why?!
# https://github.com/rusefi/rusefi/issues/684
export "EXTRA_PARAMS=-DDUMMY -DCPU_MKE16F512VLH16 -DCPU_MKE16F512VLH16_cm4 -D__USE_CMSIS \
 -DEFI_ENABLE_ASSERTS=FALSE -DCH_DBG_ENABLE_CHECKS=FALSE -DCH_DBG_ENABLE_ASSERTS=FALSE -DCH_DBG_ENABLE_STACK_CHECK=FALSE -DCH_DBG_FILL_THREADS=FALSE -DCH_DBG_THREADS_PROFILING=FALSE \
 -DDEFAULT_ENGINE_TYPE=MINIMAL_PINS"
# set BUILDDIR=build_kinetis
export BUILDDIR=build
export PROJECT_BOARD=kinetis
export USE_SMART_BUILD=yes
export PROJECT_CPU=kinetis
export DEBUG_LEVEL_OPT="-O2"
export USE_FATFS=no
export USE_BOOTLOADER=no
# -DCH_DBG_ENABLE_CHECKS=FALSE

cd ../../..

# rm -f ${BUILDDIR}/rusefi_kinetis.*

# make -r -j4

sh config/boards/common_make.sh
