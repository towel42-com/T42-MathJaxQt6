# The MIT License (MIT)
#
# Copyright (c) 2020-2021 Scott Aron Bloom
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sub-license, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
FUNCTION( CreateQRC DIRECTORY OUTFILE)
    get_filename_component( REL_TO_DIR ${OUTFILE} DIRECTORY )
	MESSAGE( STATUS "Creating ${OUTFILE} QRC for directory ${DIRECTORY}" )
	MESSAGE( STATUS "    Paths with be relative to ${REL_TO_DIR}" )
	
    file( GLOB_RECURSE  QRC_FILES RELATIVE ${REL_TO_DIR} CONFIGURE_DEPENDS ${DIRECTORY}/* Qt6MathJax.html)

    LIST( REMOVE_DUPLICATES QRC_FILES )
    LIST( FILTER QRC_FILES EXCLUDE REGEX ".*\.git.*" )
    LIST( FILTER QRC_FILES EXCLUDE REGEX ".*DS_Store.*" )

    LIST( LENGTH QRC_FILES LENGTH)
    message( STATUS "    ${LENGTH} QRC resource files found.")
    
    FILE( WRITE ${OUTFILE}  "<RCC>\n" )
    FILE( APPEND ${OUTFILE} "    <qresource prefix=\"/Qt6MathJax\">\n" )
    
    foreach(item IN LISTS QRC_FILES)
        SET( _ALIAS_ITEM ${item} )
        STRING( REPLACE "../" "" _ALIAS_ITEM ${_ALIAS_ITEM} )
        STRING( REPLACE "T42-Qt6MathJax/" "" _ALIAS_ITEM ${_ALIAS_ITEM} )
        
        FILE( APPEND ${OUTFILE} "        <file alias=\"${_ALIAS_ITEM}\">${item}</file>\n" )
    endforeach()
    FILE( APPEND ${OUTFILE} "    </qresource>\n" )
    FILE( APPEND ${OUTFILE} "</RCC>\n" )
    
	MESSAGE( STATUS "Finished generating QRC file ${OUTFILE} for directory ${DIRECTORY}" )
	
ENDFUNCTION()

