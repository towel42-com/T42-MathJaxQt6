cmake_minimum_required( VERSION 3.31 )
project( T42Qt6MathJax )

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui WebEngineWidgets )

include( include.cmake )
include( ${CMAKE_CURRENT_SOURCE_DIR}/../T42-CMakeUtils/QtProject.cmake )
find_package(Deploy REQUIRED)
add_definitions( -DT42QT6MATHJAX_EXPORTS )

add_library(
    ${PROJECT_NAME} SHARED
    ${_PROJECT_DEPENDENCIES} 
    ../assets/Qt6MathJax.js
    ../assets/Qt6MathJaxInit.js
    ../assets/Qt6MathJax.html
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

qt6_add_resources(${PROJECT_NAME} "Qt6MathJax"
    PREFIX
        "/Qt6MathJax"
    BASE
        "${CMAKE_CURRENT_SOURCE_DIR}/../assets"
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../assets/Qt6MathJax.html
        ${CMAKE_CURRENT_SOURCE_DIR}/../assets/Qt6MathJax.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../assets/Qt6MathJaxInit.js
)

target_link_libraries( ${PROJECT_NAME}
      PUBLIC
          ${project_pub_DEPS}
      PRIVATE
          ${project_pri_DEPS}
)


set_target_properties( ${PROJECT_NAME} PROPERTIES 
        FOLDER T42-Qt6MathJax
        DEBUG_POSTFIX d
)

DeployQt( ${PROJECT_NAME} . )
DeploySystem( ${PROJECT_NAME} . INSTALL_ONLY 1 )

INSTALL( FILES $<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.dll DESTINATION . )
INSTALL( FILES $<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.pdb DESTINATION . CONFIGURATIONS Debug RelWithDebInfo )

