@echo off

setlocal enabledelayedexpansion

set start=%time%

set WORK_ROOT=%cd%
set SOURCE_ROOT=%~dp0

set dev=0
set build_addon=0
set USE_VENDER_DIST=
set new_args=

set i=0
for %%a in (%*) do (
    if "%%a"=="dev" (
        set dev=1
        set build_addon=1
    ) else if "%%a"=="ci" (
        set USE_VENDER_DIST=1
        set build_addon=1
    ) else (
        set new_args=!new_args! %%a
        set /a i+=1
    )
)

set args_count=0
for /f %%a in ('set new_args[ 2^>nul ^| find /c "="') do set args_count=%%a

if %dev%==0 (
    set USE_VENDER_DIST=1
	git submodule update --init --recursive
)

if "%USE_VENDER_DIST%" == "" (
	if exist "%SOURCE_ROOT%/vender/build.cmd" (
		cd /d "%SOURCE_ROOT%/vender"
		call build %new_args%
		if ERRORLEVEL 1 goto exitbuild
	) else goto inform
)

cd /d "%SOURCE_ROOT%/fibjs"
call build %new_args%
if ERRORLEVEL 1 goto exitbuild

cd /d "%SOURCE_ROOT%/fibjs/program"
call build %new_args%
if ERRORLEVEL 1 goto exitbuild

if %build_addon%==1 (
	cd /d "%SOURCE_ROOT%/fibjs/addons"
	call build %new_args%
	if ERRORLEVEL 1 goto exitbuild
)

cd /d "%SOURCE_ROOT%/fibjs/installer"
call build %new_args%
if ERRORLEVEL 1 goto exitbuild

cd /d "%SOURCE_ROOT%"
goto finished

:inform
	echo.
	echo submodule vender is not existed!
	echo you can execute the given command to init and update it.
	echo  \$ git submodule init
	echo  \$ git submodule update
	echo.

:exitbuild
cd /d "%SOURCE_ROOT%"
exit /B 1

:finished
set end=%time%

call :TimeToMilliseconds %start% start_ms
call :TimeToMilliseconds %end% end_ms

set /a diff=end_ms-start_ms

if %diff% lss 0 set /a diff=8640000+diff

set /a hours=diff/3600000
set /a mins=(diff%%3600000)/60000
set /a secs=(diff%%60000)/1000

if %hours% lss 10 set hours=0%hours%
if %mins% lss 10 set mins=0%mins%
if %secs% lss 10 set secs=0%secs%

echo.
echo.
cmake -E cmake_echo_color --red ^
	"         _______ _________ ______  _________ _______  " ^
	"        (  ____ \\__   __/(  ___ \ \__    _/(  ____ \ " ^
	"        | (    \/   ) (   | (   ) )   )  (  | (    \/ " ^
	"        | (__       | |   | (__/ /    |  |  | (_____  " ^
	"        |  __)      | |   |  __ (     |  |  (_____  ) " ^
	"        | (         | |   | (  \ \    |  |        ) | " ^
	"        | (_     ___) (___| (___) )/\_)  )  /\____) | " ^
	"        (__/     \_______/(______/ \____/   \_______) "
echo.
echo.
echo         FIBJS has been successfully built during %hours%:%mins%:%secs%.
echo.
echo         For more information:
echo         website: https://fibjs.org
echo         repository: https://github.com/fibjs
echo.
echo.

goto :eof

:TimeToMilliseconds
set time=%1

for /f "tokens=1-4 delims=:.," %%a in ("%time%") do (
    set hours=%%a
    set minutes=%%b
    set seconds=%%c
    set milliseconds=%%d
)

set /a total_ms=(hours*3600 + minutes*60 + seconds)*1000 + milliseconds*10
set %2=%total_ms%
goto :eof