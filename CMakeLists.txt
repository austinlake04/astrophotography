cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(
	astrosight
	VERSION 0.1.0
	LANGUAGES CXX
	DESCRIPTION "Image processing application for astrophotography."
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(BUILD_TESTING "Build tests" FALSE)
    option(BUILD_SHARED_LIBS "Build shared libraries" FALSE)
	#include(CTest)
    find_package(Doxygen)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found, not building docs")
    endif()
endif()

add_compile_options(-Wall -Wextra -Wpedantic -O3)

add_subdirectory(src)
