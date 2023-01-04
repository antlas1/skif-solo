@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
set path=%path%;c:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\
REM Генерация скиф
cd ..
if not exist build mkdir build
cd build
cmake -G "Visual Studio 17 2022" ^
-DBUILD_PLATFORM_TELNET=OFF ^
-DBUILD_PLATFORM_HTTP=OFF ^
-DBUILD_PLATFORM_GUI=OFF ^
-DBUILD_SOUND_SFML=OFF ^
 ../src
PAUSE