@echo off

set WORK_ROOT=%cd%
set SOURCE_ROOT=%~dp0

if "%USE_VENDER_DIST%" == "" (
	if exist "%SOURCE_ROOT%/vender/build.cmd" (
		cd /d "%SOURCE_ROOT%/vender"
		call build %*%
		if ERRORLEVEL 1 goto exitbuild
	) else goto inform
)

cd /d "%SOURCE_ROOT%/fibjs"
call build %*%
if ERRORLEVEL 1 goto exitbuild

cd /d "%SOURCE_ROOT%/fibjs/program"
call build %*%
if ERRORLEVEL 1 goto exitbuild

cd /d "%SOURCE_ROOT%/fibjs/installer"
call build %*%
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
echo         FIBJS has been successfully built.
echo.
echo         For more information:
echo         website: http://fibjs.org
echo         repository: https://github.com/fibjs
echo.
echo.