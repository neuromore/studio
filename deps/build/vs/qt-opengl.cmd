setlocal
set MOC=%MOC% -DQ_OS_WIN -I../../include/qt -I../../include/qt/QtCore -I../../include/qt/QtOpenGL
set RCC=%RCC%
set DIRIN=../../include/qt/QtOpenGL
set DIRSRC=../../src/qt-opengl
set DIROUT=../../src/qt-opengl/.moc

CALL :moch qgl
CALL :mocs qgl_p private
CALL :mocs qglengineshadermanager_p private
CALL :moch qglshaderprogram

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
