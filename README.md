## About fibjs

fibjs 是一个建立在 Google v8 Javascript 引擎基础上的应用服务器开发框架，不同于 node.js，fibjs 采用 fiber 解决 v8 引擎的多路复用，并通过大量 c++ 组件，将重负荷运算委托给后台线程，释放 v8 线程，争取更大的并发时间。

fibjs is a runtime for javascript applictions built on google v8 JS.  Unlike node.js, fibjs uses fibers for parallelizing v8  engine. Through a large amoung of C++ components, heavy duty operations are performed in a background thread and the v8 thread is released. Thus fibjs, allows for greater concurrency 
## Online Docs

github 中有全部文档的源码和生成文档，为方便查阅，我在线上存放了一份，但是更新可能会没有 github 及时，仅供参阅。

All the source code and docs are on github. However, I use github for convenience as it is easy to store stuff online. So there may be small problems with the documentation as I may not be able to update them timely.

***文档入口(General docs (chinese)):*** http://fibjs.org/

***文档下载(Documents repository):*** https://github.com/xicilion/fibjs_docs

***了解 fibjs 的并发(Understanding how to develop for fibjs):*** http://fibjs.org/d2/de8/start_prog.html

***官方交流社区(Online discussion.):*** http://baoz.me/fibjs

ps: 请无视编译中出现的大量警告错误，那些基本上都是第三方代码库的警告，恕不消除了。
Please ignore the compiler warnings.

## Prerequisites (unix)

	GCC 4.6 or newer
	CMake 2.6 or newer
	GNU Make 3.81 or newer
	libexecinfo (FreeBSD and OpenBSD only)

----------------------------------
### on ubuntu:
	apt-get install g++
	apt-get install make
	apt-get install cmake

### 32bit on ubuntu:
	apt-get install g++-multilib

----------------------------------
### on fedora:
	yum install gcc-c++
	yum install libstdc++-static
	yum install make
	yum install cmake

### 32bit on fedora:
	yum install glibc-devel.i686
	yum install libstdc++-static.i686

----------------------------------
### on osx:
	brew install cmake

----------------------------------
### on freebsd (8,9):
	pkg_add -r cmake
	pkg_add -r libexecinfo

----------------------------------
### on freebsd 10+:
	pkg install cmake
	pkg install libexecinfo

----------------------------------
### on sunos:
	pkg install gcc-45
	pkg install system/header
	pkg install developer/build/cmake

## Build

### on unix:
	sh build

### on Windows (Visual Studio 2013 Express):
	Start
	  All Programs
	    Visual Studio 2013
	      Visual Studio Tools
	        Developer Command Prompt for VS2013
	        
	build

----------------------------------

## Install

### on linux/freebsd/osx:
	sudo sh bin/(Darwin|Linux|FreeBSD)_Release/installer.sh

### on Windows console (run as administrator):
	bin\Windows_Release\installer.exe

## Test

	cd test
	js main.js
