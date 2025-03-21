@echo off
setlocal

echo -------------------------------------
echo Building with CMake

CALL generate_files.bat
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=off
cmake --build .

echo -------------------------------------