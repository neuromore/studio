/****************************************************************************
**
** Copyright 2019 neuromore co
** Contact: https://neuromore.com/contact
**
** Commercial License Usage
** Licensees holding valid commercial neuromore licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and neuromore. For licensing terms
** and conditions see https://neuromore.com/licensing. For further
** information use the contact form at https://neuromore.com/contact.
**
** neuromore Public License Usage
** Alternatively, this file may be used under the terms of the neuromore
** Public License version 1 as published by neuromore co with exceptions as 
** appearing in the file neuromore-class-exception.md included in the 
** packaging of this file. Please review the following information to 
** ensure the neuromore Public License requirements will be met: 
** https://neuromore.com/npl
**
****************************************************************************/

// include precompiled header
#include <Studio/Precompiled.h>

// include required headers
#include "PreSessionWidget.h"
#include "SessionControlPlugin.h"
#include <Studio/Windows/VisualizationSelectWindow.h>

using namespace Core;

PreSessionWidget::PreSessionWidget(SessionControlPlugin* plugin, QWidget* parent, int buttonSize) : QWidget( parent )
{
	mPlugin = plugin;
	mStartButtonSize = buttonSize;
	Init();
}


void PreSessionWidget::Init()
{
	setObjectName("TransparentWidget");
	setMinimumHeight(mStartButtonSize);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);

	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->setMargin(0);
	setLayout(hLayout);

	// create the back to selection button
	mBackToSelectionButton = new QPushButton("Back to selection");
	hLayout->addWidget(mBackToSelectionButton);
	connect( mBackToSelectionButton, &QPushButton::clicked, this, [=]
	{
		if (GetUser()->FindRule("STUDIO_SETTING_EasyWorkflow") != NULL)
			GetLayoutManager()->SwitchToLayoutByName("Experience Selection");
	});

	if (GetUser()->FindRule("STUDIO_SETTING_EasyWorkflow") == NULL)
	{
		mBackToSelectionButton->hide();
	}

	// create the start session button
	mStartButton = new ImageButton("Images/Icons/StartSession.png", mStartButtonSize, "Begin Session");
	hLayout->addWidget(mStartButton);

	// ------------------------------------------

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->setMargin(0);
	hLayout->addLayout(vLayout);

	// right side of hLayout: grid layout
	QGridLayout* gLayout = new QGridLayout();
	gLayout->setMargin(0);
	vLayout->addLayout(gLayout);

	//
	// first row of gridlayout
	//

	// add the level selection label
	mVisSelectionLabel = new QLabel("  Visualization:");
	gLayout->addWidget( mVisSelectionLabel, 0, 0 );

	// add the level selection button (for local ones)
	mVisSelectionButton = new QPushButton();

	if (GetManager()->GetVisualizationManager()->GetNumVisualizations() == 0)
		mVisSelectionButton->setEnabled(false);

	mVisSelectionButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	mVisSelectionButton->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Eye.png") );
	connect( mVisSelectionButton, &QPushButton::clicked, this, &PreSessionWidget::OnSelectVisualizationClicked);
	gLayout->addWidget(mVisSelectionButton, 0, 1);

	//
	// second row of gridlayout
	//
/*
	// add the total time label
	mTotalTimeLabel = new QLabel("  Duration:");
	gLayout->addWidget( mTotalTimeLabel, 1, 0 );

	// row 1, col 1: horizontal layout: spinner+label
	QHBoxLayout* hLayoutBottom = new QHBoxLayout();
	hLayoutBottom->setMargin(0);
	gLayout->addLayout( hLayoutBottom, 1, 1 );

	// add the total time spinbox
	mTotalTimeSpinbox = new IntSpinBox();
	mTotalTimeSpinbox->setRange(0, INT_MAX);
	const int defaultTotalTime = (int)GetEngine()->GetSession()->GetTotalTime().InSeconds();
	mTotalTimeSpinbox->setValue(defaultTotalTime);
	hLayoutBottom->addWidget(mTotalTimeSpinbox);
	connect( mTotalTimeSpinbox, SIGNAL(valueChanged(double)), this, SLOT(OnTotalTimeChanged(double)) );

	// NOTE disabled it for now
	mTotalTimeSpinbox->setEnabled(false);

	// add seconds label
	mTotalTimeSecondsLabel = new QLabel(" seconds");
	hLayoutBottom->addWidget(mTotalTimeSecondsLabel);

	
	// add toolbar spacer widget
	QWidget* spacerWidget = new QWidget();
	spacerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	hLayoutBottom->addWidget(spacerWidget);
	*/

	//
	// third row of gridlayout
	//

	// add the user label
	mSelectUserLabel = new QLabel("  User:");
	gLayout->addWidget( mSelectUserLabel, 1, 0 );

	// add select user button
	mSelectUserButton = new QPushButton(GetUser()->CreateDisplayableName().AsChar());
	mSelectUserButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	mSelectUserButton->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Users.png") );
	connect( mSelectUserButton, &QPushButton::clicked, this, &PreSessionWidget::OnSelectUserClicked);
	gLayout->addWidget( mSelectUserButton, 1, 1 );
	
	// show report button
	mShowReportButton = new QPushButton("Show Report");
	mShowReportButton->setIcon(GetQtBaseManager()->FindIcon("/Images/Icons/Report.png"));
	mShowReportButton->setVisible(false);
	vLayout->addWidget(mShowReportButton);
	connect( mShowReportButton, SIGNAL(clicked()), mPlugin, SLOT(ShowReport()) );

	if (GetUser()->FindRule("STUDIO_SETTING_EasyWorkflow") != NULL)
	{
		mVisSelectionLabel->hide();
		mVisSelectionButton->hide();

		mSelectUserLabel->hide();
		mSelectUserButton->hide();
	}
}


// update the levelswitch combobox 
void PreSessionWidget::ReInit()
{
	/*NetworkServerClient* starterClient = GetNetworkServer()->GetPrimaryStarterClient();
	NetworkServerClient* visClient = GetNetworkServer()->GetPrimaryVisualizationClient();

	// check what clients are running and set up interface
	if (starterClient == NULL)
	{
		// first case: no starter, no visualization
		if (visClient == NULL)
		{
			// hide combobox, show no-client label
			mVisSelectionButton->show();
			mVisSelectionButton->setText("Select Visualization");
		}
		// second case: no starter, but a visualization (e.g. during development)
		else
		{
			mVisSelectionButton->show();

			// set visualization name
			mVisSelectionButton->setText(visClient->mName.AsChar());
		}
	}
	// starter is connected: fill combobox
	else
	{
		// activate combobox
		mVisSelectionButton->hide();

		// get levels from client
		Json* config = starterClient->GetConfig();
		Json::Item levelRoot = config->Find("applications");
		CORE_ASSERT( levelRoot.IsNull() == false );
		
		// add levels to combobox
		for (uint32 i=0; i<levelRoot.Size(); i++)
		{
			// get level item
			Json::Item level = levelRoot[i];
			if (level.IsNull() == true)
				continue;

			// dont add idle level
			Json::Item isIdle = level.Find( "isIdle" );
			if (isIdle.IsBool() == true)
				if (isIdle.GetBool() == true)
					continue;

			// get name of level
			Json::Item name = level.Find( "name" );
			if (name.IsString() == false)
				continue;

			String levelName = name.GetString();

			// add level to combobox
			mVisSelectionComboBox->addItem( levelName.AsChar() );

			// check if there is a visualization client whose name is contained in the level list
			if (visClient != NULL)
			{

				String clientName = visClient->mName;
				if (clientName.IsEqual( levelName )) 
				{
					// index of last added item 
					int itemIndex = mVisSelectionComboBox->count() - 1;

					// select item
					mVisSelectionComboBox->setCurrentIndex( itemIndex );
					 
					// change font to bold
					QFont itemFont = mVisSelectionComboBox->font();
					itemFont.setBold( true );
					mVisSelectionComboBox->setItemData( itemIndex, itemFont, Qt::FontRole );   // i+1 due to dummy entry
				}
			}
		}

		if (visClient == NULL)
		{
			mVisSelectionComboBox->setCurrentIndex( -1 );
		}
	}

	// reenable text-changed signals
	mVisSelectionComboBox->blockSignals( false );*/
}


void PreSessionWidget::ShowSelectUserButton(bool show)
{
	mSelectUserLabel->setVisible(show);
	mSelectUserButton->setVisible(show);
}


// called when the total session time got changed
void PreSessionWidget::OnTotalTimeChanged(double value)
{
	GetEngine()->GetSession()->SetTotalTime( value );
}


void PreSessionWidget::OnSelectVisualizationClicked()
{
	VisualizationManager* vizManager = GetManager()->GetVisualizationManager();
	if (vizManager->GetNumVisualizations() > 0 && !vizManager->IsRunning())
	{
		VisualizationSelectWindow selectVizWindow(GetMainWindow());
		selectVizWindow.exec();
	}
}
