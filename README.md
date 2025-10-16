
# Overview

*T42-MathJaxQt6* is a `cmake` based `Qt 6` library that allows you to easily add
math typesetting into any [Qt] application.

It is based on the work previously done (and forked from) 
https://github.com/nathancarter/qtmathjax.

[MathJax] is the de facto standard for mathematical typesetting on
the web, using a JavaScript-based typesetting engine.
*T42-MathJaxQt6* imports the latest version (4.0) from https://github.com/mathjax/MathJax
as a submodule.  **Note** currently the internal mechanism uses
the public https://cdn.jsdelivr.net/npm/mathjax@4 *MathJax* system rather
than using the local version.  This is necessary as there appears to be a 
bug in the chromium or node.js back-end in Qt.

The library loads the *MathJax* library via a hidden `QWebEngineView` and provides
a single API call for converting TeX code into an SVG file which can be used in any
image viewer, including [QSvgWidget].  There are both synchronous and asynchronous API calls.
You make the call via a method (or signal) and are notified when rendering is completed.

# Obtaining

Just clone this git repo, or use it as a [submodule] in your
project.  In fact, it uses [MathJax] as a [submodule] itself.

It also has a dependency on [T42-CMakeUtils] a project which contains 
a ton of [CMake] utilities.

# Usage (Asynchronous)

To use *T42-QtMathJax*, follow these steps.

1. In your top level `CMakeLists.txt` file import *T42-QtMathJax* by adding the following
   line.
   ```
   add_submodule(/path/to/T42-MathJaxQt6)
   
   ```
   This step will also update your C++ include path
2. The easiest way to use `MathJaxQt6` is to use the MathJaxQt6 GroupBox or Widget, `NTowel42::CMathJaxQt6GroupBox` or
    `NTowel42::CMathJaxQt6Widget` (the group box simply wraps the widget, with the same api);
    
    Simply construct one, like any other widget, and set the formula and you you are done.  
    
    This will create a `QScrollArea` with a `QSvgWidget` inside.
    
    The widget support scroll wheel zooming.
    
    There are a number of properties defined below to drive how the widget scales and zooms in.

3. In the source file where you need to use it, include the one
   class you need, `NTowel42::CMathJaxQt6GroupBox`, via `#include "MathJaxQt6GroupBox.h"` or `#include "MathJaxQt6Widget.h"`.
4. Enable debugging as necessary.  *T42-MathJaxQt6* uses the modern [QLoggingCategory] system with the following categories
    - MathJaxQt6 - Informational level data
    - MathJaxQt6.Console - Text/messages from the JavaScript console
    - MathJaxQt6.QRC - Reports on all the files that are contained in the QRC file
    - MathJaxQt6.Debug - Low level debug messages
5. Create an instance of the class, connect to the signals you want
    and call the `setFormula( const QString & texCode )` method.


```
NTowel42::CMathJaxQt6GroupBox mw;  
connect( &engine, &NTowel42::CMathJaxQt6::sigErrorMessage,
    [=]( const QString & msg )
    {
        qDebug() << msg;
    } );
mw->setFormula( "ax^2+bx+c=0" );
```

If you want more control, you can instead follow these instructions.  But you will be responsible for all sizing issues of the 
   SVG.  In general MathJax creates very large canvases, so you will likely have to zoom in.  The 
1. In the source file where you need to use it, include the one
   class you need, `NTowel42::CMathJaxQt6`, via `#include "MathJaxQt6.h"`.
2. Enable debugging as necessary as above
3. Create an instance of the class, connect to the signals you need (`CMathJaxQt6::sigSVGRendered is required`)
    and call the `renderSVG( const QString & texCode )` method.

```
NTowel42::CMathJaxQt6 engine;  
QByteArray svgData;
connect( &engine, &NTowel42::CMathJaxQt6::sigErrorMessage,
    [=]( const QString & msg )
    {
        qDebug() << msg;
    } );
connect( &engine &NTowel42::CMathJaxQt6::sigSVGRendered,
    [=]( const QString & formula, const QByteArray & svg )
    {
        svgData = svg;
    } );
    
auto svg = renderer.renderSVG( "ax^2+bx+c=0" );
```

**NOTE** this call is asynchonous, The call to `renderSVG` will return immediately.  The variable 
`svgData` must still be in scope when the call finishes.  It is recommended that in `QGuiApplication` 
or `QApplication` based executables, that the signal is connected to a class that can handle the parameter more effectively.

You can now render that SVG in any of
`the ways Qt provides`[QtSVG], or save it to a file.  Example:

```
myQSvgWidget->load( svgCode );
```

You can call call for multiple renderings if your system requires it, the SVG data will be returned
in order of their requested rendering.

If you use the MathJaxQt6 Widget, all this is done for you.

# Usage (Synchronous)

There is also an API for synchronous rendering.  You call 
`renderer.renderSVG( const QString &texCode, const std::function< void( const std::optional< QByteArray > & svg ) > & function )`
and the blocking call will execute and when finished call the lambda sent in.

If there was an error, svg will not have a value.

**NOTE** The signals from the engine `sigErrorMessage` and `sigSVGRendered` will not be emitted in the synchronous rendering.

# Dependencies
The following [Qt] Libraries must be added as dependencies to your application:
- [QtCore]
- [QtWidgets]
- [QtWebEngineWidgets]

**NOTE** A project is being worked on to remove the WebEngineWidgets dependency and use a node.js engine from
the [QML] [Qt] module.

# Widget Settings
The Widget and GroupBox both have a number of settings, most are accessable via the Qt property system

Q_PROPERTY( bool autoSizeToParentWidth READ autoSizeToParentWidth WRITE slotSetAutoSizeToParentWidth )
The widget can automatically set the scale factor based on the parents width.

When true, after a the SVG is rendered, the scale is set so it cleanly fits into the size of the Widget.

The SVG widget will maintain a fixed size, if the widget is larger than the parent, scroll bars will be enabled.

The default is false.

Q_PROPERTY( bool autoUpdateMinimumParentHeight READ autoUpdateMinimumParentHeight WRITE slotSetAutoUpdateMinimumParentHeight )
When true, after the a scale change, the parents minimum height will be set to a value to be able to show the full formula.
The default is false.

Q_PROPERTY( int pixelsPerFormula READ numPixelsPerFormula WRITE slotSetNumPixelsPerFormula )
The approximate number of pixels you wish each line of your formula to have.  Using \newline you can render multiple lines.
The default is 200 pixels

Q_PROPERTY( double minScale READ minScale WRITE slotSetMinScale )
The minimum scaling factor you wish to allow, the default is 0.0125.
The very small value is necessary due to MathJax creating very large canvases.

This number is used during auto scaling, if the "ideal scale" for the width is below the minimum, then the minimum is used.
This will cause the scroll bars to be enabled to be able to see the full set of formulas.

Q_PROPERTY( double maxScale READ maxScale WRITE slotSetMaxScale )
The maximum scaling factor you wish to allow, the default is 10.  This is only used in reaction to wheel events.

Q_PROPERTY( double scale READ scale WRITE slotSetScale )
The current scale used to show the SVG.  Any change to the scale emits the signal 
The default is 1.0
```
sigScaleChanged( double scale )
```

Q_PROPERTY( bool hideEmptyOrInvalid READ hideEmptyOrInvalid WRITE slotHideEmptyOrInvalid )
When true, if the svg is invalid or not set, the widget will hide itself.

# Subordination
It is not uncommon for your application to need two formulas shown side by side. One might be the formula
without values, while the other may fill in values from the user.

The widget to show the values, can be made subordinate the other.  When the formulas are 100% the same, the subordinate
widget hides itself. 

#The Widget engine
The Widget and GroupBox both have their own `NTowel42::CMathJaxQt6` engine.  However, you can override the engine with
your own. 

If you are using multiple Widgets, there is no reason to have multiple Engines.
```
widget2->setEngine( widget1->engine() );
```

Will handle your issues.  The benefit here is they will share the same cache of formulas to SVG.

# Examples

The repository comes with a 2 sample applications in the [examples] subfolder.

The first [example-cli] is a command line application that uses the synchronous 
call to produce a svg file based on the command line parameters.

The second [example-gui] is a [QMainWindow] based application that uses the asynchonous API to
render the SVG into a widget on the dialog.  

If using Qt 6, you should be able to compile and run that app
without any changes needed.  It looks like this:

![Screenshot](./screenshot.png)

# License

This project is released under the [MIT] licensing.
It is my understanding that this is compatible with the
[Apache license] under which [MathJax] is released.

[T42-CMakeUtils]:     https://github.com/towel42-com/T42-CMakeUtils
[Qt]:                 http://qt-project.org
[QtCore]:             https://doc.qt.io/qt-6/qtcore-index.html
[QtWidgets]:          https://doc.qt.io/qt-6/qtwidgets-index.html
[QtWebEngineWidgets]: https://doc.qt.io/qt-6/qtwebenginewidgets-index.html
[QtSVG]:              https://doc.qt.io/qt-6/qtsvg-index.html
[QSvgWidget]:         https://doc.qt.io/qt-6/qsvgwidget.html
[QDialog]:            https://doc.qt.io/qt-6/qdialog.html
[CMake]: http://cmake.org
[MathJax]: http://mathjax.org
[submodule]: http://schacon.github.io/git/user-manual.html#submodules
[examples]: ./examples/
[example-cli]: ./examples/cli
[example-gui]: ./examples/gui
[MIT]: https://opensource.org/license/mit
[Apache license]: https://github.com/mathjax/MathJax/blob/master/LICENSE

