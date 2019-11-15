setlocal
set MOC=%MOC% -DQ_OS_WIN -I../../include/qt -I../../include/qt/QtCore -I../../include/qt/QtWidgets -I../../include/qt/QtGui
set RCC=%RCC%
set UIC=%UIC%
set DIRIN=../../include/qt/QtWidgets
set DIRSRC=../../src/qt-widgets
set DIROUT=../../src/qt-widgets/.moc
set DIROUTUIC=../../src/qt-widgets/.uic
set DIROUTRCC=../../src/qt-widgets/.rcc

echo MOC for 'dialogs'
%MOC% %DIRIN%/qdialog.h -o %DIROUT%/moc_qdialog.cpp
%MOC% %DIRIN%/qerrormessage.h -o %DIROUT%/moc_qerrormessage.cpp
%MOC% %DIRIN%/private/qfileinfogatherer_p.h -o %DIROUT%/moc_qfileinfogatherer_p.cpp
%MOC% %DIRIN%/qfilesystemmodel.h -o %DIROUT%/moc_qfilesystemmodel.cpp
%MOC% %DIRIN%/qmessagebox.h -o %DIROUT%/moc_qmessagebox.cpp
%MOC% %DIRIN%/qprogressdialog.h -o %DIROUT%/moc_qprogressdialog.cpp
%MOC% %DIRIN%/private/qsidebar_p.h -o %DIROUT%/moc_qsidebar_p.cpp
%MOC% %DIRIN%/qwizard.h -o %DIROUT%/moc_qwizard.cpp
%MOC% %DIRIN%/qfiledialog.h -o %DIROUT%/moc_qfiledialog.cpp
%MOC% %DIRIN%/qinputdialog.h -o %DIROUT%/moc_qinputdialog.cpp
%MOC% %DIRIN%/qcolordialog.h -o %DIROUT%/moc_qcolordialog.cpp
%MOC% %DIRIN%/qfontdialog.h -o %DIROUT%/moc_qfontdialog.cpp
%MOC% %DIRSRC%/dialogs/qcolordialog.cpp -o %DIROUT%/qcolordialog.moc
%MOC% %DIRSRC%/dialogs/qfontdialog.cpp -o %DIROUT%/qfontdialog.moc
%MOC% %DIRSRC%/dialogs/qinputdialog.cpp -o %DIROUT%/qinputdialog.moc
%MOC% %DIRSRC%/dialogs/qmessagebox.cpp -o %DIROUT%/qmessagebox.moc

echo MOC for 'effects'
%MOC% %DIRIN%/qgraphicseffect.h -o %DIROUT%/moc_qgraphicseffect.cpp
%MOC% %DIRIN%/private/qgraphicseffect_p.h -o %DIROUT%/moc_qgraphicseffect_p.cpp
%MOC% %DIRIN%/private/qpixmapfilter_p.h -o %DIROUT%/moc_qpixmapfilter_p.cpp

echo MOC for 'graphicsview'
%MOC% %DIRIN%/qgraphicsanchorlayout.h -o %DIROUT%/moc_qgraphicsanchorlayout.cpp
%MOC% %DIRIN%/qgraphicsitemanimation.h -o %DIROUT%/moc_qgraphicsitemanimation.cpp
%MOC% %DIRIN%/qgraphicsitem.h -o %DIROUT%/moc_qgraphicsitem.cpp
%MOC% %DIRIN%/qgraphicsproxywidget.h -o %DIROUT%/moc_qgraphicsproxywidget.cpp
%MOC% %DIRIN%/qgraphicsscene.h -o %DIROUT%/moc_qgraphicsscene.cpp
%MOC% %DIRIN%/private/qgraphicsscenebsptreeindex_p.h -o %DIROUT%/moc_qgraphicsscenebsptreeindex_p.cpp
%MOC% %DIRIN%/private/qgraphicssceneindex_p.h -o %DIROUT%/moc_qgraphicssceneindex_p.cpp
%MOC% %DIRIN%/private/qgraphicsscenelinearindex_p.h -o %DIROUT%/moc_qgraphicsscenelinearindex_p.cpp
%MOC% %DIRIN%/qgraphicstransform.h -o %DIROUT%/moc_qgraphicstransform.cpp
%MOC% %DIRIN%/qgraphicsview.h -o %DIROUT%/moc_qgraphicsview.cpp
%MOC% %DIRIN%/qgraphicswidget.h -o %DIROUT%/moc_qgraphicswidget.cpp

echo MOC for 'itemviews'
%MOC% %DIRIN%/qabstractitemdelegate.h -o %DIROUT%/moc_qabstractitemdelegate.cpp
%MOC% %DIRIN%/qabstractitemview.h -o %DIROUT%/moc_qabstractitemview.cpp
%MOC% %DIRIN%/qcolumnview.h -o %DIROUT%/moc_qcolumnview.cpp
%MOC% %DIRIN%/private/qcolumnviewgrip_p.h -o %DIROUT%/moc_qcolumnviewgrip_p.cpp
%MOC% %DIRIN%/qdatawidgetmapper.h -o %DIROUT%/moc_qdatawidgetmapper.cpp
%MOC% %DIRIN%/qdirmodel.h -o %DIROUT%/moc_qdirmodel.cpp
%MOC% %DIRIN%/qheaderview.h -o %DIROUT%/moc_qheaderview.cpp
%MOC% %DIRIN%/qitemdelegate.h -o %DIROUT%/moc_qitemdelegate.cpp
%MOC% %DIRIN%/private/qitemeditorfactory_p.h -o %DIROUT%/moc_qitemeditorfactory_p.cpp
%MOC% %DIRIN%/qlistview.h -o %DIROUT%/moc_qlistview.cpp
%MOC% %DIRIN%/qstyleditemdelegate.h -o %DIROUT%/moc_qstyleditemdelegate.cpp
%MOC% %DIRIN%/qtablewidget.h -o %DIROUT%/moc_qtablewidget.cpp
%MOC% %DIRIN%/qtreeview.h -o %DIROUT%/moc_qtreeview.cpp
%MOC% %DIRIN%/qtreewidget.h -o %DIROUT%/moc_qtreewidget.cpp
%MOC% %DIRIN%/private/qtablewidget_p.h -o %DIROUT%/moc_qtablewidget_p.cpp
%MOC% %DIRIN%/private/qtreewidget_p.h -o %DIROUT%/moc_qtreewidget_p.cpp
%MOC% %DIRIN%/private/qitemeditorfactory_p.h -o %DIROUT%/moc_qitemeditorfactory_p.cpp
%MOC% %DIRIN%/qlistwidget.h -o %DIROUT%/moc_qlistwidget.cpp
%MOC% %DIRIN%/private/qlistwidget_p.h -o %DIROUT%/moc_qlistwidget_p.cpp
%MOC% %DIRIN%/qtableview.h -o %DIROUT%/moc_qtableview.cpp
%MOC% %DIRSRC%/itemviews/qitemeditorfactory.cpp -o %DIROUT%/qitemeditorfactory.moc
%MOC% %DIRSRC%/itemviews/qlistwidget.cpp -o %DIROUT%/qlistwidget.moc
%MOC% %DIRSRC%/itemviews/qtableview.cpp -o %DIROUT%/qtableview.moc

echo MOC for 'kernel'
%MOC% %DIRIN%/qaction.h -o %DIROUT%/moc_qaction.cpp
%MOC% %DIRIN%/qactiongroup.h -o %DIROUT%/moc_qactiongroup.cpp
%MOC% %DIRIN%/qapplication.h -o %DIROUT%/moc_qapplication.cpp
%MOC% %DIRIN%/qboxlayout.h -o %DIROUT%/moc_qboxlayout.cpp
%MOC% %DIRIN%/qdesktopwidget.h -o %DIROUT%/moc_qdesktopwidget.cpp
%MOC% %DIRIN%/private/qdesktopwidget_p.h -o %DIROUT%/moc_qdesktopwidget_p.cpp
%MOC% %DIRIN%/qformlayout.h -o %DIROUT%/moc_qformlayout.cpp
%MOC% %DIRIN%/qgesture.h -o %DIROUT%/moc_qgesture.cpp
%MOC% %DIRIN%/private/qgesturemanager_p.h -o %DIROUT%/moc_qgesturemanager_p.cpp
%MOC% %DIRIN%/qlayout.h -o %DIROUT%/moc_qlayout.cpp
%MOC% %DIRIN%/qgridlayout.h -o %DIROUT%/moc_qgridlayout.cpp
%MOC% %DIRIN%/qopenglwidget.h -o %DIROUT%/moc_qopenglwidget.cpp
%MOC% %DIRIN%/qshortcut.h -o %DIROUT%/moc_qshortcut.cpp
%MOC% %DIRIN%/qsizepolicy.h -o %DIROUT%/moc_qsizepolicy.cpp
%MOC% %DIRIN%/qstackedlayout.h -o %DIROUT%/moc_qstackedlayout.cpp
%MOC% %DIRIN%/qwidget.h -o %DIROUT%/moc_qwidget.cpp
%MOC% %DIRIN%/qwidgetaction.h -o %DIROUT%/moc_qwidgetaction.cpp
%MOC% %DIRIN%/private/qwidgetbackingstore_p.h -o %DIROUT%/moc_qwidgetbackingstore_p.cpp
%MOC% %DIRIN%/private/qwidgetwindow_p.h -o %DIROUT%/moc_qwidgetwindow_p.cpp
%MOC% %DIRIN%/private/qwindowcontainer_p.h -o %DIROUT%/moc_qwindowcontainer_p.cpp
%MOC% %DIRSRC%/kernel/qtooltip.cpp -o %DIROUT%/qtooltip.moc
%MOC% %DIRSRC%/kernel/qwhatsthis.cpp -o %DIROUT%/qwhatsthis.moc

echo MOC for 'statemachine'
%MOC% %DIRIN%/private/qbasickeyeventtransition_p.h -o %DIROUT%/moc_qbasickeyeventtransition_p.cpp
%MOC% %DIRIN%/private/qbasicmouseeventtransition_p.h -o %DIROUT%/moc_qbasicmouseeventtransition_p.cpp
%MOC% %DIRIN%/qkeyeventtransition.h -o %DIROUT%/moc_qkeyeventtransition.cpp
%MOC% %DIRIN%/qmouseeventtransition.h -o %DIROUT%/moc_qmouseeventtransition.cpp

echo MOC for 'styles'
%MOC% %DIRIN%/qcommonstyle.h -o %DIROUT%/moc_qcommonstyle.cpp
%MOC% %DIRIN%/private/qfusionstyle_p.h -o %DIROUT%/moc_qfusionstyle_p.cpp
%MOC% %DIRIN%/qproxystyle.h -o %DIROUT%/moc_qproxystyle.cpp
%MOC% %DIRIN%/qstyle.h -o %DIROUT%/moc_qstyle.cpp
%MOC% %DIRIN%/private/qstyleanimation_p.h -o %DIROUT%/moc_qstyleanimation_p.cpp
%MOC% %DIRIN%/qstyleplugin.h -o %DIROUT%/moc_qstyleplugin.cpp
%MOC% %DIRIN%/private/qstylesheetstyle_p.h -o %DIROUT%/moc_qstylesheetstyle_p.cpp
%MOC% %DIRIN%/private/qwindowsstyle_p.h -o %DIROUT%/moc_qwindowsstyle_p.cpp

echo MOC for 'util'
%MOC% %DIRIN%/qcompleter.h -o %DIROUT%/moc_qcompleter.cpp
%MOC% %DIRIN%/qscroller.h -o %DIROUT%/moc_qscroller.cpp
%MOC% %DIRIN%/qsystemtrayicon.h -o %DIROUT%/moc_qsystemtrayicon.cpp
%MOC% %DIRIN%/qundogroup.h -o %DIROUT%/moc_qundogroup.cpp
%MOC% %DIRIN%/qundostack.h -o %DIROUT%/moc_qundostack.cpp
%MOC% %DIRIN%/qundoview.h -o %DIROUT%/moc_qundoview.cpp
%MOC% %DIRIN%/private/qcompleter_p.h -o %DIROUT%/moc_qcompleter_p.cpp
%MOC% %DIRIN%/private/qflickgesture_p.h -o %DIROUT%/moc_qflickgesture_p.cpp
%MOC% %DIRIN%/private/qscroller_p.h -o %DIROUT%/moc_qscroller_p.cpp
%MOC% %DIRIN%/private/qsystemtrayicon_p.h -o %DIROUT%/moc_qsystemtrayicon_p.cpp
%MOC% %DIRIN%/private/qundostack_p.h -o %DIROUT%/moc_qundostack_p.cpp
%MOC% %DIRSRC%/util/qsystemtrayicon_x11.cpp -o %DIROUT%/qsystemtrayicon_x11.moc
%MOC% %DIRSRC%/util/qundoview.cpp -o %DIROUT%/qundoview.moc

echo MOC for 'widgets'
%MOC% %DIRIN%/qabstractbutton.h -o %DIROUT%/moc_qabstractbutton.cpp
%MOC% %DIRIN%/qabstractscrollarea.h -o %DIROUT%/moc_qabstractscrollarea.cpp
%MOC% %DIRIN%/qabstractslider.h -o %DIROUT%/moc_qabstractslider.cpp
%MOC% %DIRIN%/qabstractspinbox.h -o %DIROUT%/moc_qabstractspinbox.cpp
%MOC% %DIRIN%/qbuttongroup.h -o %DIROUT%/moc_qbuttongroup.cpp
%MOC% %DIRIN%/qcalendarwidget.h -o %DIROUT%/moc_qcalendarwidget.cpp
%MOC% %DIRIN%/qcheckbox.h -o %DIROUT%/moc_qcheckbox.cpp
%MOC% %DIRIN%/qcombobox.h -o %DIROUT%/moc_qcombobox.cpp
%MOC% %DIRIN%/qcommandlinkbutton.h -o %DIROUT%/moc_qcommandlinkbutton.cpp
%MOC% %DIRIN%/qdatetimeedit.h -o %DIROUT%/moc_qdatetimeedit.cpp
%MOC% %DIRIN%/qdial.h -o %DIROUT%/moc_qdial.cpp
%MOC% %DIRIN%/qdialogbuttonbox.h -o %DIROUT%/moc_qdialogbuttonbox.cpp
%MOC% %DIRIN%/qdockwidget.h -o %DIROUT%/moc_qdockwidget.cpp
%MOC% %DIRIN%/qfocusframe.h -o %DIROUT%/moc_qfocusframe.cpp
%MOC% %DIRIN%/qfontcombobox.h -o %DIROUT%/moc_qfontcombobox.cpp
%MOC% %DIRIN%/qframe.h -o %DIROUT%/moc_qframe.cpp
%MOC% %DIRIN%/qgroupbox.h -o %DIROUT%/moc_qgroupbox.cpp
%MOC% %DIRIN%/qkeysequenceedit.h -o %DIROUT%/moc_qkeysequenceedit.cpp
%MOC% %DIRIN%/qlabel.h -o %DIROUT%/moc_qlabel.cpp
%MOC% %DIRIN%/qlcdnumber.h -o %DIROUT%/moc_qlcdnumber.cpp
%MOC% %DIRIN%/qlineedit.h -o %DIROUT%/moc_qlineedit.cpp
%MOC% %DIRIN%/qmainwindow.h -o %DIROUT%/moc_qmainwindow.cpp
%MOC% %DIRIN%/qmdiarea.h -o %DIROUT%/moc_qmdiarea.cpp
%MOC% %DIRIN%/qmdisubwindow.h -o %DIROUT%/moc_qmdisubwindow.cpp
%MOC% %DIRIN%/qmenu.h -o %DIROUT%/moc_qmenu.cpp
%MOC% %DIRIN%/qmenubar.h -o %DIROUT%/moc_qmenubar.cpp
%MOC% %DIRIN%/qplaintextedit.h -o %DIROUT%/moc_qplaintextedit.cpp
%MOC% %DIRIN%/qprogressbar.h -o %DIROUT%/moc_qprogressbar.cpp
%MOC% %DIRIN%/qpushbutton.h -o %DIROUT%/moc_qpushbutton.cpp
%MOC% %DIRIN%/qradiobutton.h -o %DIROUT%/moc_qradiobutton.cpp
%MOC% %DIRIN%/qrubberband.h -o %DIROUT%/moc_qrubberband.cpp
%MOC% %DIRIN%/qscrollarea.h -o %DIROUT%/moc_qscrollarea.cpp
%MOC% %DIRIN%/qscrollbar.h -o %DIROUT%/moc_qscrollbar.cpp
%MOC% %DIRIN%/qsizegrip.h -o %DIROUT%/moc_qsizegrip.cpp
%MOC% %DIRIN%/qslider.h -o %DIROUT%/moc_qslider.cpp
%MOC% %DIRIN%/qspinbox.h -o %DIROUT%/moc_qspinbox.cpp
%MOC% %DIRIN%/qsplashscreen.h -o %DIROUT%/moc_qsplashscreen.cpp
%MOC% %DIRIN%/qsplitter.h -o %DIROUT%/moc_qsplitter.cpp
%MOC% %DIRIN%/qstackedwidget.h -o %DIROUT%/moc_qstackedwidget.cpp
%MOC% %DIRIN%/qstatusbar.h -o %DIROUT%/moc_qstatusbar.cpp
%MOC% %DIRIN%/qtabbar.h -o %DIROUT%/moc_qtabbar.cpp
%MOC% %DIRIN%/qtabwidget.h -o %DIROUT%/moc_qtabwidget.cpp
%MOC% %DIRIN%/qtextbrowser.h -o %DIROUT%/moc_qtextbrowser.cpp
%MOC% %DIRIN%/qtextedit.h -o %DIROUT%/moc_qtextedit.cpp
%MOC% %DIRIN%/qtoolbar.h -o %DIROUT%/moc_qtoolbar.cpp
%MOC% %DIRIN%/qtoolbox.h -o %DIROUT%/moc_qtoolbox.cpp
%MOC% %DIRIN%/qtoolbutton.h -o %DIROUT%/moc_qtoolbutton.cpp
%MOC% %DIRIN%/private/qabstractscrollarea_p.h -o %DIROUT%/moc_qabstractscrollarea_p.cpp
%MOC% %DIRIN%/private/qcombobox_p.h -o %DIROUT%/moc_qcombobox_p.cpp
%MOC% %DIRIN%/private/qdatetimeedit_p.h -o %DIROUT%/moc_qdatetimeedit_p.cpp
%MOC% %DIRIN%/private/qdockwidget_p.h -o %DIROUT%/moc_qdockwidget_p.cpp
%MOC% %DIRIN%/private/qlineedit_p.h -o %DIROUT%/moc_qlineedit_p.cpp
%MOC% %DIRIN%/private/qmainwindowlayout_p.h -o %DIROUT%/moc_qmainwindowlayout_p.cpp
%MOC% %DIRIN%/private/qplaintextedit_p.h -o %DIROUT%/moc_qplaintextedit_p.cpp
%MOC% %DIRIN%/private/qtoolbarextension_p.h -o %DIROUT%/moc_qtoolbarextension_p.cpp
%MOC% %DIRIN%/private/qtoolbarlayout_p.h -o %DIROUT%/moc_qtoolbarlayout_p.cpp
%MOC% %DIRIN%/private/qtoolbarseparator_p.h -o %DIROUT%/moc_qtoolbarseparator_p.cpp
%MOC% %DIRIN%/private/qwidgetanimator_p.h -o %DIROUT%/moc_qwidgetanimator_p.cpp
%MOC% %DIRIN%/private/qwidgetlinecontrol_p.h -o %DIROUT%/moc_qwidgetlinecontrol_p.cpp
%MOC% %DIRIN%/private/qwidgetresizehandler_p.h -o %DIROUT%/moc_qwidgetresizehandler_p.cpp
%MOC% %DIRIN%/private/qwidgettextcontrol_p.h -o %DIROUT%/moc_qwidgettextcontrol_p.cpp
%MOC% %DIRSRC%/widgets/qcalendarwidget.cpp -o %DIROUT%/qcalendarwidget.moc
%MOC% %DIRSRC%/widgets/qdockwidget.cpp -o %DIROUT%/qdockwidget.moc
%MOC% %DIRSRC%/widgets/qeffects.cpp -o %DIROUT%/qeffects.moc
%MOC% %DIRSRC%/widgets/qfontcombobox.cpp -o %DIROUT%/qfontcombobox.moc
%MOC% %DIRSRC%/widgets/qmdisubwindow.cpp -o %DIROUT%/qmdisubwindow.moc
%MOC% %DIRSRC%/widgets/qmenu.cpp -o %DIROUT%/qmenu.moc
%MOC% %DIRSRC%/widgets/qtabbar.cpp -o %DIROUT%/qtabbar.moc
%MOC% %DIRSRC%/widgets/qtoolbox.cpp -o %DIROUT%/qtoolbox.moc

echo RCC
%RCC% --name qstyle --output %DIROUTRCC%/qrc_qstyle.cpp %DIRSRC%/styles/qstyle.qrc
%RCC% --name qmessagebox --output %DIROUTRCC%/qrc_qmessagebox.cpp %DIRSRC%/dialogs/qmessagebox.qrc

echo UIC
%UIC% %DIRSRC%/dialogs/qfiledialog.ui -o %DIROUTUIC%/ui_qfiledialog.h
