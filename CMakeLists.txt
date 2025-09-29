project( Qt6MathJax )

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui WebEngineWidgets )

include( include.cmake )
include( ${CMAKE_SOURCE_DIR}/SABUtils/QtProject.cmake )

add_library(
    ${PROJECT_NAME} STATIC
    ${_PROJECT_DEPENDENCIES} 
    Qt6MathJax.html
)

file( GLOB_RECURSE QRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/MathJax/* )
list(FILTER QRC_FILES EXCLUDE REGEX ".git.*")
qt_add_resources(${PROJECT_NAME} "MathJax"
    PREFIX
        "/MathJax"
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
        ${CMAKE_CURRENT_SOURCE_DIR}/Qt6MathJax.html
)
LIST( APPEND QRC_FILES   )

target_link_libraries( ${PROJECT_NAME}
      PUBLIC
          ${project_pub_DEPS}
      PRIVATE
          ${project_pri_DEPS}
)

