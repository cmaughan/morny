@echo off

echo %Time%

if not exist "vcpkg\vcpkg.exe" (
    cd vcpkg
    call bootstrap-vcpkg.bat -disableMetrics
    cd %~dp0
)

cd vcpkg
echo Installing Libraries
vcpkg install tsl-ordered-map fmt clipp glm tinydir --triplet x64-windows-static-md --recurse
cd %~dp0

echo %Time%

