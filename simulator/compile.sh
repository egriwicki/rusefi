rm -f build/rusefi_simulator.exe
make -j4 -r
file build/rusefi_simulator
echo "TIMESTAMP $(date "+%D %T.%2N") just compiled rusefi simulator"
