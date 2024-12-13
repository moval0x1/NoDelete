@echo off
rem Resolve the current directory where the script is located
set SCRIPT_DIR=%~dp0
set BUILD_DIR=%SCRIPT_DIR%build\Desktop_Qt_6_8_0_MSVC2022_64bit-Release
set QT_BIN_DIR=C:\Qt\6.8.0\msvc2022_64\bin
set TARGET_DIR=C:\qtRelease
set EXECUTABLE_NAME=NoDelete.exe
set CONFIG_NAME=config.ini
set TARGET_EXECUTABLE=%TARGET_DIR%\%EXECUTABLE_NAME%

rem Ensure TARGET_DIR exists
if not exist "%TARGET_DIR%" mkdir "%TARGET_DIR%"

rem Delete all files from TARGET_DIR
echo Cleaning target directory...
del /q "%TARGET_DIR%\*"

rem Copy executable from BUILD_DIR to TARGET_DIR
echo Copying executable...
copy "%BUILD_DIR%\%EXECUTABLE_NAME%" "%TARGET_DIR%"
copy "%BUILD_DIR%\%CONFIG_NAME%" "%TARGET_DIR%"

rem Run windeployqt
echo Running windeployqt...
"%QT_BIN_DIR%\windeployqt.exe" --no-quick-import --no-translations --no-system-d3d-compiler "%TARGET_EXECUTABLE%"

echo Deployment completed.
pause
