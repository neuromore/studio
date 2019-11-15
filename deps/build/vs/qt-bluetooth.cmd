setlocal
set MOC=%MOC% -DQ_OS_WIN -I../../include/qt -I../../include/qt/QtCore -I../../include/qt/QtBluetooth
set DIRIN=../../include/qt/QtBluetooth
set DIRSRC=../../src/qt-bluetooth
set DIROUT=../../src/qt-bluetooth/.moc

echo MOC for root
CALL :moch qbluetoothdevicediscoveryagent
CALL :moch qbluetoothlocaldevice
CALL :moch qbluetoothserver
CALL :moch qbluetoothservicediscoveryagent
CALL :mocs qbluetoothserviceinfo_p private
CALL :moch qbluetoothsocket
CALL :moch qbluetoothtransfermanager
CALL :moch qbluetoothtransferreply
CALL :mocs qleadvertiser_p private
CALL :moch qlowenergycontroller
CALL :mocs qlowenergycontroller_p private
CALL :mocc qlowenergycontroller_winrt .
CALL :mocs qlowenergycontrollerbase_p private
CALL :moch qlowenergyservice
CALL :mocs qlowenergyserviceprivate_p private

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
