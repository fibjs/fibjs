# Building fibjs

## Prerequisites

| Dependencies | Windows | Linux | Darwin |
|--|--|--|--|
| cmake >= 3.12 | √ | √ | √ |
| clang/LLVM {8.x, 9.x} | √ | √ | √ |
| visual studio 2017 (with C++ Tool) | √ ||
| gnu make | √ | √ | √ |
| bash | √ (by **git-bash**) | √ | √ |

----------------------------------
### on Windows
#### Install LLVM

Run bat to install clang/LLVM.

```bat
REM for x64
curl -Ls -o "llvm-installer.exe" "https://github.com/llvm/llvm-project/releases/download/llvmorg-8.0.1/LLVM-8.0.1-win64.exe"
REM for x86
curl -Ls -o "llvm-installer.exe" "https://github.com/llvm/llvm-project/releases/download/llvmorg-8.0.1/LLVM-8.0.1-win32.exe"

START /WAIT llvm-installer.exe /S /D=C:\"Program Files\LLVM"

EXIT
```

You can also install clang/LLVM manually from [official releases](https://releases.llvm.org/download.html#9.0.1) (https://releases.llvm.org/download.html).

Then add `"C:\Program Files\LLVM\bin"` to your environment variable `%PATH%`.

#### Install git (with git-bash)

[`git-downloads`]:https://git-scm.com/downloads

Download git installation from [`git-downloads`]: https://git-scm.com/downloads.

After install git, you should do more setup:

1. download [make-4.3-without-guile-w32-bin.zip](https://netix.dl.sourceforge.net/project/ezwinports/make-4.3-without-guile-w32-bin.zip) from https://netix.dl.sourceforge.net/project/ezwinports/make-4.3-without-guile-w32-bin.zip, it's recommended to extract whole zip file to, 
	- (for 64 bit): extract to `C:/Program Files/Git/mingw64/`
	- (for 32 bit): extract to `C:/Program Files/Git/mingw32/`
1. make sure `sh.exe` and `make.exe` are in you `%PATH%` environtment variable.

### on ubuntu:

```sh
apt install clang
apt install make
apt install cmake
apt install git
apt install ccache
```

### 32bit on ubuntu:

```sh
apt install g++-multilib
```

### arm on ubuntu:

```sh
apt install g++-arm-linux-gnueabihf
```

### arm64 on ubuntu:

```sh
apt install g++-aarch64-linux-gnu
```

### mips on ubuntu:

```sh
apt install g++-mips-linux-gnu
```

### mips64 on ubuntu:

```sh
apt install g++-mips64-linux-gnuabi64
```

### powerpc on ubuntu:

```sh
apt install g++-powerpc-linux-gnu
```

### powerpc64 on ubuntu:

```sh
apt install g++-powerpc64-linux-gnu
```

### fix

```sh
rm -f /usr/include/asm
ln -s x86_64-linux-gnu /usr/include/i386-linux-gnu
ln -s x86_64-linux-gnu /usr/include/x86_64-linux-gnux32
```

----------------------------------
### on fedora:

```sh
dnf install gcc-c++
dnf install libstdc++-static
dnf install make
dnf install cmake
dnf install git
```

### 32bit on fedora:

```sh
dnf install glibc-devel.i686
dnf install libstdc++-static.i686
```

----------------------------------
### on osx:

```sh
brew install cmake
brew install git
```

----------------------------------
### on freebsd:

```sh
pkg install cmake
pkg install libexecinfo
pkg install git
```

## Download

```sh
git clone https://github.com/fibjs/fibjs.git --recursive
```

If a repository is cloned without the --recursive flag, the submodules can be retrieved after the fact by running this command from within the repo:
```sh
cd fibjs
git submodule update --init --recursive
```

## Update

```sh
cd fibjs
git pull
git submodule update --init --recursive
```

## Build

### on unix:
	sh build -j

### on Windows (Visual Studio 2017 Community):
	./build.cmd

----------------------------------

## Install

### on linux/freebsd/osx:

```sh
sudo sh bin/(Darwin|Linux|FreeBSD)_(i386|amd64|arm|arm64)_release/installer.sh
```

### on Windows console (run as administrator):

```sh
bin\Windows_(i386|amd64)_release\installer.exe
```

## Test

```sh
cd test
fibjs main.js
```