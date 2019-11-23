REM OLD_VER -- https://github.com/llvm/llvm-project/releases/download/llvmorg-%OLD_VER%/LLVM-%OLD_VER%-win64.exe
REM 8.0.1 -- https://github.com/llvm/llvm-project/releases/download/llvmorg-8.0.1/LLVM-8.0.1-win64.exe
REM 9.0.0 -- http://releases.llvm.org/9.0.0/LLVM-9.0.0-win64.exe

if NOT EXIST llvm-installer.exe (
    IF /I "%PLATFORM%" == "x64" (
        appveyor DownloadFile https://github.com/llvm/llvm-project/releases/download/llvmorg-8.0.1/LLVM-8.0.1-win64.exe -FileName llvm-installer.exe
        ECHO "install llvm for x64"
    )
    IF /I "%PLATFORM%" == "x86" (
        appveyor DownloadFile https://github.com/llvm/llvm-project/releases/download/llvmorg-8.0.1/LLVM-8.0.1-win32.exe -FileName llvm-installer.exe
        ECHO "install llvm for x86"
    )
)

START /WAIT llvm-installer.exe /S /D=C:\"Program Files\LLVM"
@set PATH="C:\Program Files\LLVM\bin";%PATH%

EXIT