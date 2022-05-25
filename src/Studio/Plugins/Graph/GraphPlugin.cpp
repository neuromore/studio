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
#include "GraphPlugin.h"
#include "GraphPaletteWidget.h"
#include <Graph/GraphExporter.h>

using namespace Core;

// constructor
GraphPlugin::GraphPlugin(GraphWidget::Type type, const char* typeUuid) : Plugin(typeUuid)
{
	LogDetailedInfo("Constructing graph plugin ...");

	mType						= type;
	mGraphWidget				= NULL;
	mAttributesWidget			= NULL;
	mPaletteWidget				= NULL;
	mAttributeWidgetCallback	= NULL;
	mInfoWidget					= NULL;
	mPaletteTabIndex			= 0;
}


// destructor
GraphPlugin::~GraphPlugin()
{
	LogDetailedInfo("Destructing graph plugin ...");

	CORE_EVENTMANAGER.RemoveEventHandler(this);

	// remove the interface callback
	QtBaseManager* baseMgr = GetQtBaseManager();
	AttributeWidgetFactory* factory = baseMgr->GetAttributeWidgetFactory();
	factory->RemoveCallback( mAttributeWidgetCallback, false );
	delete mAttributeWidgetCallback;
}


// init after the parent dock window has been created
bool GraphPlugin::Init()
{
	LogInfo("Initializing graph plugin ...");

	QWidget*		mainWidget		= NULL;
	QHBoxLayout*	mainLayout		= NULL;
	CreateDockMainWidget( &mainWidget, &mainLayout );

	///////////////////////////////////////////////////////////////////////////
	// Settings
	///////////////////////////////////////////////////////////////////////////

	QWidget* settingsMainWidget = new QWidget();

	QVBoxLayout* settingsMainLayout = new QVBoxLayout();
	settingsMainWidget->setLayout(settingsMainLayout);
	settingsMainLayout->setMargin(0);

	int32 settingsDialogWith	= 323;
	const int32 macWidthAdd		= 0;
	const int32 smWidthAdd		= 100;
#ifndef NEUROMORE_PLATFORM_WINDOWS
    settingsDialogWith += macWidthAdd;
#endif

	if (mType == GraphWidget::STATEMACHINE)
		settingsDialogWith += smWidthAdd;

    settingsMainWidget->setMinimumWidth( settingsDialogWith );
    settingsMainWidget->setMaximumWidth( settingsDialogWith );

	// create the attributes dock
	mAttributesWidget	= new GraphAttributesWidget(mainWidget);
	mPaletteWidget		= new GraphPaletteWidget(mType, mainWidget);

	if (mType == GraphWidget::CLASSIFIER)
		mInfoWidget		= new GraphInfoWidget(mainWidget);

	// link the graph widget to the plugin
	mPaletteWidget->SetGraphPlugin( this );

	String paletteStackName;
	if (mType == GraphWidget::CLASSIFIER)
		paletteStackName = "Node Palette";
	else
		paletteStackName = "State Palette";
	
	//Add( mPaletteWidget, paletteStackName.AsChar(), false, false, true, false );
	settingsMainLayout->addWidget(mPaletteWidget);

	//Add( mAttributesWidget, "Attributes", false, true, true, false );
	settingsMainLayout->addWidget(mAttributesWidget);

	if (mType == GraphWidget::CLASSIFIER)
	{
		//Add( mInfoWidget, "Information", true, true, true, false );
		settingsMainLayout->addWidget(mInfoWidget);

		// hide the info widget
		mInfoWidget->hide();
	}

	SetSettingsWidget( settingsMainWidget, false, 1 );

	// show settings even though there aren't any attributes
	mForceEnableSettings = true;

	///////////////////////////////////////////////////////////////////////////
	// Main graph widget
	///////////////////////////////////////////////////////////////////////////

	// create the graph widget
	mGraphWidget = new GraphWidget( mType, this, mainWidget );

	///////////////////////////////////////////////////////////////////////////
	// Toolbar (top-left)
	///////////////////////////////////////////////////////////////////////////

	Core::Array<QWidget*> toolbarWidgets;

	// add the menu
	/*mMenuBar = new QMenuBar();
	toolbarWidgets.Add(mMenuBar);

	// file actions
	QMenu* fileMenu = mMenuBar->addMenu( tr("&File") );

	// save
	QAction* saveAction = fileMenu->addAction( tr("Save"), this, SLOT(OnSave()) );
	saveAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Save.png") );

	// edit menu
	QMenu* editMenu = mMenuBar->addMenu( tr("&Edit") );

	// align menu
	QMenu* alignMenu = mMenuBar->addMenu( tr("&Align") );

		// align left
		QAction* alignAction = alignMenu->addAction( tr("Left"), mGraphWidget, SLOT(OnAlignLeft()) );
		alignAction->setIcon( GetQtBaseManager()->FindIcon("Images/Graph/AlignLeft.png") );

		// align right
		alignAction = alignMenu->addAction( tr("Right"), mGraphWidget, SLOT(OnAlignRight()) );
		alignAction->setIcon( GetQtBaseManager()->FindIcon("Images/Graph/AlignRight.png") );

		// align top
		alignAction = alignMenu->addAction( tr("Top"), mGraphWidget, SLOT(OnAlignTop()) );
		alignAction->setIcon( GetQtBaseManager()->FindIcon("Images/Graph/AlignTop.png") );

		// align bottom
		alignAction = alignMenu->addAction( tr("Bottom"), mGraphWidget, SLOT(OnAlignBottom()) );
		alignAction->setIcon( GetQtBaseManager()->FindIcon("Images/Graph/AlignBottom.png") );*/

	// add save button
	mSaveButton			= new ImageButton("Images/Icons/Save.png", DEFAULT_ICONSIZE, "Save");					connect(mSaveButton, SIGNAL(clicked()), this, SLOT(OnSave()));							toolbarWidgets.Add(mSaveButton);

	mAlignLeftButton	= new ImageButton("Images/Graph/AlignLeft.png", DEFAULT_ICONSIZE, "Align Left");		connect(mAlignLeftButton, SIGNAL(clicked()), mGraphWidget, SLOT(OnAlignLeft()));		toolbarWidgets.Add(mAlignLeftButton);
	mAlignRightButton	= new ImageButton("Images/Graph/AlignRight.png", DEFAULT_ICONSIZE, "Align Right");		connect(mAlignRightButton, SIGNAL(clicked()), mGraphWidget, SLOT(OnAlignRight()));		toolbarWidgets.Add(mAlignRightButton);
	mAlignTopButton		= new ImageButton("Images/Graph/AlignTop.png", DEFAULT_ICONSIZE, "Align Top");			connect(mAlignTopButton, SIGNAL(clicked()), mGraphWidget, SLOT(OnAlignTop()));			toolbarWidgets.Add(mAlignTopButton);
	mAlignBottomButton	= new ImageButton("Images/Graph/AlignBottom.png", DEFAULT_ICONSIZE, "Align Bottom");	connect(mAlignBottomButton, SIGNAL(clicked()), mGraphWidget, SLOT(OnAlignBottom()));	toolbarWidgets.Add(mAlignBottomButton);

	// add play button
	mPlayButton = new ImageButton( "Images/Icons/Play.png", DEFAULT_ICONSIZE, "Start Engine");
	connect(mPlayButton, SIGNAL(clicked()), this, SLOT(OnPlayButtonClicked()));
	toolbarWidgets.Add(mPlayButton);
	mPlayButton->setVisible(false);

	// add pause button
	mPauseButton = new ImageButton( "Images/Icons/Pause.png", DEFAULT_ICONSIZE, "Pause Engine");
	connect(mPauseButton, SIGNAL(clicked()), this, SLOT(OnPauseButtonClicked()));
	toolbarWidgets.Add(mPauseButton);
	mPauseButton->setVisible(false);
	mPauseButton->hide();

	// add Stop button
	mStopButton = new ImageButton( "Images/Icons/Stop.png", DEFAULT_ICONSIZE, "Stop Engine");
	connect(mStopButton, SIGNAL(clicked()), this, SLOT(OnStopButtonClicked()));
	toolbarWidgets.Add(mStopButton);
	mStopButton->setVisible(false);
	mStopButton->hide();

	// add resync graph button
	mResyncButton = new ImageButton( "Images/Icons/Refresh.png", DEFAULT_ICONSIZE, "Resync Engine");
	connect(mResyncButton, SIGNAL(clicked()), this, SLOT(OnResyncButtonClicked()));
	toolbarWidgets.Add(mResyncButton);

	// TODO add reload button for reloading design from backend ("discard changes")

	// Show Errors
	mShowErrorsButton = new ImageButton( "Images/Icons/Error.png", DEFAULT_ICONSIZE, "Design has Errors (click to show)");
	connect(mShowErrorsButton, SIGNAL(clicked()), this, SLOT(OnShowErrorsButtonClicked()));
	toolbarWidgets.Add(mShowErrorsButton);
	mShowErrorsButton->setVisible(false);
	
	// Show warnings
	// TODO transform into general show-warning button instead of just 'deprecated'
	mShowWarningsButton = new ImageButton( "Images/Icons/Warning.png", DEFAULT_ICONSIZE, "Design has Warnings (click to show)"); // 
	connect(mShowWarningsButton, SIGNAL(clicked()), this, SLOT(OnShowWarningsButtonClicked()));
	toolbarWidgets.Add(mShowWarningsButton);
	mShowWarningsButton->setVisible(false);

	// Show bug
	// TODO move unstable to warnings??
	mUnstableIcon = new ImageButton( "Images/Icons/Bug.png", DEFAULT_ICONSIZE, "Contains Unstable Objects");
	toolbarWidgets.Add(mUnstableIcon);
	mUnstableIcon->setVisible(false);

	///////////////////////////////////////////////////////////////////////////
	// Fill everything
	///////////////////////////////////////////////////////////////////////////
	FillLayouts(mainWidget, mainLayout, toolbarWidgets, "Tools", "Tool", mGraphWidget);

	// create a new attribute widget callback and add it to the attribute widget factory
	LogDebug("Adding attribute widget callback ...");
	mAttributeWidgetCallback = new AttributeWidgetCallback(this);
	GetQtBaseManager()->GetAttributeWidgetFactory()->AddCallback( mAttributeWidgetCallback );

	// attach to event system
	LogDebug("Attaching graph widget to event system ...");
	CORE_EVENTMANAGER.AddEventHandler(this);

	// show the active classifier
	OnGraphChanged( NULL );

	mainWidget->show();

	LogInfo("Graph plugin successfully initialized");

	// automatically show active classifier/statemachine
	// show the specified graph
	switch (mType)
	{
		case GraphWidget::CLASSIFIER:	{ OnGraphChanged(GetEngine()->GetActiveClassifier()); break; }
		case GraphWidget::STATEMACHINE:	{ OnGraphChanged(GetEngine()->GetActiveStateMachine()); break; }
		default:						{ OnGraphChanged(GetEngine()->GetActiveClassifier()); break; }
	}

	return true;
}


// Update interface
void GraphPlugin::UpdateInterface()
{
	if (mInfoWidget != NULL && mInfoWidget->isVisible())
		mInfoWidget->UpdateInterface();

	// toolbar

	Graph* graph = mGraphWidget->GetGraph();
	GraphShared* shared = mGraphWidget->GetShared();
	if (graph != NULL && GetSession()->IsRunning() == false)
	{
		// hide save button if graph is readonly
		const bool canSave = graph->GetCreud().Update() && graph->GetUseSettings() == false;
		mSaveButton->setVisible(canSave);

		// FIXME not every action causes a change in IsDirty flag? (node attributes)
		// disable save button if graph was not modified yet
		//const bool wasModified = graph->IsDirty();
		//mSaveButton->setEnabled(wasModified);

		const bool canAlign = shared->GetNumSelectedNodes() > 1;
		mAlignLeftButton->setVisible(canAlign);
		mAlignRightButton->setVisible(canAlign);
		mAlignTopButton->setVisible(canAlign);
		mAlignBottomButton->setVisible(canAlign);
	}
	else
	{
		mSaveButton->setVisible(false);

		mAlignLeftButton->setVisible(false);
		mAlignRightButton->setVisible(false);
		mAlignTopButton->setVisible(false);
		mAlignBottomButton->setVisible(false);
	}

	// hide/show play/pause button
	// TODO replace with dedicated Classifier/Statemachine/EngineControl widget
	
	// update visibility 
	//const bool engineRunning = GetEngine()->IsRunning();
	//const bool enginePaused = false;// GetEngine()->IsPaused(); // TODO FIXME
	//mPlayButton->setHidden(engineRunning && !enginePaused);
	//mPauseButton->setHidden(! (engineRunning && !enginePaused));
	//mStopButton->setHidden(!engineRunning);

	// disable controls if session is running
	const bool sessionRunning = GetSession()->IsRunning();
	//mPlayButton->setEnabled(!sessionRunning);
	//mPauseButton->setEnabled(!sessionRunning);
	//mStopButton->setEnabled(!sessionRunning);
	mResyncButton->setEnabled(!sessionRunning);

	if (graph != NULL)
	{
		// show error button if graph has errors
		const bool graphHasErrors = graph->HasError();
		mShowErrorsButton->setVisible(graphHasErrors);
		if (graphHasErrors == false) // have to clear tooltips here
			mShowErrorsButton->setToolTip("");
		
		// show warning button
		const bool graphHasWarnings = graph->HasWarning();
		mShowWarningsButton->setVisible(graphHasWarnings);
		if (graphHasWarnings == false) // have to clear tooltips here
			mShowWarningsButton->setToolTip("");
	}
}


// called after the layout got loaded
void GraphPlugin::OnAfterLoadLayout()
{
	// show the specified graph
	switch(mType)
	{
		case GraphWidget::CLASSIFIER:	{ OnGraphChanged( GetEngine()->GetActiveClassifier() ); break; }
		case GraphWidget::STATEMACHINE:	{ OnGraphChanged( GetEngine()->GetActiveStateMachine() ); break; }
		default:						{ OnGraphChanged( GetEngine()->GetActiveClassifier() ); break; }
	}
}


void GraphPlugin::OnSave()
{
	if (mGraphWidget == NULL)
		return;

	Graph* graph = mGraphWidget->GetGraph();
	if (graph == NULL)
		return;

	// dont allow save in settings mode
	if (graph->GetUseSettings() == true)
		return;

	GetBackendInterface()->GetFileSystem()->SaveGraph( GetUser()->GetToken(), graph->GetUuid(), graph );

	// update title (dirty flag changed)
	SetTitleFromGraph(graph);
}


// EVENTS ----------------------------------------------

// called when the classifier got changed
void GraphPlugin::OnActiveClassifierChanged(Classifier* classifier)
{
	if (mType != GraphWidget::CLASSIFIER)
		return;

	OnGraphChanged(classifier);
}


// called when the state machine got changed
void GraphPlugin::OnActiveStateMachineChanged(StateMachine* stateMachine)
{
	if (mType != GraphWidget::STATEMACHINE)
		return;

	OnGraphChanged(stateMachine);
}

// -----------------------------------------------------

void GraphPlugin::OnGraphChanged(Graph* graph)
{
	// set the plugin name to the shown graph, if there is non preset show the plugin name
	SetTitleFromGraph(graph);

	if (mGraphWidget != NULL)
		mGraphWidget->InitForGraph(graph);

	if (mPaletteWidget != NULL)
		mPaletteWidget->Init();

	if (mAttributesWidget != NULL)
		mAttributesWidget->InitForGraphObject(graph);
}


void GraphPlugin::OnGraphModified(Graph* graph, GraphObject* object)
{
	// update plugin title from graph
	if (graph == mGraphWidget->GetGraph())
		SetTitleFromGraph(graph);
}


// construct and set the final graph plugin title
void GraphPlugin::SetTitleFromGraph(Graph* graph)
{
	String title;
	if (graph != NULL)
	{
		title.Format("%s - %s", GetName(), graph->GetName());

		// settings mode: classifier is not writeable, but experience is locked -> allow saving settings
		//const Experience* experience = GetEngine()->GetActiveExperience();
		const bool settingsMode = graph->GetUseSettings();

		// read only and modify exclude each other, so we just print one or the other 
		if (settingsMode)
			title += " (settings mode)";
		else if (graph->GetCreud().Update() == false)
			title += " (read-only)";
		
		if (graph->IsDirty() == true)
			title += " (modified)";
	}
	else
		title = GetName();

	SetTitle(title);
}


// attribute was changed by user using the attribute widget
void GraphPlugin::AttributeWidgetCallback::OnAttributeChanged(Core::Attribute* attribute)
{
	Graph* graph = NULL;

	// show the specified graph
	switch (mPlugin->GetType())
	{
		case GraphWidget::STATEMACHINE:	{ graph = GetEngine()->GetActiveStateMachine(); break; }
		case GraphWidget::CLASSIFIER:	{ graph = GetEngine()->GetActiveClassifier(); break; }
		default:						{ graph = GetEngine()->GetActiveClassifier(); break; }
	}

	if (graph == NULL)
		return;

	// forward event to graph
	if (graph->OnAttributeChanged(attribute) == true)
	{
		// graph contained the attribute, dirty flag might have changed
		mPlugin->SetTitleFromGraph(graph);
	}
}


void GraphPlugin::OnPlayButtonClicked()
{
	// TODO FIXME
	// start or unpause engine
	//if (GetEngine()->IsPaused() == true)
	//	GetEngine()->SetPaused(false);
	//else
	//	GetEngine()->Start();


	// hide button, show pause button
	//mPlayButton->hide();
	//mPauseButton->show();
}


void GraphPlugin::OnPauseButtonClicked()
{
	// TODO fixme
	//GetEngine()->SetPaused();
	
	// hide pause button, show play button
	//mPlayButton->show();
	//mPauseButton->hide();
}

void GraphPlugin::OnStopButtonClicked()
{
	// TODO fixme
	//GetEngine()->Stop();
}


// restart engine
void GraphPlugin::OnResyncButtonClicked()
{
	// reset engine and reload parameters
	// TODO fixme
	//GetEngine()->ScheduleResync();

	GetEngine()->Reset();

	// also reload parameters
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	Experience* experience = GetEngine()->GetActiveExperience();
	GetBackendInterface()->GetParameters()->Load(false, *GetSessionUser(), experience, classifier);
}


void GraphPlugin::OnShowErrorsButtonClicked()
{
	Graph* graph = mGraphWidget->GetGraph();
	if (graph == NULL)
		return;
	
	// show error button if graph has errors
	const bool graphHasErrors = graph->HasError();
	mShowErrorsButton->setVisible(graphHasErrors);
	if (graphHasErrors == true)
	{
		Array<String> errorMessages;
		graph->CreateErrorList(errorMessages);

		String joined;
		String::Join(errorMessages, "<br>", joined);
		mTempString.Format("<p style=\"color:red\"><b>Design has %i Errors:</b><br>%s</p>", errorMessages.Size(), joined.AsChar());
			
		mShowErrorsButton->setToolTip(mTempString.AsChar());
	}
	else
		mShowErrorsButton->setToolTip("");

	//QToolTip::showText(mShowErrorsButton->mapToGlobal( QPoint( 0, 0 ) ), mShowErrorsButton->toolTip());
}


void GraphPlugin::OnShowWarningsButtonClicked()
{
	Graph* graph = mGraphWidget->GetGraph();
	if (graph == NULL)
		return;

	const bool graphHasWarnings = graph->HasWarning();
	if (graphHasWarnings == true)
	{
		Array<String> warningMessages;
		graph->CreateWarningsList(warningMessages);

		String joined;
		String::Join(warningMessages, "<br>", joined);
		mTempString.Format("<p style=\"color:yellow\"><b>Design has %i Warnings:</b><br>%s</p>", warningMessages.Size(), joined.AsChar());
			
		mShowWarningsButton->setToolTip(mTempString.AsChar());
	}
	else
		mShowWarningsButton->setToolTip("");
	
	//QToolTip::showText(mShowWarningsButton->mapToGlobal( QPoint( 0, 0 ) ), mShowWarningsButton->toolTip());
}
