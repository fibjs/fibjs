if(${OS} STREQUAL "Linux")
	if(NOT HOST_TARGET_SAME_BIT)
		return()
	endif()

	include(FindPkgConfig)
	pkg_search_module(zlib REQUIRED zlib)

	if(NOT zlib_FOUND)
		return()
	endif()

	if(zlib_VERSION VERSION_LESS "1.2.7")
	  	message(STATUS "zlib version: ${zlib_VERSION} found. but less then 1.2.7 ")
		return()
	endif()

	set(zlib_include_dirs ${zlib_INCLUDE_DIRS})
	string(REPLACE ";" " " zlib_cflags "${zlib_CFLAGS}")
	string(REPLACE ";" " " zlib_ldflags "${zlib_LDFLAGS}")
	set(zlib_found true)

	message(STATUS "zlib found. version: [${zlib_VERSION}].")
	message(STATUS "\tcompile flags: [${zlib_cflags}], link flags: [${zlib_ldflags}]")
	message(STATUS "\tinclude_dirs: [${zlib_INCLUDE_DIRS}], library_dirs: [${zlib_LIBRARY_DIRS}]")
endif()
