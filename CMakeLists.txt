cmake_minimum_required( VERSION 3.31 )
project( T42-Qt6MathJax-root )

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui WebEngineWidgets )
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/T42-CMakeUtils/Modules ${CMAKE_MODULE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory( src )
add_subdirectory( examples )

set_target_properties( Qt6MathJax_other_files PROPERTIES FOLDER CMakePredefinedTargets/QtAutoProjects )
set_target_properties( Qt6MathJax_resources_1 PROPERTIES FOLDER CMakePredefinedTargets/QtAutoProjects )
set_target_properties( Qt6MathJax_resources_2 PROPERTIES FOLDER CMakePredefinedTargets/QtAutoProjects )
set_target_properties( Qt6MathJax-CLIExample_qmlimportscan PROPERTIES FOLDER CMakePredefinedTargets/QtAutoProjects )
set_target_properties( Qt6MathJax-GUIExample_qmlimportscan PROPERTIES FOLDER CMakePredefinedTargets/QtAutoProjects )

