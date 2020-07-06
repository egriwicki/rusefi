#!/bin/sh

# file gen_config_board.sh

#set -x

echo "This script reads rusefi_config.txt and produces firmware persistent configuration headers"
echo "the storage section of rusefiXXX.ini is updated as well"

if [ -z "$1" ]; then
	echo "Board name parameter expected"
	exit 1
fi

BOARDNAME=$1
SHORT_BOARDNAME=$2

echo "BOARDNAME=${BOARDNAME} SHORT_BOARDNAME=${SHORT_BOARDNAME}"

sh gen_signature.sh ${SHORT_BOARDNAME}

java -DSystemOut.name=gen_config_board \
	-Drusefi.generator.lazyfile.enabled=true \
	-cp ../java_tools/ConfigDefinition.jar \
	com.rusefi.board_generator.BoardReader \
	-yaml config/boards/${BOARDNAME}/mapping.yaml \
	-firmware_path . \
  -output_file tunerstudio/${BOARDNAME}_prefix.txt \
	-enumInputFile controllers/algo/rusefi_enums.h \
	-enumInputFile controllers/algo/rusefi_hw_enums.h

[ $? -eq 0 ] || { echo "ERROR generating TunerStudio config for ${BOARDNAME}"; exit 1; }

java -DSystemOut.name=gen_config_board \
	-jar ../java_tools/ConfigDefinition.jar \
	-definition integration/rusefi_config.txt \
	-tool gen_config.sh \
	-ts_destination tunerstudio \
	-cache ${BOARDNAME} \
	-cache_zip_file tunerstudio/cache.zip \
	-firing_order controllers/algo/firing_order.h \
	-ts_output_name rusefi_${BOARDNAME}.ini \
	-signature tunerstudio/signature_${SHORT_BOARDNAME}.txt \
	-signature_destination controllers/generated/signature_${SHORT_BOARDNAME}.h \
	-prepend tunerstudio/${BOARDNAME}_prefix.txt \
	-prepend config/boards/${BOARDNAME}/prepend.txt

[ $? -eq 0 ] || { echo "ERROR generating TunerStudio config for ${BOARDNAME}"; exit 1; }

if [ -z "${TS_PATH}" ]; then
	echo "TS_PATH not defined"
else
	if [ -d "${TS_PATH}/dev_${BOARDNAME}/" ]; then
		echo "This would automatically copy latest file to 'dev_${BOARDNAME}' TS project $TS_PATH"
		cp -v tunerstudio/rusefi_${BOARDNAME}.ini ${TS_PATH}/dev_${BOARDNAME}/projectCfg/mainController.ini
	fi
fi

exit 0
