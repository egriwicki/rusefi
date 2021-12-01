COMMON_GEN_CONFIG="-jar ../java_tools/ConfigDefinition.jar \
 -enumInputFile controllers/algo/engine_types.h \
 -enumInputFile controllers/algo/rusefi_enums.h \
 -cache_zip_file tunerstudio/generated/cache.zip \
 -readfile OUTPUTS_SECTION console/binary/generated/output_channels.ini \
 -readfile DATALOG_SECTION console/binary/generated/data_logs.ini \
 -ts_destination tunerstudio \
 -firing_order controllers/algo/firing_order.h \
 -definition integration/rusefi_config.txt"
