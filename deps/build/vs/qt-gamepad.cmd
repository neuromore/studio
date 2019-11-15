setlocal
set MOC=%MOC% -DQ_OS_WIN -I../../include/qt
set RCC=%RCC%
set UIC=%UIC%
set DIRIN=../../include/qt/QtGamepad
set DIRSRC=../../src/qt-gamepad
set DIROUT=../../src/qt-gamepad/.moc
set DIROUTUIC=../../src/qt-gamepad/.uic
set DIROUTRCC=../../src/qt-gamepad/.rcc

echo MOC for root
%MOC% %DIRIN%/qgamepad.h -o %DIROUT%/moc_qgamepad.cpp
%MOC% %DIRIN%/qgamepadkeynavigation.h -o %DIROUT%/moc_qgamepadkeynavigation.cpp
%MOC% %DIRIN%/qgamepadmanager.h -o %DIROUT%/moc_qgamepadmanager.cpp
