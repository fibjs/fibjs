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

---------------------------------- 
### on osx: 
	brew install cmake

---------------------------------- 
### on freebsd: 
	pkg_add -r cmake 
	pkg_add -r subversion 
	pkg_add -r libexecinfo

---------------------------------- 
### build: 
	sh build

---------------------------------- 
### install: 
	sudo sh bin/Release/installer.sh
