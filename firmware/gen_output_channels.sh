#!/bin/bash


source gen_config_common.sh
echo "Using COMMON_GEN_CONFIG [$COMMON_GEN_CONFIG]"

java -DSystemOut.name=logs/gen_output_channels \
 -jar ../java_tools/ConfigDefinition.jar \
 -cache_zip_file tunerstudio/generated/cache.zip \
 -prepend integration/rusefi_config.txt \
 -definition console/binary/output_channels.txt \
 -ts_outputs_section console/binary/output_channels.ini \
 -cache . \
 -with_c_defines false \
 -c_destination console/binary/ts_outputs_generated.h