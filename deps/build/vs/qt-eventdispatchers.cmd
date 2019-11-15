setlocal
set MOC=%MOC% -DQ_OS_WIN -DQT_STATIC -DQT_STATICPLUGIN -I../../include/qt -I../../include/qt/QtCore -I../../include/qt/QtGui -I../../src/qt-platform-windows
set RCC=%RCC%
set UIC=%UIC%
set DIRIN=../../include/qt/QtEventDispatcherSupport
set DIRSRC=../../src/qt-eventdispatchers
set DIROUT=../../src/qt-eventdispatchers/.moc

echo MOC for root
CALL :mocs qwindowsguieventdispatcher_p private

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
