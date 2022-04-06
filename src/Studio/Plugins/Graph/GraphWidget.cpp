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

// include the required headers
#include "GraphWidget.h"
#include <Core/Timer.h>
#include <Core/EventManager.h>
#include <Core/Math.h>
#include <Graph/ProcessorNode.h>
#include <EngineManager.h>
#include "GraphPlugin.h"
#include "GraphRendererState.h"
#include "ColorPalette.h"
#include "GraphPaletteWidget.h"
#include "../../AppManager.h"
#include "../../MainWindow.h"
#include <QToolTip>
#include <QMenu>
#include <QOpenGLContext>
#include <QScreen>


using namespace Core;

// constructor
GraphWidget::GraphWidget(Type type, GraphPlugin* plugin, QWidget* parent) : QOpenGLWidget(parent)
{
	CORE_ASSERT(plugin != NULL);

	LogDebug("Constructing graph widget ...");

	mType					= type;
	mGraphPlugin			= plugin;
	connect(this, SIGNAL(SelectionChanged()), this, SLOT(OnSelectionChanged()));

	mAllowInteraction		= true;
	mDrawDebugInfo			= false;
	mRenderer				= NULL;

	// mouse handling
	mLocalMousePos			= QPoint( 0, 0 );
	mGlobalMousePos			= QPoint( 0, 0 );
	mMousePressEventNode	= NULL;
	mLeftButtonPressed		= false;
	mMidButtonPressed		= false;
	mRightButtonPressed		= false;
	mAltPressed				= false;
	mShiftPressed			= false;
	mControlPressed			= false;

	// selection
	mRectSelecting			= false;
	mSelectStart			= QPoint( 0, 0 );
	mSelectEnd				= QPoint( 0, 0 );

	mNoClassifierShownError			= "Please select a classifier from the back-end file system before starting a session";
	mNoStateMachineShownError			= "Please select a state machine from the back-end file system before starting a session";
	mGraphProtectionModeError	= "Due to security restrictions, the graph won't be visible.";

	setAutoFillBackground(false);

	// enable mouse tracking to capture mouse movements over the widget
	setMouseTracking( true );

	// gesture support
    //setAttribute( Qt::WA_AcceptTouchEvents, true );
    //setAttribute( Qt::WA_TouchPadAcceptSingleTouchEvents, true );
	//grabGesture( Qt::PanGesture );
	//grabGesture( Qt::PinchGesture );
	//grabGesture( Qt::SwipeGesture );

	// accept drag&drop
	setAcceptDrops( true );

	// setup to get focus when we click or use the mouse wheel
	setFocusPolicy( (Qt::FocusPolicy)(Qt::ClickFocus | Qt::WheelFocus) );

	setAutoFillBackground(false);

	// attach to event system
	CORE_EVENTMANAGER.AddEventHandler(this);

	connect( &mShared, SIGNAL(SelectionChanged()), this, SLOT(OnEmitSelectionChangedSignal()) );
}


// destructor
GraphWidget::~GraphWidget()
{
	LogDebug("Destructing graph widget ...");

	// delete temporary copyied objects
	ClearCopyBuffer();

	// detach from event system
	CORE_EVENTMANAGER.RemoveEventHandler(this);

	delete mRenderer;

	// unregister OpenGL widget
	GetOpenGLManager()->UnregisterOpenGLWidget(this);
}


void GraphWidget::paintGL()
{
	GetQtBaseManager()->GetMainWindow()->GetOpenGLFpsCounter().UnPause();

	mFpsCounter.BeginTiming();
	const double timeDelta = mTimer.GetTimeDelta().InSeconds();

	// set the screen information and update the scaling based on that
	QOpenGLContext* glContext = context();
	if (glContext != NULL)
	{
		QScreen* screen = glContext->screen();
		if (screen != NULL)
			mShared.SetScreenInfo( screen->physicalSize().width(), screen->physicalSize().height(), screen->physicalDotsPerInch() );
	}

	if (mShownGraph != NULL)
		mRenderer->ResetNodeCaches(mShownGraph);

	// allow editing, if classifier allows it and session is not running
	if (mShownGraph != NULL && GetSession()->IsRunning() == false)
		SetAllowInteraction(mShownGraph->AllowEditing());
	else
		SetAllowInteraction(false);

	mShared.Update(timeDelta);

	// start painting
	QPainter painter;
	painter.begin(this);

	painter.setRenderHint( QPainter::Antialiasing );
	painter.setRenderHint( QPainter::HighQualityAntialiasing );
	painter.setRenderHint( QPainter::TextAntialiasing );
	painter.setRenderHint( QPainter::SmoothPixmapTransform );

	// get the width and height
	const uint32	width		= this->width();
	const uint32	height		= this->height();
	const QRect		visibleRect	= QRect( 0,0, width, height );

	// fill the background
	painter.setBrush( mShared.GetBackgroundColor() );
	painter.setPen( Qt::NoPen );
	painter.drawRect( rect() );

	// setup the transform
	painter.setTransform( mShared.GetTransform() );

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// render the grid
	/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef GRAPH_PERFORMANCESTATS
	Timer gridTimer;
#endif
	mShared.RenderGrid( &mShared, painter, mShared.GetTransform(), mShared.GetScale(), mShared.GetGridColor(), mShared.GetSubGridColor(), width, height );
#ifdef GRAPH_PERFORMANCESTATS
	const float gridRenderTime = gridTimer.GetTime().InSeconds();
#endif

#ifdef GRAPH_PERFORMANCESTATS
	float connectionRenderTime	= 0.0f;
	float nodeRenderTime		= 0.0f;
#endif
	if (mShownGraph != NULL && mGraphProtectionMode == false)
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////
		// render connections
		/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef GRAPH_PERFORMANCESTATS
		Timer connectionsTimer;
#endif
		mRenderer->RenderConnections( mShownGraph, painter, mRelinkConnectionInfo, mOnMouseOverNode, mGlobalMousePos, isEnabled(), visibleRect );
#ifdef GRAPH_PERFORMANCESTATS
		connectionRenderTime = connectionsTimer.GetTime().InSeconds();
#endif

		/////////////////////////////////////////////////////////////////////////////////////////////////
		// render the nodes
		/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef GRAPH_PERFORMANCESTATS
		Timer nodesTimer;
#endif
		mRenderer->RenderNodes( mShownGraph, painter, mCreateConnectionInfo, isEnabled(), visibleRect, mGlobalMousePos );
#ifdef GRAPH_PERFORMANCESTATS
		nodeRenderTime = nodesTimer.GetTime().InSeconds();
#endif

		//////////////////////////////////////////

		// render the connection we are creating, if any
		if (mCreateConnectionInfo.IsCreatingConnection() == true)
			mRenderer->RenderCreateConnection( painter, mShownGraph, mCreateConnectionInfo, mGlobalMousePos );

		// render the connection that we're currently relinking
		if (mRelinkConnectionInfo.IsRelinking() == true)
			mRenderer->RenderRelinkConnection( mShownGraph, painter, mRelinkConnectionInfo, mGlobalMousePos );

		//////////////////////////////////////////

		// render the node debug text overlay
		if (mDrawDebugInfo == true)
			mRenderer->RenderNodeDebugInfo(mShownGraph, painter, mCreateConnectionInfo, isEnabled(), visibleRect, mGlobalMousePos);
	}
	else
	{
		painter.resetTransform();

		if (mGraphProtectionMode == true)
			mRenderer->RenderText( true, painter, mGraphProtectionModeError, mShared.GetErrorBlinkColor(), QRect(0, 0, width, height), mShared.GetStandardFont(), mShared.GetStandardFontMetrics(), Qt::AlignCenter );
		else
			// no graph active
			mRenderer->RenderText( true, painter, mType == CLASSIFIER ? mNoClassifierShownError : mNoStateMachineShownError, mShared.GetErrorBlinkColor(), QRect(0, 0, width, height), mShared.GetStandardFont(), mShared.GetStandardFontMetrics(), Qt::AlignCenter );
	}

	// render selection rect
	if (mRectSelecting == true)
	{
		// calculate the selection rect
		painter.resetTransform();
		QRect selectRect;
		CalcSelectRect( selectRect );

		// draw selection rect
		QColor rectSelectBGColor = mShared.GetSelectedColor();
		rectSelectBGColor.setAlphaF( 0.5f );
		painter.setBrush( rectSelectBGColor );
		painter.setPen( mShared.GetSelectedColor() );

		painter.drawRect( selectRect );
	}

	// draw the border
	//QColor borderColor = mShared.GetBackgroundColor();
	//if (hasFocus())
	//	borderColor = mShared.GetActiveBorderColor();

	painter.resetTransform();

	/*QPen pen( borderColor, hasFocus() ? 3 : 2 );
	painter.setPen( pen );
	painter.setBrush( Qt::NoBrush );

	painter.drawLine(0, 0, width, 0);
	painter.drawLine(width, 0, width, height);
	painter.drawLine(0, 0, 0, height);
	painter.drawLine(0, height, width, height);*/

	// stop painting
	painter.end();

	mFpsCounter.StopTiming();

	// render FPS counter
	if (GetQtBaseManager()->GetMainWindow()->GetShowFPS() == true || GetQtBaseManager()->GetMainWindow()->GetShowPerformanceInfo() == true)
	{
		// begin painting
		painter.begin(this);

		mRenderer->RenderText( false, painter, mFpsCounter.GetTextString(), ColorPalette::Shared::GetTextQColor(), QRect(5*mShared.GetScreenScaling(), (2+0*15)*mShared.GetScreenScaling(), 200*mShared.GetScreenScaling(), 15*mShared.GetScreenScaling()), mShared.GetStandardFont(), mShared.GetStandardFontMetrics(), Qt::AlignLeft );

#ifdef GRAPH_PERFORMANCESTATS
		if (GetQtBaseManager()->GetMainWindow()->GetShowPerformanceInfo() == true)
		{
			// grid render time
			float gridTheoreticalFPS = 0.0;
			if (IsClose<float>(gridRenderTime, 0.0f, Math::epsilon) == false)
				gridTheoreticalFPS = 1000.0f / (gridRenderTime * 1000.0f);
			mTempString.Format("Grid: %.1fms (%i FPS)", gridRenderTime * 1000.0f, (int)gridTheoreticalFPS);

			mRenderer->RenderText( false, painter, mTempString, ColorPalette::Shared::GetTextQColor(), QRect(5*mShared.GetScreenScaling(), (2+1*15)*mShared.GetScreenScaling(), 200*mShared.GetScreenScaling(), 15*mShared.GetScreenScaling()), mShared.GetStandardFont(), mShared.GetStandardFontMetrics(), Qt::AlignLeft );

			// connection render time
			float connectionTheoreticalFPS = 0.0;
			if (IsClose<float>(connectionRenderTime, 0.0f, Math::epsilon) == false)
				connectionTheoreticalFPS = 1000.0f / (connectionRenderTime * 1000.0f);
			mTempString.Format("Connections: %.1fms (%i FPS)", connectionRenderTime * 1000.0f, (int)connectionTheoreticalFPS);

			mRenderer->RenderText( false, painter, mTempString, ColorPalette::Shared::GetTextQColor(), QRect(5*mShared.GetScreenScaling(), (2+2*15)*mShared.GetScreenScaling(), 200*mShared.GetScreenScaling(), 15*mShared.GetScreenScaling()), mShared.GetStandardFont(), mShared.GetStandardFontMetrics(), Qt::AlignLeft );

			// nodes render time
			float nodeTheoreticalFPS = 0.0;
			if (IsClose<float>(nodeRenderTime, 0.0f, Math::epsilon) == false)
				nodeTheoreticalFPS = 1000.0f / (nodeRenderTime  * 1000.0f);
			mTempString.Format("Nodes: %.1fms (%i FPS)", nodeRenderTime * 1000.0f, (int)nodeTheoreticalFPS);

			mRenderer->RenderText( false, painter, mTempString, ColorPalette::Shared::GetTextQColor(), QRect(5*mShared.GetScreenScaling(), (2+3*15)*mShared.GetScreenScaling(), 200*mShared.GetScreenScaling(), 15*mShared.GetScreenScaling()), mShared.GetStandardFont(), mShared.GetStandardFontMetrics(), Qt::AlignLeft );

			mTempString.Format( "Cache: TextPixmaps=%i, StaticTexts=%i", mRenderer->GetPixmapTextCache()->GetNumCachedElements(), mRenderer->GetStaticTextCache()->GetNumCachedElements() );
			mRenderer->RenderText( false, painter, mTempString, ColorPalette::Shared::GetTextQColor(), QRect(5*mShared.GetScreenScaling(), (2+4*15)*mShared.GetScreenScaling(), 200*mShared.GetScreenScaling(), 15*mShared.GetScreenScaling()), mShared.GetStandardFont(), mShared.GetStandardFontMetrics(), Qt::AlignLeft );
		}
#endif

		// stop painting
		painter.end();
	}

	GetQtBaseManager()->GetMainWindow()->GetOpenGLFpsCounter().Pause();
}

//
//void GraphWidget::ShowGraph(const char* graphNodeName, bool updateHistory)
//{
//	// find the node based on the name
//	Node* node = mGraph->FindNodeByName(graphNodeName);
//	if (node == NULL)
//		return;
//
//	// check if the node is a graph itself
//	if (node->GetBaseType() != Graph::BASE_TYPE)
//		return;
//
//	// type-cast the node to a graph
//	Graph* newGraph = static_cast<Graph*>(node);
//
//	// show the new graph
//	ShowGraph( newGraph, updateHistory );
//}


void GraphWidget::ShowGraph(Graph* graph, bool updateHistory)
{
	LogDetailedInfo("Showing graph ...");

	mShownGraph = graph;
	mRectSelecting	= false;
	mOnMouseOverNode= NULL;

	delete mRenderer;
	mRenderer = NULL;

	if (mShownGraph != NULL && mShownGraph->GetType() == StateMachine::TYPE_ID)
		mRenderer = new GraphRendererState(&mShared);
	else
		mRenderer = new GraphRenderer(&mShared);

	mRenderer->ResetNodeCaches(mShownGraph);

	// reinitialize pixmap and static text caches
	mRenderer->GetPixmapTextCache()->Clear();
	mRenderer->GetPixmapTextCache()->Init();
	mRenderer->GetStaticTextCache()->Clear();
	mRenderer->GetStaticTextCache()->Init();

	mShared.ResetTransform();
	
	mShared.UnselectAllNodes();
	mShared.UnselectAllConnections();

	// NOTE: why 2x FitActiveGraphOnScreen on windows?
	FitActiveGraphOnScreen();

#ifdef NEUROMORE_PLATFORM_OSX
	CenterGraph(mShownGraph);
#else
	FitActiveGraphOnScreen();
#endif

	// update the navigation link widget
//	if (mNavigationLinkWidget != NULL)
//		mNavigationLinkWidget->Update(mGraph, mShownGraph, updateHistory);
}


void GraphWidget::InitForGraph(Graph* graph)
{
	LogDetailedInfo("Initializing graph ...");

	if (graph != NULL)
	{
		if (graph->GetCreud().Read() == true)
		{
			mGraph = graph;
			mGraphProtectionMode = false;
		}
		else
		{
			mGraph = NULL;
			mGraphProtectionMode = true;
		}
	}
	else
		mGraph = NULL;

	ShowGraph(mGraph);

	// set pivot element (otherwise its (0,0) until the first resize event happens)
	mShared.SetScalePivot( QPoint( width()/2, height()/2) );

}


// calculate the selection rect
void GraphWidget::CalcSelectRect(QRect& outRect)
{
	const int32 startX = Min<int32>( mSelectStart.x(), mSelectEnd.x() );
	const int32 startY = Min<int32>( mSelectStart.y(), mSelectEnd.y() );
	const int32 width  = abs( mSelectEnd.x() - mSelectStart.x() );
	const int32 height = abs( mSelectEnd.y() - mSelectStart.y() );

	outRect = QRect(startX, startY, width, height);
}


// called when mouse moved
void GraphWidget::mouseMoveEvent(QMouseEvent* event)
{
	// update control modifier flags
	mAltPressed			= event->modifiers() & Qt::AltModifier;
	mShiftPressed		= event->modifiers() & Qt::ShiftModifier;
	mControlPressed		= event->modifiers() & Qt::ControlModifier;

	// get the mouse position and calculate the global mouse position
	QPoint delta	= event->pos() - mLocalMousePos;
	mLocalMousePos	= event->pos();
	mGlobalMousePos	= GraphHelpers::LocalToGlobal( mShared.GetTransform(), mLocalMousePos );

	// accumulate the number of pixels moved since the last click
	const int32 numPixelsMoved = (int32)Math::Abs(delta.x()) + (int32)Math::Abs(delta.y());
	if (mLeftButtonPressed == false && mMidButtonPressed == false && mRightButtonPressed == true && mAltPressed == false)
		mPixelsMovedSinceRightClick += numPixelsMoved;
	mPixelsMovedSinceClick += numPixelsMoved;

	// find node under currently pressed mouse position
	mOnMouseOverNode = FindNode(mGlobalMousePos);

	//LogInfo( "MousePos: local=(%i, %i), global=(%i, %i)", mLocalMousePos.x(), mLocalMousePos.y(), mGlobalMousePos.x(), mGlobalMousePos.y() );

	// if we are panning
	if ((mMidButtonPressed == true || mRightButtonPressed == true) && mLeftButtonPressed == false)
	{
		// handle mouse wrapping, to enable smoother panning
		bool mouseWrapped = false;
		if (event->x() > (int32)width())
		{
			mouseWrapped = true;
			QCursor::setPos( QPoint(event->globalX() - width(), event->globalY()) );
			mLocalMousePos = QPoint(event->x() - width(), event->y());
		}
		else if (event->x() < 0)
		{
			mouseWrapped = true;
			QCursor::setPos( QPoint(event->globalX() + width(), event->globalY()) );
			mLocalMousePos = QPoint( event->x() + width(), event->y() );
		}

		if (event->y() > (int32)height())
		{
			mouseWrapped = true;
			QCursor::setPos( QPoint(event->globalX(), event->globalY() - height()) );
			mLocalMousePos = QPoint( event->x(), event->y()-height() );
		}
		else if (event->y() < 0)
		{
			mouseWrapped = true;
			QCursor::setPos( QPoint(event->globalX(), event->globalY() + height()) );
			mLocalMousePos = QPoint( event->x(), event->y() + height() );
		}

		// don't apply the delta, if mouse has been wrapped
		if (mouseWrapped == true)
			delta = QPoint(0, 0);

        ApplyScroll(delta.x(), delta.y() );
		setCursor( Qt::ClosedHandCursor );
	}

	// in case we are rect selecting
	if (mRectSelecting == true && mCreateConnectionInfo.IsCreatingConnection() == false && mRelinkConnectionInfo.IsRelinking() == false)
	{
		mSelectEnd = mLocalMousePos;
	}

	// in case we pressed on the node
	if (mAllowInteraction == true && mMousePressEventNode != NULL && mCreateConnectionInfo.IsCreatingConnection() == false && mRelinkConnectionInfo.IsRelinking() == false && mMidButtonPressed == false && mRightButtonPressed == false)
	{
		// move all selected nodes
		const int32 moveX = delta.x() * (1.0f/mShared.GetScale());
		const int32 moveY = delta.y() * (1.0f/mShared.GetScale());

		// get the number of selected (unlocked) nodes and iterate through them
		const uint32 numSelectedNodes = mShared.GetNumSelectedNodes();
		for (uint32 i=0; i<numSelectedNodes; ++i)
		{
			Node* node = mShared.GetSelectedNode(i);

			// skip locked nodes
			if (node->IsLocked() == true)
				continue;

			// calculate the new node position and set it
			const int32 newPosX = node->GetVisualPosX() + moveX;
			const int32 newPosY = node->GetVisualPosY() + moveY;
			node->SetVisualPos( newPosX, newPosY );
		}
	}
}


// called when mouse button got pressed
void GraphWidget::mousePressEvent(QMouseEvent* event)
{
	// update the mouse buttons and control modifier flags
	mLeftButtonPressed	= event->buttons() == Qt::LeftButton;
	mMidButtonPressed	= event->buttons() == Qt::MiddleButton;
	mRightButtonPressed	= event->buttons() == Qt::RightButton;
	mAltPressed			= event->modifiers() & Qt::AltModifier;
	mShiftPressed		= event->modifiers() & Qt::ShiftModifier;
	mControlPressed		= event->modifiers() & Qt::ControlModifier;

	// reset the number of pixels moved since the last click
	mPixelsMovedSinceClick		= 0;
	mPixelsMovedSinceRightClick = 0;

	// find node under currently pressed mouse position
	mMousePressEventNode = FindNode(mGlobalMousePos);

	// if we press the left or right mouse button
	if ((mLeftButtonPressed == true || mRightButtonPressed == true ) && mMidButtonPressed == false)
	{
		/////////////////////////////////////////////////////////////////////////////////
		// 1. check if we clicked on a node, if yes select it
		/////////////////////////////////////////////////////////////////////////////////
		if (mMousePressEventNode != NULL)
		{
			// check if the arrow of the clicked node got clicked with the left mouse button
			bool arrowClicked = mRenderer->IsPointOverNodeIcon( mShownGraph, mMousePressEventNode, mGlobalMousePos );
			if (mAllowInteraction == true && arrowClicked == true && mLeftButtonPressed == true && mMousePressEventNode->IsLocked() == false)
			{
				// FIXME VIP collapse state needs more features: calculate port position easily (everything already works, except the drawing of connections)
				// cycle through the collapsed state
				const uint32 collapsedState = mMousePressEventNode->GetCollapsedState();
				//const uint32 newCollapsedState = (collapsedState + 1) % Node::NUM_COLLAPSED_STATES;

				// just toggle between the two states
				const Node::ECollapsedState newCollapsedState = (collapsedState == Node::COLLAPSE_ALL) ?  Node::COLLAPSE_NONE : Node::COLLAPSE_ALL;
				mMousePressEventNode->SetCollapsedState( (Node::ECollapsedState)newCollapsedState);

				// unselect all
				mShared.UnselectAllNodes();
				mShared.UnselectAllConnections();
			}
			else
			{
				// only allow reselection in case we click on a new node
				if (mShared.IsNodeSelected(mMousePressEventNode) == false)
				{
					// unselect all
					mShared.UnselectAllNodes();
					mShared.UnselectAllConnections();
				
					// select the clicked node
					mShared.SelectNode(mMousePressEventNode);
				}
			}
		}
		else
		{
			// unselect all
			mShared.UnselectAllNodes();
			mShared.UnselectAllConnections();

			// in case no node got clicked, check if we clicked on a connection
			Connection* connection = mRenderer->FindConnection( mShownGraph, mGlobalMousePos );
			if (connection != NULL)
			{
				// select the clicked connection
				mShared.SelectConnection( connection );
			}
		}

		/////////////////////////////////////////////////////////////////////////////////
		// 2. create new connection
		/////////////////////////////////////////////////////////////////////////////////

		Node*		portNode	= NULL;
		uint32		portNr		= CORE_INVALIDINDEX32;
		bool		isInputPort	= false;
		Port*		port = mRenderer->FindPort( mShownGraph, mGlobalMousePos.x(), mGlobalMousePos.y(), &portNode, &portNr, &isInputPort );
		Connection* connection = mRenderer->FindConnection( mShownGraph, mGlobalMousePos );

		// in case the mouse is currently over a connection
		if (mMousePressEventNode == NULL && portNode == NULL && connection != NULL && mRenderer->AllowCreateConnectionsFromConnections() == true)
		{
			portNode = connection->GetSourceNode();
			portNr = connection->GetSourcePort();
			port = &portNode->GetOutputPort(portNr);
			isInputPort = false;
		}
		
		// in case the mouse is currently over a port 
		if (portNode != NULL)
		{
			
			if (mAllowInteraction == true && port != NULL)
			{
				if (isInputPort == false || (isInputPort == true && mRenderer->AllowCreateConnectionsStartingFromInputPorts() == true))
				{
					QRect portNodeRect = mRenderer->CalcNodeRect(mShownGraph, portNode);
					QPoint offset = mGlobalMousePos - portNodeRect.topLeft();
					mCreateConnectionInfo.StartCreateConnection(portNr, isInputPort, portNode, port, offset);
				}
			}
		}
		// in case we are currently in empty space
		else if (mMousePressEventNode == NULL) 
		{
			// start rect selection if graph is editable
			if (mMidButtonPressed == false && mRightButtonPressed == false && GetAllowInteraction() == true)
			{
				mRectSelecting = true;
				mSelectStart = mLocalMousePos;
				mSelectEnd = mLocalMousePos;
			}
		}

		/////////////////////////////////////////////////////////////////////////////////
		// 3. relink connection
		/////////////////////////////////////////////////////////////////////////////////
		if (connection != NULL && connection->GetType() == StateTransition::TYPE_ID && mMousePressEventNode == NULL)
		{
			// relink head (arrow,end,target)?
			const bool isCloseToHead = mRenderer->IsCloseToHead( connection, mGlobalMousePos );
			if (isCloseToHead == true)
				mRelinkConnectionInfo.StartRelinkHead( connection );

			// relink tail (start,source)?
			const bool isCloseToTail = mRenderer->IsCloseToTail( connection, mGlobalMousePos );
			if (isCloseToTail == true)
				mRelinkConnectionInfo.StartRelinkTail( connection );
		}
	}
}


// called when mouse button got released
void GraphWidget::mouseReleaseEvent(QMouseEvent* event)
{
	// check if the mouse moved
	const int32	deadZonePixels	= 5;
	const bool	mouseMovedAfterRightClick = mPixelsMovedSinceRightClick > deadZonePixels;
	const bool	mouseMovedAfterClick = mPixelsMovedSinceClick > deadZonePixels;

	// context menu handling
	if (mAllowInteraction == true && mRightButtonPressed == true && mouseMovedAfterRightClick == false)
		OnContextMenuEvent();

	// find node under currently pressed mouse position
	Node* mouseReleaseEventNode = FindNode(mGlobalMousePos);


	//////////////////////////////////
	// in case mouse did NOT move since the last click
	if (mouseMovedAfterClick == false)
	{
		// only allow reselection in case we click on node that was already part of a multi-selection
		if (mShared.IsNodeSelected(mouseReleaseEventNode) == true && mShared.GetNumSelectedNodes() > 1)
		{
			// unselect all
			mShared.UnselectAllNodes();
			mShared.UnselectAllConnections();
				
			// select the clicked node
			mShared.SelectNode(mouseReleaseEventNode);
		}
	}
	//////////////////////////////////
	// in case mouse moved since the last click
	else
	{
		// in case we clicked on a port and want to create a new connection
		if (mCreateConnectionInfo.IsCreatingConnection() == true)
		{
			// get the information of the port where we released the mouse
			uint32		targetPortNr;
			bool		targetIsInputPort;
			Node*		targetNode;
			Port*		port = mRenderer->FindPort( mShownGraph, mGlobalMousePos.x(), mGlobalMousePos.y(), &targetNode, &targetPortNr, &targetIsInputPort );

			// get the information of the port where we started
			Node*		sourceNode			= mCreateConnectionInfo.mConNode;
			int32		sourcePortNr		= mCreateConnectionInfo.mConPortNr;
			//bool		sourceIsInputPort	= mCreateConnectionInfo.mConIsInputPort;

			if (mShownGraph->GetType() == Classifier::TYPE_ID)
			{
				// only create a new connection in case the port is valid and the port that we started with is on the opposite side
				if (port != NULL &&
					mCreateConnectionInfo.mConIsInputPort != targetIsInputPort &&				// source and target ports must not be identical
					mCreateConnectionInfo.mConNode != targetNode &&								// source and target nodes must not be identical
					mCreateConnectionInfo.mConPort->IsCompatibleWith(*port) == true &&			// ports are must be commpatible
					sourceNode->FindNodeOnInputs(targetNode) == false &&						// target node is not a parent of the source node (prevents cyclic graphs)
					targetNode->FindNodeOnOutputs(sourceNode) == false )							// target node is not a child of the source node (prevents cyclic graphs)
				{

					QRect targetNodeRect = mRenderer->CalcNodeRect( mShownGraph, targetNode );
					QPoint endOffset = mGlobalMousePos - targetNodeRect.topLeft();

					// create the connection if it doesn't exist already
					if (targetIsInputPort == true)
					{
						if (mShownGraph->HasInputConnection(targetNode, targetPortNr) == false)
							mShownGraph->AddConnection(sourceNode, sourcePortNr, targetNode, targetPortNr, mCreateConnectionInfo.mConStartOffset.x(), mCreateConnectionInfo.mConStartOffset.y(), endOffset.x(), endOffset.y());
					}
					else
					{
						if (mShownGraph->HasInputConnection(sourceNode, sourcePortNr) == false)
							mShownGraph->AddConnection(targetNode, targetPortNr, sourceNode, sourcePortNr, mCreateConnectionInfo.mConStartOffset.x(), mCreateConnectionInfo.mConStartOffset.y(), endOffset.x(), endOffset.y());
					}
				}
			}
			else if (mShownGraph->GetType() == StateMachine::TYPE_ID)
			{
				if (port != NULL && targetNode != NULL && mCreateConnectionInfo.mConNode != targetNode)
				{
					QRect targetNodeRect = mRenderer->CalcNodeRect( mShownGraph, targetNode );
					QPoint endOffset = mGlobalMousePos - targetNodeRect.topLeft();

					mShownGraph->AddConnection(sourceNode, sourcePortNr, targetNode, targetPortNr, mCreateConnectionInfo.mConStartOffset.x(), mCreateConnectionInfo.mConStartOffset.y(), endOffset.x(), endOffset.y());
				}
			}
		}

		// in case we were rect selecting
		if (mRectSelecting == true && mCreateConnectionInfo.IsCreatingConnection() == false)
		{
			// calculate the selection rect
			QRect selectRect;
			CalcSelectRect( selectRect );

			// select things inside it
			if (selectRect.isEmpty() == false)
			{
				selectRect = mShared.GetTransform().inverted().mapRect( selectRect );

				// select nodes when alt is not pressed
				const bool overwriteSelection = (mControlPressed == false);
				mRenderer->SelectNodesAndConnectionsInRect( mShownGraph, selectRect, overwriteSelection, true, mControlPressed );
			}
		}
	}

	// finish relinking
	if (mRelinkConnectionInfo.IsRelinking() == true)
	{
		Node*		portNode	= NULL;
		uint32		portNr		= CORE_INVALIDINDEX32;
		bool		isInputPort	= false;
		/*Port*		port = */mRenderer->FindPort( mShownGraph, mGlobalMousePos.x(), mGlobalMousePos.y(), &portNode, &portNr, &isInputPort );

		// relinked transition head
		if (portNode != NULL && mRelinkConnectionInfo.IsRelinkingHead() == true && mRelinkConnectionInfo.GetRelinkHeadConnection() != NULL && mRelinkConnectionInfo.GetRelinkHeadConnection()->GetType() == StateTransition::TYPE_ID)
		{
			StateTransition* transition = static_cast<StateTransition*>( mRelinkConnectionInfo.GetRelinkHeadConnection() );

			// don't allow the source node being the target node
			if (transition->GetSourceNode() != portNode)
			{
				transition->SetTargetNode( portNode );

				QPoint newEndOffset = mGlobalMousePos - QPoint(portNode->GetVisualPosX(), portNode->GetVisualPosY());
				transition->SetVisualOffsets( transition->GetVisualStartOffsetX(), transition->GetVisualStartOffsetY(), newEndOffset.x(), newEndOffset.y() );
			}
		}

		// relinked transition tail
		if (portNode != NULL && mRelinkConnectionInfo.IsRelinkingTail() == true && mRelinkConnectionInfo.GetRelinkTailConnection() != NULL && mRelinkConnectionInfo.GetRelinkTailConnection()->GetType() == StateTransition::TYPE_ID)
		{
			StateTransition* transition = static_cast<StateTransition*>( mRelinkConnectionInfo.GetRelinkTailConnection() );

			// don't allow the source node being the target node
			if (transition->GetTargetNode() != portNode)
			{
				transition->SetSourceNode( portNode );

				QPoint newStartOffset = mGlobalMousePos - QPoint(portNode->GetVisualPosX(), portNode->GetVisualPosY());
				transition->SetVisualOffsets( newStartOffset.x(), newStartOffset.y(), transition->GetVisualEndOffsetX(), transition->GetVisualEndOffsetY() );
			}
		}
	}

	// update the mouse buttons and control modifier flags
	mLeftButtonPressed	= event->buttons() == Qt::LeftButton;
	mMidButtonPressed	= event->buttons() == Qt::MiddleButton;
	mRightButtonPressed	= event->buttons() == Qt::RightButton;
	mAltPressed			= event->modifiers() & Qt::AltModifier;
	mShiftPressed		= event->modifiers() & Qt::ShiftModifier;
	mControlPressed		= event->modifiers() & Qt::ControlModifier;

	// reset all temporary settings
	setCursor( Qt::ArrowCursor );
	mRectSelecting			= false;
	mMousePressEventNode	= NULL;
	mCreateConnectionInfo.StopCreateConnection();
	mRelinkConnectionInfo.StopRelinking();
}


// called when mouse button got double clicked
void GraphWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	// update the mouse buttons and control modifier flags
	mLeftButtonPressed	= event->buttons() == Qt::LeftButton;
	mMidButtonPressed	= event->buttons() == Qt::MiddleButton;
	mRightButtonPressed	= event->buttons() == Qt::RightButton;
	mAltPressed			= event->modifiers() & Qt::AltModifier;
	mShiftPressed		= event->modifiers() & Qt::ShiftModifier;
	mControlPressed		= event->modifiers() & Qt::ControlModifier;

	if (mShownGraph != NULL)
	{
		// find the node under the mouse cursor
		Node* doubleClickedNode = FindNode(mGlobalMousePos);
		if (doubleClickedNode != NULL)
		{
			// Note: double click can be replaced with "Decend Into Node" the moment have hierarchical graphs; for now we just use it to pop open the attributes pane
			//// check if the double clicked node is a graph itself
			//if (doubleClickedNode->GetBaseType() == Graph::BASE_TYPE)
			//{
			//	// type-cast the double clicked node to a graph
			//	Graph* newGraph = static_cast<Graph*>(doubleClickedNode);
			//	
			//	// show the double clicked graph
			//	ShowGraph(newGraph);
			//}

			// show settings panel if not already and move focus to graph attributes
			if (mGraphPlugin->GetSettingsVisibility() == false)
			{
				mGraphPlugin->OnOpenCloseSettings();
				mGraphPlugin->GetGraphAttributesWidget()->setFocus();
			}
		}
	}

	event->accept();

	// reset all temporary settings
	setCursor( Qt::ArrowCursor );
	mRectSelecting			= false;
	mMousePressEventNode	= NULL;
	mCreateConnectionInfo.StopCreateConnection();
}


// called when the mouse wheel got spinned
void GraphWidget::wheelEvent(QWheelEvent* event)
{
    QPoint		numPixels	= event->pixelDelta();
    //QPoint		numDegrees	= event->angleDelta() / 8;
	const float scaleDelta	= (event->delta() / 120.0f) * 0.05f;

#ifdef NEUROMORE_PLATFORM_OSX
    //LogInfo("GraphWidget::wheelEvent(): x=%i, y=%i", numPixels.x(), numPixels.y() );
    
    if ((event->modifiers() & Qt::AltModifier) == false)
		ApplyScroll( numPixels.x(), numPixels.y() );
	else
		ApplyScale( scaleDelta );

#else
	// only handle vertical events
	if (event->orientation() != Qt::Vertical)
		return;

	
	ApplyScale( scaleDelta );
#endif
}


void GraphWidget::ApplyScale(float scaleDelta)
{
	// calculate the new scale value
	const float lowestScale = 0.05f;
	mShared.SetScale( Clamp<float>(mShared.GetScale() + scaleDelta, lowestScale, 1.0f), true );
}


void GraphWidget::ApplyScroll(int deltaX, int deltaY)
{
    mShared.SetScrollOffset( mShared.GetScrollOffset() + QPoint(deltaX, deltaY) * (1.0f/mShared.GetScale()) );
}


// called when a key got pressed
void GraphWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_A)
	{
		FitActiveGraphOnScreen();
		event->accept();
		return;
	}

	if (event->key() == Qt::Key_Left)
	{
		OnAlignLeft();
		event->accept();
		return;
	}

	if (event->key() == Qt::Key_Right)
	{
		OnAlignRight();
		event->accept();
		return;
	}

	if (event->key() == Qt::Key_Up)
	{
		OnAlignTop();
		event->accept();
		return;
	}

	if (event->key() == Qt::Key_Down)
	{
		OnAlignBottom();
		event->accept();
		return;
	}

#ifdef NEUROMORE_PLATFORM_OSX
	if (event->key() == Qt::Key_Backspace)
#else
	if (event->key() == Qt::Key_Delete)
#endif
	{
		OnRemoveSelected();
		event->accept();
		return;
	}

	if (event->key() == Qt::Key_X && event->modifiers() & Qt::ControlModifier)
	{
		OnCutObjects();
		event->accept();
		return;
	}

	if (event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier)
	{
		OnCopyObjects();
		event->accept();
		return;
	}

	if (event->key() == Qt::Key_V && event->modifiers() & Qt::ControlModifier)
	{
		OnPasteObjects();
		event->accept();
		return;
	}


#ifndef PRODUCTION_BUILD
	if (event->key() == Qt::Key_D)
	{
		// toggle debug view
		mDrawDebugInfo = !mDrawDebugInfo;
		event->accept();
		return;
	}
#endif

	return QOpenGLWidget::keyPressEvent(event);
}


// called when a key got released
void GraphWidget::keyReleaseEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_A)															{ event->accept(); return; }
	if (event->key() == Qt::Key_Left)														{ event->accept(); return; }
	if (event->key() == Qt::Key_Right)														{ event->accept(); return; }
	if (event->key() == Qt::Key_Up == true)													{ event->accept(); return; }
	if (event->key() == Qt::Key_Down == true)												{ event->accept(); return; }
	if (event->key() == Qt::Key_Delete == true)												{ event->accept(); return; }
	if (event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier)				{ event->accept(); return; }
	if (event->key() == Qt::Key_V && event->modifiers() & Qt::ControlModifier)				{ event->accept(); return; }
	if (event->key() == Qt::Key_X && event->modifiers() & Qt::ControlModifier)				{ event->accept(); return; }

#ifndef PRODUCTION_BUILD
	if (event->key() == Qt::Key_D)															{ event->accept(); return; }
#endif

	return QOpenGLWidget::keyReleaseEvent(event);
}


void GraphWidget::initializeGL()
{
	LogDetailedInfo("Initializing OpenGL for graph widget ...");

	// base class initialize
	QOpenGLWidget::initializeGL();

	initializeOpenGLFunctions();

	// re-register OpenGL widget
	GetOpenGLManager()->UnregisterOpenGLWidget(this);
	GetOpenGLManager()->RegisterOpenGLWidget(this);
}


// called when the widget got resized
void GraphWidget::resizeGL(int width, int height)
{
	QOpenGLWidget::resizeGL(width, height);

//	const int width		= event->size().width();
//	const int height	= event->size().height();

/*	mShared.SetScalePivot( QPoint( event->size().width()/2, event->size().height()/2 ) );

	QPoint	scrollOffset	= mShared.GetScrollOffset();
	int32	scrollOffsetX	= scrollOffset.x();
	int32	scrollOffsetY	= scrollOffset.y();

	// calculate the size delta
	QPoint	oldSize			= QPoint( event->oldSize().width(), event->oldSize().height() );
	QPoint	size			= QPoint( event->size().width(), event->size().height() );
	QPoint	diff			= oldSize-size;

	static QPoint sizeDiff(0,0);
	sizeDiff += diff;

	const int32 sizeDiffX		= sizeDiff.x();
	const int32 halfSizeDiffX	= sizeDiffX/2;

	const int32 sizeDiffY		= sizeDiff.y();
	const int32 halfSizeDiffY	= sizeDiffY/2;

	if (halfSizeDiffX != 0)
	{
		scrollOffsetX -= halfSizeDiffX;
		const int32 modRes = halfSizeDiffX % 2;
		sizeDiff.setX( modRes );
	}

	if (halfSizeDiffY != 0)
	{
		scrollOffsetY -= halfSizeDiffY;
		const int32 modRes = halfSizeDiffY % 2;
		sizeDiff.setY( modRes );
	}

	mShared.SetScrollOffset( QPoint(scrollOffsetX, scrollOffsetY) );*/
}


// called when the selected nodes should be removed
void GraphWidget::OnRemoveSelected()
{
	// only allow removal in case interaction is allowed
	if (mAllowInteraction == false)
		return;

	// remove selected nodes
	Array<Node*> selectedNodes = mShared.GetSelectedNodesArray();
	const uint32 numSelectedNodes = selectedNodes.Size();
	for (uint32 i = 0; i < numSelectedNodes; ++i)
	{
		// skip locked nodes
		if (selectedNodes[i]->IsLocked() == true)
			continue;

		mShownGraph->RemoveNode( selectedNodes[i] );
	}

	// remove selected connections
	Array<Connection*> selectedConnections = mShared.GetSelectedConnectionsArray();
	const uint32 numSelectedConnections = selectedConnections.Size();
	for (uint32 i = 0; i < numSelectedConnections; ++i)
	{
		// skip locked connections
		if (selectedConnections[i]->IsLocked() == true)
			continue;

		mShownGraph->RemoveConnection(selectedConnections[i]);
	}
}


// Development feature: called when the Reset menu was clicked on selected nodes or connections
void GraphWidget::OnResetSelected()
{
	// reset nodes
	Array<Node*> selectedNodes = mShared.GetSelectedNodesArray();
	const uint32 numSelectedNodes = selectedNodes.Size();
	for (uint32 i = 0; i < numSelectedNodes; ++i)
	{
		if (selectedNodes[i]->IsLocked() == true)
			continue;

		selectedNodes[i]->ResetAsync();
	}

	/*
	// get channelset from connection and reset it
	Array<Connection*> selectedConnections = mShared.GetSelectedConnectionsArray();
	const uint32 numSelectedConnections = selectedConnections.Size();
	for (uint32 i = 0; i < numSelectedConnections; ++i)
	{
		Node* node = selectedConnections[i]->GetSourceNode();
		uint16 port = selectedConnections[i]->GetSourcePort();
		MultiChannel* output = node->GetOutputPort(port).GetChannels();
		if (output != NULL)
			output->Reset();

		if (selectedConnections[i]->IsLocked() == true)
			continue;

		// reset connection
		selectedConnections[i]->Reset();
	}
	*/
}


// called when the selected nodes should be removed
void GraphWidget::OnEnableDisableSelected()
{
	// only allow in case interaction is allowed
	if (mAllowInteraction == false)
		return;

	// count number of enabled nodes withing the selected nodes
	Array<Node*> selectedNodes = mShared.GetSelectedNodesArray();
	const uint32 numSelectedNodes = selectedNodes.Size();
	uint32 numEnabledNodes = 0;	
	for (uint32 i = 0; i < numSelectedNodes; ++i)
		if (selectedNodes[i]->IsEnabled() == true)
			numEnabledNodes++;

	// get desired toggle state
	bool nextEnableState = true;
	if (numEnabledNodes > numSelectedNodes / 2.0)
		nextEnableState = false;

	// enable/disable nodes
	for (uint32 i = 0; i < numSelectedNodes; ++i)
	{
		if (selectedNodes[i]->IsLocked() == true)
			continue;

		selectedNodes[i]->SetEnabled(nextEnableState);
	}

	// enable/disable connections inbetween selected nodes
	Array<Connection*> selectedConnections = mShared.GetSelectedConnectionsArray();
	const uint32 numSelectedConnections = selectedConnections.Size();
	for (uint32 i = 0; i < numSelectedConnections; ++i)
	{
		// filter out connections that are not connected to two selected nodes
		if (selectedNodes.Contains(selectedConnections[i]->GetSourceNode()) == false || 
			selectedNodes.Contains(selectedConnections[i]->GetTargetNode()) == false)
			continue;

		if (selectedConnections[i]->IsLocked() == true)
			continue;

		selectedConnections[i]->SetEnabled(nextEnableState);
	}
}


// called when the selected nodes should be removed
void GraphWidget::OnLockUnlockSelected()
{
	// only allow in case interaction is allowed
	if (mAllowInteraction == false)
		return;

	// count number of locked nodes within the selected nodes
	Array<Node*> selectedNodes = mShared.GetSelectedNodesArray();
	const uint32 numSelectedNodes = selectedNodes.Size();
	uint32 numLockedNodes = 0;	
	for (uint32 i = 0; i < numSelectedNodes; ++i)
		if (selectedNodes[i]->IsLocked() == true)
			numLockedNodes++;

	// get desired lock state
	bool nextLockState = true;
	if (numLockedNodes > numSelectedNodes / 2.0)
		nextLockState = false;

	// lock/unlock nodes
	for (uint32 i = 0; i < numSelectedNodes; ++i)
		selectedNodes[i]->SetLocked(nextLockState);

	// lock/unlock connections inbetween selected nodes
	Array<Connection*> selectedConnections = mShared.GetSelectedConnectionsArray();
	const uint32 numSelectedConnections = selectedConnections.Size();
	for (uint32 i = 0; i < numSelectedConnections; ++i)
	{
		// filter out connections that are not connected to two selected nodes
		if (selectedNodes.Contains(selectedConnections[i]->GetSourceNode()) == false || 
			selectedNodes.Contains(selectedConnections[i]->GetTargetNode()) == false)
			continue;

		selectedConnections[i]->SetLocked(nextLockState);
	}
}


// called when we create a new node with the context menu
void GraphWidget::OnContextMenuCreateNode()
{
	assert( sender()->inherits("QAction") == true );
	QAction* action = qobject_cast<QAction*>( sender() );

	// get the graph object factory
	GraphObjectFactory* objectFactory = GetEngine()->GetGraphObjectFactory();

	// get the type id from the action and create the node by type
	String typeUuid = FromQtString( action->property("typeUuid").toString() );
	GraphObject* object = objectFactory->CreateObjectByTypeUuid( mShownGraph, typeUuid.AsChar() );

	if (object == NULL)
		return;

	Node* node = static_cast<Node*>(object);

	// check create permission
	const bool create = GraphHelpers::GetCreud(node).Create();
	if (create == false)
	{
		String upgradeReason;
		upgradeReason.Format( "%s is not available in %s. To enjoy this feature, please upgrade to one of our subscription plans.", node->GetReadableType(), GetManager()->GetAppName().AsChar() );
		GetMainWindow()->ShowUpgradeWindow( upgradeReason.AsChar() );
		return;
	}

	// set the node position
	const QPoint nodePos = mGlobalMousePos;
	node->SetVisualPos( nodePos.x(), nodePos.y() );

	// set the node name
	node->SetName( node->GetReadableType() );

	// 4. reset
	node->Reset();

	// add the new node to the graph
	mShownGraph->AddNode( node );
}


// creates the context menu 
void GraphWidget::OnContextMenuEvent()
{
	// check if the graph is active
	if (mShownGraph == NULL)
		return;

	// create the context menu
	QMenu menu(this);

	// only add the create node menu entry in case we didn't click on a node
	if (mMousePressEventNode == NULL)
	{
		// get the graph object factory
		GraphObjectFactory* objectFactory = GetEngine()->GetGraphObjectFactory();

		// create a submenu for creating nodes
		QMenu* createMenu = menu.addMenu( GetQtBaseManager()->FindIcon("Images/Icons/Plus.png"), "Add Node" );

		switch(mType)
		{
			case GraphWidget::CLASSIFIER:
			{
				// iterate through the node categories
				for (uint32 category=0; category<SPNode::NUM_CATEGORIES; ++category)
				{
					QMenu* categoryMenu = NULL;

					// get the number of registered nodes in the factory and iterate through them
					const uint32 numRegisteredNodes = objectFactory->GetNumRegisteredObjects();
					for (uint32 i=0; i<numRegisteredNodes; ++i)
					{
						GraphObject* registeredObject = objectFactory->GetRegisteredObject(i);

						if (registeredObject->GetBaseType() != Node::BASE_TYPE)
							continue;

						Node* registeredNode = static_cast<Node*>(registeredObject);

						// skip all nodes that are not part of the given category
						if (registeredNode->GetPaletteCategory() != (SPNode::ECategory)category)
							continue;

						// create category together with the first valid node
						if (categoryMenu == NULL)
							categoryMenu = createMenu->addMenu( SPNode::GetCategoryName((SPNode::ECategory)category) );

						QString actionName = registeredNode->GetReadableType();
						QAction* nodeAction = categoryMenu->addAction( actionName );
						nodeAction->setProperty( "typeUuid", registeredNode->GetTypeUuid() );
						connect( nodeAction, SIGNAL(triggered()), this, SLOT(OnContextMenuCreateNode()) );

						// add icon
						nodeAction->setIcon( GraphPaletteWidget::GetNodeIcon(registeredNode) );

						// handle create permission
						//const bool create = GraphHelpers::GetCreud(registeredNode).Create();
						//nodeAction->setEnabled(create);
					}
				}

				break;
			}

			case GraphWidget::STATEMACHINE:
			{
				// iterate through the node categories
				for (uint32 category=0; category<State::NUM_CATEGORIES; ++category)
				{
					QMenu* categoryMenu = NULL;

					// get the number of registered nodes in the factory and iterate through them
					const uint32 numRegisteredNodes = objectFactory->GetNumRegisteredObjects();
					for (uint32 i=0; i<numRegisteredNodes; ++i)
					{
						GraphObject* registeredObject = objectFactory->GetRegisteredObject(i);

						if (registeredObject->GetBaseType() != State::BASE_TYPE)
							continue;

						Node* registeredNode = static_cast<Node*>(registeredObject);

						// skip all nodes that are not part of the given category
						if (registeredNode->GetPaletteCategory() != (State::ECategory)category)
							continue;

						// create category together with the first valid node
						if (categoryMenu == NULL)
							categoryMenu = createMenu->addMenu( State::GetCategoryName((State::ECategory)category) );

						QString actionName = registeredNode->GetReadableType();
						QAction* nodeAction = categoryMenu->addAction( actionName );
						nodeAction->setProperty( "typeUuid", registeredNode->GetTypeUuid() );
						connect( nodeAction, SIGNAL(triggered()), this, SLOT(OnContextMenuCreateNode()) );

						// add icon
						nodeAction->setIcon( GraphPaletteWidget::GetNodeIcon(registeredNode) );

						// handle create permission
						//const bool create = GraphHelpers::GetCreud(registeredNode).Create();
						//nodeAction->setEnabled(create);
					}
				}
			}

			default: { break; }
		}
	}
	else
	{
		// get the parent graph of the selected node
		Graph* parentGraph = mMousePressEventNode->GetParent();

		// state machine specific
		if (parentGraph != NULL && parentGraph->GetType() == StateMachine::TYPE_ID && mMousePressEventNode != NULL && mMousePressEventNode->GetBaseType() == State::BASE_TYPE)
		{
			//StateMachine* stateMachine = static_cast<StateMachine*>( parentNode );
			//State* state = static_cast<State*>(mMousePressEventNode);

			QAction* triggerStartAction = menu.addAction("&Activate");
			//setAsEntryStateAction->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/EntryState.png") );
			connect(triggerStartAction, SIGNAL(triggered()), this, SLOT(OnForceStateActivate()));
		}
	}

	const uint32 numSelectedNodes		= mShared.GetNumSelectedNodes();
	const uint32 numSelectedConnections	= mShared.GetNumSelectedConnections();


	////////////////////////
	// enable/disable action
#ifndef PRODUCTION_BUILD
	if (numSelectedNodes > 0 || numSelectedConnections > 0)
	{
		Array<Node*> selectedNodes = mShared.GetSelectedNodesArray();

		uint32 numEnabledNodes = 0;
		for (uint32 i = 0; i < numSelectedNodes; ++i)
			if (selectedNodes[i]->IsEnabled() == true)
				numEnabledNodes++;

		QAction* enableDisableAction = NULL;
		if (numEnabledNodes <= numSelectedNodes / 2)
		{
			enableDisableAction  = menu.addAction("Enable");
			enableDisableAction->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Enable.png"));
		}
		else
		{
			enableDisableAction  = menu.addAction("Disable");
			enableDisableAction->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Disable.png"));
		}

		//enableDisableAction->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Reset.png"));
		connect(enableDisableAction, SIGNAL(triggered()), this, SLOT(OnEnableDisableSelected()));
	}
#endif

	/////////////////////
	// lock/unlock action
	if (numSelectedNodes > 0 || numSelectedConnections > 0)
	{
		Array<Node*> selectedNodes = mShared.GetSelectedNodesArray();

		uint32 numLockedNodes = 0;
		for (uint32 i = 0; i < numSelectedNodes; ++i)
			if (selectedNodes[i]->IsLocked() == true)
				numLockedNodes++;

		QAction* lockUnlockAction = NULL;
		if (numLockedNodes <= numSelectedNodes / 2)
		{
			lockUnlockAction  = menu.addAction("Lock");
			lockUnlockAction->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/LockClosed.png"));
		}
		else
		{
			lockUnlockAction  = menu.addAction("Unlock");
			lockUnlockAction->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/LockOpen.png"));
		}

		connect(lockUnlockAction, SIGNAL(triggered()), this, SLOT(OnLockUnlockSelected()));
	}

	/////////////////
	// node alignment
	if (numSelectedNodes > 1)
	{
		// align left
		QAction* alignLeftAction = menu.addAction( "Align Left" );
		alignLeftAction->setIcon( GetQtBaseManager()->FindIcon("Images/Graph/AlignLeft.png") );
		connect( alignLeftAction, SIGNAL(triggered()), this, SLOT(OnAlignLeft()) );

		// align right
		QAction* alignRightAction = menu.addAction( "Align Right" );
		alignRightAction->setIcon( GetQtBaseManager()->FindIcon("Images/Graph/AlignRight.png") );
		connect( alignRightAction, SIGNAL(triggered()), this, SLOT(OnAlignRight()) );

		// align top
		QAction* alignTopAction = menu.addAction( "Align Top" );
		alignTopAction->setIcon( GetQtBaseManager()->FindIcon("Images/Graph/AlignTop.png") );
		connect( alignTopAction, SIGNAL(triggered()), this, SLOT(OnAlignTop()) );

		// align bottom
		QAction* alignBottomAction = menu.addAction( "Align Bottom" );
		alignBottomAction->setIcon( GetQtBaseManager()->FindIcon("Images/Graph/AlignBottom.png") );
		connect( alignBottomAction, SIGNAL(triggered()), this, SLOT(OnAlignBottom()) );
	}

#ifndef PRODUCTION_BUILD
	/////////////////////////////////////////////////
	// development: reset selected node or connection
	if (numSelectedNodes > 0 || numSelectedConnections > 0)
	{
		QAction* resetAction = menu.addAction("Reset");
		resetAction->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Reset.png"));
		connect(resetAction, SIGNAL(triggered()), this, SLOT(OnResetSelected()));
	}
#endif

	////////////////////////////////
	// remove node/connection action
	String removeActionName;
	if (numSelectedNodes == 1)				removeActionName = "Remove Node";
	else if (numSelectedNodes > 1)			removeActionName = "Remove Nodes";
	else if (numSelectedConnections == 1)	removeActionName = "Remove Connection";
	else if (numSelectedConnections > 1)	removeActionName = "Remove Connections";

	// add the remove action in case we selected at least one node or connection
	if (removeActionName.IsEmpty() == false)
	{
		QAction* removeAction = menu.addAction( removeActionName.AsChar() );
		removeAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Remove.png") );
		connect( removeAction, SIGNAL(triggered()), this, SLOT(OnRemoveSelected()) );
	}



	// show the menu at the given position
	if (menu.isEmpty() == false)
		menu.exec( QCursor::pos() );
}


// calculate the rect from the entire graph
QRect GraphWidget::CalcRectFromGraph(Graph* graph)
{
	QRect result;

	// get the number of nodes and iterate through them
	const uint32 numNodes = graph->GetNumNodes();
	for (uint32 i = 0; i < numNodes; ++i)
	{
		Node* node = graph->GetNode(i);

		// calculate the node rect and add it to the result
		QRect nodeRect = mRenderer->CalcNodeRect(mShownGraph, node);

		// add the rect
		result = result.united(nodeRect);
	}

	return result;
}


QRect GraphWidget::CalcRectFromNodes(const Core::Array<Node*>& nodes)
{
	QRect result;

	// get the number of nodes and iterate through them
	const uint32 numNodes = nodes.Size();
	for (uint32 i = 0; i<numNodes; ++i)
	{
		Node* node = nodes[i];

		// calculate the node rect and add it to the result
		QRect nodeRect = mRenderer->CalcNodeRect(mShownGraph, node);

		// add the rect
		result = result.united(nodeRect);
	}

	return result;

}



// fit the graph on the screen
void GraphWidget::FitGraphOnScreen(Graph* graph, int32 width, int32 height)
{
	if (graph == NULL)
		return;

	// fit the entire graph in the view
	QRect sceneRect = CalcRectFromGraph(graph);
	if (sceneRect.isEmpty() == false)
	{
		sceneRect.adjust(-5, -5, 5, 5);
		ZoomOnRect( sceneRect, width, height );
    }
}


void GraphWidget::CenterGraph(Graph* graph)
{
    if (graph == NULL)
        return;
    
    // fit the entire graph in the view
    QRect sceneRect = CalcRectFromGraph(graph);
    if (sceneRect.isEmpty() == false)
    {
        sceneRect.adjust(-5, -5, 5, 5);
        ScrollRectCentered( sceneRect, width(), height() );
    }
}


// scroll to the center of the visible rect without zooming
void GraphWidget::ScrollRectCentered(const QRect& rect, int32 width, int32 height)
{
	QRect localRect = rect;

	// calculate the space left after we move this the rect to the upperleft of the screen
	const int32 widthLeft	= width - localRect.width();
	const int32 heightLeft	= height - localRect.height();

	// center the rect in the middle of the screen
	QPoint offset;
	const int32 left= localRect.left();
	const int32 top	= localRect.top();
	offset.setX( -left + widthLeft/2 );
	offset.setY( -top  + heightLeft/2 );

	mShared.SetScrollOffset( offset );
	mShared.SetScale( 1.0f );
}


// make the given rect visible
void GraphWidget::ZoomOnRect(const QRect& rect, int32 width, int32 height)
{
	QRect localRect = rect;

	// calculate the space left after we move this the rect to the upperleft of the screen
	const int32 widthLeft	= width - localRect.width();
	const int32 heightLeft	= height - localRect.height();

	if (widthLeft > 0 && heightLeft > 0)
	{
		// center the rect in the middle of the screen
		ScrollRectCentered( rect, width, height );
	}
	else
	{
		// grow the rect a bit to keep some empty space around the borders
		localRect.adjust(-5, -5, 5, 5);

		// put the center of the selection in the middle of the screen
		QPoint offset = -localRect.center() + QPoint(width/2, height/2);
		mShared.SetScrollOffset( offset );

		// set the zoom factor so it exactly fits
		// find out how many extra pixels we need to fit on screen
		const int32 widthDif  = localRect.width()  - width;
		const int32 heightDif = localRect.height() - height;

		// calculate how much zoom out we need for width and height
		float widthZoom  = 1.0f;
		float heightZoom = 1.0f;

		if (widthDif > 0)
			widthZoom  = 1.0f / ((widthDif/(float)width) + 1.0f);

		if (heightDif > 0)
			heightZoom = 1.0f / ((heightDif/(float)height) + 1.0f);

		mShared.SetScale( Min<float>(widthZoom, heightZoom) );
	}
}


// find the node under the given mouse position
Node* GraphWidget::FindNode(const QPoint& globalPoint)
{
	// check if the graph is active
	if (mShownGraph == NULL)
		return NULL;

	// get the number of nodes and iterate through them
	const uint32 numNodes = mShownGraph->GetNumNodes();
	for (uint32 i=0; i<numNodes; ++i)
	{
		Node* node = mShownGraph->GetNode(i);
		
		if (mRenderer->IsPointOnNode(mShownGraph, node, globalPoint) == true)
			return node;
	}

	// not found
	return NULL;
}


// called when anything in the selection changed
void GraphWidget::OnSelectionChanged()
{
	GraphAttributesWidget* attributesWidget = mGraphPlugin->GetGraphAttributesWidget();
	if (attributesWidget == NULL)
		return;

	const uint32 numSelectedNodes		= mShared.GetNumSelectedNodes();
	const uint32 numSelectedConnections	= mShared.GetNumSelectedConnections();

	if (numSelectedNodes == 1 && numSelectedConnections == 0)
		attributesWidget->InitForGraphObject( mShared.GetSelectedNode(0) );
	else if (numSelectedConnections == 1 && numSelectedNodes == 0)
		attributesWidget->InitForGraphObject( mShared.GetSelectedConnection(0) );
	else if (numSelectedNodes == 0 && numSelectedConnections == 0 && mGraph != NULL)
		attributesWidget->InitForGraphObject( mGraph );
	else
		attributesWidget->InitForGraphObject( NULL );
}


void GraphWidget::OnCutObjects()
{
	return;
	ClearCopyBuffer();

	CopySelection();

	OnRemoveSelected();
}


void GraphWidget::OnCopyObjects()
{
	ClearCopyBuffer();

	CopySelection();
}


void GraphWidget::OnPasteObjects()
{
	// paste either over selected objects or at the last mouse position
	QPoint pastePos;
	if (mShared.GetNumSelectedNodes() == 0)
		pastePos = mLocalMousePos;
	else
		pastePos = CalcRectFromNodes(mShared.GetSelectedNodesArray()).center();

	PasteAt(pastePos);
}


// copy selected objets into buffers
void GraphWidget::CopySelection()
{
	// copy all nodes first
	const uint32 numNodes = mShared.GetNumSelectedNodes();
	mCopiedNodes.Reserve(mCopiedNodes.Size() + numNodes);
	for (uint32 i = 0; i < numNodes; ++i)
	{
		Node* original = mShared.GetSelectedNode(i);

		Node* copy = static_cast<Node*>(original->Clone(mShownGraph));
		copy->Init();
		copy->CreateDefaultAttributeValues();
		copy->CopyFrom(*original);
		copy->SetName(original->GetName());
		
		mCopiedNodes.Add(copy);
	}

	// copy connections
	const uint32 numCons = mShared.GetNumSelectedConnections();
	mCopiedConnections.Reserve(mCopiedConnections.Size() + numCons);
	for (uint32 i = 0; i < numCons; ++i)
	{
		Connection* con = mShared.GetSelectedConnection(i);
		Connection* copy = static_cast<Connection*>(con->Clone(mShownGraph));
		mCopiedConnections.Add(copy);
	}
}

// delete objects in buffer
void GraphWidget::ClearCopyBuffer()
{
	DestructArray(mCopiedNodes);
	DestructArray(mCopiedConnections);
}


void GraphWidget::PasteAt(QPoint targetPos)
{
	// nothing was copied
	if (mCopiedNodes.Size() == 0)
		return;

	// mode 1: copy attributes from a single node to another one of the same type
	if (mShared.GetNumSelectedNodes() == 1 && mCopiedNodes.Size() == 1)
	{
		Node* fromNode = mCopiedNodes.GetFirst();
		Node* toNode = mShared.GetSelectedNode(0);
		if (fromNode->GetType() == toNode->GetType())
		{
			toNode->CopyFrom(*fromNode);
			toNode->SetName(fromNode->GetName());
			toNode->OnAttributesChanged();
			return;
		}

	}
	// mode 2: paste multiple object
	else
	{
		// NOTE: disabled until fixed

		return;

		// remove selected objects first
		//if (mShared.GetNumSelectedNodes() > 0)
		//	OnRemoveSelected();

		
		const uint32 numNodes = mCopiedNodes.Size();
		const uint32 numCons = mCopiedConnections.Size();

		// copied nodes' center
		QPoint bufferPos = CalcRectFromNodes(mCopiedNodes).center();

		// calculate offset that moves the pasted nodes to the correct position
		QPoint offset = targetPos - bufferPos;

		Array<Node*> pasteNodes; pasteNodes.Reserve(numNodes);

		// clone nodes first
		for (uint32 i = 0; i < numNodes; ++i)
		{
			Node* original = mCopiedNodes[i];
			Node* copy = static_cast<Node*>(original->Clone(mShownGraph));
			copy->Init();
			copy->CreateDefaultAttributeValues();
			copy->SetName(original->GetName());
			copy->CopyFrom(*mCopiedNodes[i]);

			// move to new pos
			copy->SetVisualPos(copy->GetVisualPosX() + offset.x(), copy->GetVisualPosY() + offset.y());
			
			copy->Reset();
			mShownGraph->AddNode(copy);
			pasteNodes.Add(copy);
		}

		// clone connections
		for (uint32 i = 0; i < numCons; ++i)
		{
			Connection* con = mCopiedConnections[i];

			// check and update source/target node references in connection copy
			Node* targetNode = NULL;
			Node* sourceNode = NULL;
			for (uint32 j = 0; j < numNodes; ++j)
			{
				Node* node = mCopiedNodes[j];
				if (con->GetSourceNode() == node)
					sourceNode = pasteNodes[j];

				if (con->GetTargetNode() == node)
					targetNode = pasteNodes[j];
			}

			// connection is not between two copied nodes -> dont paste
			if (sourceNode == NULL || targetNode == NULL)
				continue;

			mShownGraph->AddConnection(sourceNode, con->GetSourcePort(), targetNode, con->GetTargetPort());
		}


	}

}


bool GraphWidget::event(QEvent* event)
{
	// forward gesture events to our own gesture event
	//if (event->type() == QEvent::Gesture)
	//	return gestureEvent( static_cast<QGestureEvent*>(event) );

	// tooltip event
	if (event->type() == QEvent::ToolTip && mShownGraph != NULL)
	{
		QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);

		QFontMetrics fontMetrics(QToolTip::font());

		QFont boldFont = QToolTip::font();
		boldFont.setBold(true);
		QFontMetrics boldFontMetrics(boldFont);
		String toolTipString;

		QPoint localPos		= helpEvent->pos();
		QPoint globalPos	= GraphHelpers::LocalToGlobal( mShared.GetTransform(), localPos );
		QPoint tooltipPos	= helpEvent->globalPos();

		// connection tooltip
		Connection* connectionOverMouse = mRenderer->FindConnection( mShownGraph, globalPos);
		if (connectionOverMouse != NULL)
		{
			const uint32	sourcePortNr	= connectionOverMouse->GetSourcePort();
			const uint32	targetPortNr	= connectionOverMouse->GetTargetPort();
			Node*		sourceNode		= connectionOverMouse->GetSourceNode();
			Node*		targetNode		= connectionOverMouse->GetTargetNode();

			// get the source and the target node colors
			QColor sourceColor, targetColor;
			if (sourceNode != NULL) sourceColor = mShared.GetNodeColor( isEnabled(), sourceNode );
			if (targetNode != NULL) targetColor = mShared.GetNodeColor( isEnabled(), targetNode );

			// get the source and the target node names
			String sourceNodeName, targetNodeName;
			if (sourceNode != NULL) sourceNodeName = sourceNode->GetName();
			if (targetNode != NULL) targetNodeName = targetNode->GetName();

			// get the source and the target port names
			String sourcePortName, targetPortName;
			if (sourceNode != NULL) sourcePortName = sourceNode->GetOutputPort(sourcePortNr).GetName();
			if (targetNode != NULL) targetPortName = targetNode->GetInputPort(targetPortNr).GetName();
		
			//int columnSourceWidth = boldFontMetrics.width(sourceNodeName.AsChar()) + boldFontMetrics.width(" ") + fontMetrics.width("(Port: ") +  fontMetrics.width( sourcePortName.AsChar() ) + fontMetrics.width(")");
			//int columnTargetWidth = boldFontMetrics.width(targetNodeName.AsChar()) + boldFontMetrics.width(" ") + fontMetrics.width("(Port: ") +  fontMetrics.width( targetPortName.AsChar() ) + fontMetrics.width(")");
						
			// construct the html tooltip string

			// start the table
			toolTipString = "<qt><table border=\"0\">";

			// output information about the channel
			const uint32 type =  sourceNode->GetOutputPort(sourcePortNr).GetValueAttribute()->GetType();
			switch (type)
			{
				case AttributeBool::TYPE_ID :
				case AttributeFloat::TYPE_ID :
					mTempString.Format( "<tr><td><b>Type:</b></td><td><nobr>%s (deprecated)</nobr></td></tr>", (type==AttributeBool::TYPE_ID ? "Boolean" : "Float"));
					toolTipString += mTempString;
					break;

				case AttributeChannels<double>::TYPE_ID_DOUBLE:
				{
					MultiChannel* channels = sourceNode->GetOutputPort(sourcePortNr).GetChannels();
					
					if (channels == NULL || channels->GetNumChannels() == 0)
					{
						// type name
						mTempString.Format( "<tr><td><b>Type:</b></td><td><nobr>Empty Channel</nobr></td></tr>");
						toolTipString += mTempString;
					}
					else
					{
						// type name
						mTempString.Format( "<tr><td><b>Type:</b></td><td><nobr>%s</nobr></td></tr>", (channels->GetNumChannels() > 1 ? "Multichannel" : "Single Channel"));
						toolTipString += mTempString;

						// if multichannel: number of channels
						if (channels->GetNumChannels() > 1)
						{
							mTempString.Format( "<tr><td><b><nobr>Channels:</nobr></b></td><td><nobr>%i</nobr></td></tr>", channels->GetNumChannels());
							toolTipString += mTempString;
						}
						else // if single channel: show name instead of multichannelsize
						{
							mTempString.Format( "<tr><td><b><nobr>Channel Name:</nobr></b></td><td><nobr>%s</nobr></td></tr>", channels->GetChannel(0)->GetName());
							toolTipString += mTempString;
						}

						Channel<double>* channel = channels->GetChannel(0)->AsType<double>();

						// sample rate
						mTempString.Format( "<tr><td><b><nobr>Sample Rate:</nobr></b></td><td><nobr>%.2f</nobr></td></tr>", channel->GetSampleRate());
						toolTipString += mTempString;

						// is active
						mTempString.Format("<tr><td><b><nobr>Is Active:</nobr></b></td><td><nobr>%s</nobr></td></tr>", (channel->IsActive() ? "True" : "False"));
						toolTipString += mTempString;

						// is independent or synced
						mTempString.Format("<tr><td><b><nobr>Is Independent:</nobr></b></td><td><nobr>%s</nobr></td></tr>", (channel->IsIndependent() ? "True" : "False"));
						toolTipString += mTempString;
#ifndef PRODUCTION_BUILD
						// sample counter
						mTempString.Format( "<tr><td><b><nobr>Reference:</nobr></b></td><td><nobr>%x</nobr></td></tr>", channel);
						toolTipString += mTempString;

						// sample counter
						mTempString.Format( "<tr><td><b><nobr>Sample Counter:</nobr></b></td><td><nobr>%i</nobr></td></tr>", channel->GetSampleCounter());
						toolTipString += mTempString;

						// num samples
						mTempString.Format( "<tr><td><b><nobr>Num Samples:</nobr></b></td><td><nobr>%i</nobr></td></tr>", channel->GetNumSamples());
						toolTipString += mTempString;

						// buffer size
						mTempString.Format( "<tr><td><b><nobr>Buffer Size:</nobr></b></td><td><nobr>%i</nobr></td></tr>", channel->GetBufferSize());
						toolTipString += mTempString;

						// start time
						mTempString.Format("<tr><td><b><nobr>Start Time:</nobr></b></td><td><nobr>%.4f</nobr></td></tr>", channel->GetStartTime().InSeconds());
						toolTipString += mTempString;

						// elapsed time
						mTempString.Format("<tr><td><b><nobr>Elapsed Time:</nobr></b></td><td><nobr>%.4f</nobr></td></tr>", channel->GetElapsedTime().InSeconds());
						toolTipString += mTempString;

						// last sample time
						mTempString.Format("<tr><td><b><nobr>Last Sample Time:</nobr></b></td><td><nobr>%.4f</nobr></td></tr>", channel->GetLastSampleTime().InSeconds());
						toolTipString += mTempString;

						// latency
						mTempString.Format("<tr><td><b><nobr>Latency:</nobr></b></td><td><nobr>%.5f</nobr></td></tr>", channel->GetLatency());
						toolTipString += mTempString;

#endif
					}
					break;
				}

				case AttributeChannels<Spectrum>::TYPE_ID_SPECTRUM:
				{
					MultiChannel* channels = sourceNode->GetOutputPort(sourcePortNr).GetChannels();

					if (channels == NULL ||  channels->GetNumChannels() == 0)
					{
						// type name
						mTempString.Format( "<tr><td><b>Type:</b></td><td><nobr>Empty Complex Spectrum Channel</nobr></td></tr>");
						toolTipString += mTempString;
					}
					else
					{
						// type name
						mTempString.Format( "<tr><td><b>Type:</b></td><td><nobr>Complex Spectrum Channel%s</nobr></td></tr>", (channels->GetNumChannels() > 1 ? "s" : ""));
						toolTipString += mTempString;

						if (channels->GetNumChannels() > 1)
						{
							mTempString.Format( "<tr><td><b><nobr>Channels:</nobr></b></td><td><nobr>%i</nobr></td></tr>", channels->GetNumChannels());
							toolTipString += mTempString;
						} 
						else // if single channel: show name instead of multichannelsize
						{
							mTempString.Format( "<tr><td><b><nobr>Channel Name:</nobr></b></td><td><nobr>%s</nobr></td></tr>", channels->GetChannel(0)->GetName());
							toolTipString += mTempString;
						}
							
						Channel<Spectrum>* channel = channels->GetChannel(0)->AsType<Spectrum>();
						// sample rate
						mTempString.Format( "<tr><td><b><nobr>Sample Rate:</nobr></b></td><td><nobr>%.2f</nobr></td></tr>", channel->GetSampleRate());
						toolTipString += mTempString;

						// spectrum properties
						if (channel->GetNumSamples() > 0)
						{
							const Spectrum& spectrum = channel->GetLastSample();

							mTempString.Format( "<tr><td><b><nobr>Num Bins:</nobr></b></td><td><nobr>%i</nobr></td></tr>", spectrum.GetNumBins());
							toolTipString += mTempString;

							mTempString.Format( "<tr><td><b><nobr>Frequency:</nobr></b></td><td><nobr>%.2fHz - %.2fHz</nobr></td></tr>", 0.0, spectrum.GetMaxFrequency());
							toolTipString += mTempString;
						}

// developer output for connection
#ifndef PRODUCTION_BUILD
						// sample counter
						mTempString.Format( "<tr><td><b><nobr>Reference:</nobr></b></td><td><nobr>%x</nobr></td></tr>", channel);
						toolTipString += mTempString;

						// sample counter
						mTempString.Format( "<tr><td><b><nobr>Sample Counter:</nobr></b></td><td><nobr>%i</nobr></td></tr>", channel->GetSampleCounter());
						toolTipString += mTempString;
							
						// buffer size
						mTempString.Format( "<tr><td><b><nobr>Buffer Size:</nobr></b></td><td><nobr>%i</nobr></td></tr>", channel->GetBufferSize());
						toolTipString += mTempString;

						// num samples
						mTempString.Format( "<tr><td><b><nobr>Num Samples:</nobr></b></td><td><nobr>%i</nobr></td></tr>", channel->GetNumSamples());
						toolTipString += mTempString;
#endif

					}
					break;
				}
				
			}

			// source
			mTempString.Format( "<tr><td><b>Source:</b></td><td><nobr><p style=\"color:rgb(%i,%i,%i)\"><b>%s </b>(Port: %s)</p></nobr></td></tr>", sourceColor.red(), sourceColor.green(), sourceColor.blue(), sourceNodeName.AsChar(),sourcePortName.AsChar() );
			toolTipString += mTempString;

			
			//toolTipString.FormatAdd("<tr><td width=\"%i\"><p style=\"color:rgb(%i,%i,%i)\"><b>%s </b>(Port: %s)</p></td> <td>to</td> <td width=\"%i\"><p style=\"color:rgb(%i,%i,%i)\"><b>%s </b>(Port: %s)</p></td></tr>", columnSourceWidth, , columnTargetWidth, );

			// target
			mTempString.Format( "<tr><td><b>Target:</b></td><td><nobr><p style=\"color:rgb(%i,%i,%i)\"><b>%s </b>(Port: %s)</p></nobr></td></tr>", targetColor.red(), targetColor.green(), targetColor.blue(), targetNodeName.AsChar(), targetPortName.AsChar());
			toolTipString += mTempString;
			
			// complete the table
			toolTipString += "</table></qt>";
		}


		// node tooltip
		Node* nodeOverMouse = FindNode(globalPos);
		if (nodeOverMouse != NULL)
		{
			// start the table
			toolTipString = "<qt><table border=\"0\">";

			// show error message if node is in error state
			const uint32 numErrors = nodeOverMouse->GetNumErrors();
			if (numErrors > 0)
			{
				for (uint32 i=0; i<numErrors; ++i)
				{
					mTempString.Format( "<tr><td colspan=2><p style=\"color:rgb(255,0,0)\"><b>Error: %s</b></p></td></tr>", nodeOverMouse->GetError(i).mMessage.AsChar() );
					toolTipString += mTempString;
				}
			}

			// show warnings 
			const uint32 numWarnings = nodeOverMouse->GetNumWarnings();
			if (numWarnings > 0)
			{
				for (uint32 i=0; i<numWarnings; ++i)
				{
					mTempString.Format( "<tr><td colspan=2><p style=\"color:rgb(255,255,0)\"><b>Warning: %s</b></p></td></tr>", nodeOverMouse->GetWarning(i).mMessage.AsChar() );
					toolTipString += mTempString;
				}
			}

			// node name
			mTempString.Format( "<tr><td><b>Name:</b></td><td><nobr>%s</nobr></td></tr>", nodeOverMouse->GetName() );
			toolTipString += mTempString;

			// node palette name
			mTempString.Format( "<tr><td><b>Type:</b></td><td><nobr>%s</nobr></td></tr>", nodeOverMouse->GetReadableType() );
			toolTipString += mTempString;

			// incoming connections
			mTempString.Format( "<tr><td><b>Incoming Connections:</b></td><td>%i</td></tr>", mShownGraph->CalcNumInputConnections(nodeOverMouse) );
			toolTipString += mTempString;

			// outgoing connections
			mTempString.Format( "<tr><td width=\"130\"><b>Outgoing Connections:</b></td><td>%i</td></tr>", mShownGraph->CalcNumOutputConnections(nodeOverMouse) );
			toolTipString += mTempString;

// developer output for Nodes
#ifndef PRODUCTION_BUILD
			
			// SPNode and subtypes // FIXME use proper RTTI system when implemented
			if (nodeOverMouse->GetNodeType() == ProcessorNode::NODE_TYPE || 
				nodeOverMouse->GetNodeType() == OutputNode::NODE_TYPE)
			{
				SPNode* node = static_cast<SPNode*>(nodeOverMouse);
				
				mTempString.Format( "<tr><td width=\"130\"><b>Input Size</b></td><td>%i</td></tr>", node->GetInputReader()->GetNumChannels());
				toolTipString += mTempString;
				mTempString.Format( "<tr><td width=\"130\"><b>Input Change</b></td><td>%i / %i / %i</td></tr>", node->GetInputReader()->HasInputChanged(ChannelReader::REFERENCE), node->GetInputReader()->HasInputChanged(ChannelReader::RESET));
				toolTipString += mTempString;
				mTempString.Format( "<tr><td width=\"130\"><b>Min New Samples</b></td><td>%i</td></tr>", node->GetInputReader()->GetMinNumNewSamples());
				toolTipString += mTempString;
				mTempString.Format( "<tr><td width=\"130\"><b>Delay (s)</b></td><td>%.5f</td></tr>", node->GetDelay(0,0));
				toolTipString += mTempString;
				mTempString.Format( "<tr><td width=\"130\"><b>Max Delay out</b></td><td>%.5f</td></tr>", node->FindMaximumDelay());
				toolTipString += mTempString;
				mTempString.Format( "<tr><td width=\"130\"><b>Latency (s)</b></td><td>%.5f</td></tr>", node->GetLatency(0,0));
				toolTipString += mTempString;
				
				const uint32 numPorts = node->GetNumInputPorts();
				for (uint32 i=0; i<numPorts; ++i)
				{
					if (node->GetInputPort(i).HasConnection() == false)
						continue;

					if (node->GetInputPort(i).GetChannels()->GetNumChannels() == 1)
					{
						MultiChannelReader* reader = node->GetInputReader();
						if (i < reader->GetChannels()->GetNumChannels())
						{
							ChannelBase* channel = reader->GetReader(i)->GetChannel();
							mTempString.Format( "<tr><td width=\"130\"><b>New Samples %i</b></td><td>%i</td></tr>", i, reader->GetReader(i)->GetNumNewSamples());
							toolTipString += mTempString;

							if (channel != NULL)
							{
								mTempString.Format( "<tr><td width=\"130\"><b>Start Time %i</b></td><td>%.5f</td></tr>", i, channel->GetStartTime().InSeconds());
								toolTipString += mTempString;
							}
						}
					}
				}
			}

			// OutputNode
			if (nodeOverMouse->GetNodeType() == OutputNode::NODE_TYPE)
			{
				OutputNode* node = static_cast<OutputNode*>(nodeOverMouse);

				const uint32 numChannels = node->GetNumOutputChannels();
				for (uint32 i=0; i<numChannels; ++i)
				{
					mTempString.Format( "<tr><td width=\"130\"><b>Output Samples</b></td><td>%i</td></tr>", node->GetOutputChannel(i)->GetNumSamples());
					toolTipString += mTempString;
					mTempString.Format( "<tr><td width=\"130\"><b>Last Burst</b></td><td>%i</td></tr>", node->GetLastBurstSize(i));
					toolTipString += mTempString;
				}
			}

#endif

			// complete the table
			toolTipString += "</table></qt>";
		}

		if (toolTipString.IsEmpty() == false)
		{
			QRect toolTipRect(globalPos.x()-4, globalPos.y()-4, 8, 8);
			QToolTip::showText( tooltipPos, toolTipString.AsChar(), this, toolTipRect );
		}
	}

	return QOpenGLWidget::event(event);
}


// when dropping stuff in our window
void GraphWidget::dropEvent(QDropEvent* event)
{
	// directly return if no graph is active
	if (mShownGraph == NULL)
	{
		event->ignore();
		return;
	}

	// directly return if no graph is not editable
	if (GetAllowInteraction() == false)
	{
		event->ignore();
		return;
	}

	// dont accept dragging/drop from and to yourself
	if (event->source() == this)
		return;

	// only accept copy actions
	if (event->dropAction() != Qt::CopyAction || event->mimeData()->hasText() == false)
	{
		event->ignore();
		return;
	}

	// if we have text, get it
	String dropText = FromQtString( event->mimeData()->text() );

	// extract the class name
	Array<String> parts = dropText.Split( StringCharacter::semiColon );
	if (parts.Size() != 2)
	{
		LogError("GraphWidget::dropEvent() - Incorrect syntax using drop data '%s'", FromQtString(event->mimeData()->text()).AsChar());
		event->ignore();
		return;
	}

	String nodeTypeUuid = parts[1];

	// create the node by type string
	GraphObject* object = GetGraphObjectFactory()->CreateObjectByTypeUuid( mShownGraph, nodeTypeUuid.AsChar() );
	if (object == NULL)
		return;

	Node* node = static_cast<Node*>(object);

	// check create permission
	const bool create = GraphHelpers::GetCreud(node).Create();
	if (create == false)
	{
		String upgradeReason;
		upgradeReason.Format( "%s is not available in %s.\nTo enjoy this feature, please upgrade to one of our subscription plans.", node->GetReadableType(), GetManager()->GetAppName().AsChar() );
		GetMainWindow()->ShowUpgradeWindow( upgradeReason.AsChar() );
		return;
	}

	// set the node name
	node->SetName( node->GetReadableType() );

	// reset
	node->Reset();


	// add the new node to the graph
	mShownGraph->AddNode(node);

	// calculate the node rect
	QRect nodeRect = mRenderer->CalcNodeRect( mShownGraph, node );
	const int32 halfNodeHeight	= nodeRect.height() / 2;
	const int32 halfNodeWidth	= nodeRect.width() / 2;

	// calculate the drop position and set that as node position
	QPoint offset = GraphHelpers::LocalToGlobal( mShared.GetTransform(), event->pos() );
	node->SetVisualPos( offset.x() - halfNodeWidth, offset.y() - halfNodeHeight );

	// reset some mouse handling flags
	mRectSelecting		= false;
	mLeftButtonPressed	= false;
	mLocalMousePos		= event->pos();
	mGlobalMousePos		= GraphHelpers::LocalToGlobal( mShared.GetTransform(), event->pos() );
	mCreateConnectionInfo.StopCreateConnection();

	event->accept();

	// base class drop event
	QOpenGLWidget::dropEvent(event);
}

// drop evnter event
void GraphWidget::dragEnterEvent(QDragEnterEvent* event)
{
	// directly return if no graph is active
	if (mShownGraph == NULL)
	{
		event->ignore();
		return;
	}

	// ignore event if graph is not editable
	if (GetAllowInteraction() == false)
		event->ignore();
	else
		event->accept();

	// if we have text, get it
	String dropText = FromQtString( event->mimeData()->text() );

	// extract the class name
	Array<String> parts = dropText.Split( StringCharacter::semiColon );
	if (parts.Size() != 2)
	{
		LogError("GraphWidget::dropEvent() - Incorrect syntax using drop data '%s'", FromQtString(event->mimeData()->text()).AsChar());
		event->ignore();
		return;
	}

	const char* prefixText;
	switch (mType)
	{
		case CLASSIFIER:	{ prefixText = "Node"; break; }
		case STATEMACHINE:	{ prefixText = "State"; break; }
		default:			{ prefixText = ""; break; }
	}

	if (parts[0].IsEqual(prefixText) == true)
		event->accept();
	else
		event->ignore();
}

/*
// gesture event
bool GraphWidget::gestureEvent(QGestureEvent* event)
{
    QList<QGesture*> gestures = event->gestures();
    const int numGestures = gestures.size();
    for (int i=0; i<numGestures; ++i)
    {
        QGesture* gesture = gestures[i];
        LogInfo("gestureEvent(): GestureType=%i", gesture->gestureType());
    }
    
    LogInfo("gestureEvent(): NumGestures=%i", numGestures);
    

	QSwipeGesture*	swipe	= static_cast<QSwipeGesture*>( event->gesture( Qt::SwipeGesture) );
	QPanGesture*	pan		= static_cast<QPanGesture*>( event->gesture( Qt::PanGesture) );
	QPinchGesture*	pinch	= static_cast<QPinchGesture*>( event->gesture( Qt::PinchGesture) );

	//if (swipe != NULL)OnSwipeGesture( event, swipe );
	if (pan != NULL)	OnPanGesture( event, pan );
	if (pinch != NULL)	OnPinchGesture( event, pinch );

	event->accept();

	return true;
}


// pan event
void GraphWidget::OnPanGesture(QGestureEvent* event, QPanGesture* gesture)
{
	QPointF delta = gesture->delta();

	LogInfo("GraphWidget::OnPanGesture(): delta=(%.2f, %.2f)", delta.x(), delta.y());

	//mShared.SetScrollOffset( mShared.GetScrollOffset() + QPoint(delta.x(), delta.y()) * (1.0f/mShared.GetScale()) );
}


// pinch event
void GraphWidget::OnPinchGesture(QGestureEvent* event, QPinchGesture* gesture)
{
	QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
	if (changeFlags & QPinchGesture::RotationAngleChanged)
	{
		const qreal value = gesture->property("rotationAngle").toReal();
		const qreal lastValue = gesture->property("lastRotationAngle").toReal();
		const qreal rotationAngleDelta = value - lastValue;
		//rotationAngle += rotationAngleDelta;
		//LogInfo( "GraphWidget::OnPinchGesture(): RotDelta=%.2f, RotAbs=%.4f", rotationAngleDelta, rotationAngle );
		LogInfo( "GraphWidget::OnPinchGesture(): RotDelta=%.2f", rotationAngleDelta );
	}
    if (changeFlags & QPinchGesture::CenterPointChanged)
    {
        QPointF centerPoint = gesture->centerPoint();
        LogInfo( "GraphWidget::OnPinchGesture(): CenterPoint=(%.2f, %.2f)", centerPoint.x(), centerPoint.y() );
    }
	if (changeFlags & QPinchGesture::ScaleFactorChanged)
	{
		qreal value = gesture->property("scaleFactor").toReal();
		LogInfo( "GraphWidget::OnPinchGesture(): Scale=%.4f", value );

	//	ApplyScale( value );
	}
	if (gesture->state() == Qt::GestureFinished)
	{
		//scaleFactor *= currentStepScaleFactor;
		//currentStepScaleFactor = 1;
		LogInfo("GraphWidget::OnPinchGesture(): Finished");
	}
}


// swipe event
void GraphWidget::OnSwipeGesture(QGestureEvent* event, QSwipeGesture* gesture)
{
	if (gesture->state() == Qt::GestureFinished)
	{
		if (gesture->horizontalDirection() == QSwipeGesture::Left || gesture->verticalDirection() == QSwipeGesture::Up)
		{
			LogInfo( "GraphWidget::OnSwipeGesture(): swipe to previous" );
			//GoToPrev();
		}
		else
		{
			LogInfo( "GraphWidget::OnSwipeGesture(): swipe to next" );
			//GoToNext();
		}
	}
}
*/


// activate the state 
void GraphWidget::OnForceStateActivate()
{
	// go through all selected nodes
	Array<Node*> selectedNodes = mShared.GetSelectedNodesArray();
	const uint32 numSelectedNodes = selectedNodes.Size();
	for (uint32 i=0; i<numSelectedNodes; ++i)
	{
		if (selectedNodes[i]->GetBaseType() == State::BASE_TYPE)
		{
			State* state = static_cast<State*>(selectedNodes[i]);
			state->ForceActivate();
		}
	}
}


