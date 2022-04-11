/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

// include required files
#include "QtBaseManager.h"
#include "Windows/ProgressWindowManager.h"
#include "Version.h"
#include <QIcon>
#include <QDir>
#include <QUuid>
#include <QTimer>
#include <QApplication>
#include <QDateTime>
#include <QTextStream>
#include <QDesktopWidget>
#include <QStandardPaths>
#include <Core/LogManager.h>
#include <EngineManager.h>


using namespace Core;

// the global Qt base manager
QTBASE_API QtBaseManager* gQtBaseManager = NULL;

class EngineCallback : public EngineManager::Callback
{
	public:
		String GenerateRandomUUID()
		{
			QString uuidString = QUuid::createUuid().toString();

			// windows used GUIDs which have a {} around the UUID, remove that!
			if (uuidString.startsWith('{') == true && uuidString.count('{') == 1)		uuidString.remove('{');
			if (uuidString.endsWith('}') == true && uuidString.count('}') == 1)			uuidString.remove('}');

			//LogInfo( "%s", FromQtString(uuidString).AsChar() );

			return FromQtString( uuidString );
		}

		Time Now()
		{
			return FromQtTime( QDateTime::currentDateTimeUtc() );
		}
};


// constructor
QtBaseManager::QtBaseManager() : QObject()
{
	LogDetailedInfo("Constructing Qt base manager ...");

	mExperienceAssetCache		= NULL;
	mMainWindow					= NULL;
	mProgressWindowManager		= NULL;
	mAttributeWidgetFactory		= NULL;
	mProgressWindowManager		= NULL;
	mBackendInterface			= NULL;
	mLayoutManager				= NULL;
	mStatusPopupManager			= NULL;
	mInterfaceEnabled			= false;
	mInterfaceUpdatePaused		= false;
	mLockInterface				= true;

	// set version
	mVersion = Version( NEUROMORE_QTBASE_VERSION_MAJOR, NEUROMORE_QTBASE_VERSION_MINOR, NEUROMORE_QTBASE_VERSION_PATCH );

	mCompileDate.Format( "%s", CORE_DATE );
}


// destructor
QtBaseManager::~QtBaseManager()
{
	LogDetailedInfo("Destructing Qt base manager ...");

	// get the number of icons and destroy them
	const uint32 numIcons = mIcons.Size();
	for (uint32 i=0; i<numIcons; ++i)
		delete mIcons[i];
	mIcons.Clear();

	delete mExperienceAssetCache;
	delete mBackendInterface;
	delete mAttributeWidgetFactory;
	delete mProgressWindowManager;
	delete mLayoutManager;
	delete mStatusPopupManager;
}


void QtBaseManager::Init()
{
	QCoreApplication::addLibraryPath("./");

	// create the attribute widget factory
	mAttributeWidgetFactory	= new AttributeWidgetFactory();

	// create the progress window manager
	mProgressWindowManager = new ProgressWindowManager();

	// create the back-end interface
	mBackendInterface = new BackendInterface();

	// initializing the layout manager
	mLayoutManager = new LayoutManager();

	// initialize the status popup manager
	mStatusPopupManager = new StatusPopupManager();

	// initialize the experience asset cache
	mExperienceAssetCache = new ExperienceAssetCache();

	// create the engine callback
	LogDetailedInfo("Constructing neuromore Engine callback ...");
	GetEngine()->SetCallback( new EngineCallback() );

	// update the physiological data folder path
	UpdatePhysiologicalDataFolder();
}


const QIcon& QtBaseManager::FindIcon(const char* filename)
{
	// check if an icon with this filename is already loaded
	const uint32 numIcons = mIcons.Size();
	for (uint32 i=0; i<numIcons; ++i)
	{
		if (mIcons[i]->mFilename.IsEqualNoCase(filename) == true)
			return *(mIcons[i]->mIcon);
	}

	//  we need to load the icon
	QIcon* icon = NULL;

	// 1) load from resources
	QString resFilename = ":/" + QString( filename );
	if (QFile::exists( resFilename ) == true)
		icon = new QIcon( resFilename );

	// 2) default to loading from disk
	else
	{	
		QString diskFilename = GetQtBaseManager()->GetAppDir() + filename;
		if (QFile::exists( diskFilename ) == true)
			icon = new QIcon( diskFilename );
		else
			LogError( "file not found: %s", filename);
	}

	IconData* iconData = new IconData(filename, icon);
	mIcons.Add( iconData );

	return *(icon);
}


// remove the given file from disk
bool QtBaseManager::RemoveFileFromDisk(const char* filename)
{
    QFile file(filename);
    return file.remove();
}


String QtBaseManager::GetAppDataFolder()
{
	QString result = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0);

#ifdef NEUROMORE_PLATFORM_WINDOWS
	result += "\\";
#endif
#ifdef NEUROMORE_PLATFORM_OSX
    result += "/";
#endif
#ifdef NEUROMORE_PLATFORM_LINUX
	result += "/";
#endif

	result = QDir::toNativeSeparators( result );

	// make sure our folder exists
	QDir appDataFolder(result);
	appDataFolder.mkpath(result);

	return FromQtString(result);
}


void QtBaseManager::UpdatePhysiologicalDataFolder()
{
	QString result = GetAppDataFolder().AsChar();
	result += "Data";

#ifdef NEUROMORE_PLATFORM_WINDOWS
	result += "\\";
#endif
#ifdef NEUROMORE_PLATFORM_OSX
    result += "/";
#endif
#ifdef NEUROMORE_PLATFORM_LINUX
	result += "/";
#endif

	result = QDir::toNativeSeparators( result );

	// make sure our physiological data folder exists
	QDir appDataFolder(result);
	appDataFolder.mkpath(result);

	mPhysiologicalDataFolder = FromQtString(result);
}


// loads our default style
bool QtBaseManager::LoadDefaultStyleSheet( QWidget* applyTo )
{
	return LoadStyleSheet( ":/Styles/Default.style", applyTo );
}

// load a style sheet from disk or resource file
bool QtBaseManager::LoadStyleSheet(const QString& filename, QWidget* applyTo)
{
	LogDetailedInfo( "Loading style sheet '%s' ...", FromQtString(filename).AsChar() );

	// try to open the file
	QFile file( filename );
	if (file.open(QIODevice::ReadOnly | QIODevice::Text) == false)
		return false;

	// read all lines of text
	QString styleText;
	QTextStream in(&file);
	while (in.atEnd() == false)
		styleText += in.readLine();

	// set the style
	LogDetailedInfo( "Applying style sheet ..." );
	applyTo->setStyleSheet( styleText );
	 
	return true;
}


void QtBaseManager::SetWidgetAsInvalidInput(QWidget* widget, bool isValid)
{
	if (isValid == true)
		widget->setStyleSheet("");
	else
		widget->setStyleSheet("border: 1px solid red;");
}


const char* QtBaseManager::ConstructHTMLLink(const char* text, const Color& color)
{
	int32 r = color.r * 256;
	int32 g = color.g * 256;
	int32 b = color.b * 256;

	mTempString.Format( "<qt><style>a { color: rgb(%i, %i, %i); } a:hover { color: rgb(40, 40, 40); }</style><a href='%s'>%s</a></qt>", r, g, b, text, text );
	return mTempString.AsChar();
}

/*
String QtBaseManager::CreateButtonStyle(const char* iconPath, const char* iconHoverPath, const char* iconDisabledPath)
{
	String result;
	result.Reserve(1024);

	// normal mode
	result += "QPushButton\n";
	result += "{\n";
	result += "    background-color: transparent;\n";
	result += "    border: none;\n";
	result += "    border-radius: 0px;\n";
	result += "    image: url(:";
	result +=      iconPath;
	result += ");\n";
	result += "    padding: 0px;\n";
	result += "}";

	// hover
	result += "QPushButton:hover\n";
	result += "{\n";
	result += "    image: url(:";
	result +=      iconHoverPath;
	result += ");";
	result += "}\n";
	
	// disabled
	result += "QPushButton:disabled\n";
	result += "{\n";
	result += "    image: url(:";
	result +=      iconDisabledPath;
	result += ");";
	result += "}\n";
	
	return result;
}


void QtBaseManager::MakeHoverButton(QPushButton* button, const char* iconPath, const char* iconHoverPath, const char* iconDisabledPath, const char* toolTipText, uint32 width, uint32 height)
{
	// set the stylesheet
	String stylesheet = CreateButtonStyle( iconPath, iconHoverPath, iconDisabledPath );
	button->setStyleSheet( stylesheet.AsChar() );

	button->setToolTip(toolTipText );
	button->setMinimumSize(width, height);
	button->setMaximumSize(width, height);
}
*/

void QtBaseManager::MakeTransparentButton(QPushButton* button, const char* iconFilename, const char* toolTipText, uint32 width, uint32 height)
{
	button->setObjectName("TransparentButton");
	button->setToolTip(toolTipText );
	button->setMinimumSize(width, height);
	button->setMaximumSize(width, height);
	button->setIcon( GetQtBaseManager()->FindIcon(iconFilename) );
}


bool QtBaseManager::RemoveDirectory(const QString &path)
{
    QDir dir( path );

    dir.setFilter( QDir::NoDotAndDotDot | QDir::Files );
    foreach( QString dirItem, dir.entryList() )
        dir.remove( dirItem );

    dir.setFilter( QDir::NoDotAndDotDot | QDir::Dirs );
    foreach( QString dirItem, dir.entryList() )
    {
        QDir subDir( dir.absoluteFilePath( dirItem ) );
        subDir.removeRecursively();
    }

	return true;
}


bool QtBaseManager::CopyDirectory(const QString &src, const QString &dst)
{
    QDir dir(src);
    if (! dir.exists())
        return false;

    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
        QString dst_path = dst + QDir::separator() + d;
        dir.mkpath(dst_path);
        CopyDirectory(src+ QDir::separator() + d, dst_path);
    }

    foreach (QString f, dir.entryList(QDir::Files))
	{
		QString filename = src + QDir::separator() + f;

		// skip the following files
		if (filename.contains("client.wyc") == false &&
			filename.contains("wyUpdate.exe") == false)
			QFile::copy(filename, dst + QDir::separator() + f);
    }

	return true;
}


void QtBaseManager::EnableInterface(bool isEnabled)
{
	mInterfaceEnabled = isEnabled;

	GetEngine()->SetIsRunning( mInterfaceEnabled );
	GetMainWindow()->setEnabled( mInterfaceEnabled );
}


void QtBaseManager::SetPauseInterface(bool isPaused)
{
	mInterfaceUpdatePaused = isPaused;

	mMainWindow->SetEngineTimerEnabled( !isPaused );
	mMainWindow->SetRealtimeUITimerEnabled( !isPaused );
	mMainWindow->SetInterfaceTimerEnabled( !isPaused );
}


void QtBaseManager::UpdateInterfaceLock()
{
	bool isInterfaceEnabled	= IsInterfaceEnabled();
	bool allowInterface		= false;

	// allow the interface being enabled in case the authentication center allows it and we do not show a progress window
	if (mLockInterface == false && GetQtBaseManager()->GetProgressWindowManager()->IsProgressWindowActive() == false)
		allowInterface = true;

	// activate or deactivate the interface in case the old differs from the new state
	if (isInterfaceEnabled != allowInterface)
	{
		if (allowInterface == true)
			LogDetailedInfo("Enabling user interface ...");
		else
			LogDetailedInfo("Disabling user interface ...");

		GetQtBaseManager()->EnableInterface( allowInterface );
	}
}


void QtBaseManager::ForceTerminateApplication()
{
	// TODO: this doesn't work as we haven't called qApp->exec() yet, this means the qt event loop is not active and won't be able to catch the exit event yet
	qApp->exit();

	// TODO: this is a hack!
	// fire a quit event in XX milliseconds to the application (when the event loop is active)
	QTimer::singleShot(100, qApp, SLOT(quit()));		// 0.1	seconds
	QTimer::singleShot(250, qApp, SLOT(quit()));		// 0.25	seconds
	QTimer::singleShot(1000, qApp, SLOT(quit()));		// 1	second
	QTimer::singleShot(5000, qApp, SLOT(quit()));		// 5	seconds
	QTimer::singleShot(10000, qApp, SLOT(quit()));		// 10	seconds
	QTimer::singleShot(20000, qApp, SLOT(quit()));		// 20	seconds
	QTimer::singleShot(30000, qApp, SLOT(quit()));		// 30	seconds
	QTimer::singleShot(60000, qApp, SLOT(quit()));		// 1	minute
	QTimer::singleShot(120000, qApp, SLOT(quit()));		// 2	minutes
	QTimer::singleShot(300000, qApp, SLOT(quit()));		// 5	minutes
	QTimer::singleShot(600000, qApp, SLOT(quit()));		// 10	minutes
	QTimer::singleShot(3600000, qApp, SLOT(quit()));	// 60	minutes
}


void QtBaseManager::CenterToScreen(QWidget* widget)
{
	if (widget == NULL)
		return;

	QPoint pos = QCursor::pos();
	if (mMainWindow != NULL)
		pos = mMainWindow->mapToGlobal( mMainWindow->rect().center() );

	QDesktopWidget* desktopWidget = QApplication::desktop();
	QRect desktopRect = desktopWidget->screenGeometry( desktopWidget->screenNumber(pos) );
	
	int32 x = desktopRect.width() / 2 - widget->width() / 2 + desktopRect.left();
	int32 y = desktopRect.height() / 2 - widget->height() / 2 + desktopRect.top();
	widget->move( x, y );
}

//--------------------------------------------------

// initialize the Qt base manager
bool QtBaseInitializer::Init(const char* appDir)
{
	// create the gCore object
	if (gQtBaseManager != NULL)
		return true;

	LogDetailedInfo("Initializing Qt resources ...");
	Q_INIT_RESOURCE( QtBaseResources );
	Q_INIT_RESOURCE( GraphResources );
	Q_INIT_RESOURCE( DeviceResources );
	Q_INIT_RESOURCE( LayoutResources );
	Q_INIT_RESOURCE( Assets );

	LogDetailedInfo("Constructing Qt base manager ...");
	gQtBaseManager = new QtBaseManager();
	gQtBaseManager->SetAppDir( appDir );
	return true;
}


// shutdown the Qt base manager
void QtBaseInitializer::Shutdown()
{
	LogInfo("Shutting down Qt base ...");

	// delete the manager
	delete gQtBaseManager;
	gQtBaseManager = NULL;
}
