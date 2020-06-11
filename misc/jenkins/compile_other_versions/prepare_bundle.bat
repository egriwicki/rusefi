set script_name=build_version.bat
echo Entering %script_name% with %bundle_name%
echo "RUSEFI_BUILD_FTP_USER=%RUSEFI_BUILD_FTP_USER%"

if %RUSEFI_BUILD_FTP_USER%.==. (
    echo RUSEFI_BUILD_FTP_USER not set - not packaging
    exit 0
)

rem This depends on Cygwin date copied under 'datecyg' name to avoid conflict with Windows date
rem By the way, '%%' is the way to escape % in batch files
rem this is copy-pasted at build_current_bundle.bat
for /f %%i in ('datecyg +%%Y%%m%%d_%%H%%M%%S') do set TIMESTAMP=%%i

set folder=snapshot_%TIMESTAMP%_%bundle_name%_rusefi
set folder=temp\%folder%

set bundle_full_name=rusefi_bundle_%bundle_name%

call misc\jenkins\build_working_folder.bat

pwd
echo "exiting %script_name%"
