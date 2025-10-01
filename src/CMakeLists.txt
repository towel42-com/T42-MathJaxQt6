cmake_minimum_required( VERSION 3.31 )
project( Qt6MathJax )

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui WebEngineWidgets )

include( include.cmake )
include( ${CMAKE_CURRENT_SOURCE_DIR}/../T42-CMakeUtils/QtProject.cmake )
find_package(Deploy REQUIRED)

add_library(
    ${PROJECT_NAME} STATIC
    ${_PROJECT_DEPENDENCIES} 
    ../Qt6MathJax.js
    ../Qt6MathJaxInit.js
    ../Qt6MathJax.html
)

file( GLOB_RECURSE QRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../MathJax/* )
list(FILTER QRC_FILES EXCLUDE REGEX ".git.*")
qt_add_resources(${PROJECT_NAME} "MathJax"
    PREFIX
        "/"
    BASE
        "MathJax"
    FILES
        ${QRC_FILES}
)

qt_add_resources(${PROJECT_NAME} "${PROJECT_NAME}"
    PREFIX
        "/${PROJECT_NAME}"
    BASE
        "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../Qt6MathJax.html
        ${CMAKE_CURRENT_SOURCE_DIR}/../Qt6MathJax.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../Qt6MathJaxInit.js
)

target_link_libraries( ${PROJECT_NAME}
      PUBLIC
          ${project_pub_DEPS}
      PRIVATE
          ${project_pri_DEPS}
)


set_target_properties( ${PROJECT_NAME} PROPERTIES FOLDER T42-Qt6MathJax-root )

