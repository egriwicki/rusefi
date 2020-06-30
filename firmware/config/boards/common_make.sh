SCRIPT_NAME="common_make.sh"
echo "Entering $SCRIPT_NAME"

mkdir .dep
# todo: start using env variable for number of threads or for '-r'
make -j4 -r
[ -e build/rusefi.hex ] || { echo "FAILED to compile by $SCRIPT_NAME with $PROJECT_BOARD $DEBUG_LEVEL_OPT and $EXTRA_PARAMS"; exit 1; }

mkdir deliver

rm -f deliver/rusefi.dfu
echo "$SCRIPT_NAME: invoking hex2dfu"
if uname | grep "NT"; then
 chmod u+x ../misc/encedo_hex2dfu/hex2dfu.exe
 ../misc/encedo_hex2dfu/hex2dfu.exe -i build/rusefi.hex -o deliver/rusefi.dfu
else
 chmod u+x ../misc/encedo_hex2dfu/hex2dfu.bin
 ../misc/encedo_hex2dfu/hex2dfu.bin -i build/rusefi.hex -o deliver/rusefi.dfu
fi
cp build/rusefi.bin deliver/

echo "$SCRIPT_NAME: deliver folder"
ls -l deliver

sh config/boards/clean_env_variables.sh
