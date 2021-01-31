@echo off
pip install conan

rmdir /s /q build
mkdir build
cd build

conan install .. -b missing -s build_type=Debug -s compiler.runtime=MTd
conan install .. -b missing -s build_type=Release -s compiler.runtime=MT

cmake -G "Visual Studio 16 2019" -A x64 ..