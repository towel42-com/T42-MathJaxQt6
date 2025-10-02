cmake_minimum_required( VERSION 3.31 )
project( T42-Qt6MathJax-root )

file( REAL_PATH ~/Dropbox/home/bin/T42-Qt6MathJax CMAKE_INSTALL_PREFIX EXPAND_TILDE)
MESSAGE( STATUS "Installing to ${CMAKE_INSTALL_PREFIX}" )

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/T42-CMakeUtils/Modules ${CMAKE_MODULE_PATH})

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui WebEngineWidgets )
find_package( QtBinDir REQUIRED )
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory( src )
add_subdirectory( examples )

set_target_properties( T42Qt6MathJax_other_files PROPERTIES FOLDER CMakePredefinedTargets/QtAutoProjects )
#set_target_properties( Qt6MathJax_resources_1 PROPERTIES FOLDER CMakePredefinedTargets/QtAutoProjects )
#set_target_properties( Qt6MathJax_resources_2 PROPERTIES FOLDER CMakePredefinedTargets/QtAutoProjects )
set_target_properties( Qt6MathJax-CLIExample_qmlimportscan PROPERTIES FOLDER CMakePredefinedTargets/QtAutoProjects )
set_target_properties( Qt6MathJax-GUIExample_qmlimportscan PROPERTIES FOLDER CMakePredefinedTargets/QtAutoProjects )

