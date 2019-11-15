/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "Layout.h"
#include "PluginSystem/PluginManager.h"
#include "QtBaseManager.h"
#include <Core/LogManager.h>
#include <Core/Array.h>
#include <Core/Timer.h>
#include <QGuiApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QSize>


using namespace Core;

// constructor
Layout::Layout(EType type, const char* visualName, const char* filename, const char* ruleName, const char* uuid) : QObject()
{
	mType		= type;
	mVisualName	= visualName;
	mRuleName	= ruleName;
	mFilename	= filename;
	mUuid		= uuid;
}


// destructor
Layout::~Layout()
{
}


// save the current layout to a file
bool Layout::Save(const char* filename)
{
	Timer saveTimer;

	// create our json parser
	Json json;
	Json::Item rootItem = json.GetRootItem();

	// write the number of active plugins
	PluginManager* pluginManager = GetPluginManager();
	const uint32 numPlugins = pluginManager->GetNumActivePlugins();

	rootItem.AddInt( "versionHigh", 1 );
	rootItem.AddInt( "versionLow", 0 );
	rootItem.AddString( "compileDate", GetQtBaseManager()->GetCompileDate() );

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	QMainWindow*	mainWindow					= GetQtBaseManager()->GetMainWindow();
	QByteArray		mainWindowGeometry			= mainWindow->saveGeometry();
	String			mainWindowGeometryString	= FromQtString( mainWindowGeometry.toBase64() );
	QByteArray		mainWindowState				= mainWindow->saveState();
	String			mainWindowStateString		= FromQtString( mainWindowState.toBase64() );

	Json::Item mainWindowItem = rootItem.AddObject( "mainWindow" );

	mainWindowItem.AddInt( "posX", mainWindow->pos().x() );
	mainWindowItem.AddInt( "posY", mainWindow->pos().y() );
	mainWindowItem.AddInt( "width", mainWindow->width() );
	mainWindowItem.AddInt( "height", mainWindow->height() );
	mainWindowItem.AddBool( "isMaximized", mainWindow->isMaximized() );
	mainWindowItem.AddString( "geometry", mainWindowGeometryString.AsChar() );
	mainWindowItem.AddString( "state", mainWindowStateString.AsChar() );


	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Json::Item resolutionsItem = jsonParser.AddArray( "screenConfigs", rootItem );

	//QScreen* primaryScreen = qApp->primaryScreen();

	//mScreenConfigs.Add( ScreenConfig( Screen(primaryScreen), MainWindowState(mainWindow) ) );

	//Layout::ScreenConfig( Screen(primaryScreen), MainWindowState(mainWindow) ).Save( &jsonParser, resolutionsItem);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Json::Item pluginsItem = rootItem.AddArray("plugins");

	// for each plugin (window)
	for (uint32 i=0; i<numPlugins; ++i)
	{
		Plugin* plugin = GetPluginManager()->GetActivePlugin(i);

		// only save visible plugins
		if (plugin->IsVisible() == false)
			continue;

		// add the plugin item
		Json::Item pluginItem = pluginsItem.AddObject();

		// add the attributes
		pluginItem.AddString( "objectName",	FromQtString(plugin->GetObjectName()).AsChar() );
		pluginItem.AddString( "typeUuid",	plugin->GetTypeUuid() );

		// save custom plugin data
		plugin->SaveSettings( json, pluginItem );
	}

	// save data to disk
	bool result = json.WriteToFile(filename);

	// log the timing information
	const float saveTime = saveTimer.GetTime().InSeconds();
	LogInfo( "Saving layout '%s' (%.1f ms)", filename, saveTime * 1000.0f );

	return result;
}


// TODO move this to layout manager
// load the layout from a given file
bool Layout::Load(const char* filename)
{
	LogInfo( "Loading layout '%s' ...", filename );

	Timer loadTimer;

	LogDetailedInfo( "Clearing active plugins ..." );
	GetPluginManager()->ClearActivePlugins(false);
	LogDetailedInfo( "Active plugins cleared" );

	//---------------------------------------------------------------------------------

	LogDetailedInfo( "Opening file '%s'...", filename );
	QFile file(filename);
	if (file.open(QFile::ReadOnly | QFile::Text) == false)
	{
		LogError( "QFile: Could not open file '%s'", filename );
		return false;
	}

	LogDetailedInfo( "Reading text from file ..." );
	QTextStream textStream(&file);
    String jsonString;
	FromQtString(textStream.readAll(), &jsonString);

	// create our json parser
	LogDetailedInfo( "Parsing json string ..." );
	Json json;
	if (json.Parse(jsonString) == false)
		return false;

	Json::Item rootItem			= json.GetRootItem();
	//Json::Item versionHighItem	= rootItem.Find( "versionHigh" );
	//Json::Item versionLowItem		= rootItem.Find( "versionLow" );
	//Json::Item compileDateItem	= rootItem.Find( "compileDate" );
	Json::Item pluginsItem		= rootItem.Find( "plugins" );
	Json::Item mainWindowItem	= rootItem.Find( "mainWindow" );

	//---------------------------------------------------------------------------------

	if (pluginsItem.IsArray() == true)
	{
		// get the number of plugins and iterate through them
		const uint32 numPlugins = pluginsItem.Size();
		for (uint32 i=0; i<numPlugins; ++i)
		{
			Json::Item pluginItem		= pluginsItem[i];
			Json::Item objectNameItem	= pluginItem.Find( "objectName" );
			Json::Item typeUuidItem		= pluginItem.Find( "typeUuid" );

			// make sure all needed items are valid
			if (objectNameItem.IsString() == false || typeUuidItem.IsString() == false)
				continue;

			String objectName	= objectNameItem.GetString();
			String typeUuid		= typeUuidItem.GetString();

			//Core::LogDetailedInfo("Loading plugin settings for plugin '%s' (objectName=%s)...", pluginName.AsChar(), objectName.AsChar());
			//Core::LogDetailedInfo("   + Data size    = %d bytes", pluginHeader.mDataSize);
			//Core::LogDetailedInfo("   + Data version = %d", pluginHeader.mDataVersion);

			// the plugin to apply the settings to etc
			Plugin* plugin = NULL;

			LogDetailedInfo( "Creating plugin %s (UUID=%s)...", objectName.AsChar(), typeUuid.AsChar() );

			// try to create the plugin of this type
			if (plugin == NULL)
				plugin = GetPluginManager()->CreatePlugin( typeUuid.AsChar(), objectName.AsChar(), false, &json, &pluginItem );

			// if it still couldn't be created
			if (plugin == NULL)
			{
				LogCritical("Failed to create plugin window of type '%s'.", typeUuid.AsChar());
			}
			else
			{
				plugin->SetVisible(false);
			}
		}
	}

	//---------------------------------------------------------------------------------

	QMainWindow* mainWindow = GetQtBaseManager()->GetMainWindow();
	QRect screenRect = QApplication::desktop()->screenGeometry();

	int32 mainWindowPosX		= 0;
	int32 mainWindowPosY		= 0;
	int32 mainWindowWidth		= screenRect.width();
	int32 mainWindowHeight		= screenRect.height();
	bool  mainWindowMaximized	= true;

	if (mainWindowItem.IsNull() == false)
	{
		Json::Item posXItem		= mainWindowItem.Find( "posX" );
		Json::Item posYItem		= mainWindowItem.Find( "posY" );
		Json::Item widthItem	= mainWindowItem.Find( "width" );
		Json::Item heightItem	= mainWindowItem.Find( "height" );
		Json::Item maximizedItem= mainWindowItem.Find( "isMaximized" );
		Json::Item geometryItem	= mainWindowItem.Find( "geometry" );
		Json::Item stateItem	= mainWindowItem.Find( "state" );

		if (posXItem.IsNumber() == true)		mainWindowPosX		= posXItem.GetInt();
		if (posYItem.IsNumber() == true)		mainWindowPosY		= posYItem.GetInt();
		if (widthItem.IsNumber() == true)		mainWindowWidth		= widthItem.GetInt();
		if (heightItem.IsNumber() == true)		mainWindowHeight	= heightItem.GetInt();
		if (maximizedItem.IsBool() == true)		mainWindowMaximized	= maximizedItem.GetBool();

		/*LogInfo(" - MainWindow");
		LogInfo("    + Pos: (%i, %i)", mainWindowPosX, mainWindowPosY );
		LogInfo("    + Size: (%i, %i)", mainWindowWidth, mainWindowHeight );
		LogInfo("    + IsMaximized: %i", mainWindowMaximized );*/

		// load the window geometry
		if (geometryItem.IsString() == true)
		{
			String mainWindowGeometryString = geometryItem.GetString();
			mMainWindowGeometry = QByteArray::fromBase64( mainWindowGeometryString.AsChar() );
            //LogInfo( "mainWindowGeometry: %s", FromQtString(mainWindowGeometryString).AsChar() );
		}
		else
			mMainWindowGeometry.clear();

		// load the window states
		if (stateItem.IsString() == true)
		{
			String mainWindowStateString = stateItem.GetString();
			mMainWindowState = QByteArray::fromBase64( mainWindowStateString.AsChar() );
			//LogInfo( "mainWindowState: %s", FromQtString(mainWindowStateString).AsChar() );
		}
		else
			mMainWindowState.clear();

		// restore state and geometry
		OnRestore();

		// don't do this! This is done inside the window state and geometry already! Fucks up the layouts
		//mainWindow->move(mainWindowPosX, mainWindowPosY);
		//mainWindow->resize(mainWindowWidth, mainWindowHeight);

		mMainWindowShowMode = MAINWINDOWSHOW_NORMAL;
		if (mainWindowMaximized == true)
			mMainWindowShowMode = MAINWINDOWSHOW_MAXIMIZED;

		// show main window and restore the geometry and the state one more time (Qt bug? Some bug from our side?)
		// why QTimer::singleShot? It seems like the message loop after creating the plugins got to be updated once to show the main window in the correct size (or maximized)
		QTimer::singleShot( 500, this, SLOT(OnShowMainWindow()) );
	}

	//---------------------------------------------------------------------------------

	// trigger the OnAfterLoadLayout callbacks
	const uint32 numActivePlugins = GetPluginManager()->GetNumActivePlugins();
	for (uint32 p=0; p<numActivePlugins; ++p)
	{
		Plugin* plugin = GetPluginManager()->GetActivePlugin(p);
		plugin->OnAfterLoadLayout();
		plugin->SetVisible(true);
	}

	//---------------------------------------------------------------------------------

	// reinitialize the window menu
	GetPluginManager()->ReInitMenu();

	//---------------------------------------------------------------------------------

	// log the timing information
	const float loadTime = loadTimer.GetTime().InSeconds();
	LogInfo( "Layout '%s' loaded in %.1f ms", filename, loadTime * 1000.0f );

	return true;
}


void Layout::OnShowMainWindow()
{
	QMainWindow* mainWindow = GetQtBaseManager()->GetMainWindow();

	// maximized state
	if (mMainWindowShowMode == MAINWINDOWSHOW_MAXIMIZED)
		mainWindow->showMaximized();
	else
		mainWindow->showNormal();

	// why QTimer::singleShot? It seems like the message loop has to be updated before restoring geometry and state works correctly
	QTimer::singleShot( 500, this, SLOT(OnRestore()) );
}


void Layout::OnRestore()
{
	QMainWindow* mainWindow = GetQtBaseManager()->GetMainWindow();
	mainWindow->restoreState( mMainWindowState );
	mainWindow->restoreGeometry( mMainWindowGeometry );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
Layout::Screen::Screen()
{
	mWidth	= -1;
	mHeight	= -1;
}

// constructor
Layout::Screen::Screen(QScreen* screen)
{
	QSize screenSize = screen->size();

	mWidth	= screenSize.width();
	mHeight	= screenSize.height();
}


// constructor
Layout::Screen::Screen(int32 width, int32 height)
{
	mWidth	= width;
	mHeight	= height;
}


bool Layout::Screen::Save(Json& json, Json::Item& item) const
{
	item.AddInt( "width", mWidth );
	item.AddInt( "height", mHeight );

	return true;
}


// constructor
Layout::MainWindowState::MainWindowState()
{
	mPosX			= 0;
	mPosY			= 0;
	mWidth			= -1;
	mHeight			= -1;
	mIsMaximized	= true;
}


// constructor
Layout::MainWindowState::MainWindowState(const char* windowGeometry, const char* windowState, int32 windowPosX, int32 windowPosY, int32 windowWidth, int32 windowHeight, bool isMaximized)
{
	mGeometry		= windowGeometry;
	mState			= windowState;
	mPosX			= windowPosX;
	mPosY			= windowPosY;
	mWidth			= windowWidth;
	mHeight			= windowHeight;
	mIsMaximized	= isMaximized;
}


// constructor
Layout::MainWindowState::MainWindowState(QMainWindow* mainWindow)
{
	QByteArray	mainWindowGeometry	= mainWindow->saveGeometry();
	QByteArray	mainWindowState		= mainWindow->saveState();

	mGeometry		= FromQtString( mainWindowGeometry.toBase64() );
	mState			= FromQtString( mainWindowState.toBase64() );
	mPosX			= mainWindow->pos().x();
	mPosY			= mainWindow->pos().y();
	mWidth			= mainWindow->width();
	mHeight			= mainWindow->height();
	mIsMaximized	= mainWindow->isMaximized();
}


bool Layout::MainWindowState::Save(Json& json, Json::Item& item) const
{
	item.AddInt( "posX", mPosX );
	item.AddInt( "posY", mPosY );
	item.AddInt( "width", mWidth );
	item.AddInt( "height", mHeight );
	item.AddBool( "isMaximized", mIsMaximized );
	item.AddString( "geometry", mGeometry.AsChar() );
	item.AddString( "state", mState.AsChar() );

	return true;
}


bool Layout::ScreenConfig::Save(Json& json, Json::Item& item) const
{
	// save the screens
	Json::Item screensItem	= item.AddArray("screens");
	Json::Item screenItem	= screensItem.AddObject();
	mScreen.Save( json, screenItem );

	// save the main window state
	Json::Item mainWindowStateItem	= item.AddObject("mainWindowState");
	mState.Save( json, mainWindowStateItem );

	return true;
}
