# Building fibjs

## Prerequisites (unix)

- GCC 4.8 or newer
- CMake 2.6 or newer
- GNU Make 3.81 or newer
- libexecinfo (FreeBSD and OpenBSD only)

----------------------------------
### on ubuntu:

```sh
apt install g++
apt install make
apt install cmake
apt install git
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
git clone https://github.com/fibjs/fibjs.git
cd fibjs
git submodule init
git submodule update
```

## Update

```sh
cd fibjs
git pull
git submodule update
```

## Build

### on unix:
	sh build -j

### on Windows (Visual Studio 2015 Express):
	Start
	  All Programs
	    Visual Studio 2015
	      Visual Studio Tools
	        Developer Command Prompt for VS2015

	build

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