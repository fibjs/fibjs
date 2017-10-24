@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

if defined ProgramFiles(x86) (set HOST_ARCH=amd64) else (set HOST_ARCH=i386)
set TARGET_ARCH=!HOST_ARCH!

set BUILD_TYPE=release
set XP=""
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

    if "%%a"=="xp" (
    	set XP="_XP"
        set ARG_ERROR=no
    )

    if "%%a"=="clean" (
    	set BUILD_TYPE=clean
        set ARG_ERROR=no
    )

    if "%%a"=="-h" goto usage
    if "%%a"=="--help" goto usage

    if "!ARG_ERROR!"=="yes" (
        echo illegal option "%%a"
        goto usage
    )
)

if "!BUILD_TYPE!"=="clean" (
    if "!TARGET_ARCH!"=="amd64" (
        rmdir /S/Q out\Windows_amd64_Debug
        rmdir /S/Q out\Windows_amd64_Release
    )

    if "!TARGET_ARCH!"=="i386" (
        rmdir /S/Q out\Windows_i386_Debug
        rmdir /S/Q out\Windows_i386_Release
    )
    goto out
)

if "!TARGET_ARCH!"=="amd64" (set Platform=x64) else (set Platform=Win32)

msbuild fibjs.sln /t:Build /p:Configuration=!BUILD_TYPE!;Platform=!Platform!;PlatformToolset=v141!XP! !MT!

if "!BUILD_TYPE!"=="release" (
	cd bin\Windows_!TARGET_ARCH!_!BUILD_TYPE!
    fibjs ../../fibjs/gen_install.js
    cd ..\..
    cd installer
    msbuild installer.sln /t:Build /p:Configuration=Release;Platform=!Platform!;PlatformToolset=v141!XP! !MT!
    cd ..
)

goto out

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

:out