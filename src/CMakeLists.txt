project(astrosight)
cmake_minimum_required(VERSION 3.23.5 FATAL_ERROR)

find_package(${META_PROJECT_NAME} REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../")

set(target gui)
set(sources
    qml.qrc 
    gui.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Qt6::Core Qt6::Gui Qt6::Qml)

qt_add_executable(${target}
    ${sources}
)

target_link_libraries(${target} PRIVATE Qt6::Core Qt6::Gui Qt6::Qml)
# target_link_libraries(${target} Qt6::Core)
# target_link_libraries(${target} Qt6::Gui)
# target_link_libraries(${target} Qt6::Qml)

qt_import_plugins(${target}
    INCLUDE Qt6::QCocoaIntegrationPlugin
)

set(DEFULAT_COMPILE_OPTIONS
    -Wall -Wextra -Wpedantic
)
target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)

set(DEBUG_COMPILE_OPTIONS
    -O0 -Werror
)
target_compile_options(${target}
    PRIVATE
    "$<$<CONFIG:DEBUG>:${DEBUG_COMPILE_OPTIONS}>"
)

set(DEBUG_COMPILE_OPTIONS
    -O3 -Wno-dev
)
target_compile_options(${target}
    PRIVATE
    "$<$<CONFIG:RELEASE>:${RELEASE_COMPILE_OPTIONS}>"
)
