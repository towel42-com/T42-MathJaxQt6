cmake_minimum_required( VERSION 3.31 )
project( MathJaxQt6-root )

file( REAL_PATH ~/Dropbox/home/bin/MathJaxQt6 CMAKE_INSTALL_PREFIX EXPAND_TILDE)
MESSAGE( STATUS "Installing to ${CMAKE_INSTALL_PREFIX}" )

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/T42-CMakeUtils/Modules ${CMAKE_MODULE_PATH})

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui WebEngineWidgets )
find_package( QtBinDir REQUIRED )
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory( src )
add_subdirectory( examples )

set_target_properties( MathJaxQt6-CLIExample_qmlimportscan PROPERTIES FOLDER QtAutoProjects )
set_target_properties( MathJaxQt6-GUIExample_qmlimportscan PROPERTIES FOLDER QtAutoProjects )
