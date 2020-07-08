#!/bin/sh

#set -x
#TS_PATH="/home/<user>/TunerStudioProjects/"

echo "This batch files reads rusefi_config.txt and produses firmware persistent configuration headers"
echo "the storage section of rusefi.ini is updated as well"

rm gen_config.log
rm gen_config_board.log

mkdir build

sh gen_signature.sh all

java -DSystemOut.name=gen_config \
 -Drusefi.generator.lazyfile.enabled=true \
 -jar ../java_tools/ConfigDefinition.jar \
 -definition integration/rusefi_config.txt \
 -romraider integration \
 -ts_destination tunerstudio \
 -ts_output_name generated/rusefi.ini \
 -cache . \
 -cache_zip_file tunerstudio/generated/cache.zip \
 -with_c_defines false \
 -initialize_to_zero false \
 -tool gen_config.sh \
 -c_defines        controllers/generated/rusefi_generated.h \
 -firing_order     controllers/algo/firing_order.h \
 -c_destination    controllers/generated/engine_configuration_generated_structures.h \
 -c_fsio_constants controllers/generated/fsio_enums_generated.def \
 -c_fsio_getters   controllers/generated/fsio_getters.def \
 -c_fsio_names     controllers/generated/fsio_names.def \
 -c_fsio_strings   controllers/generated/fsio_strings.def \
 -java_destination ../java_console/models/src/main/java/com/rusefi/config/generated/Fields.java \
 -signature tunerstudio/generated/signature_all.txt \
 -signature_destination controllers/generated/signature_all.h \
 -romraider_destination ../java_console/rusefi.xml

[ $? -eq 0 ] || { echo "ERROR generating default"; exit 1; }

if [ -z "${TS_PATH}" ]; then
 echo "TS_PATH not defined"
 # it's nice to have default location
 TS_PATH="${HOMEDRIVE}${HOMEPATH}\Documents\TunerStudioProjects"
fi

echo "This would automatically copy latest file to 'dev' TS projects to ${TS_PATH}"
cp -v tunerstudio/generated/rusefi.ini $TS_PATH/dev/projectCfg/mainController.ini
cp -v tunerstudio/generated/rusefi_mre_f4.ini $TS_PATH/mre_f4/projectCfg/mainController.ini

for BOARD in "microrusefi mre_f7" "microrusefi mre_f4" "frankenso frankenso_na6" "prometheus prometheus_469" "prometheus prometheus_405" "proteus proteus_f7" "proteus proteus_f4"; do
 BOARD_NAME="${BOARD% *}"
 BOARD_SHORT_NAME="${BOARD#* }"
 sh gen_config_board.sh $BOARD_NAME $BOARD_SHORT_NAME
 [ $? -eq 0 ] || { echo "ERROR generating board $BOARD_NAME $BOARD_SHORT_NAME"; exit 1; }
done

cd config/boards/kinetis/config
sh gen_config.sh

exit 0
