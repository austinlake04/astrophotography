cmake_minimum_required(VERSION 3.23.5 FATAL_ERROR)

find_package(${META_PROJECT_NAME} REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../")

set(target test)
message(STATUS "Test ${target}")

set(sources
    test.cpp
)

add_executable(${target}
    ${sources}
)

add_executable(${META_PROJECT_NAME}::${target} ALIAS ${target})

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)

target_include_directories(${target}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${PROJECT_BINARY_DIR}/source/include
)


target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${META_PROJECT_NAME}
)

target_compile_definitions(${target}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
)


target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)
