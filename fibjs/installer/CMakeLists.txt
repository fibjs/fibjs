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
    # gen_install.vbs produces fibjs.cab (makecab) from the built fibjs.exe and
    # encodes that cab as js.h, the C array installer.cpp embeds: the header has
    # to exist before the installer is compiled, and it can only be generated
    # after fibjs.exe has been linked.  The generation is a target of its own
    # that the installer waits for; target-level dependencies are honoured by
    # every generator - the Visual Studio generators turn them into project
    # references, so MSBuild builds gen_install before it opens installer.cpp.
    add_custom_target(gen_install
        COMMAND cscript ${CMAKE_CURRENT_SOURCE_DIR}/gen_install.vbs
        WORKING_DIRECTORY "${BIN_PATH}"
        COMMENT "Compressing fibjs.exe and encoding it as js.h"
        VERBATIM)
    add_dependencies(gen_install fibjs)

    add_executable(${name} ${src_list})
    add_dependencies(${name} gen_install)

    # BIN_PATH is set by option_libs.cmake (from BT_BIN_DIR), so the installer
    # lands in bin/<OS>_<ARCH>_<TYPE> like every other artifact; it is also the
    # include directory of the generated js.h.
    include_directories("${BIN_PATH}")

    setup_result_library(${name})

    # The `package` step still triggers the generation (through the installer),
    # which keeps `cmake --build <build dir> --target package` self-contained.
    add_custom_target(package ALL
        COMMAND dumpbin /dependents ${BIN_PATH}/fibjs.exe
        WORKING_DIRECTORY "${BIN_PATH}"
        VERBATIM)
    add_dependencies(package ${name})
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