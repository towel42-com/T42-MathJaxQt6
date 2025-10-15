cmake_minimum_required( VERSION 3.31 )
project( MathJaxQt6 )

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui WebEngineWidgets SvgWidgets )

include( include.cmake )
include( ${CMAKE_CURRENT_SOURCE_DIR}/../T42-CMakeUtils/QtProject.cmake )
find_package(Deploy REQUIRED)
add_definitions( -DT42MATHJAXQT6_EXPORTS )

add_library(
    ${PROJECT_NAME} SHARED
    ${_PROJECT_DEPENDENCIES} 
    ../assets/MathJaxQt6.js
    ../assets/MathJaxQt6Init.js
    ../assets/MathJaxQt6.html
)

file( GLOB_RECURSE QRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../mathjax/* )
list(FILTER QRC_FILES EXCLUDE REGEX ".git.*")
qt6_add_resources(${PROJECT_NAME} "mathjax"
    PREFIX
        "/"
    BASE
        "mathjax"
    FILES
        ${QRC_FILES}
)

qt6_add_resources(${PROJECT_NAME} "MathJaxQt6"
    PREFIX
        "/MathJaxQt6"
    BASE
        "${CMAKE_CURRENT_SOURCE_DIR}/../assets"
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../assets/MathJaxQt6.html
        ${CMAKE_CURRENT_SOURCE_DIR}/../assets/MathJaxQt6.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../assets/MathJaxQt6Init.js
)

target_link_libraries( ${PROJECT_NAME}
      PUBLIC
          ${project_pub_DEPS}
      PRIVATE
          ${project_pri_DEPS}
)


set_target_properties( ${PROJECT_NAME} PROPERTIES 
        FOLDER SharedLibs
        DEBUG_POSTFIX d
)

if ( NOT TOWEL42_NO_DEPLOY_SUBMODULES )
    DeployQt( ${PROJECT_NAME} . )
    DeploySystem( ${PROJECT_NAME} . INSTALL_ONLY 1 )
endif()

INSTALL( FILES $<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.dll DESTINATION . )
INSTALL( FILES $<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.pdb DESTINATION . CONFIGURATIONS Debug RelWithDebInfo )

set_target_properties( MathJaxQt6_other_files PROPERTIES FOLDER QtAutoProjects )
#set_target_properties( Qt6MathJax_resources_1 PROPERTIES FOLDER QtAutoProjects )
#set_target_properties( Qt6MathJax_resources_2 PROPERTIES FOLDER QtAutoProjects )
