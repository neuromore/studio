setlocal
set MOC=%MOC% -DQ_OS_WIN -DQT_STATIC -DQT_STATICPLUGIN -I../../include/qt -I../../include/qt/QtCore -I../../include/qt/QtGui -I../../src/qt-platform-windows
set RCC=%RCC%
set UIC=%UIC%
set DIRIN=../../src/qt-platform-windows
set DIRSRC=../../src/qt-platform-windows
set DIROUT=../../src/qt-platform-windows/.moc
set DIROUTUIC=../../src/qt-platform-windows/.uic
set DIROUTRCC=../../src/qt-platform-windows/.rcc

echo MOC for root
CALL :mocc main
CALL :mocc qwindowsdialoghelpers
CALL :moch qwindowsgdinativeinterface
CALL :moch qwindowsinputcontext
CALL :moch qwindowsmenu
CALL :moch qwindowsnativeinterface
CALL :mocs qwindowsuiabaseprovider uiautomation
CALL :mocs qwindowsuiamainprovider uiautomation
CALL :mocs qwindowsuiaprovidercache uiautomation

echo RCC
%RCC% --name cursors --output %DIROUTRCC%/qrc_cursors.cpp %DIRSRC%/cursors.qrc
%RCC% --name openglblacklists --output %DIROUTRCC%/qrc_openglblacklists.cpp %DIRSRC%/openglblacklists.qrc

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
