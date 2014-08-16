if(${OS} STREQUAL "Linux")
	if(zlib_found)
		message(STATUS "*** Use system wide installed zlib. ***")
		set(flags "${flags} ${zlib_cflags}")
		set(link_flags "${link_flags} ${zlib_ldflags}")

		set(zlib_src_list
			${PROJECT_SOURCE_DIR}/../src/zlib/compress.c
			${PROJECT_SOURCE_DIR}/../src/zlib/inftrees.c
			${PROJECT_SOURCE_DIR}/../src/zlib/uncompr.c
			${PROJECT_SOURCE_DIR}/../src/zlib/deflate.c
			${PROJECT_SOURCE_DIR}/../src/zlib/zutil.c
			${PROJECT_SOURCE_DIR}/../src/zlib/trees.c
			${PROJECT_SOURCE_DIR}/../src/zlib/inflate.c
			${PROJECT_SOURCE_DIR}/../src/zlib/inffast.c
			${PROJECT_SOURCE_DIR}/../src/zlib/adler32.c
			${PROJECT_SOURCE_DIR}/../src/zlib/crc32.c
		)
		foreach(zlib_src ${zlib_src_list})
			list(REMOVE_ITEM src_list ${zlib_src})
		endforeach(zlib_src ${zlib_src_list})
	else()
		message(STATUS "*** Use our provided zlib. ***")
		include_directories(BEFORE SYSTEM "${PROJECT_SOURCE_DIR}/../include/zlib")
	endif()
else()
	message(STATUS "*** Use our provided zlib. ***")
	include_directories(BEFORE SYSTEM "${PROJECT_SOURCE_DIR}/../include/zlib")
endif()
