#pragma once

// From Std
#include <algorithm>
#include <memory>

// From Engine
#include <Engine/Core/StandardHeaders.h>
#include <Engine/Core/String.h>
#include <Engine/Core/Array.h>
#include <Engine/Core/Json.h>
#include <Engine/Core/LogManager.h>
#include <Engine/Core/EventManager.h>
#include <Engine/Core/EncryptedJSONFile.h>
#include <Engine/Core/Math.h>
#include <Engine/Core/Time.h>
#include <Engine/Core/Timer.h>
#include <Engine/Core/Thread.h>
#include <Engine/Core/ThreadHandler.h>
#include <Engine/Graph/Graph.h>
#include <Engine/Graph/Port.h>
#include <Engine/Graph/Node.h>
#include <Engine/Graph/State.h>
#include <Engine/Graph/Connection.h>
#include <Engine/Graph/StateMachine.h>
#include <Engine/BciDevice.h>
#include <Engine/EngineManager.h>
#include <Engine/Device.h>
#include <Engine/DeviceDriver.h>
#include <Engine/DeviceManager.h>
#include <Engine/Config.h>
#include <Engine/Notifications.h>
#include <Engine/User.h>

// Qt
#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QUuid>
#include <QBuffer>
#include <QMetaType>
#include <QByteArray>
#include <QColor>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QMainWindow>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpacerItem>
#include <QtGui>
#include <QSizePolicy>
#include <QPixmap>
#include <QTextEdit>
#include <QMessageBox>
#include <QIcon>
#include <QHeaderView>
#include <QDir>
#include <QDesktopServices>
#include <QListWidget>
#include <QStackedWidget>
#include <QSettings>
#include <QScreen>
#include <QSurface>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPalette>
#include <QSysInfo>
#include <QFont>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QFileDialog>
#include <QFileInfo>
#include <QSplashScreen>
#include <QDragEnterEvent>
#include <QtPlugin>
#include <QTimer>
#include <QProcess>
#include <QToolBar>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <QTableWidget>
#include <QMimeData>
#include <QTabBar>
#include <QSplitter>
#include <QStaticText>
#include <QOpenGLContext>
#include <QToolTip>
#include <QMenu>
#include <QMenuBar>
#include <QVariant>
#include <QSignalMapper>
#include <QCoreApplication>
#include <QMovie>
#include <QSoundEffect>
#include <QClipboard>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QApplication>
#include <QTextDocument>
#include <QTextOption>

// From QtBase
#include <QtBase/QtBaseConfig.h>
#include <QtBase/QtBaseManager.h>
#include <QtBase/ColorPalette.h>
#include <QtBase/DockHeader.h>
#include <QtBase/PluginSystem/Plugin.h>
#include <QtBase/PluginSystem/PluginManager.h>
#include <QtBase/System/SerialPort.h>
#include <QtBase/System/BluetoothHelpers.h>
#include <QtBase/Backend/BackendHelpers.h>
#include <QtBase/Backend/Request.h>
#include <QtBase/Backend/Response.h>
#include <QtBase/ImageButton.h>
#include <QtBase/Slider.h>
#include <QtBase/AttributeWidgets/AttributeSetGridWidget.h>
#include <QtBase/AttributeWidgets/AttributeWidgetFactory.h>
#include <QtBase/AttributeWidgets/PropertyTreeWidget.h>

// From Rendering
#include <Studio/Rendering/OpenGLWidget2DHelpers.h>

// From Devices
#include <Studio/Devices/DeviceHelpers.h>

// From Plugins
#include <Studio/Plugins/Graph/GraphHelpers.h>
#include <Studio/Plugins/Graph/GraphShared.h>

// From Root
#include <Studio/Config.h>
#include <Studio/Branding.h>
#include <Studio/AppManager.h>
#include <Studio/MainWindow.h>
