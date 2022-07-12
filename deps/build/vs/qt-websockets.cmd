setlocal
set MOC=%MOC% -DQ_OS_WIN -I../../include/qt -I../../include/qt/QtWebSockets
set RCC=%RCC%
set UIC=%UIC%
set DIRIN=../../include/qt/QtWebSockets
set DIRSRC=../../src/qt-websockets
set DIROUT=../../src/qt-websockets/.moc

%MOC% %DIRIN%/qwebsocket.h -o %DIROUT%/moc_qwebsocket.cpp
%MOC% %DIRIN%/qwebsocketserver.h -o %DIROUT%/moc_qwebsocketserver.cpp

%MOC% %DIRIN%/private/qsslserver_p.h -o %DIROUT%/moc_qsslserver_p.cpp
%MOC% %DIRIN%/private/qwebsocketdataprocessor_p.h -o %DIROUT%/moc_qwebsocketdataprocessor_p.cpp
%MOC% %DIRIN%/private/qwebsockethandshakeresponse_p.h -o %DIROUT%/moc_qwebsockethandshakeresponse_p.cpp
