setlocal
set MOC=%MOC% -DQ_OS_WIN -I../../include/qt -I../../include/qt/QtCore
set RCC=%RCC%
set DIRIN=../../include/qt/QtCore
set DIRSRC=../../src/qt-core
set DIROUT=../../src/qt-core/.moc
set DIROUTRCC=../../src/qt-core/.rcc

echo MOC for 'animation'
%MOC% %DIRIN%/qabstractanimation.h -o %DIROUT%/moc_qabstractanimation.cpp
%MOC% %DIRIN%/qanimationgroup.h -o %DIROUT%/moc_qanimationgroup.cpp
%MOC% %DIRIN%/qparallelanimationgroup.h -o %DIROUT%/moc_qparallelanimationgroup.cpp
%MOC% %DIRIN%/qpauseanimation.h -o %DIROUT%/moc_qpauseanimation.cpp
%MOC% %DIRIN%/qpropertyanimation.h -o %DIROUT%/moc_qpropertyanimation.cpp
%MOC% %DIRIN%/qsequentialanimationgroup.h -o %DIROUT%/moc_qsequentialanimationgroup.cpp
%MOC% %DIRIN%/qvariantanimation.h -o %DIROUT%/moc_qvariantanimation.cpp
%MOC% %DIRIN%/private/qabstractanimation_p.h -o %DIROUT%/moc_qabstractanimation_p.cpp

%MOC% %DIRIN%/qlibrary.h -o %DIROUT%/moc_qlibrary.cpp
%MOC% %DIRIN%/qpluginloader.h -o %DIROUT%/moc_qpluginloader.cpp
%MOC% %DIRIN%/private/qfactoryloader_p.h -o %DIROUT%/moc_qfactoryloader_p.cpp

echo MOC for 'kernel'
%MOC% %DIRIN%/qabstracteventdispatcher.h -o %DIROUT%/moc_qabstracteventdispatcher.cpp
%MOC% %DIRIN%/qcoreapplication.h -o %DIROUT%/moc_qcoreapplication.cpp
%MOC% %DIRIN%/qcoreevent.h -o %DIROUT%/moc_qcoreevent.cpp
%MOC% %DIRIN%/qeventloop.h -o %DIROUT%/moc_qeventloop.cpp
CALL :mocs qeventdispatcher_win_p private
CALL :moch qmimedata
CALL :moch qmimetype
%MOC% %DIRIN%/qnamespace.h -o %DIROUT%/moc_qnamespace.cpp
%MOC% %DIRIN%/qobjectcleanuphandler.h -o %DIROUT%/moc_qobjectcleanuphandler.cpp
%MOC% %DIRIN%/qsharedmemory.h -o %DIROUT%/moc_qsharedmemory.cpp
%MOC% %DIRIN%/qobject.h -o %DIROUT%/moc_qobject.cpp
%MOC% %DIRIN%/qsignalmapper.h -o %DIROUT%/moc_qsignalmapper.cpp
%MOC% %DIRIN%/qsocketnotifier.h -o %DIROUT%/moc_qsocketnotifier.cpp
%MOC% %DIRIN%/qtranslator.h -o %DIROUT%/moc_qtranslator.cpp
%MOC% %DIRIN%/qtimer.h -o %DIROUT%/moc_qtimer.cpp
%MOC% %DIRSRC%/kernel/qtimer.cpp -o %DIROUT%/qtimer.moc
CALL :moch qwineventnotifier

echo MOC for 'itemmodel'
%MOC% %DIRIN%/qabstractitemmodel.h -o %DIROUT%/moc_qabstractitemmodel.cpp
%MOC% %DIRIN%/qabstractproxymodel.h -o %DIROUT%/moc_qabstractproxymodel.cpp
%MOC% %DIRIN%/qidentityproxymodel.h -o %DIROUT%/moc_qidentityproxymodel.cpp
%MOC% %DIRIN%/qitemselectionmodel.h -o %DIROUT%/moc_qitemselectionmodel.cpp
%MOC% %DIRIN%/qsortfilterproxymodel.h -o %DIROUT%/moc_qsortfilterproxymodel.cpp
%MOC% %DIRIN%/qstringlistmodel.h -o %DIROUT%/moc_qstringlistmodel.cpp

echo MOC for 'io'
%MOC% %DIRIN%/qbuffer.h -o %DIROUT%/moc_qbuffer.cpp
%MOC% %DIRIN%/qfile.h -o %DIROUT%/moc_qfile.cpp
%MOC% %DIRIN%/qfiledevice.h -o %DIROUT%/moc_qfiledevice.cpp
%MOC% %DIRIN%/qfileselector.h -o %DIROUT%/moc_qfileselector.cpp
%MOC% %DIRIN%/qfilesystemwatcher.h -o %DIROUT%/moc_qfilesystemwatcher.cpp
%MOC% %DIRIN%/private/qfilesystemwatcher_p.h -o %DIROUT%/moc_qfilesystemwatcher_p.cpp
%MOC% %DIRIN%/private/qfilesystemwatcher_polling_p.h -o %DIROUT%/moc_qfilesystemwatcher_polling_p.cpp
CALL :mocs qfilesystemwatcher_win_p private
%MOC% %DIRIN%/qiodevice.h -o %DIROUT%/moc_qiodevice.cpp
%MOC% %DIRIN%/private/qnoncontiguousbytedevice_p.h -o %DIROUT%/moc_qnoncontiguousbytedevice_p.cpp
%MOC% %DIRIN%/qprocess.h -o %DIROUT%/moc_qprocess.cpp
%MOC% %DIRIN%/qsavefile.h -o %DIROUT%/moc_qsavefile.cpp
%MOC% %DIRIN%/qsettings.h -o %DIROUT%/moc_qsettings.cpp
%MOC% %DIRIN%/qstandardpaths.h -o %DIROUT%/moc_qstandardpaths.cpp
%MOC% %DIRIN%/qtemporaryfile.h -o %DIROUT%/moc_qtemporaryfile.cpp
%MOC% %DIRSRC%/io/qfilesystemwatcher_win.cpp -o %DIROUT%/qfilesystemwatcher_win.moc
CALL :mocs qwindowspipereader_p private
CALL :mocs qwindowspipewriter_p private

%MOC% %DIRIN%/qcborcommon.h -o %DIROUT%/moc_qcborcommon.cpp
%MOC% %DIRIN%/qcborvalue.h -o %DIROUT%/moc_qcborvalue.cpp
%MOC% %DIRIN%/qcborstream.h -o %DIROUT%/moc_qcborstream.cpp
%MOC% %DIRIN%/private/qtextstream_p.h -o %DIROUT%/moc_qtextstream_p.cpp

%MOC% %DIRIN%/qabstractstate.h -o %DIROUT%/moc_qabstractstate.cpp
%MOC% %DIRIN%/qabstracttransition.h -o %DIROUT%/moc_qabstracttransition.cpp
%MOC% %DIRIN%/qeventtransition.h -o %DIROUT%/moc_qeventtransition.cpp
%MOC% %DIRIN%/qfinalstate.h -o %DIROUT%/moc_qfinalstate.cpp
%MOC% %DIRIN%/qhistorystate.h -o %DIROUT%/moc_qhistorystate.cpp
%MOC% %DIRSRC%/statemachine/qhistorystate.cpp -o %DIROUT%/qhistorystate.moc
%MOC% %DIRIN%/qsignaltransition.h -o %DIROUT%/moc_qsignaltransition.cpp
%MOC% %DIRIN%/qstate.h -o %DIROUT%/moc_qstate.cpp
%MOC% %DIRIN%/qstatemachine.h -o %DIROUT%/moc_qstatemachine.cpp
%MOC% %DIRSRC%/statemachine/qstatemachine.cpp -o %DIROUT%/qstatemachine.moc

%MOC% %DIRIN%/qfuturewatcher.h -o %DIROUT%/moc_qfuturewatcher.cpp
%MOC% %DIRIN%/qthread.h -o %DIROUT%/moc_qthread.cpp
%MOC% %DIRIN%/qthreadpool.h -o %DIROUT%/moc_qthreadpool.cpp

%MOC% %DIRIN%/qcryptographichash.h -o %DIROUT%/moc_qcryptographichash.cpp
%MOC% %DIRIN%/qeasingcurve.h -o %DIROUT%/moc_qeasingcurve.cpp
%MOC% %DIRIN%/qlocale.h -o %DIROUT%/moc_qlocale.cpp
%MOC% %DIRIN%/qtimeline.h -o %DIROUT%/moc_qtimeline.cpp

echo RCC
%RCC% --name mimetypes --output %DIROUTRCC%/qrc_mimetypes.cpp %DIRSRC%/mimetypes/mimetypes.qrc

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
