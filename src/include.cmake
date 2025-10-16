# The MIT License (MIT)
#
# Copyright (c) 2025 Scott Aron Bloom
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

set(qtproject_SRCS
    MathJaxQt6.cpp
    MathJaxQt6Widget.cpp
    MathJaxQt6GroupBox.cpp
)

set(qtproject_H
    ../include/MathJaxQt6.h
    ../include/private/MathJaxQt6_private.h
    ../include/MathJaxQt6Widget.h
    ../include/MathJaxQt6GroupBox.h
)

set(project_H
    ../include/MathJaxQt6Export.h
)

set(qtproject_UIS
)

set(qtproject_QRC
)

SET( project_pub_DEPS
    Qt6::Core
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::SvgWidgets
    SABUtils
    ${project_pub_DEPS}
)

