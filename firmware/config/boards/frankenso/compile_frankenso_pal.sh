#!/bin/sh

cd ../../..

export EXTRA_PARAMS="-DDUMMY \
 -DDEFAULT_ENGINE_TYPE=MIATA_NA6_VAF \
 -DSHORT_BOARD_NAME=frankenso_na6 \
 -DHAL_TRIGGER_USE_PAL=TRUE \
 -DEFI_VEHICLE_SPEED=FALSE \
 -DHAL_USE_ICU=FALSE \
 -DEFI_LOGIC_ANALYZER=FALSE \
 -DFIRMWARE_ID=\\\"frankensoNA6\\\""

sh config/boards/common_make.sh

