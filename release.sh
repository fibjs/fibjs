#!/bin/bash

cp bin/Darwin_amd64_release/fibjs release/fibjs-osx
cp bin/Darwin_i386_release/fibjs release/fibjs-osx32
cp bin/Linux_amd64_release/fibjs release/fibjs-linux
cp bin/Linux_i386_release/fibjs release/fibjs-linux32
cp bin/Linux_arm_release/fibjs release/fibjs-linux-arm
cp bin/Linux_arm64_release/fibjs release/fibjs-linux-arm64
cp bin/FreeBSD_amd64_release/fibjs release/fibjs-freebsd
cp bin/FreeBSD_i386_release/fibjs release/fibjs-freebsd32
cp bin/Windows_amd64_release/fibjs.exe release/fibjs-win.exe
cp bin/Windows_i386_release/fibjs.exe release/fibjs-win32.exe
