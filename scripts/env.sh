#!/usr/bin/env sh

get_build_envs()
{
	HOST_OS=`uname`
	HOST_ARCH=`uname -m`

	case ${HOST_ARCH} in
		i386|i686) HOST_ARCH="i386"
			;;
		x86_64|amd64) HOST_ARCH="amd64"
			;;
		armv6|armv7|armv7s|armv7l) HOST_ARCH="arm"
			;;
		aarch64) HOST_ARCH="arm64"
			;;
		mips|mipsel) HOST_ARCH="mips"
			;;
		mips64) HOST_ARCH="mips64"
			;;
		powerpc) HOST_ARCH="ppc"
			;;
		ppc64) HOST_ARCH="ppc64"
			;;
	esac

	TARGET_OS=$HOST_OS
	TARGET_ARCH=$HOST_ARCH
	if [ -z "${BUILD_TYPE}" ]; then
		BUILD_TYPE="release"
	fi
}