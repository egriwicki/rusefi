# This batch files reads rusefi_config.txt and produses firmware persistent configuration headers
# the storage section of rusefi.ini is updated as well

cd ../../../..

pwd

java \
 -DSystemOut.name=gen_config_kinetis_board \
 -cp ../java_tools/ConfigDefinition.jar \
 com.rusefi.board_generator.BoardReader \
 -board kinetis \
 -firmware_path . \
 -out config/boards/kinetis/config/tunerstudio \
 -enumInputFile controllers/algo/rusefi_enums.h \
 -enumInputFile config/boards/kinetis/rusefi_hw_enums.h

java \
 -DSystemOut.name=gen_config_kinetis \
 -Drusefi.generator.lazyfile.enabled=true \
 -jar ../java_tools/ConfigDefinition.jar \
 -definition integration/rusefi_config.txt \
 -cache tunerstudio/cache/kinetis \
 -ts_destination tunerstudio \
 -tool kineris_gen_config.bat \
 -firing_order controllers/algo/firing_order.h \
 -with_c_defines false \
 -initialize_to_zero false \
 -ts_output_name rusefi_kinetis.ini \
 -c_defines config/boards/kinetis/config/controllers/algo/rusefi_generated.h \
 -c_destination config/boards/kinetis/config/controllers/algo/engine_configuration_generated_structures.h \
 -prepend config/boards/kinetis/config/rusefi_config_kinetis.txt \
 -prepend config/boards/kinetis/config/tunerstudio/kinetis_prefix.txt

