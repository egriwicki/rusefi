#!/usr/bin/env bash

USER=$1
PASS=$2
HOST=$3
BUNDLE_FILE_NAME=$4

if [ -n "${BUNDLE_FILE_NAME}" ]; then
  echo "BUNDLE_FILE_NAME is ${BUNDLE_FILE_NAME}"
else
  echo "BUNDLE_FILE_NAME argument not specified"
  exit 1
fi

SCRIPT_NAME=$(basename "$0")
# technical debt: more than one file uses magic 'rusefi_bundle_' constant, can we extract constant?
FULL_BUNDLE_FILE="rusefi_bundle_${BUNDLE_FILE_NAME}.zip"
UPDATE_BUNDLE_FILE="rusefi_bundle_${BUNDLE_FILE_NAME}_autoupdate.zip"

if [ -n "${USER}" -a -n "$PASS" -a -n "${HOST}" ]; then
 echo "$SCRIPT_NAME: Uploading both bundles"
 RET=0
 if [ "$LTS" == "true" -a -n "$REF" ]; then
   DESTINATION_FOLDER="build_server/lts/${REF}"
 else
   DESTINATION_FOLDER="build_server"
 fi
 tar -czf - $FULL_BUNDLE_FILE    | sshpass -p $PASS ssh -o StrictHostKeyChecking=no ${USER}@${HOST} "mkdir -p ${DESTINATION_FOLDER};            tar -xzf - -C ${DESTINATION_FOLDER}"
 RET=$((RET+$?+PIPESTATUS))
 if [ -f $UPDATE_BUNDLE_FILE ]; then
   tar -czf - $UPDATE_BUNDLE_FILE  | sshpass -p $PASS ssh -o StrictHostKeyChecking=no ${USER}@${HOST} "mkdir -p ${DESTINATION_FOLDER}/autoupdate; tar -xzf - -C ${DESTINATION_FOLDER}/autoupdate"
   RET=$((RET+$?+PIPESTATUS))
 else
    echo "File $UPDATE_BUNDLE_FILE does not exist."
 fi
 if [ $RET -ne 0 ]; then
  echo "$SCRIPT_NAME: Bundle upload failed"
  exit 1
 fi
else
 echo "$SCRIPT_NAME: Upload not configured"
fi
