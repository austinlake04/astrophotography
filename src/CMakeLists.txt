cmake_minimum_required(VERSION 3.23.5 FATAL_ERROR)

find_package(${META_PROJECT_NAME} REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../")

set(target gui)
set(sources
	main.qml
	gui.cpp
)

find_package(Qt5 REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${Qt5_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIR})

add_executable(${target}
	${sources}
)

target_link_libraries(${target} ${OpenCV_LIBS})
target_link_libraries(${target} Qt5::Core)
target_link_libraries(${target} Qt5::Gui)
target_link_libraries(${target} Qt5::Qml)

qt_import_plugins(${target}
    INCLUDE Qt5::QCocoaIntegrationPlugin
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
