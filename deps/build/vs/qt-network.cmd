setlocal
set MOC=%MOC% -DQ_OS_WIN -I../../include/qt -I../../include/qt/QtCore -I../../include/qt/QtNetwork
set RCC=%RCC%
set DIRIN=../../include/qt/QtNetwork
set DIRSRC=../../src/qt-network
set DIROUT=../../src/qt-network/.moc

CALL :moch qabstractnetworkcache
CALL :moch qabstractsocket
CALL :mocs qabstractsocketengine_p private
CALL :mocs qbearerengine_p private
CALL :mocs qbearerplugin_p private
CALL :moch qdnslookup
CALL :mocs qdnslookup_p private
CALL :moch qdtls
CALL :mocc qftp access
CALL :mocs qftp_p private
CALL :mocs qhostinfo_p private
CALL :mocs qhttp2protocolhandler_p private
CALL :moch qhttpmultipart
CALL :mocs qhttpnetworkconnection_p private
CALL :mocs qhttpnetworkconnectionchannel_p private
CALL :mocs qhttpnetworkreply_p private
CALL :mocs qhttpsocketengine_p private
CALL :mocs qhttpthreaddelegate_p private
CALL :moch qlocalserver
CALL :moch qlocalsocket
CALL :mocs qnativesocketengine_p private
CALL :mocs qnetworkaccessbackend_p private
CALL :mocs qnetworkaccesscache_p private
CALL :mocs qnetworkaccessdebugpipebackend_p private
CALL :mocs qnetworkaccessfilebackend_p private
CALL :mocs qnetworkaccessftpbackend_p private
CALL :moch qnetworkaccessmanager
CALL :moch qnetworkconfigmanager
CALL :mocs qnetworkconfigmanager_p private
CALL :moch qnetworkcookiejar
CALL :moch qnetworkdiskcache
CALL :mocs qnetworkfile_p private
CALL :moch qnetworkinterface
CALL :moch qnetworkproxy
CALL :moch qnetworkreply
CALL :mocs qnetworkreplydataimpl_p private
CALL :mocs qnetworkreplyfileimpl_p private
CALL :mocs qnetworkreplyhttpimpl_p private
CALL :mocs qnetworkreplyimpl_p private
CALL :moch qnetworksession
CALL :mocs qnetworksession_p private
CALL :mocs qsocks5socketengine_p private
CALL :mocs qspdyprotocolhandler_p private
CALL :moch qsslsocket
CALL :moch qtcpserver
CALL :moch qtcpsocket
CALL :moch qudpsocket
CALL :mocs qwindowscarootfetcher_p private

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
