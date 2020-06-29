rem
rem This script would compile firmware, dev console and win32 simulator into a single bundle file
rem This script depends on Cygwin tools: zip
rem

set script_name=build_current_bundle
set "root_folder=%cd%"
echo %script_name Entering root_folder=%root_folder%
FOR %%i IN ("%root_folder%") DO (set root_folder=%%~si)
echo %script_name Short name %root_folder%"

echo build_current_bundle.bat: Hello rusEfi build full bundle
echo %date% %time%

cd firmware/bootloader
call !clean_bootloader.bat
call !compile_bootloader_discovery407.bat
if not exist bootloader_generated.hxx echo FAILED TO COMPILE BOOTLOADER
if not exist bootloader_generated.hxx exit -1
pwd
cd ../..
rem At root folder here

cd firmware
echo %date% %time%


echo build_current_bundle.bat: will be Erasing chip
if not exist flash_erase407.bat echo NOT FOUND flash_erase.bat
if not exist flash_erase407.bat exit -1
echo build_current_bundle.bat: Erasing chip
pwd
rem Using magic 'cd' system variable to save current location here
set "cur_folder=%cd%"
call flash_erase407.bat
cd %cur_folder%
pwd


echo build_current_bundle.bat: Building firmware
call clean.bat
git submodule update --init

call update_version.bat

call clean_compile_two_versions.bat
if not exist deliver/rusefi_no_asserts.hex echo Just to confirm - FAILED to compile no_asserts
if not exist deliver/rusefi_no_asserts.hex exit -1

if not exist deliver/rusefi.hex echo Just to confirm - FAILED to compile default DEBUG
if not exist deliver/rusefi.hex exit -1


echo "%script_name%: Building DFU"
..\misc\encedo_hex2dfu\hex2dfu.exe -i deliver/rusefi_no_asserts.hex -o deliver/rusefi_no_asserts.dfu
..\misc\encedo_hex2dfu\hex2dfu.exe -i deliver/rusefi.hex            -o deliver/rusefi.dfu
ls -l deliver
cd ..

rem At root folder here

call misc\jenkins\build_java_console.bat
if not exist java_console_binary/rusefi_console.jar exit -1

call misc\jenkins\build_simulator.bat
if not exist simulator/build/rusefi_simulator.exe exit -1

set stm_arch=stm32f407
rem This depends on Cygwin date copied under 'datecyg' name to avoid conflict with Windows date
rem By the way, '%%' is the way to escape % in batch files
rem this is copy-pasted at build_version.bat
for /f %%i in ('datecyg +%%Y%%m%%d_%%H%%M%%S') do set TIMESTAMP=%%i

set folder=snapshot_%TIMESTAMP%_%stm_arch%_rusefi
echo "%script_name%: folder variable1=%folder%"
set folder=temp\%folder%
echo "%script_name%: folder variable3=%folder%"

pwd
set bundle_full_name=rusefi_bundle
call misc\jenkins\build_working_folder.bat
IF NOT ERRORLEVEL 0 (
 echo %script_name%: ERROR: invoking build_working_folder.bat
 EXIT /B 1
)

echo "%script_name%: Building only console"
pwd
dir
zip %root_folder%/temp/rusefi_console.zip %root_folder%/java_console_binary/rusefi_console.jar %root_folder%/java_console/rusefi.xml

if not exist %root_folder%/temp/rusefi_console.zip echo CONSOLE ZIP FAILED
if not exist %root_folder%/temp/rusefi_console.zip exit -1

echo "%script_name%: only console ready"

echo "%script_name%: Going back to root folder"
cd %root_folder%
pwd

echo "TIMESTAMP %date% %time%"
pwd
echo "exiting %script_name%"
