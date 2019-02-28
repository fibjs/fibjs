#!/bin/bash

# this script is to create a fibjs cross-compile envirment using clang on ubuntu.
# reference to eos_build_ubuntu.sh at https://github.com/EOSIO/eos/blob/master/scripts/eosio_build_ubuntu.sh.
# read more about How To Cross-compilation using Clang at https://llvm.org/docs/HowToCrossCompileLLVM.html and http://clang.llvm.org/docs/CrossCompilation.html.

OS_VER=$( grep VERSION_ID /etc/os-release | cut -d'=' -f2 | sed 's/[^0-9\.]//gI' )
OS_MAJ=$(echo "${OS_VER}" | cut -d'.' -f1)
OS_MIN=$(echo "${OS_VER}" | cut -d'.' -f2)

MEM_MEG=$( free -m | sed -n 2p | tr -s ' ' | cut -d\  -f2 || cut -d' ' -f2 )
CPU_SPEED=$( lscpu | grep -m1 "MHz" | tr -s ' ' | cut -d\  -f3 || cut -d' ' -f3 | cut -d'.' -f1 )
CPU_CORE=$( nproc )
MEM_GIG=$(( ((MEM_MEG / 1000) / 2) ))

DISK_INSTALL=$(df -h . | tail -1 | tr -s ' ' | cut -d\  -f1 || cut -d' ' -f1)
DISK_TOTAL_KB=$(df . | tail -1 | awk '{print $2}')
DISK_AVAIL_KB=$(df . | tail -1 | awk '{print $4}')
DISK_TOTAL=$(( DISK_TOTAL_KB / 1048576 ))
DISK_AVAIL=$(( DISK_AVAIL_KB / 1048576 ))

printf "\\nOS name: ${OS_NAME}\\n"
printf "OS Version: ${OS_VER}\\n"
printf "CPU speed: ${CPU_SPEED}Mhz\\n"
printf "CPU cores: %s\\n" "${CPU_CORE}"
printf "Physical Memory: ${MEM_MEG} Mgb\\n"
printf "Disk install: ${DISK_INSTALL}\\n"
printf "Disk space total: ${DISK_TOTAL%.*}G\\n"
printf "Disk space available: ${DISK_AVAIL%.*}G\\n"

if [ "${OS_MAJ}" -lt 16 ]; then
	printf "You must be running Ubuntu 16.04.x or higher to install fibjs.\\n"
	printf "Exiting now.\\n"
	exit 1
fi

DEP_ARRAY=(make cmake git xz-utils build-essential)
declare -A CROSS_ARRY_MAP=(["mips"]="g++-5-mips-linux-gnu" ["mips64"]="g++-5-mips64-linux-gnuabi64" ["ppc"]="g++-5-powerpc-linux-gnu" ["ppc64"]="g++-5-powerpc64-linux-gnu" ["arm"]="g++-5-arm-linux-gnueabihf" ["arm64"]="g++-5-aarch64-linux-gnu")

ANSWER=""
COUNT=1
DISPLAY=""
DEP=""
NO_UPDATE=false
ClangInstalledDir=""

UpdateAPTSource()
{
	if ! apt-get update; then
		printf " - APT update failed.\\n"
		exit 1;
	else
		printf " - APT update complete.\\n"
	fi
}

CheckAndGetClangInstallDir()
{
	if clang-6.0 --version 2>/dev/null || false; then
		printf "clang-6.0 has installed. \\n"
		ClangInstalledDir=$(clang-6.0 --version | grep InstalledDir | awk -F': ' '{print $2}')
	else
		DEP_ARRAY[${#DEP_ARRAY[@]}]="clang-6.0"
	fi
}

# Self arch do not have to install
CROSS_COMPILE=${CROSS_ARRY_MAP["$1"]}
if [ -n "$CROSS_COMPILE" ]; then
	DEP_ARRAY[${#DEP_ARRAY[@]}]="${CROSS_ARRY_MAP["$1"]}"
fi

CheckAndGetClangInstallDir

# Checking for installed dependencies
printf "\\nChecking for installed dependencies...\\n"
for (( i=0; i<${#DEP_ARRAY[@]}; i++ )); do
	pkg=$( dpkg -s "${DEP_ARRAY[$i]}" 2>/dev/null | grep Status | tr -s ' ' | cut -d\  -f4 )
	if [ -z "$pkg" ]; then
		DEP=$DEP" ${DEP_ARRAY[$i]} "
		DISPLAY="${DISPLAY}${COUNT}. ${DEP_ARRAY[$i]}\\n"
		printf " - Package %s${bldred} NOT${txtrst} found!\\n" "${DEP_ARRAY[$i]}"
		(( COUNT++ ))
	else
		printf " - Package %s found.\\n" "${DEP_ARRAY[$i]}"
		continue
	fi
done

if [ "${COUNT}" -gt 1 ]; then
	printf "\\nThe following dependencies are required to install fibjs:\\n"
	printf "\\n${DISPLAY}\\n" 
	read -p "Do you wish to install these packages? (y/n) " ANSWER;
	case $ANSWER in
		[Yy]* )
			UpdateAPTSource 
			if ! apt-get -y install ${DEP}; then
				printf " - APT dependency failed.\\n"
				exit 1
			else
				printf " - APT dependencies installed successfully.\\n"
			fi
		;;
		[Nn]* ) echo "User aborting installation of required dependencies, Exiting now."; exit 1;;
		* ) echo "Please type 'y' for yes or 'n' for no."; exit 1;;
	esac
else 
	printf " - No required APT dependencies to install.\\n"
	NO_UPDATE=true
fi 

if [ $1 == "i386" ]; then
	if [ -d /usr/include/i386-linux-gnu ]; then
		di=$(ls -lF  /usr/include/i386-linux-gnu | awk '{print $NF}')
		if [ "${di}" != "x86_64-linux-gnu/" ]; then
			ln -s x86_64-linux-gnu /usr/include/i386-linux-gnu
			if [ $? == 0 ]; then
				printf " - link i386-linux-gnu successfully \\n"
			else
				printf " - link i386-linux-gnu failed \\n"
				exit 1
			fi
		fi
	else
		ln -s x86_64-linux-gnu /usr/include/i386-linux-gnu
		if [ $? == 0 ]; then
			printf " - link i386-linux-gnu successfully \\n"
		else
			printf " - link i386-linux-gnu failed \\n"
			exit 1
		fi
	fi

	if [ -d /usr/include/x86_64-linux-gnux32 ]; then
		di=$(ls -lF  /usr/include/x86_64-linux-gnux32 | awk '{print $NF}')
		if [ "${di}" != "x86_64-linux-gnu/" ]; then
			ln -s x86_64-linux-gnu /usr/include/x86_64-linux-gnux32
			if [ $? == 0 ]; then
				printf " - link x86_64-linux-gnux32 successfully \\n"
			else
				printf " - link x86_64-linux-gnux32 failed \\n"
				exit 1
			fi
		fi
	else
		ln -s x86_64-linux-gnu /usr/include/x86_64-linux-gnux32
		if [ $? == 0 ]; then
			printf " - link x86_64-linux-gnux32 successfully \\n"
		else
			printf " - link x86_64-linux-gnux32 failed \\n"
			exit 1
		fi
	fi
fi

# No required APT dependencies to install. not i386 arch, not amd64 arch
if [ ! -z "${CROSS_ARRY_MAP[$1]}" ]; then
	printf " - Checking cross-compile dictionary. \\n"
	rm -f /usr/bin/${CROSS_ARRY_MAP[$1]:6}-gcc
	rm -f /usr/bin/${CROSS_ARRY_MAP[$1]:6}-g++
	ln -s ${CROSS_ARRY_MAP[$1]:6}-gcc-5 /usr/bin/${CROSS_ARRY_MAP[$1]:6}-gcc
	ln -s ${CROSS_ARRY_MAP[$1]:6}-g++-5 /usr/bin/${CROSS_ARRY_MAP[$1]:6}-g++
fi