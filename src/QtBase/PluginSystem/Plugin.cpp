/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

// include the required headers
#include "Plugin.h"
#include "../DockHeader.h"
#include "../QtBaseManager.h"
#include "../PluginSystem/PluginManager.h"
#include <Core/LogManager.h>
#include <Core/Counter.h>


using namespace Core;

// constructor
Plugin::Plugin(const char* typeUuid) : QObject(), Core::AttributeSet()
{
	mTypeUuid				= typeUuid;
	mDock					= NULL;
	mId						= CORE_COUNTER.Next();
	mForceEnableSettings	= false;
	mIsLocked				= false;

	mSettingsButton			= NULL;
	mSettingsWidget			= NULL;

	mRealtimeWidget			= NULL;
	mShowPerformanceInfo	= false;
}


// destructor
Plugin::~Plugin()
{
	if (mDock != NULL)
	{
		// hide the dock widget
		mDock->hide();
		
		// remove dock widget from the main window
		GetQtBaseManager()->GetMainWindow()->removeDockWidget( mDock );

		// close the dock widget
		mDock->close();
		
		// safe delete 
		//mDock->deleteLater();
		delete mDock;
	}
}


// create the base interface
void Plugin::CreateBaseInterface(const char* objectName)
{
	// get the main window and the plugin manager
	QMainWindow* mainWindow = GetQtBaseManager()->GetMainWindow();

	// create a window for the plugin
	mDock = new DockWidget( GetName() );
	mDock->setAllowedAreas(Qt::AllDockWidgetAreas);

	// set the custom dock widget header
	mDockHeader = new DockHeader( mDock );
	connect( mDockHeader, SIGNAL(RemovePlugin()), this, SLOT(OnRemovePlugin()) );
	mDock->setTitleBarWidget( mDockHeader );

	mDock->setFeatures( GetDockWidgetFeatures() );

	if (objectName == NULL)
	{
		QString newName = GenerateRandomUuid().AsChar();
		LogInfo("Plugin name: %s", newName.toUtf8().data());
		SetObjectName( newName );
	}
	else
		SetObjectName(objectName);

	mainWindow->addDockWidget( Qt::TopDockWidgetArea, mDock );

	mDockHeader->OnUpdate();
}


void Plugin::SetVisible(bool isVisible)
{
	if (isVisible == true)
		mDock->show();
	else
		mDock->hide();
}


bool Plugin::IsVisible()
{
	return mDock->isVisible();
}


void Plugin::SetTitle(const char* title)
{
	if (mDockHeader != NULL)
		mDockHeader->SetTitle(title);
}


void Plugin::SetTitleBarVisible(bool visible)
{
	if (visible == true)
		mDock->setTitleBarWidget(mDockHeader);
	else
		mDock->setTitleBarWidget(new QWidget());
}


void Plugin::SetLocked(bool locked)
{
	if (mIsLocked == locked)
		return;

	mIsLocked = locked;

	mDockHeader->setEnabled(mIsLocked == false);
	//SetTitleBarVisible(mIsLocked == false);

	mDock->setFeatures(GetDockWidgetFeatures());
}


// register attributes and create the default values
void Plugin::RegisterAttributes()
{
	// register attributes
	AttributeSettings* attributeSettings = RegisterAttribute("Is Settings Window Visible", "settingsWindowVisibility", "", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	attributeSettings->SetVisible(false);
	attributeSettings->SetDefaultValue( AttributeBool::Create(false) );

	// create default attribute values
	CreateDefaultAttributeValues();
}


void Plugin::CreateDockMainWidget(QWidget** outMainWidget, QHBoxLayout** outMainHLayout)
{
	// main widget
	QWidget* mainWidget = new QWidget(mDock);
	mainWidget->setFocusPolicy(Qt::WheelFocus);

	// main layout
	QHBoxLayout* mainLayout = new QHBoxLayout();
	mainWidget->setLayout(mainLayout);

	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);

	// prepare the dock window
	mDock->SetContents(mainWidget);

	*outMainWidget	= mainWidget;
	*outMainHLayout	= mainLayout;
}


void Plugin::FillLayouts(QWidget* mainWidget, QHBoxLayout* mainHLayout, const Core::Array<QWidget*>& toolbarWidgets, const char* buttonText, const char* iconPrefix, QWidget* renderWidget)
{
	// vertical layout for the left side (left of the settings dialog stack)
	QVBoxLayout* leftVLayout = new QVBoxLayout();
	mainHLayout->addLayout(leftVLayout);

	mainHLayout->addWidget( mSettingsWidget );

	leftVLayout->setMargin(0);
	leftVLayout->setSpacing(0);

	// toolbar layout
	QHBoxLayout* toolbarHLayout = new QHBoxLayout();
	leftVLayout->addLayout( toolbarHLayout );
	
	toolbarHLayout->setMargin(3);
	toolbarHLayout->setSpacing(10);
	toolbarHLayout->setAlignment( Qt::AlignLeft );

	// add all toolbar widgets
	const uint32 numToolbarWidgets = toolbarWidgets.Size();
	for (uint32 i=0; i<numToolbarWidgets; ++i)
		toolbarHLayout->addWidget( toolbarWidgets[i] );

	// add toolbar spacer widget
	QWidget* spacerWidget = new QWidget();
	spacerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	toolbarHLayout->addWidget(spacerWidget);

	// settings button
	String iconFilename;
	iconFilename.Format("/Images/Icons/%s.png", iconPrefix);

	mSettingsButton = new ImageButton( iconFilename.AsChar(), DEFAULT_ICONSIZE, buttonText, ImageButton::TYPE_STDICON, mDock );
	connect( mSettingsButton, SIGNAL(clicked()), this, SLOT(OnOpenCloseSettings()) );

	toolbarHLayout->addWidget( mSettingsButton );

	leftVLayout->addWidget(renderWidget);

	UpdateSettingsButtonVisibility();

	// is settings window visible?
	const bool settingsWindowVisibility = GetBoolAttributeByName( "settingsWindowVisibility" );

	if (mSettingsWidget != NULL)
		mSettingsWidget->setVisible( settingsWindowVisibility );
}


void Plugin::SetSettingsWidget(QWidget* widget, bool bottomSpacer, int margin)
{
	QWidget* helperWidget = new QWidget(mDock);
	helperWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored);

	QVBoxLayout* vLayout = new QVBoxLayout();
	helperWidget->setLayout(vLayout);
	vLayout->setMargin(margin);

	vLayout->addWidget(widget);

	// add spacer widget
	if (bottomSpacer == true)
	{
		QWidget* spacerWidget = new QWidget();
		spacerWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
		vLayout->addWidget(spacerWidget);
	}

	mSettingsWidget = helperWidget;

	if (mSettingsWidget != NULL)
		mSettingsWidget->hide();
}


// called to open and close the settings window
void Plugin::OnOpenCloseSettings()
{
	if (mSettingsWidget != NULL && mSettingsWidget->isVisible() == true)
	{
		mSettingsWidget->hide();
		SetBoolAttribute( "settingsWindowVisibility", false );
	}
	else
	{
		if (mForceEnableSettings == true || HasVisibleAttributes() == true)
		{
			if (mSettingsWidget != NULL)
				mSettingsWidget->show();

			SetBoolAttribute( "settingsWindowVisibility", true );
		}
	}
}


// show or hide the settings button
void Plugin::UpdateSettingsButtonVisibility()
{
	bool showSettings = mForceEnableSettings == true || HasVisibleAttributes() == true;
	mSettingsButton->setVisible( showSettings );
}


// real-time update
void Plugin::RealtimeUpdate()
{
	if (mRealtimeWidget == NULL)
		return;

	// update in case the widget is visible
	if (mRealtimeWidget->isVisible() == true)
		mRealtimeWidget->update();
}


QDockWidget::DockWidgetFeatures Plugin::GetDockWidgetFeatures() const
{
	QDockWidget::DockWidgetFeatures features = QDockWidget::NoDockWidgetFeatures;
	if (IsClosable() && !IsLocked())		features |= QDockWidget::DockWidgetClosable;
	if (IsVertical())						features |= QDockWidget::DockWidgetVerticalTitleBar;
	if (IsMovable() && !IsLocked())			features |= QDockWidget::DockWidgetMovable;
	if (IsFloatable() && !IsLocked())		features |= QDockWidget::DockWidgetFloatable;
	return features;
}