cmake_minimum_required(VERSION 3.20.3)

set(CMAKE_CXX_STANDARD 17)
project(simple_time_lib VERSION 0.1)
enable_testing()

set(CONANBUILD_INFO_FILE "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
set(CONANBUILD_INFO_MULTICONFIGURATION_FILE "${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake")
set(CONAN_OPTIONS "TARGETS;KEEP_RPATHS")

if (EXISTS "${CONANBUILD_INFO_MULTICONFIGURATION_FILE}")
	include(${CONANBUILD_INFO_MULTICONFIGURATION_FILE})
	set(CONAN_CMAKE_MULTI ON)
elseif (EXISTS "${CONANBUILD_INFO_FILE}")
	include(${CONANBUILD_INFO_FILE})
	list(APPEND CONAN_OPTIONS "NO_OUTPUT_DIRS")
	set(CONAN_CMAKE_MULTI OFF)
else()
	message(FATAL_ERROR "Cannot load any conanbuildinfo file.")
endif()

conan_basic_setup(${CONAN_OPTIONS})

if(APPLE)
	add_definitions("-Wall")
	add_definitions("-Wextra")
	add_definitions("-Wpedantic")
endif()

add_library(${PROJECT_NAME} STATIC src/restriction_times.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER src/restriction_times.hpp)

add_executable(test_restrictions test/test_restrictions.cpp)
target_include_directories(test_restrictions PRIVATE src/)
target_link_libraries(test_restrictions CONAN_PKG::gtest
                                        ${PROJECT_NAME}
    )

include(GoogleTest)
gtest_discover_tests(test_restrictions)
