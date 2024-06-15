project(astrosight)
cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

find_package(${META_PROJECT_NAME} REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../")

set(target gui)
set(sources
    main.qml
    backend.cpp
    main.cpp
)

find_package(argparse REQUIRED)
find_package(ccfits REQUIRED)
find_package(libraw REQUIRED)
find_package(OpenCV REQUIRED)

set(Qt6_LIBS
    Core
    Gui
    Qml
)
find_package(Qt6 REQUIRED COMPONENTS ${Qt6_LIBS})

option(WITH_QT "Turn QT on" ON)
option(WITH_GTK "Turn GTK off" OFF)
option(WITH_WIN32UI "Turn Win32UI off" OFF)

include_directories(
	${libraw_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
    ${Qt6Core_INCLUDE_DIRS}
    ${Qt6Gui_INCLUDE_DIRS}
	${Qt6Qml_INCLUDE_DIRS}
)

add_executable(${target}
    ${sources}
)

target_link_libraries(${target} argparse::argparse ccfits::ccfits libraw::libraw ${OpenCV_LIBS} Qt6::Core Qt6::Gui Qt6::Qml)
