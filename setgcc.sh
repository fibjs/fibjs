#!/bin/bash 
usage() {
        echo 
        echo Sets the default version of gcc, g++, etc
        echo Usage:
        echo 
        echo "    gcc-set-default-version <VERSION>"
        echo 
        exit
}
cd /usr/bin
if [ -z $1 ] ; then 
        usage;
fi 
set_default() {
        if [ -e "$1-$2" ] ; then 
                echo $1-$2 is now the default
                ln -sf $1-$2 $1
        else 
                echo $1-$2 is not installed
        fi
}
for i in gcc cpp g++ gcov gccbug ; do 
        set_default $i $1
done