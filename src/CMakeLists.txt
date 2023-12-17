cmake_minimum_required(VERSION 3.23.5 FATAL_ERROR)

find_package(${META_PROJECT_NAME} REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../")

set(target astrosight)
message(STATUS "Test ${target}")

add_executable(${target}
    main.qml
    gui.cpp
)

find_package(argparse REQUIRED)
find_package(libraw REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Qt6 REQUIRED)

qt_standard_project_setup()

target_link_libraries(${target} ${argparse_LIBS})
target_link_libraries(${target} ${libraw_LIBS})
target_link_libraries(${target} ${OpenCV_LIBS})
target_link_libraries(${target} ${Qt6_LIBS})

target_include_directories(${target} PRIVATE ${argparse_INCLUDE_DIRS})
target_include_directories(${target} PRIVATE ${libraw_INCLUDE_DIRS})
target_include_directories(${target} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_include_directories(${target} PRIVATE ${Qt6_INCLUDE_DIRS} )

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)

target_include_directories(${target}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${PROJECT_BINARY_DIR}/include
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

set(DEFULAT_COMPILE_OPTIONS
	-Wall -Wextra -Wpedantic
)
target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)

set(DEBUG_COMPILE_OPTIONS
    -O0
)
target_compile_options(${target}
    PRIVATE
    "$<$<CONFIG:DEBUG>:${DEBUG_COMPILE_OPTIONS}>"
)

set(DEBUG_COMPILE_OPTIONS
    -O3 -Werror
)
target_compile_options(${target}
    PRIVATE
    "$<$<CONFIG:RELEASE>:${RELEASE_COMPILE_OPTIONS}>"
)

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)
