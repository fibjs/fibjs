#!/bin/bash

V8_PATH=~/Downloads/source/js/v8

WORK_PATH=`pwd`
cd $V8_PATH

git pull
make ia32.release

cd $WORK_PATH

sh do_patch.sh
sh mk_patch.sh
