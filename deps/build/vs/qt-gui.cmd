setlocal
set MOC=%MOC% -DQ_OS_WIN -I../../include/qt -I../../include/qt/QtCore -I../../include/qt/QtWidgets -I../../include/qt/QtGui
set RCC=%RCC%
set DIRIN=../../include/qt/QtGui
set DIRSRC=../../src/qt-gui
set DIROUT=../../src/qt-gui/.moc
set DIROUTRCC=../../src/qt-gui/.rcc

echo MOC for 'accessible'
CALL :moch qaccessible
CALL :moch qaccessiblebridge
CALL :mocs qaccessiblecache_p private
CALL :moch qaccessibleplugin

echo MOC for 'image'
CALL :moch qiconengineplugin
CALL :moch qimage
CALL :moch qimageiohandler
CALL :moch qmovie
CALL :moch qpictureformatplugin
CALL :mocc qpixmapcache image

echo MOC for 'itemmodels'
CALL :moch qstandarditemmodel

echo MOC for 'kernel'
CALL :moch qclipboard
CALL :mocs qdnd_p private
CALL :moch qdrag
CALL :moch qevent
CALL :moch qgenericplugin
CALL :moch qguiapplication
CALL :mocs qinputcontrol_p private
CALL :mocs qinputdevicemanager_p private
CALL :moch qinputmethod
CALL :mocs qkeymapper_p private
CALL :moch qkeysequence
CALL :moch qoffscreensurface
CALL :moch qopenglcontext
CALL :moch qopenglwindow
CALL :moch qpaintdevicewindow
CALL :moch qpalette
CALL :mocs qplatformbackingstore qpa
CALL :mocs qplatformdialoghelper qpa
CALL :mocs qplatformgraphicsbuffer qpa
CALL :mocs qplatforminputcontext qpa
CALL :mocs qplatforminputcontextplugin_p qpa
CALL :mocs qplatformintegrationplugin qpa
CALL :mocs qplatformmenu qpa
CALL :mocs qplatformnativeinterface qpa
CALL :mocs qplatformsharedgraphicscache qpa
CALL :mocs qplatformsystemtrayicon qpa
CALL :mocs qplatformthemeplugin qpa
CALL :moch qscreen
CALL :moch qsessionmanager
CALL :moch qsurface
CALL :moch qsurfaceformat
CALL :moch qtouchdevice
CALL :moch qwindow

echo MOC for 'opengl'
CALL :moch qopengldebug
CALL :mocs qopenglengineshadermanager_p private
CALL :moch qopenglshaderprogram
CALL :moch qopengltexture
CALL :moch qopenglvertexarrayobject

echo MOC for 'painting'
CALL :moch qbrush
CALL :moch qpainter

echo MOC for 'text'
CALL :moch qabstracttextdocumentlayout
CALL :moch qfont
CALL :moch qfontdatabase
CALL :mocs qinternalmimedata_p private
CALL :moch qsyntaxhighlighter
CALL :moch qtextdocument
CALL :mocs qtextdocumentlayout_p private
CALL :moch qtextformat
CALL :mocs qtextimagehandler_p private
CALL :moch qtextlist
CALL :moch qtextobject
CALL :moch qtexttable

echo MOC for 'util'
CALL :mocc qdesktopservices util
CALL :moch qpdfwriter
CALL :moch qvalidator

echo MOC for 'unsorted'
CALL :moch qrasterwindow
CALL :mocs qshaderlanguage_p private
CALL :mocs qshapedpixmapdndwindow_p private
CALL :moch qstylehints

echo RCC
%RCC% --name qpdf --output %DIROUTRCC%/qrc_qpdf.cpp %DIRSRC%/painting/qpdf.qrc
%RCC% --name qmake_webgradients --output %DIROUTRCC%/qrc_qmake_webgradients.cpp %DIRSRC%/painting/qmake_webgradients.qrc

goto :eof

:moch
%MOC% %DIRIN%/%1.h -o %DIROUT%/moc_%1.cpp
goto:eof

:mocs
%MOC% %DIRIN%/%2/%1.h -o %DIROUT%/moc_%1.cpp
goto:eof

:mocc
%MOC% %DIRSRC%/%2/%1.cpp -o %DIROUT%/%1.moc
goto:eof
