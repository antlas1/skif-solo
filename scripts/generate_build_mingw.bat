set path=%path%;F:\Programs\cmake-3.25.0-windows-x86_64\bin;C:\Qt\Tools\winlibs_mingw32_gcc12\bin\
cd ..
if not exist build-mignw-dyn mkdir build-mignw-dyn
cd build-mignw-dyn
cmake -G "MinGW Makefiles" -DUSE_STUBGAME=ON -DSOUNDMODE=disabled -DBUILD_TELNET=OFF ../src -DVCPKG_TARGET_TRIPLET=x86-mingw-dynamic -DCMAKE_TOOLCHAIN_FILE=C:\workspace\tools\vcpkg_mingw\scripts\buildsystems\vcpkg.cmake ../src
cmake --build .
pause