#!/bin/bash

WORK_PATH=`pwd`
cd ../../../source/js/v8/v8
V8_PATH=`pwd`

# git pull
make ia32.release

cd $WORK_PATH

fibjs update.js $V8_PATH