## 关于 fibjs

fibjs 是一个建立在 Google v8 Javascript 引擎基础上的应用服务器开发框架，不同于 node.js，fibjs 采用 fiber 解决 v8 引擎的多路复用，并通过大量 c++ 组件，将重负荷运算委托给后台线程，释放 v8 线程，争取更大的并发时间。

## 在线文档

github 中有全部文档的源码和生成文档，为方便查阅，我在线上存放了一份，但是更新可能会没有 github 及时，仅供参阅。

文档入口: http://prj.baoz.cn/docs/fibjs/
了解 fibjs 的并发: http://prj.baoz.cn/docs/fibjs/d2/de8/start_prog.html

## Install

### on linux/freebsd/osx:
	sudo sh installer.sh

### on Windows(run as administrator):
	installer

## Test

	cd test
	js main.js

----------------------------------

## Prerequisites (unix)

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

## Build

### on unix:
	sh build

### on Windows:
	Start
	  All Programs
	    Visual Studio 2013
	      Visual Studio Tools
	        Developer Command Prompt for VS2013
	        
	build
