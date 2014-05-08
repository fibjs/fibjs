@echo off

if "%1"=="Release" goto release
if "%1"=="Debug" goto debug
if "%1"=="Release32" goto release32
if "%1"=="Debug32" goto debug32
if "%1"=="clean" goto clean

echo build [Release ^| Release32 ^| Debug ^| Debug32 ^| clean]

goto out

:release
msbuild exlib.sln /t:Build /p:Configuration=Release;Platform=x64
cd bin\Windows_Release
fibjs ../../fibjs/gen_install.js
cd ..\..
cd installer
msbuild installer.sln /t:Build /p:Configuration=Release;Platform=x64
cd ..
goto out

:debug
msbuild exlib.sln /t:Build /p:Configuration=Debug;Platform=x64
goto out

:release32
msbuild exlib.sln /t:Build /p:Configuration=Release;Platform=Win32
cd bin\Windows_Release32
fibjs ../../fibjs/gen_install.js
cd ..\..
cd installer
msbuild installer.sln /t:Build /p:Configuration=Release;Platform=Win32
cd ..
goto out

:debug32
msbuild exlib.sln /t:Build /p:Configuration=Debug;Platform=Win32
goto out

:clean
msbuild exlib.sln /t:Clean
cd installer
msbuild installer.sln /t:Clean
cd ..
goto out

:out
