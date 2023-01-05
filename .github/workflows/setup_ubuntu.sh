#!/bin/bash

sudo apt update

if [ "$BUILD_TARGET" == "" ]; then
  sudo apt install clang -y
  sudo apt install make -y
  sudo apt install cmake -y
  sudo apt install git -y
  sudo apt install libx11-dev -y

  if [[ "$TARGET_ARCH" == "amd64" ]]; then
    sudo apt install g++-x86-64-linux-gnu -y
  fi

  if [[ "$TARGET_ARCH" == "i386" ]]; then
    sudo apt install g++-i686-linux-gnu -y
  fi

  if [[ "$TARGET_ARCH" == "arm" ]]; then
    sudo apt install g++-arm-linux-gnueabihf -y
  fi

  if [[ "$TARGET_ARCH" == "arm64" ]]; then
    sudo apt install g++-aarch64-linux-gnu -y
  fi

  if [[ "$TARGET_ARCH" == "mips64" ]]; then
    sudo apt install g++-mips64el-linux-gnuabi64 -y
  fi

  if [[ "$TARGET_ARCH" == "ppc64" ]]; then
    sudo apt install g++-powerpc64le-linux-gnu -y
  fi
fi

sudo apt install qemu-user-static -y
