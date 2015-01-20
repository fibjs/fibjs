#!/bin/bash

WORK_PATH=`pwd`
cd ~/Downloads/source/js/v8/v8
V8_PATH=`pwd`

git pull
make ia32.release

cd $WORK_PATH

js update.js $V8_PATH