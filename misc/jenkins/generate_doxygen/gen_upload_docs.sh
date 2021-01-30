#!/bin/bash

echo "Should be executed from project root folder. Will try to upload to $RUSEFI_FTP_SERVER"
pwd
# ibom is part of Doxygen job simply in order to reduce workspace HDD usage on my tiny build server
bash misc/jenkins/InteractiveHtmlBom/run.sh

if [ -n "$RUSEFI_FTP_SERVER" ]; then
  echo "Uploading IBOMs"
  ncftpput -R -v -u "$RUSEFI_DOXYGEN_FTP_USER" -p "$RUSEFI_DOXYGEN_FTP_PASS" "$RUSEFI_FTP_SERVER" /ibom hardware/ibom/*
fi

pwd
cd firmware

doxygen || { echo "doxygen run FAILED"; exit 1; }

# http://www.ncftp.com/download/
# actually Cygwin http://rusefi.com/wiki/index.php?title=Internal:Software:Build_Server
cd ../doxygen
if [ -n "$RUSEFI_FTP_SERVER" ]; then
  echo "Uploading Doxygen"
  ncftpput -R -v -u "$RUSEFI_DOXYGEN_FTP_USER" -p "$RUSEFI_DOXYGEN_FTP_PASS" "$RUSEFI_FTP_SERVER" /html html/
fi
[ $? -eq 0 ] || { echo "upload FAILED"; exit 1; }

cd ..
mkdir pinouts
bash misc/pinout-gen/gen.sh firmware/config/boards/proteus/black23.yaml > pinouts/black23.html
cp firmware/config/boards/proteus/black23.jpg pinouts/black23.jpg
bash misc/pinout-gen/gen.sh firmware/config/boards/proteus/black35.yaml > pinouts/black35.html
cp firmware/config/boards/proteus/black35.jpg pinouts/black35.jpg

if [ -n "$RUSEFI_FTP_SERVER" ]; then
  echo "Uploading Pinouts"
  ncftpput -R -v -u "$RUSEFI_DOXYGEN_FTP_USER" -p "$RUSEFI_DOXYGEN_FTP_PASS" "$RUSEFI_FTP_SERVER" /pinouts pinouts/
fi
[ $? -eq 0 ] || { echo "upload FAILED"; exit 1; }
