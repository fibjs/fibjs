if(NOT BUILD_TYPE)
	set(BUILD_TYPE "debug")
endif()

string(TOLOWER ${BUILD_TYPE} BUILD_TYPE_TMP)
string(REGEX REPLACE "(debug|release)[0-9]*" "\\1" BUILD_DEBUG_RELEASE ${BUILD_TYPE_TMP})
string(REGEX REPLACE "(debug|release)((32)*)" "\\2" TARGET_OS_BIT ${BUILD_TYPE_TMP})

# check target bitwide
if(TARGET_OS_BIT)
	if(${TARGET_OS_BIT} STREQUAL "32")
		set(TARGET_OS_32BIT true)
		set(TARGET_OS_64BIT false)
	endif()
else()
	set(TARGET_OS_BIT "64")
	set(TARGET_OS_32BIT false)
	set(TARGET_OS_64BIT true)
endif()

# check host & target a same bitwide
if(HOST_OS_64BIT AND TARGET_OS_64BIT)
	set(HOST_TARGET_SAME_BIT true)
elseif(HOST_OS_32BIT AND TARGET_OS_32BIT)
	set(HOST_TARGET_SAME_BIT true)
else()
	set(HOST_TARGET_SAME_BIT false)
endif()

message(STATUS "Host   OS : [${OS} ${HOST_OS_BIT}bit].")
message(STATUS "Target OS : [${OS} ${TARGET_OS_BIT}bit].")
message(STATUS "Host's and Target's bitwide are same? : [${HOST_TARGET_SAME_BIT}].")
