# Prerequisites (unix):

	GCC 4.2 or newer
	CMake 2.6 or newer
	GNU Make 3.81 or newer
	libexecinfo (FreeBSD and OpenBSD only)

----------------------------------
### on ubuntu:
	apt-get install g++
	apt-get install make
	apt-get install cmake
	apt-get install subversion

### 32bit on ubuntu:
	apt-get install g++-multilib

----------------------------------
### on fedora:
	yum install gcc-c++
	yum install libstdc++-static
	yum install make
	yum install cmake
	yum install subversion

### 32bit on fedora:
	yum install glibc-devel.i686
	yum install libstdc++-static.i686

----------------------------------
### on osx:
	brew install cmake

----------------------------------
### on freebsd (8,9):
	pkg_add -r cmake
	pkg_add -r subversion
	pkg_add -r libexecinfo

----------------------------------
### on freebsd 10+:
	pkg install cmake
	pkg install subversion
	pkg install libexecinfo

----------------------------------
### on sunos:
	pkg install gcc-45
	pkg install system/header
	pkg install developer/build/cmake
	pkg install developer/versioning/subversion

# Build:

### on unix:
	sh build

----------------------------------
### on Windows:
	Start
	  All Programs
	    Visual Studio 2013
	      Visual Studio Tools
	        Developer Command Prompt for VS2013
	        
	build

# Test:

	cd test
	../bin/{OS}_Release/fibjs main.js

# Install:

### on linux/freebsd/osx:
	sudo sh bin/{OS}_Release/installer.sh  // x86_64
	sudo sh bin/{OS}_Release32/installer.sh // ia32

----------------------------------
### on Windows(run as administrator):
	bin/Windows_Release/installer  // x86_64
	bin/Windows_Release32/installer // ia32

# Run:

	js
	js stuff.js
