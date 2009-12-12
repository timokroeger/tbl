cmake_minimum_required(VERSION 2.8)

project(tbl)

if(WIN32)
	ADD_DEFINITIONS(-DTBL_BUILD -DTBL_SHARED)
endif(WIN32)

add_subdirectory(src)
add_subdirectory(test)

enable_testing()