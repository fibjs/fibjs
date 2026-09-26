cmake_minimum_required(VERSION 3.10)

set(name "installer")
project(${name})

include(${CMAKE_CURRENT_LIST_DIR}/../../vender/build_tools/cmake/option.cmake)

# ----------------------------------------------------------------------------
# Release packaging (replaces the historical installer/build.cmake).
#
# The packaging is part of the default build, exactly like before:
#
#   Windows  installer.exe (gen_install.vbs) + fibjs.cab
#   Unix     fibjs.tar.gz + installer.sh, see package.sh
#
# It can also be run alone: cmake --build <build dir> --target package
# ----------------------------------------------------------------------------
if("${BUILD_OS}" STREQUAL "Windows")
    add_executable(${name} ${src_list})

    # BIN_PATH is set by option_libs.cmake (from BT_BIN_DIR), so the installer
    # lands in bin/<OS>_<ARCH>_<TYPE> like every other artifact.
    include_directories("${BIN_PATH}")

    setup_result_library(${name})

    add_custom_target(package ALL
        COMMAND cscript ${CMAKE_CURRENT_SOURCE_DIR}/gen_install.vbs
        COMMAND dumpbin /dependents ${BIN_PATH}/fibjs.exe
        WORKING_DIRECTORY "${BIN_PATH}"
        DEPENDS ${name} fibjs
        VERBATIM)
else()
    # Unix keeps no installer binary (the historical build only packaged there).
    # The archives are real build outputs: they are produced from the fibjs
    # binary, rebuilt only when needed, and part of the default build.
    set(package_outputs
        "${BIN_PATH}/fibjs.tar.gz"
        "${BIN_PATH}/installer.sh")

    add_custom_command(OUTPUT ${package_outputs}
        COMMAND ${CMAKE_COMMAND} -E tar -zcf fibjs.tar.gz fibjs
        COMMAND sh -c "cat '${CMAKE_CURRENT_SOURCE_DIR}/src/installer.txt' fibjs.tar.gz > installer.sh; chmod 777 installer.sh"
        WORKING_DIRECTORY "${BIN_PATH}"
        DEPENDS fibjs
        COMMENT "Packaging fibjs.tar.gz and installer.sh"
        VERBATIM)

    add_custom_target(installer ALL DEPENDS ${package_outputs})

    # `package` is the cross-platform name of this step (on Windows it drives
    # gen_install.vbs); on Unix it is an alias of the packaging above.
    add_custom_target(package DEPENDS installer)

    # Runtime library requirements of the binary (informational, like before).
    # The report prints its own separator and the list of packaged artifacts
    # (see package.sh), so the packaging stays visible even when it is up to
    # date and nothing had to be regenerated.
    add_custom_command(TARGET installer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E env BIN_PATH=${BIN_PATH} bash ${CMAKE_CURRENT_SOURCE_DIR}/package.sh
        VERBATIM)
endif()