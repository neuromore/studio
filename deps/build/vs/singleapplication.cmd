setlocal
set MOC=%MOC% -DQ_OS_WIN -I../../include/singleapplication -I../../include/qt 
set RCC=%RCC%
set UIC=%UIC%
set DIRIN=../../include/singleapplication
set DIRSRC=../../src/singleapplication
set DIROUT=../../src/singleapplication/.moc

%MOC% %DIRIN%/singleapplication.h -o %DIROUT%/moc_singleapplication.cpp
%MOC% %DIRIN%/singleapplication_p.h -o %DIROUT%/moc_singleapplication_p.cpp
