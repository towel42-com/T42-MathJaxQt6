project( MathFormulaEngine )

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui WebEngineWidgets )

include( include.cmake )
include( ${CMAKE_SOURCE_DIR}/SABUtils/QtProject.cmake )
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules ${CMAKE_MODULE_PATH})

find_package( CreateQRC REQUIRED )
CreateQRC( ${CMAKE_CURRENT_SOURCE_DIR}/MathJax ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.qrc )


add_library(
    ${PROJECT_NAME} STATIC
    ${_PROJECT_DEPENDENCIES} 
)
target_link_libraries( ${PROJECT_NAME}
      PUBLIC
          ${project_pub_DEPS}
      PRIVATE
          ${project_pri_DEPS}
)
