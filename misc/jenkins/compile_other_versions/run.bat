set script_name=run.bat
echo Entering %script_name%

pwd

call misc\jenkins\build_java_console.bat
if not exist java_console_binary/rusefi_console.jar exit -1

call misc\jenkins\build_simulator.bat
if not exist simulator/build/rusefi_simulator.exe exit -1

call misc\jenkins\compile_other_versions\compile_and_upload.bat frankenso frankenso_na6
IF NOT ERRORLEVEL 0 echo ERROR invoking compile_and_upload.bat
IF NOT ERRORLEVEL 0 EXIT /B 1
pwd

call misc\jenkins\compile_other_versions\compile_and_upload.bat microrusefi mre rusefi_microrusefi.ini
IF NOT ERRORLEVEL 0 echo ERROR invoking compile_and_upload.bat
IF NOT ERRORLEVEL 0 EXIT /B 1
pwd

call misc\jenkins\compile_other_versions\compile_and_upload.bat nucleo_f746 stm32f746_nucleo
IF NOT ERRORLEVEL 0 echo ERROR invoking compile_and_upload.bat
IF NOT ERRORLEVEL 0 EXIT /B 1
pwd


call misc\jenkins\compile_other_versions\compile_and_upload.bat nucleo_f767 stm32f767_nucleo
IF NOT ERRORLEVEL 0 echo ERROR invoking compile_and_upload.bat
IF NOT ERRORLEVEL 0 EXIT /B 1
pwd


call misc\jenkins\compile_other_versions\compile_and_upload.bat nucleo_f767 stm32f767_osc
IF NOT ERRORLEVEL 0 echo ERROR invoking compile_and_upload.bat
IF NOT ERRORLEVEL 0 EXIT /B 1
pwd


call misc\jenkins\compile_other_versions\compile_and_upload.bat prometheus prometheus_405
IF NOT ERRORLEVEL 0 echo ERROR invoking compile_and_upload.bat
IF NOT ERRORLEVEL 0 EXIT /B 1
pwd


call misc\jenkins\compile_other_versions\compile_and_upload.bat prometheus prometheus_469
IF NOT ERRORLEVEL 0 echo ERROR invoking compile_and_upload.bat
IF NOT ERRORLEVEL 0 EXIT /B 1
pwd

