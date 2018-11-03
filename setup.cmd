@echo off
SetLocal EnableDelayedExpansion

:: Unfortunately, we cannot build both x64 and x86 binaries from the same VS command window (it's unfortunate enough
:: that we even need a VS command window...), so use the Platform environment variable to initialize
if "%Platform%" == "" (
    echo ERROR: Please run the setup script from a VS command window
    goto :eof
)

call :cmake %Platform% Debug
call :cmake %Platform% Release
goto :eof

:cmake
echo === Configuring CMake for configuration %~2 and architecture %~1 ===

set TARGET_DIR=build\%~1%~2
mkdir %TARGET_DIR% > NUL 2>&1
pushd %TARGET_DIR%

set CC=clang-cl
set CXX=clang-cl

if "%~1" == "x86" (
    set CFLAGS=-m32
    set CXXFLAGS=-m32
)

set ADDITIONAL_FLAGS=

:: Build GoogleTest with /MD(d) option
:: set ADDITIONAL_FLAGS=%ADDITIONAL_FLAGS% -DBUILD_SHARED_LIBS=ON

:: TODO: Figure this out
set ADDITIONAL_FLAGS=%ADDITIONAL_FLAGS% -DBENCHMARK_ENABLE_TESTING=OFF
set ADDITIONAL_FLAGS=%ADDITIONAL_FLAGS% -DBENCHMARK_ENABLE_GTEST_TESTS=OFF

cmake -GNinja -DCMAKE_BUILD_TYPE=%~2 -DCMAKE_LINKER:PATH=lld-link %ADDITIONAL_FLAGS% ..\..

echo.
popd
goto :eof
