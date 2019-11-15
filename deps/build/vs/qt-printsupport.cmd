setlocal
set MOC=%MOC% -DQ_OS_WIN -I../../include/qt
set RCC=%RCC%
set UIC=%UIC%
set DIRIN=../../include/qt/QtPrintSupport
set DIRSRC=../../src/qt-printsupport
set DIROUT=../../src/qt-printsupport/.moc
set DIROUTUIC=../../src/qt-printsupport/.uic
set DIROUTRCC=../../src/qt-printsupport/.rcc

echo MOC for 'dialogs'
%MOC% %DIRIN%/qprintdialog.h -o %DIROUT%/moc_qprintdialog.cpp
%MOC% %DIRIN%/qprintpreviewdialog.h -o %DIROUT%/moc_qprintpreviewdialog.cpp
%MOC% %DIRSRC%/dialogs/qprintpreviewdialog.cpp -o %DIROUT%/qprintpreviewdialog.moc

echo MOC for 'widgets'
%MOC% %DIRIN%/qprintpreviewwidget.h -o %DIROUT%/moc_qprintpreviewwidget.cpp
%MOC% %DIRSRC%/widgets/qprintpreviewwidget.cpp -o %DIROUT%/qprintpreviewwidget.moc

echo RCC
%RCC% --name qprintdialog --output %DIROUTRCC%/qrc_qprintdialog.cpp %DIRSRC%/dialogs/qprintdialog.qrc

echo UIC
%UIC% %DIRSRC%/dialogs/qpagesetupwidget.ui -o %DIROUTUIC%/ui_qpagesetupwidget.h
%UIC% %DIRSRC%/dialogs/qprintpropertieswidget.ui -o %DIROUTUIC%/ui_qprintpropertieswidget.h
%UIC% %DIRSRC%/dialogs/qprintsettingsoutput.ui -o %DIROUTUIC%/ui_qprintsettingsoutput.h
%UIC% %DIRSRC%/dialogs/qprintwidget.ui -o %DIROUTUIC%/ui_qprintwidget.h
