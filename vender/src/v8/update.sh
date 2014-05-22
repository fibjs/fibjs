#!/bin/bash

V8_PATH=~/Downloads/source/js/v8

WORK_PATH=`pwd`
cd $V8_PATH

svn revert -R .
svn up
make ia32.release

cd $WORK_PATH

sh do_patch.sh
sh mk_patch.sh
