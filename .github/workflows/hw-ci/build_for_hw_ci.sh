#!/usr/bin/env bash

HW_FOLDER=$1
HW_TARGET=$2

# for instance
# .github/workflows/hw-ci/build_for_hw_ci.sh config/boards/f407-discovery f407-discovery
# .github/workflows/hw-ci/build_for_hw_ci.sh config/boards/proteus        proteus_f4
# .github/workflows/hw-ci/build_for_hw_ci.sh config/boards/proteus        proteus_f7_debug

set -e
echo "HW CI build [$HW_FOLDER][$HW_TARGET]"

cd firmware

export BOARD_META_PATH=$(bash bin/find_meta_info.sh ${HW_FOLDER} ${HW_TARGET})

./gen_live_documentation.sh
./gen_config_board.sh $HW_FOLDER $HW_TARGET

echo "We aren't guaranteed a clean machine every time, so manually clean the output."
make clean
cd ..

export EXTRA_2_PARAMS=-DHARDWARE_CI

echo Build Firmware
misc/jenkins/compile_other_versions/compile.sh
