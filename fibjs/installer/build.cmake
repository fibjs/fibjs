cmake_minimum_required(VERSION 3.5)

if(NOT "${CLEAN_BUILD}" STREQUAL "true")
    include(../../vender/build_tools/cmake-scripts/get_env.cmake)

    set(WORK_ROOT $ENV{WORK_ROOT})
    if(NOT WORK_ROOT)
        set(WORK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
    endif()

    set(BIN_PATH "${WORK_ROOT}/bin/${DIST_DIRNAME}")

    if(${BUILD_TYPE} STREQUAL "release")
        if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
            execute_process(WORKING_DIRECTORY ${BIN_PATH} COMMAND cscript ${CMAKE_CURRENT_SOURCE_DIR}/gen_install.vbs)
            build("${CMAKE_CURRENT_SOURCE_DIR}" "${WORK_ROOT}" "installer")

            message("")
            execute_process(COMMAND dumpbin /dependents ${BIN_PATH}/fibjs.exe)
            message("")
        else()
            execute_process(WORKING_DIRECTORY ${BIN_PATH} COMMAND ${CMAKE_COMMAND} -E tar -zcf fibjs.tar.gz fibjs)
            message("[100%] Built target fibjs.tar.gz")

            execute_process(OUTPUT_FILE ${BIN_PATH}/installer.sh
                COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/src/installer.txt ${BIN_PATH}/fibjs.tar.gz)
            execute_process(COMMAND chmod 777 ${BIN_PATH}/installer.sh)
            message("[100%] Built target install.sh")

            message("")
            if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
                message("==== GLIBC ====")
                execute_process(COMMAND objdump ${BIN_PATH}/fibjs -p COMMAND grep GLIBCX*_[0-9.]* -o COMMAND sort COMMAND uniq)
            elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
                execute_process(COMMAND otool -L ${BIN_PATH}/fibjs)
            endif()
            message("")
        endif()
    endif()
endif()
