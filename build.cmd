@echo off

set WORK_ROOT=%cd%
set SOURCE_ROOT=%~dp0

if exist "%SOURCE_ROOT%/vender" (
    cd "%SOURCE_ROOT%/vender"
    call build %*%
    if ERRORLEVEL 1 goto exitbuild
)else goto inform

cd "%SOURCE_ROOT%/fibjs"
call build %*%
if ERRORLEVEL 1 goto exitbuild

cd "%SOURCE_ROOT%/fibjs/program"
call build %*%
if ERRORLEVEL 1 goto exitbuild

cd "%SOURCE_ROOT%/fibjs/installer"
call build %*%
if ERRORLEVEL 1 goto exitbuild

cd "%SOURCE_ROOT%"
goto finished

:inform
	echo.
	echo submodule vender is not existed!
	echo you can execute the given command to init and update it.
	echo  \$ git submodule init
	echo  \$ git submodule update
	echo.

:exitbuild
cd "%SOURCE_ROOT%"
exit /B 1

:finished