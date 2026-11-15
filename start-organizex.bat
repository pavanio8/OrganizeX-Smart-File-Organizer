@echo off
setlocal
cd /d "%~dp0"

set "CMAKE_EXE=cmake"
where cmake >nul 2>nul
if errorlevel 1 (
    if not exist "%LOCALAPPDATA%\OrganizeX\toolchain\cmake-4.5.0\bin\cmake.exe" (
        echo CMake was not found. Install CMake 3.16 or newer and a C++17 compiler.
        exit /b 1
    )
    set "CMAKE_EXE=%LOCALAPPDATA%\OrganizeX\toolchain\cmake-4.5.0\bin\cmake.exe"
)

if exist "%LOCALAPPDATA%\OrganizeX\toolchain\llvm-mingw-20261104\bin\clang++.exe" (
    set "PATH=%LOCALAPPDATA%\OrganizeX\toolchain\llvm-mingw-20261104\bin;%PATH%"
)

where node >nul 2>nul
if errorlevel 1 (
    echo Node.js was not found. Install Node.js 18 or newer to serve the frontend.
    exit /b 1
)

"%CMAKE_EXE%" -S backend -B backend\build
if errorlevel 1 exit /b 1

"%CMAKE_EXE%" --build backend\build --config Release
if errorlevel 1 exit /b 1

set "BACKEND_EXE=%~dp0backend\build\organizeX_server.exe"
if not exist "%BACKEND_EXE%" set "BACKEND_EXE=%~dp0backend\build\Release\organizeX_server.exe"
if not exist "%BACKEND_EXE%" (
    echo The CMake build completed, but organizeX_server.exe was not found.
    exit /b 1
)

start "OrganizeX Frontend" cmd /k node "%~dp0frontend\server.js"
echo.
echo Frontend: http://127.0.0.1:5500
echo Backend:  http://127.0.0.1:8080
echo Keep this window open to run the backend. Press Ctrl+C to stop it.
"%BACKEND_EXE%"
