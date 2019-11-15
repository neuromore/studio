setlocal
set MOC=%MOC% -DQ_OS_WIN -I../../include/qt
set RCC=%RCC%
set UIC=%UIC%
set DIRIN=../../include/qt/QtMultimediaWidgets
set DIRSRC=../../src/qt-multimediawidgets
set DIROUT=../../src/qt-multimediawidgets/.moc
set DIROUTUIC=../../src/qt-multimediawidgets/.uic
set DIROUTRCC=../../src/qt-multimediawidgets/.rcc

echo MOC for root
%MOC% %DIRIN%/qcameraviewfinder.h -o %DIROUT%/moc_qcameraviewfinder.cpp
%MOC% %DIRIN%/qgraphicsvideoitem.h -o %DIROUT%/moc_qgraphicsvideoitem.cpp
%MOC% %DIRIN%/qvideowidget.h -o %DIROUT%/moc_qvideowidget.cpp
%MOC% %DIRIN%/qvideowidgetcontrol.h -o %DIROUT%/moc_qvideowidgetcontrol.cpp
%MOC% %DIRIN%/private/qpaintervideosurface_p.h -o %DIROUT%/moc_qpaintervideosurface_p.cpp
%MOC% %DIRIN%/private/qvideowidget_p.h -o %DIROUT%/moc_qvideowidget_p.cpp
