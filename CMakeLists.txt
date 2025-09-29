cmake_minimum_required( VERSION 3.31 )
project( Qt6MathJax-root )

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui WebEngineWidgets )

add_subdirectory( lib )
add_subdirectory( example )

set_target_properties( Qt6MathJax PROPERTIES FOLDER Qt6MathJax )
set_target_properties( Qt6MathJax_other_files PROPERTIES FOLDER CMakePredefinedTargets/QtAutoProjects )
set_target_properties( Qt6MathJax_resources_1 PROPERTIES FOLDER CMakePredefinedTargets/QtAutoProjects )
set_target_properties( Qt6MathJax_resources_2 PROPERTIES FOLDER CMakePredefinedTargets/QtAutoProjects )

set_target_properties( Qt6MathJax-Example_qmlimportscan PROPERTIES FOLDER CMakePredefinedTargets/QtAutoProjects )

