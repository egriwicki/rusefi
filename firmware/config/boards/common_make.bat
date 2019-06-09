set script_name=common_make.bat
echo Entering %script_name%

rem todo: start using env variable for number of threads or for '-r'
maje -j4 -r
if not exist build/rusefi.hex echo FAILED to compile by %script_name%
if not exist build/rusefi.hex exit -1

../misc/encedo_hex2dfu/hex2dfu.exe -i build/rusefi.hex -o build/rusefi.dfu