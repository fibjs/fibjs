#!/bin/bash

set -ev

CUSTOM_INSTALL=true
COMPILER=""

if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
    echo "ARCH IS: ${ARCH}"
    sudo add-apt-repository 'deb http://us.archive.ubuntu.com/ubuntu/ xenial main restricted'
    sudo add-apt-repository 'deb http://us.archive.ubuntu.com/ubuntu/ xenial-updates main restricted'
    sudo add-apt-repository 'deb http://us.archive.ubuntu.com/ubuntu/ xenial universe'
    sudo add-apt-repository 'deb http://us.archive.ubuntu.com/ubuntu/ xenial-updates universe'
    sudo add-apt-repository 'deb http://us.archive.ubuntu.com/ubuntu/ xenial multiverse'
    sudo add-apt-repository 'deb http://us.archive.ubuntu.com/ubuntu/ xenial-updates multiverse'
    sudo add-apt-repository 'deb http://us.archive.ubuntu.com/ubuntu/ xenial-backports main restricted universe multiverse'
    sudo add-apt-repository 'deb http://security.ubuntu.com/ubuntu xenial-security main restricted'
    sudo add-apt-repository 'deb http://security.ubuntu.com/ubuntu xenial-security universe'
    sudo add-apt-repository 'deb http://security.ubuntu.com/ubuntu xenial-security multiverse'

    sudo apt-get -qq update

    sudo rm -f /usr/bin/gcc
    sudo rm -f /usr/bin/g++ 

    sudo ln -s gcc-5 /usr/bin/gcc
    sudo ln -s g++-5 /usr/bin/g++

    case "${ARCH}" in
        amd64)
            CUSTOM_INSTALL=""
            ;;
        i386)
            sudo apt-get install g++-multilib -y
            sudo rm -f /usr/include/asm
            sudo ln -s x86_64-linux-gnu /usr/include/i386-linux-gnu
            sudo ln -s x86_64-linux-gnu /usr/include/x86_64-linux-gnux32
            
            sudo rm -f /usr/bin/x86_64-linux-gnu-gcc
            sudo rm -f /usr/bin/x86_64-linux-gnu-g++

            sudo ln -s gcc-5 /usr/bin/x86_64-linux-gnu-gcc
            sudo ln -s g++-5 /usr/bin/x86_64-linux-gnu-g++

            CUSTOM_INSTALL=""
            ;;
        arm)
            COMPILER=arm-linux-gnueabihf
            ;;
        arm64)
            COMPILER=aarch64-linux-gnu
            ;;
        mips)
            COMPILER=mips-linux-gnu        
            ;;
        mips64)
            COMPILER=mips64-linux-gnuabi64        
            ;;
        ppc)
            COMPILER=powerpc-linux-gnu        
            ;;
        ppc64)
            COMPILER=powerpc64-linux-gnu        
            ;;
    esac

    if [[ $CUSTOM_INSTALL == "true" ]]; then
        sudo apt-get install g++-5-${COMPILER} -y
            
        sudo rm -f /usr/bin/${COMPILER}-gcc
        sudo rm -f /usr/bin/${COMPILER}-g++
        
        sudo ln -s ${COMPILER}-gcc-5 /usr/bin/${COMPILER}-gcc
        sudo ln -s ${COMPILER}-g++-5 /usr/bin/${COMPILER}-g++
    fi
fi

exit 0;