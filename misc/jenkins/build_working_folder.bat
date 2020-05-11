set script_name=build_working_folder.bat
echo Entering %script_name%

pwd
echo %script_name%: Working folder: %folder%
mkdir %folder%
set console_folder=%folder%\console
set drivers_folder=%folder%\drivers
mkdir %console_folder%
mkdir %drivers_folder%
ls -l %folder%

wget https://rusefi.com/build_server/st_files/silent_st_drivers.exe -P %drivers_folder%
if not exist %drivers_folder%/silent_st_drivers.exe (
 echo %script_name%: ERROR DOWNLOADING silent_st_drivers.exe
 EXIT /B 1
)

if %ini_file_override%x==nox (
    set ini_file_override=rusefi.ini
    echo %script_name%: N/a ini_file_override specified
)
if %ini_file_override%.==. (
    set ini_file_override=rusefi.ini
    echo %script_name%: No ini_file_override specified
)
echo %script_name%: Will use %ini_file_override%

if %rusefi_console_settings%.==. (
  echo %script_name%: No rusefi_console_settings
) else (
  echo Using rusefi_console_settings [%rusefi_console_settings%]
  cp %rusefi_console_settings% %console_folder%
)

cp java_console_binary/rusefi_console.jar %console_folder%
cp simulator/build/rusefi_simulator.exe %console_folder%
cp misc/console_launcher/rusefi_console.exe %console_folder%
cp java_console/rusefi.xml %console_folder%

cp misc/console_launcher/readme.html %folder%

cp firmware/tunerstudio/%ini_file_override% %folder%
rem Unsetting since would not be used anywhere else
set ini_file_override=
set rusefi_console_settings=

rem users probably do not really care for this file
rem cp firmware/svnversion.h %folder%

cp -r misc/install/openocd %console_folder%
cp -r misc/install/DfuSe %console_folder%
rem 407 has additional version of firmware
cp firmware/deliver/rusefi_no_asserts.bin %folder%
cp firmware/deliver/rusefi_no_asserts.dfu %folder%
rem 746 builds one version at the moment
rem probably not needed cp firmware/build/rusefi.hex %folder%
cp firmware/deliver/rusefi.bin %folder%
rem probably not needed cp firmware/build/rusefi.elf %folder%
cp firmware/deliver/rusefi.dfu %folder%

if not exist firmware/deliver/rusefi.bin echo %script_name%: rusefi.bin not found
if not exist firmware/deliver/rusefi.bin exit -1

cd temp


echo "Building bundle"
pwd
set zip_name=rusefi_bundle.zip
zip -r %zip_name% *
IF NOT ERRORLEVEL 0 echo %script_name%: ERROR INVOKING zip
IF NOT ERRORLEVEL 0 EXIT /B 1

echo %script_name%: Bundle %zip_name% ready
ls -l %zip_name%
cd ..
echo "%script_name%: We are back in root directory"

pwd
echo Exiting %script_name%