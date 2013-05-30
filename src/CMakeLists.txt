
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

find_package(Boost 1.53 COMPONENTS regex REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

include_directories(.)

add_subdirectory(keen)

if (NOT DEFINED SKIP_TESTS)
	add_subdirectory(test)
endif()

