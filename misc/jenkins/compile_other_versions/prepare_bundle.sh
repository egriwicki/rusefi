#!/bin/bash

BUNDLE_NAME="$1"
export INI_FILE_OVERRIDE="$2"
SCRIPT_NAME="prepare_bundle.sh"
echo "Entering $SCRIPT_NAME with ${BUNDLE_NAME}"
echo "RUSEFI_BUILD_FTP_USER=$RUSEFI_BUILD_FTP_USER"

TIMESTAMP=$(date "+%Y%m%d_%H%M%S")

export FOLDER="temp/snapshot_${TIMESTAMP}_${BUNDLE_NAME}_rusefi"

export BUNDLE_FULL_NAME="rusefi_bundle_${BUNDLE_NAME}"

bash misc/jenkins/build_working_folder.sh

pwd
echo "exiting $SCRIPT_NAME"
