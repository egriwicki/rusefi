@echo off
sh.exe misc\jenkins\build_java_console.sh || (
  if exist C:\cygwin64 (
    C:\cygwin64\bin\sh.exe misc\jenkins\build_java_console.sh
  ) else (
    if exist C:\cygwin ( C:\cygwin\bin\sh.exe misc\jenkins\build_java_console.sh )
  )
)
