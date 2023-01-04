@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"

REM Генерация скиф
cd ..
if not exist build mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A "Win32" -DCMAKE_TOOLCHAIN_FILE=C:\workspace\tools\vcpkg_git\scripts\buildsystems\vcpkg.cmake ../src