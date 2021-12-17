#!/bin/bash

# file gen_config_board.sh
#        for example ./gen_config_board.sh hellen/hellen128 hellen128 rusefi_hellen128mercedes.ini
#                 or ./gen_config_board.sh hellen/hellen128 hellen128
# which is short for ./gen_config_board.sh hellen/hellen128 hellen128 rusefi_hellen128.ini

set -e

echo "This script reads rusefi_config.txt and produces firmware persistent configuration headers"
echo "the storage section of rusefiXXX.ini is updated as well"

if [ -z "$1" ]; then
	echo "Board name parameter expected"
	exit 1
fi

BOARDNAME=$1
SHORT_BOARDNAME=$2
if [ -n "$3" ]; then
  INI="$3"
else
  INI="rusefi_${SHORT_BOARDNAME}.ini"
fi

echo "BOARDNAME=${BOARDNAME} SHORT_BOARDNAME=${SHORT_BOARDNAME}"
if [ "${SHORT_BOARDNAME}" = "all" ]; then
  SOMETHINGSOMETHING=false
else
  SOMETHINGSOMETHING=true
fi

bash gen_signature.sh ${SHORT_BOARDNAME}

PREPEND_FILE=config/boards/${BOARDNAME}/prepend.txt

BOARD_SPECIFIC_URL=$(cat $PREPEND_FILE | grep MAIN_HELP_URL | cut -d " " -f 3 | sed -e 's/^"//' -e 's/"$//')

echo "BOARD_SPECIFIC_URL=[$BOARD_SPECIFIC_URL] for [$BOARDNAME] as [$SHORT_BOARDNAME]"
if [ "" = "$BOARD_SPECIFIC_URL" ]; then
  BOARD_SPECIFIC_URL=https://rusefi.com/s/wiki
fi
echo "BOARD_SPECIFIC_URL=[$BOARD_SPECIFIC_URL]"

source gen_config_common.sh
echo "Using COMMON_GEN_CONFIG [$COMMON_GEN_CONFIG]"

# work in progress: migrating to rusefi_${BUNDLE_NAME}.txt
java -DSystemOut.name=logs/gen_config_board \
 $COMMON_GEN_CONFIG \
  -romraider integration \
	-tool gen_config.sh \
 -field_lookup_file controllers/lua/generated/value_lookup_generated.cpp \
	-board ${BOARDNAME} \
	-ts_output_name generated/${INI} \
	-cache ${SHORT_BOARDNAME} \
  -with_c_defines $SOMETHINGSOMETHING \
  -initialize_to_zero $SOMETHINGSOMETHING \
	-signature tunerstudio/generated/signature_${SHORT_BOARDNAME}.txt \
	-signature_destination controllers/generated/signature_${SHORT_BOARDNAME}.h \
  -java_destination ../java_console/models/src/main/java/com/rusefi/config/generated/Fields.java \
	-enumInputFile controllers/algo/rusefi_hw_enums.h \
  -romraider_destination ../java_console/rusefi.xml \
  -c_defines        controllers/generated/rusefi_generated.h \
  -c_destination    controllers/generated/engine_configuration_generated_structures.h \
	-prepend $PREPEND_FILE

[ $? -eq 0 ] || { echo "ERROR generating TunerStudio config for ${BOARDNAME}"; exit 1; }

# todo: make things consistent by
# 0) having generated content not in the same folder with the tool generating content?
# 1) using unique file name for each configuration?
# 2) leverage consistent caching mechanism so that image is generated only in case of fresh .ini. Laziest approach would be to return exit code from java process above
#
hw_layer/mass_storage/create_ini_image.sh            ./tunerstudio/generated/${INI} ./hw_layer/mass_storage/ramdisk_image.h             112 ${SHORT_BOARDNAME} ${BOARD_SPECIFIC_URL}
hw_layer/mass_storage/create_ini_image_compressed.sh ./tunerstudio/generated/${INI} ./hw_layer/mass_storage/ramdisk_image_compressed.h 1024 ${SHORT_BOARDNAME} ${BOARD_SPECIFIC_URL}

exit 0
