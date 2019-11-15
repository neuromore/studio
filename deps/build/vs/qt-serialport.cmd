setlocal
set MOC=%MOC% -DQ_OS_WIN -I../../include/qt -I../../include/qt/QtSerialPort
set RCC=%RCC%
set UIC=%UIC%
set DIRIN=../../include/qt/QtSerialPort
set DIRSRC=../../src/qt-serialport
set DIROUT=../../src/qt-serialport/.moc
set DIROUTUIC=../../src/qt-serialport/.uic
set DIROUTRCC=../../src/qt-serialport/.rcc

echo MOC for root
%MOC% %DIRIN%/qserialport.h -o %DIROUT%/moc_qserialport.cpp
%MOC% %DIRIN%/private/qwinoverlappedionotifier_p.h -o %DIROUT%/moc_qwinoverlappedionotifier_p.cpp
