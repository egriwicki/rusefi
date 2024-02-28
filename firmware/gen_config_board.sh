#!/bin/bash

# file gen_config_board.sh
#        for example ./gen_config_board.sh config/boards/hellen/hellen128 hellen128
#                    ./gen_config_board.sh config/boards/hellen/hellen-honda-k hellen-honda-k
#                 or ./gen_config_board.sh config/boards/atlas atlas
#                    ./gen_config_board.sh config/boards/proteus proteus_f7
#                    ./gen_config_board.sh config/boards/hellen/uaefi uaefi
#                    ./gen_config_board.sh config/boards/hellen/small-can-board small-can-board
#                    ./gen_config_board.sh config/boards/f407-discovery f407-discovery
#                    ./gen_config_board.sh config/boards/nucleo_f767 nucleo_f767

set -e

echo "This script reads rusefi_config.txt and produces firmware persistent configuration headers"
echo "the storage section of rusefiXXX.ini is updated as well"

BOARD_DIR=${1:-$BOARD_DIR}
SHORT_BOARD_NAME=${2:-$SHORT_BOARD_NAME}
INI=${3:-"rusefi_$SHORT_BOARD_NAME.ini"}

if [ -z "$BOARD_DIR" ]; then
	echo "Board dir parameter expected"
	exit 1
fi

if [ -z "$SHORT_BOARD_NAME" ]; then
	echo "Short board name parameter expected"
	exit 1
fi

echo "BOARD_DIR=${BOARD_DIR} SHORT_BOARD_NAME=${SHORT_BOARD_NAME}"

which realpath >/dev/null 2>&1 || (which grealpath >/dev/null 2>&1 && alias realpath='grealpath')
FDIR=$(realpath $(dirname "$0"))
BOARD_DIR=$(realpath --relative-to "$FDIR" "$BOARD_DIR")

cd "$FDIR"

source gen_config_common.sh
echo "Using COMMON_GEN_CONFIG [$COMMON_GEN_CONFIG]"

# in rare cases order of arguments is important - '-tool' should be specified before '-definition'
java \
 $COMMON_GEN_CONFIG_PREFIX \
 	-tool gen_config.sh \
 $COMMON_GEN_CONFIG \
	-enumInputFile controllers/algo/rusefi_hw_stm32_enums.h \
	-enumInputFile controllers/algo/rusefi_hw_adc_enums.h \
  -c_defines        controllers/generated/rusefi_generated_${SHORT_BOARD_NAME}.h \
  -c_destination    controllers/generated/engine_configuration_generated_structures_${SHORT_BOARD_NAME}.h

[ $? -eq 0 ] || { echo "ERROR generating TunerStudio config for ${BOARD_DIR}"; exit 1; }

if [ -z "META_OUTPUT_ROOT_FOLDER" ]; then
	META_OUTPUT_ROOT_FOLDER=""
fi

echo "Happy ${SHORT_BOARD_NAME}!"
exit 0
