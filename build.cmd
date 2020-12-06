@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

if defined ProgramFiles(x86) (set HOST_ARCH=amd64) else (set HOST_ARCH=i386)

if /I "%HOST_ARCH%" == "" (
    if /I "%PROCESSOR_ARCHITECTURE%" == "AMD64" ( @set HOST_ARCH=amd64 ) else ( @set HOST_ARCH=i386 )
)

if /I "%TARGET_ARCH%" == "" (
    @set TARGET_ARCH=%HOST_ARCH%
)

set BUILD_TYPE=release
set MT=/m
set ARG_ERROR=no

for %%a in (%*) do (
    set ARG_ERROR=yes

    if "%%a"=="amd64" (
    	set TARGET_ARCH=amd64
        set ARG_ERROR=no
    )

    if "%%a"=="i386" (
    	set TARGET_ARCH=i386
        set ARG_ERROR=no
    )

    if "%%a"=="release" (
    	set BUILD_TYPE=release
        set ARG_ERROR=no
    )

    if "%%a"=="debug" (
    	set BUILD_TYPE=debug
        set ARG_ERROR=no
        set MT=
    )

    if "%%a"=="clean" (
    	set BUILD_TYPE=clean
        set ARG_ERROR=no
    )

    if "%%a"=="-h" goto usage
    if "%%a"=="--help" goto usage

    if "%ARG_ERROR%"=="yes" (
        echo illegal option "%%a"
        goto usage
    )
)

if "%BUILD_TYPE%"=="clean" (
    if "%TARGET_ARCH%"=="amd64" (
        rmdir /S/Q out\Windows_amd64_Debug
        rmdir /S/Q out\Windows_amd64_Release
    )

    if "%TARGET_ARCH%"=="i386" (
        rmdir /S/Q out\Windows_i386_Debug
        rmdir /S/Q out\Windows_i386_Release
    )
    goto out
)

if "%TARGET_ARCH%"=="amd64" (set TargetPlatform=x64) else (set TargetPlatform=Win32)

SET VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe
@rem Visual Studio 2017
FOR /f "delims=" %%A IN ('"%VSWHERE%" -property installationPath -prerelease -version [15.0^,16.0^)') DO (
    SET VSINSTALLPATH=%%A
    echo "VSINSTALLPATH is %VSINSTALLPATH%"
)

REM Run vcvars*.bat to set environment variables, to tell clang use vc tool chains on vs 2017 rather than post one(if existed.)
IF "%__VCVARSALL_VER%" == "" (
    IF /I "%TargetPlatform%" == "x64" (
        call "%VSINSTALLPATH%\VC\Auxiliary\Build\vcvars64.bat"
        ECHO "x64 building..."
    ) 
    IF /I "%TargetPlatform%" == "Win32" (
        call "%VSINSTALLPATH%\VC\Auxiliary\Build\vcvars32.bat"
        ECHO "x86 building..."
    )
)

REM we have set two essential env-vars: BUILD_TYPE, TARGET_ARCH, which would be passed transparently and used in Unix shell file .\build
ECHO "start %TARGET_ARCH% building..."
"C:\Program Files\Git\bin\sh.exe" .\build %TARGET_ARCH%
    
if "%BUILD_TYPE%"=="release" (
	cd bin\Windows_%TARGET_ARCH%_%BUILD_TYPE%
    fibjs ../../fibjs/program/gen_install.js
    cd ..\..
    cd installer
    msbuild installer.sln /t:Build /p:Configuration=Release;Platform=%TargetPlatform% %MT%
    cd ..
)

goto finished

:usage
echo.
echo Usage: build [release ^| debug ^| i386 ^| amd64 ^| noxp ^| clean] [-h]
echo Options:
echo   release, debug: 
echo       Specifies the build type.
echo   i386, amd64:
echo       Specifies the architecture for code generation.
echo   xp:
echo       Build as xp support.
echo   clean: 
echo       Clean the build folder.
echo   -h, --help:
echo       Print this message and exit.
echo.

:finished