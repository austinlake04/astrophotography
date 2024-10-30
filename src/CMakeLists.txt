project(astrosight)
cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

find_package(${META_PROJECT_NAME} REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(target gui)
set(sources
    main.qml
    backend.cpp
    main.c
)

add_executable(${target}
    ${sources}
)

target_compile_options(${target} PUBLIC -Wall -Wextra -Wpedantic -O3)
