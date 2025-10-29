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

function(dump_list list_name)
    LIST( LENGTH ${list_name} _len)
    message(STATUS "Contents of list '${list_name}': Length: ${_len}")
    if("${${list_name}}" STREQUAL "")
        message(STATUS "  (List is empty)")
    else()
        set( fileNum 0 )
        set( max 10 )
        math(EXPR min "${_len} - ${max}" )
        
        foreach(item IN LISTS ${list_name})
            math(EXPR fileNum "${fileNum} + 1" OUTPUT_FORMAT DECIMAL)
            if ( ( fileNum LESS max ) OR ( fileNum GREATER_EQUAL min ) )
                message(STATUS "  - ${fileNum} - ${item}")
            endif()
            if ( ( fileNum EQUAL max ) AND ( _len GREATER max ) )
                message( STATUS "..............." )
            endif()
        endforeach()
    endif()
endfunction()

file( GLOB_RECURSE QRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../mathjax/bundle/* )

foreach( srcFile ${QRC_FILES} )
    cmake_path(RELATIVE_PATH srcFile BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../mathjax/bundle/ OUTPUT_VARIABLE relFile)
    #MESSAGE( STATUS "File: ${srcFile} Alias: ${relFile}" )
    set_source_files_properties( 
        ${srcFile}
        PROPERTIES
            QT_RESOURCE_ALIAS ${relFile}
    )
endforeach()

qt6_add_resources(${PROJECT_NAME} "mathjax"
    PREFIX
        "/mathjax"
    FILES
        ${QRC_FILES}
    OUTPUT_TARGETS 
        MATHJAX_OTHER_TARGETS
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
    OUTPUT_TARGETS 
        MATHJAXQT6_OTHER_TARGETS
)

target_include_directories( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/.. )
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

set_target_properties( ${MATHJAX_OTHER_TARGETS} PROPERTIES FOLDER QtAutoProjects )
set_target_properties( ${MATHJAXQT6_OTHER_TARGETS} PROPERTIES FOLDER QtAutoProjects )
