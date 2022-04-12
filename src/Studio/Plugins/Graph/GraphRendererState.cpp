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
#include "GraphRendererState.h"
#include <Core/Math.h>
#include <QtBaseManager.h>


using namespace Core;

// constructor
GraphRendererState::GraphRendererState(GraphShared* shared) : GraphRenderer(shared)
{
}


// destructor
GraphRendererState::~GraphRendererState()
{
}


// render graph connections
void GraphRendererState::RenderConnections(Graph* graph, QPainter& painter, const GraphHelpers::RelinkConnectionInfo& relinkConnectionInfo, Node* onMouseOverNode, const QPoint& globalMousePos, bool isWidgetEnabled, const QRect& visibleRect)
{
	// render the connections
	QPen connectionPen;
	QBrush connectionBrush;

	// get the number of child connections and iterate through them
	const uint32 numConnections = graph->GetNumConnections();
	for (uint32 c=0; c<numConnections; ++c)
	{
		StateTransition* transition = static_cast<StateTransition*>( graph->GetConnection(c) );

		// skip the transition that we're currently relinking
		if (relinkConnectionInfo.GetRelinkHeadConnection() == transition || relinkConnectionInfo.GetRelinkTailConnection() == transition)
			continue;

		const bool isSelected = mShared->IsConnectionSelected(transition);
		//const bool isProcessed= true; //GetEngine()->GetSession()->IsRunning();
				
		// highlight state of the connection
		bool isHighlighted = false;
		if (onMouseOverNode != NULL)
		{
			// highlight the connection in case we are currently hovering the node where the current connection is plugged into
			if (transition->GetSourceNode() == onMouseOverNode || transition->GetTargetNode() == onMouseOverNode)
				isHighlighted = true;
		}

		// in case the connection is not highlighted yet, check if we are over it with the mouse
		if (isHighlighted == false && IsCloseToConnection(graph, globalMousePos, transition) == true)
			isHighlighted = true;

		// render the transition
		bool isConnectedHighlighted = false;
		QColor baseColor = mShared->GetConnectionColor();

		const bool isHeadHighlighted = IsCloseToHead(transition, globalMousePos);
		const bool isTailHighlighted = IsCloseToTail(transition, globalMousePos);

		RenderStateTransition( transition, painter, baseColor, isSelected, isHighlighted, isConnectedHighlighted, isHeadHighlighted, isTailHighlighted );
	}
}


// render the given state transition
void GraphRendererState::RenderStateTransition(QPainter& painter, QPen& pen, QBrush& brush, const QPoint& startPos, const QPoint& endPos, bool isDisabled, bool isTransitioning, float progress, const QColor& baseColor, bool isSelected, bool isHighlighted, bool isConnectedHighlighted, bool isHeadHighlighted, bool isTailHighlighted)
{
	// check if the transition is degenerated and return directly in this case
	if ((endPos-startPos).manhattanLength() <= 1)
		return;

	// render the arrow
	RenderArrow( painter, pen, brush, startPos, endPos, baseColor, isSelected, isHighlighted, isConnectedHighlighted, isHeadHighlighted, isTransitioning, isDisabled, progress );


	const int circleRadius = 6.0f;

	brush.setColor( mShared->GetSelectedColor() );

	// render head highlight
	if (isHeadHighlighted == true)
	{
		// calculate the normalized direction vector of the transition from tail to head
		Core::Vector2 dir = Core::Vector2( endPos.x()-startPos.x(), endPos.y()-startPos.y() );
		dir.Normalize();

		Core::Vector2 newStart = Core::Vector2(endPos.x(), endPos.y()) - dir * (float)circleRadius;
		painter.drawEllipse( QPoint(newStart.x, newStart.y), circleRadius, circleRadius );
	}

	// render head highlight
	if (isTailHighlighted == true)
	{
		// calculate the normalized direction vector of the transition from tail to head
		Core::Vector2 dir = Core::Vector2( endPos.x()-startPos.x(), endPos.y()-startPos.y() );
		dir.Normalize();

		Core::Vector2 newStart = Core::Vector2(startPos.x(), startPos.y()) + dir * (float)circleRadius;
		painter.drawEllipse( QPoint(newStart.x, newStart.y), circleRadius, circleRadius );
	}
}


// render the given state transition
void GraphRendererState::RenderStateTransition(StateTransition* transition, QPainter& painter, const QColor& baseColor, bool isSelected, bool isHighlighted, bool isConnectedHighlighted, bool isHeadHighlighted, bool isTailHighlighted)
{
	StateMachine*	stateMachine			= transition->GetStateMachine();
	const bool		isDisabled				= transition->IsDisabled();
	const bool		isTransitioning			= transition->IsTransitioning();
	Node*			sourceNode				= transition->GetSourceNode();
	Node*			targetNode				= transition->GetTargetNode();

	// skip rendering the transition in case there is no space between the source and target node
	Vector2 sourceNodeCenter( sourceNode->GetVisualPosX(), sourceNode->GetVisualPosY() );
	Vector2 targetNodeCenter( targetNode->GetVisualPosX(), targetNode->GetVisualPosY() );
	Vector2 diff = targetNodeCenter - sourceNodeCenter;

	if (diff.Length() < mShared->GetStateDiameter())
		return;
	

	QPoint start, end;
	CalcTransitionStartAndEndPoints( stateMachine, transition, start, end, this );

	float progress = -1.0f;
	if (transition->IsTransitioning() == true)
		progress = transition->GetProgress();

	QPen pen;
	QBrush brush;

	RenderStateTransition( painter, pen, brush, start, end, isDisabled, isTransitioning, progress, baseColor, isSelected, isHighlighted, isConnectedHighlighted, isHeadHighlighted, isTailHighlighted );

	// visualize the conditions
	RenderStateConditions( transition, &painter, &pen, &brush, start, end );
}


void GraphRendererState::RenderArrow(QPainter& painter, QPen& pen, QBrush& brush, const QPoint& start, const QPoint& end, const QColor& baseColor, bool isSelected, bool isHighlighted, bool isConnectedHighlighted, bool isHeadHighlighted, bool isTransitioning, bool isDisabled, float progress)
{
	QColor color;

	// draw some small horizontal lines that go outside of the connection port
	if (isSelected)
	{
		pen.setWidth( 2 );
		color = mShared->GetSelectedColor();
	}
	else
	{
		pen.setWidthF( 1.5f );
		color = baseColor;

		//if (isTransitioning == true)
		//	color.setRgb(11,187,239);
	}

	// darken the color in case the transition is disabled
	if (isDisabled == true)
		color = color.darker(165);

	// lighten the color in case the transition is highlighted
	if (isHighlighted == true)
	{
		color = color.lighter(150);
		painter.setOpacity( 1.0 );
	}

	// lighten the color in case the transition is connected to the currently selected node
	if (isConnectedHighlighted == true)
	{
		pen.setWidth( 2 );
		color = color.lighter(150);
		painter.setOpacity( 1.0 );
	}

	// set the pen
	pen.setColor( color );
	pen.setStyle( Qt::SolidLine );
	painter.setPen( pen );

	// set the brush
	brush.setColor( color );
	brush.setStyle( Qt::SolidPattern );
	painter.setBrush( brush );

	// calculate the line direction
	Vector2 lineDir = Vector2(end.x(), end.y()) - Vector2(start.x(), start.y());

	// make sure the transition isn't starting and ending at the same position, if so, return directly
	if (IsClose<float>( lineDir.x, 0.0f, Math::epsilon ) == true &&
		IsClose<float>( lineDir.y, 0.0f, Math::epsilon ) == true)
		return;

	// if it is safe, get the length and normalize the direction vector
	float length = lineDir.Length();
	lineDir.Normalize();

	QPointF direction;
	direction.setX( lineDir.x * 16.0f );
	direction.setY( lineDir.y * 16.0f );

	QPointF normalOffset((end.y() - start.y()) / length, (start.x() - end.x()) / length);

	QPointF points[3];
	points[0] = end;
	points[1] = end - direction + (normalOffset * 4.0f);
	points[2] = end - direction - (normalOffset * 4.0f);

	// draw line
	if (isDisabled == true)
	{
		pen.setStyle( Qt::DashLine );
		painter.setPen( pen );
	}
	else
	{
		pen.setStyle( Qt::SolidLine );
		painter.setPen( pen );
	}

	// get the transition blend weight
	if (progress >= 0.0f && progress <= 1.0f)
	{
		// linear gradient for the background
		QLinearGradient gradient(start, end);

		gradient.setColorAt( 0.0, color );
		gradient.setColorAt( Clamp(progress-0.2f, 0.0f, 1.0f), color );
		gradient.setColorAt( Clamp(progress-0.1f, 0.0f, 1.0f), QColor(131,208,245) );
		gradient.setColorAt( Clamp(progress, 0.0f, 1.0f), QColor(131,208,245) );
		gradient.setColorAt( Clamp(progress+0.1f, 0.0f, 1.0f), QColor(131,208,245) );
		gradient.setColorAt( Clamp(progress+0.2f, 0.0f, 1.0f), color );
		gradient.setColorAt( 1.0, color );

		painter.setBrush( gradient );

		pen.setBrush( gradient );
		painter.setPen( pen );
	}

	painter.drawLine(start, end);

	// render arrow head triangle
/*	if (isHeadHighlighted == true)
	{
		QColor headTailColor(0,255,0);
		brush.setColor( headTailColor );
		painter.setBrush( brush );
		pen.setColor( headTailColor );
		painter.setPen( pen );
	}*/

	brush.setStyle( Qt::SolidPattern );
	painter.drawPolygon( points, 3 );

	/*if (isHeadHighlighted == true)
	{
		brush.setColor( color );
		painter.setBrush( brush );
		pen.setColor( color );
		painter.setPen( pen );
	}*/
}


// visualize the transition conditions
void GraphRendererState::RenderStateConditions(StateTransition* transition, QPainter* painter, QPen* pen, QBrush* brush, QPoint& start, QPoint& end)
{
	// if the transition is invalid, return directly
	if (transition == NULL)
		return;

	// get the number of conditions and return directly in case there are none
	const uint32 numConditions = transition->GetNumConditions();
	if (numConditions == 0)
		return;

	StateMachine*	stateMachine			= transition->GetStateMachine();
	//const bool	isWildcardTransition	= transition->IsWildcardTransition();
	const bool		isDisabled				= transition->IsDisabled();
	//Node*			sourceNode				= transition->GetSourceNode();
	//Node*			targetNode				= transition->GetTargetNode();

	// disable the dash pattern in case the transition is disabled
	pen->setStyle( Qt::SolidLine );
	painter->setPen( *pen );

	// check if the conditions shall be rendered or not
	// only visualize the conditions in case they are possible to reach from the currently active state in the state machine or if they belong to a wildcard transition
	// or in case the source or target nodes are either selected or hovered
	bool renderConditions = false;
	/*if (transition->GetSourceNode() == currentState || transition->IsWildcardTransition() == true || stateMachine->GetActiveTransition() == transition ||
		(sourceNode != NULL && sourceNode->IsSelected() == true) || targetNode->IsSelected() == true ||			// check for a selected source or target node
		(sourceNode != NULL && sourceNode->IsHighlighted() == true)  || targetNode->IsHighlighted() == true ||	// check for a hovered source or target node
		IsSelected() == true || IsHighlighted() == true)															// check if this transition is hovered or selected*/
		renderConditions = true;

	// check if the transition has to chance to trigger any moment
	bool transitionActive = transition->IsTransitioning();

	const uint32 numActiveStates = stateMachine->GetNumActiveStates();
	for (uint32 i=0; i<numActiveStates; ++i)
	{
		if (transition->GetSourceNode() == stateMachine->GetActiveState(i))
		{
			transitionActive = true;
			break;
		}
	}

	//if (renderConditions == true)
	{
		const Vector2	transitionStart(start.rx(), start.ry());
		const Vector2	transitionEnd(end.rx(), end.ry());

		// precalculate some values we need for the condition rendering
		const float		circleDiameter	= 5.0f;
		const float		circleStride	= 5.0f;
		const float		elementSize		= circleDiameter + circleStride;
		const Vector2	localEnd		= transitionEnd - transitionStart;

		// only draw the transition conditions in case the arrow has enough space for it, avoid zero rect sized crashes as well
		if (localEnd.Length() > numConditions * elementSize)
		{
			const Vector2	transitionMid	= transitionStart + localEnd*0.5;
			const Vector2	transitionDir	= localEnd.Normalized();
			//const float		halfLength		= localEnd.Length() * 0.5f;
			const Vector2	conditionStart	= transitionMid - transitionDir*(elementSize*0.5f*(float)numConditions);
			const Vector2	conditionEnd	= transitionMid + transitionDir*(elementSize*0.5f*(float)numConditions);

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Polygon rendered under the condition circles
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			const float		halfWidth		= circleDiameter;
			const float		tailExtension	= 8.0f;
			QPointF			polyDir			= QPointF(transitionDir.x, transitionDir.y);		// normalized transition direction
			QPointF			polyStartA		= QPointF(conditionStart.x, conditionStart.y) - polyDir * elementSize * 0.25f;		// point where the first condition circle is rendered
			QPointF			polyStartB		= polyStartA - polyDir * tailExtension;				// start point of the polygon (closer to transition start than the first actual condition)
			QPointF			polyEndA		= QPointF(conditionEnd.x, conditionEnd.y) - polyDir * elementSize * 0.75f;			// place where the last condition circle is rendered
			QPointF			polyEndB		= polyEndA + polyDir * tailExtension;				// poly end point (closest point to transition target)
			QPointF			polyLeft		= QPointF(  polyDir.y(), -polyDir.x() );			// rotate by -90 degrees (orthogonal to direction, left) 
			QPointF			polyRight		= QPointF( -polyDir.y(),  polyDir.x() );			// rotate by +90 degrees (orthogonal to direction, right)

			QPointF points[6];
			points[0] = polyStartB;
			points[1] = polyStartA	+ polyRight * halfWidth;
			points[2] = polyEndA	+ polyRight * halfWidth;
			points[3] = polyEndB;
			points[4] = polyEndA	+ polyLeft * halfWidth;
			points[5] = polyStartA	+ polyLeft * halfWidth;
			painter->drawConvexPolygon( points, 6 );

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Render condition traffic lights
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			for (uint32 i=0; i<numConditions; ++i)
			{
				// get access to the condition
				StateTransitionCondition* condition = transition->GetCondition(i);

				// set the condition color either green if the test went okay or red if the test returned false
				QColor conditionColor;
				if (condition->TestCondition() == true)
					conditionColor = Qt::green;
				else
					conditionColor = Qt::red;

				if (transitionActive == false || isDisabled == true)
					conditionColor = QColor(100,100,100);

				if (renderConditions == false)
					conditionColor = conditionColor.darker(250);

				// darken the color in case the transition is disabled
				if (isDisabled == true)
					conditionColor = conditionColor.darker(185);

				brush->setColor(conditionColor);

				// calculate the circle middle point
				const Vector2 circleMid = conditionStart  + transitionDir*(elementSize*(float)i);

				// render the circle per condition
				painter->setBrush( *brush );
				painter->drawEllipse( QPointF(circleMid.x, circleMid.y) , circleDiameter, circleDiameter );
			}
		}
	}
}


// render all nodes
void GraphRendererState::RenderNodes(Graph* graph, QPainter& painter, const GraphHelpers::CreateConnectionInfo& createConnectionInfo, bool isWidgetEnabled, const QRect& visibleRect, const QPoint& mousePos)
{
	// make sure the graph is valid
	if (graph == NULL)
		return;

	// make sure the graph is a state machine
	if (graph->GetType() != StateMachine::TYPE_ID)
	{
		LogWarning("GraphRendererState::RenderNodes(): Input graph is no state machine");
		CORE_ASSERT(false);
		return;
	}

	// down-cast to state machine
	StateMachine* stateMachine = static_cast<StateMachine*>(graph);

	// get the number of states in the state machine and render them
	const uint32 numStates = stateMachine->GetNumNodes();
	for (uint32 i = 0; i<numStates; ++i)
		RenderState( stateMachine, stateMachine->GetState(i), painter, createConnectionInfo, isWidgetEnabled, visibleRect, mousePos );
}


// render the given state
void GraphRendererState::RenderState(StateMachine* stateMachine, State* state, QPainter& painter, const GraphHelpers::CreateConnectionInfo& createConnectionInfo, bool isWidgetEnabled, const QRect& visibleRect, const QPoint& mousePos)
{
	const float solidOpacity		 = 1.0f;
	const float halfTransOpacity	 = 0.60f;
	//const float inactiveTransOpacity = 0.2f;

	const bool isActiveState = state->IsActive() || state->IsTransitioning();

	// calc node dimensions
	int32 maxInputWidth, maxOutputWidth;
	QRect rect = CalcNodeRect( stateMachine, state, &maxInputWidth, &maxOutputWidth );

	// calculate state weight (changes with transition progression)
	double weight = state->GetTransitionProgress();

	// calc the rect in screen space (after scrolling and zooming)
	QRect finalRect = mShared->GetTransform().mapRect( rect );

	// check if the node is visible, skip directly if it is not
	bool isVisible = finalRect.intersects( visibleRect );
	if (isVisible == false)
		return;

	const float defaultOpacity	= 1.0f;
	const bool isSelected		= mShared->IsNodeSelected(state);
	//const bool isReady		= node->IsUpdateReady();


	// opacity
	float opacityFactor = defaultOpacity;
	if (isWidgetEnabled == false)	opacityFactor *= 0.35f;
	if (opacityFactor < 0.065f)		opacityFactor = 0.065f;
	painter.setOpacity( opacityFactor );

	// check if the mouse is over the node, if yes highlight the node
	bool isHighlighted = false;
	if (IsPointOnNode(stateMachine, state, mousePos) == true)
		isHighlighted = true;

	const bool highlightInputPort = isHighlighted == true || (createConnectionInfo.IsCreatingConnection() == true && createConnectionInfo.mConNode != state);

	// render node shadow
	//if (renderShadow == true)
	//	GraphHelpers::RenderNodeShadow(painter);

	// background color
	QColor bgColor;
	if (isSelected == true)
		bgColor = mShared->GetSelectedColor();
	else
		bgColor = mShared->GetNodeColor( isWidgetEnabled, state );

	// blinking error
#ifndef SCREENSHOT_MODE
	if (state->HasError() == true && isSelected == false && isWidgetEnabled == true)
	{
		bgColor = mShared->GetErrorBlinkColor();
	}
#endif

	// border color
	//QColor borderColor;
	//if (isSelected == true)
	//	borderColor = mShared->GetSelectedColor();
	//else
	//	borderColor = mShared->GetActiveBorderColor();

	QColor bgColor2;
	bgColor2 = bgColor.darker(200); // make darker actually, 30% of the old color, same as bgColor * 0.3f;

	QColor textColor = isSelected ? Qt::black : Qt::white;

	// is highlighted/hovered (on-mouse-over effect)
	if (isHighlighted == true)
	{
		bgColor = bgColor.lighter(120);
		bgColor2 = bgColor2.lighter(120);
	}

	// draw the main rect
	QLinearGradient bgGradient(0, rect.top(), 0, rect.bottom());
	bgGradient.setColorAt(0.0f, bgColor);
	bgGradient.setColorAt(1.0f, bgColor2);

	// border
	const double borderWidth  = 2.0 * mShared->GetScreenScaling();
	QPen borderPen = QPen(bgColor);
	borderPen.setWidth(borderWidth * mShared->GetScreenScaling());

	//painter.drawRoundedRect( rect, GraphHelpers::GetBorderRadius(), GraphHelpers::GetBorderRadius() );

	const qreal oldOpacity = painter.opacity();

	if (isActiveState == true)
	{
		float halfBorderSize = 10.0f * weight;

		QBrush oldBrush = painter.brush();

		painter.setOpacity( halfTransOpacity * weight );

		painter.setBrush( mShared->GetReadOnlyColor() );
		QRect outerRect( QPoint(state->GetVisualPosX()-halfBorderSize, state->GetVisualPosY()-halfBorderSize), QSize(mShared->GetStateDiameter()+2*halfBorderSize, mShared->GetStateDiameter()+2*halfBorderSize) );

		painter.setPen( Qt::NoPen );
		painter.drawEllipse( outerRect );

		float startAngle, spanAngle;

		if (mShared->IsActiveStateAnimationPhaseA() == true)
		{
			startAngle	= 90;
			spanAngle	= (1.0-mShared->GetActiveStateAnimOffset()) * 360;
		}
		else
		{
			float currentSpan = (1.0-mShared->GetActiveStateAnimOffset()) * 360;
			startAngle	= 90 + currentSpan;
			spanAngle	= 360 - currentSpan;
		}

		painter.setPen( Qt::NoPen );
		painter.drawPie( outerRect, startAngle*16, spanAngle*16 );

		painter.setBrush(oldBrush);
	}

	if (highlightInputPort == false)
	{
		// draw the background circle area
		if (isActiveState == true)
			painter.setOpacity( halfTransOpacity + (1.0-halfTransOpacity) * weight );
		else
			painter.setOpacity( halfTransOpacity );

		painter.setPen( Qt::NoPen );
		painter.setBrush( bgGradient );
		painter.drawEllipse( rect );

		// draw the solid circle border
		painter.setOpacity( solidOpacity );
		painter.setPen( borderPen );
		painter.setBrush( Qt::NoBrush );
		painter.drawEllipse( rect );
	}
	else
	{
		painter.setOpacity( 0.35f );
		painter.setPen( Qt::NoPen );
		painter.setBrush( bgGradient );
		QRect outerRect( QPoint(state->GetVisualPosX(), state->GetVisualPosY()), QSize(mShared->GetStateDiameter(), mShared->GetStateDiameter()) );
		painter.drawEllipse( outerRect );

		painter.setOpacity( halfTransOpacity );
		painter.setPen( borderPen );
		painter.setBrush( bgGradient );
		QRect innerRect( QPoint(state->GetVisualPosX()+mShared->GetStateOutputPortSize(), state->GetVisualPosY()+mShared->GetStateOutputPortSize()), QSize(mShared->GetStateDiameter()-2.0*mShared->GetStateOutputPortSize(), mShared->GetStateDiameter()-2.0*mShared->GetStateOutputPortSize()) );
		painter.drawEllipse( innerRect );
	}

	// if the scale is so small that we can still see the small things
	if (mShared->GetScale() > 0.3f)
	{
		// draw the visualize area
		//if (mCanVisualize == true)
		//	RenderVisualizeRect(painter, bgColor, bgColor2);

		// render the marker which indicates that you can go inside this node
		//RenderHasChildsIndicator(painter, pen, borderColor, bgColor2);
	}

	painter.setOpacity(oldOpacity);

	//painter.setClipping( false );
	//painter.setClipping( true );

	// render icon area
	const int iconSize = rect.width() / 2;
	const int wPadding = (rect.width() - iconSize) / 2;
	const int hPadding = (rect.height() - iconSize) / 2;
	QRect iconRect = QRect(rect.left() + wPadding, rect.top() + hPadding, iconSize, iconSize);

	// now draw the icon
	painter.setPen(Qt::NoPen);
	painter.setOpacity(0.5);
	painter.drawPixmap(iconRect, FindNodeIcon(state, iconSize));
	painter.setOpacity(1);

	
	// some rects we need for the text
	QRect nameRect( 0, 0, rect.width(), rect.height() );

	RenderText( true, painter, state->GetNameString(), textColor, rect, mShared->GetNodeHeaderFont(), mShared->GetNodeHeaderMetrics(), Qt::AlignCenter );

	//RenderDebugInfo(painter);
}



// calc rect around the node
QRect GraphRendererState::CalcNodeRect(Graph* graph, Node* node, int32* outMaxInputWidth, int32* outMaxOutputWidth)
{
	// make sure the array sizes are all correct
	UpdateNodeCaches(graph);

	// find the node index and check for a valid node cache
	const uint32 nodeIndex = graph->FindNodeIndex(node);
	if (mNodeCaches[nodeIndex].IsValid() == true)
	{
		if (outMaxInputWidth != NULL)	*outMaxInputWidth = mNodeCaches[nodeIndex].mMaxInputWidth;
		if (outMaxOutputWidth != NULL)	*outMaxOutputWidth = mNodeCaches[nodeIndex].mMaxOutputWidth;

		return mNodeCaches[nodeIndex].mRect;
	}

	// calc node rect
	int32 maxInputWidth = 0;
	int32 maxOutputWidth= 0;
	const int32 requiredWidth = CalcRequiredStateWidth( node, mShared->GetNodeHeaderMetrics() );
	const int32 requiredHeight= CalcRequiredStateHeight();
	QRect rect( node->GetVisualPosX(), node->GetVisualPosY(), requiredWidth, requiredHeight );

	if (outMaxInputWidth != NULL)	*outMaxInputWidth = maxInputWidth;
	if (outMaxOutputWidth != NULL)	*outMaxOutputWidth = maxOutputWidth;

	// update the node cache
	mNodeCaches[nodeIndex].mRect			= rect;
	mNodeCaches[nodeIndex].mMaxInputWidth	= maxInputWidth;
	mNodeCaches[nodeIndex].mMaxOutputWidth	= maxOutputWidth;
	mNodeCaches[nodeIndex].mIsValid = true;

	return rect;
}


// calculate the connection rect
QRect GraphRendererState::CalcConnectionRect(Graph* graph, Connection* connection)
{
	StateMachine*		stateMachine	= static_cast<StateMachine*>(graph);
	StateTransition*	transition		= static_cast<StateTransition*>(connection);

	QPoint start, end;
	CalcTransitionStartAndEndPoints( stateMachine, transition, start, end, this );
	return QRect( start, end );
}


// render the connection we're creating, if any
void GraphRendererState::RenderCreateConnection(QPainter& painter, Graph* graph, const GraphHelpers::CreateConnectionInfo& createConnectionInfo, const QPoint& globalMousePos)
{
	// calculate the start and end point
	QPoint start, end;
	QRect createConNodeRect = CalcNodeRect( graph, createConnectionInfo.mConNode );
	start = createConNodeRect.topLeft() + createConnectionInfo.mConStartOffset;
	end	  = globalMousePos;

	// get the color
	QColor color = mShared->GetConnectionColor();

	// render the arrow
	QPen pen;
	QBrush brush;
	RenderArrow( painter, pen, brush, start, end, color, false, false, false, false, false, false, -1.0f );
}


int32 GraphRendererState::CalcRequiredStateHeight()
{
	return GraphShared::GetStateDiameter();
}


int32 GraphRendererState::CalcRequiredStateWidth(Node* node, const QFontMetrics& headerFontMetrics)
{
	return GraphShared::GetStateDiameter();
}


// find the condition at the mouse position
StateTransitionCondition* GraphRendererState::FindCondition(StateMachine* stateMachine, StateTransition* transition, QPoint mousePos, GraphRenderer* renderer)
{
	// if the transition is invalid, return directly
	if (transition == NULL)
		return NULL;

	QPoint start, end;
	CalcTransitionStartAndEndPoints( stateMachine, transition, start, end, renderer );

	const Vector2	transitionStart(start.rx(), start.ry());
	const Vector2	transitionEnd(end.rx(), end.ry());

	// get the number of conditions
	const uint32 numConditions = transition->GetNumConditions();

	// precalculate some values we need for the condition rendering
	const float		circleDiameter	= 3.0f;
	const float		circleStride	= 4.0f;
	const float		elementSize		= circleDiameter + circleStride;
	const Vector2	localEnd		= transitionEnd - transitionStart;

	// only draw the transition conditions in case the arrow has enough space for it, avoid zero rect sized crashes as well
	if (localEnd.Length() > numConditions * elementSize)
	{
		const Vector2	transitionMid	= transitionStart + localEnd*0.5f;
		const Vector2	transitionDir	= localEnd.Normalized();
		//const float		halfLength		= localEnd.Length() * 0.5f;
		const Vector2	conditionStart	= transitionMid - transitionDir*(elementSize*0.5f*(float)numConditions);

		// iterate through the conditions and render them
		for (uint32 i=0; i<numConditions; ++i)
		{
			// get access to the condition
			StateTransitionCondition* condition = transition->GetCondition(i);

			// calculate the circle middle point
			const Vector2 circleMid = conditionStart  + transitionDir*(elementSize*(float)i);

			const float distance =  Vector2( Vector2(mousePos.x(), mousePos.y()) - circleMid ).SafeLength();
			if (distance <= circleDiameter)
				return condition;
		}
	}

	return NULL;
}


// does it intersects the rect
bool GraphRendererState::IntersectsTransition(StateMachine* stateMachine, StateTransition* transition, const QRect& rect, GraphRenderer* renderer)
{
	QPoint start, end;
	CalcTransitionStartAndEndPoints( stateMachine, transition, start, end, renderer );

	return GraphHelpers::LineIntersectsRect( rect, start.x(), start.y(), end.x(), end.y() );
}


//
bool GraphRendererState::IsCloseToTransition(StateMachine* stateMachine, StateTransition* transition, const QPoint& point, GraphRenderer* renderer)
{	
	QPoint start, end;
	CalcTransitionStartAndEndPoints( stateMachine, transition, start, end, renderer );

	return (GraphHelpers::DistanceToLine(start.x(), start.y(), end.x(), end.y(), point.x(), point.y()) <= 5.0f);
}


bool GraphRendererState::IsCloseToTransitionHead(StateMachine* stateMachine, StateTransition* transition, const QPoint& point, GraphRenderer* renderer)
{
	QPoint start, end;
	CalcTransitionStartAndEndPoints( stateMachine, transition, start, end, renderer );

	Vector2 dir = Vector2( end.x()-start.x(), end.y()-start.y() );
	dir.Normalize();
	Vector2 newStart = Vector2(end.x(), end.y()) - dir * 5.0f;

	return (GraphHelpers::DistanceToLine(newStart.x, newStart.y, end.x(), end.y(), point.x(), point.y()) <= 7.0f);
}


bool GraphRendererState::IsCloseToTransitionTail(StateMachine* stateMachine, StateTransition* transition, const QPoint& point, GraphRenderer* renderer)
{
	QPoint start, end;
	CalcTransitionStartAndEndPoints( stateMachine, transition, start, end, renderer );

	Vector2 dir = Vector2( end.x()-start.x(), end.y()-start.y() );
	dir.Normalize();
	Vector2 newStart = Vector2(start.x(), start.y()) + dir * 6.0f;

	return (Vector2(newStart - Vector2(point.x(), point.y())).SafeLength() <= 6.0f);
}


// calc the start and end point
void GraphRendererState::CalcTransitionStartAndEndPoints(StateMachine* stateMachine, StateTransition* transition, QPoint& outStart, QPoint& outEnd, GraphRenderer* renderer)
{
	Node*	sourceNode		= transition->GetSourceNode();
	Node*	targetNode		= transition->GetTargetNode();

	// calc target node rect
	int32 targetMaxInputWidth, targetMaxOutputWidth;
	QRect targetNodeRect = renderer->CalcNodeRect( stateMachine, targetNode, &targetMaxInputWidth, &targetMaxOutputWidth );

	// calc source node rect
	QRect sourceNodeRect;
	if (sourceNode != NULL)
	{
		int32 sourceMaxInputWidth, sourceMaxOutputWidth;
		sourceNodeRect = renderer->CalcNodeRect( stateMachine, sourceNode, &sourceMaxInputWidth, &sourceMaxOutputWidth );
	}

	QPoint startOffset( transition->GetVisualStartOffsetX(), transition->GetVisualStartOffsetY() );
	QPoint endOffset( transition->GetVisualEndOffsetX(), transition->GetVisualEndOffsetY() );

	QPoint end	 = targetNodeRect.topLeft() + endOffset;
	QPoint start = startOffset;
	if (sourceNode != NULL)
		start += sourceNodeRect.topLeft();

	QRect sourceRect;
	if (sourceNode != NULL)
		sourceRect = sourceNodeRect;
	//sourceRect.adjust(-2,-2,2,2);

	QRect targetRect = targetNodeRect;
	targetRect.adjust(-2,-2,2,2);

	// calc the real start point
	double realX, realY;
	//if (GraphHelpers::LineIntersectsRect(sourceRect, start.x(), start.y(), end.x(), end.y(), &realX, &realY))
	Vector2 sourceStateCenter = Vector2(sourceNode->GetVisualPosX(), sourceNode->GetVisualPosY()) + Vector2(GraphShared::GetStateRadius(), GraphShared::GetStateRadius());
	if (GraphHelpers::LineIntersectsCircle( sourceStateCenter.x, sourceStateCenter.y, GraphShared::GetStateRadius(), start.x(), start.y(), end.x(), end.y(), false, &realX, &realY) == true)
	{
		start.setX( realX );
		start.setY( realY );
	}

	// calc the real end point
	Vector2 targetStateCenter = Vector2(targetNode->GetVisualPosX(), targetNode->GetVisualPosY()) + Vector2(GraphShared::GetStateRadius(), GraphShared::GetStateRadius());
	//if (GraphHelpers::LineIntersectsRect(targetRect, start.x(), start.y(), end.x(), end.y(), &realX, &realY))
	if (GraphHelpers::LineIntersectsCircle( targetStateCenter.x, targetStateCenter.y, GraphShared::GetStateRadius(), start.x(), start.y(), end.x(), end.y(), true, &realX, &realY) == true)
	{
		end.setX( realX );
		end.setY( realY );
	}

	outStart	= start;
	outEnd		= end;
}


// is the given point close to the connection
bool GraphRendererState::IsCloseToConnection(Graph* graph, const QPoint& point, Connection* connection)
{
	StateMachine*		stateMachine	= static_cast<StateMachine*>(graph);
	StateTransition*	transition		= static_cast<StateTransition*>(connection);

	QPoint start, end;
	CalcTransitionStartAndEndPoints( stateMachine, transition, start, end, this );
	return (GraphHelpers::DistanceToLine(start.x(), start.y(), end.x(), end.y(), point.x(), point.y()) <= 5.0f);
}


bool GraphRendererState::IsPointOnNode(Graph* graph, Node* node, const QPoint& globalPoint)
{
	Vector2 center	= Vector2(node->GetVisualPosX(), node->GetVisualPosY()) + Vector2(GraphShared::GetStateRadius(), GraphShared::GetStateRadius());

	return GraphHelpers::IsInCircle( center.x, center.y, GraphShared::GetStateRadius(), globalPoint.x(), globalPoint.y() );
}


bool GraphRendererState::IsPointInInputPort(Graph* graph, Node* node, const QPoint& point)
{
	Vector2 center	= Vector2(node->GetVisualPosX(), node->GetVisualPosY()) + Vector2(GraphShared::GetStateRadius(), GraphShared::GetStateRadius());

	return GraphHelpers::IsInCircle( center.x, center.y, GraphShared::GetStateRadius()-GraphShared::GetStateOutputPortSize(), point.x(), point.y() );
}


bool GraphRendererState::IsPointInOutputPort(Graph* graph, Node* node, const QPoint& point)
{
	if (IsPointOnNode(graph, node, point) == true && IsPointInInputPort(graph, node, point) == false)
		return true;

	return false;
}


// remove all input ports
Port* GraphRendererState::FindPort(Graph* graph, Node* node, int32 x, int32 y, uint32* outPortNr, bool* outIsInputPort, bool includeInputPorts)
{
	
	// if the node is collapsed we can skip directly, too
	if (node->GetCollapsedState() == Node::COLLAPSE_ALL)
		return NULL;

	// calculate the node rect
	/*QRect nodeRect = */CalcNodeRect( graph, node );

	// check the input ports
	if (includeInputPorts == true)
	{
		if (IsPointInInputPort(graph, node, QPoint(x, y)) == true)
		{
			*outPortNr		= 0;
			*outIsInputPort = true;
			return &(node->GetInputPort(0));
		}
	}

	// check the output ports
	if (IsPointInOutputPort(graph, node, QPoint(x, y)) == true)
	{
		*outPortNr		= 0;
		*outIsInputPort = false;
		return &(node->GetOutputPort(0));
	}

	return NULL;
}


// find the port at a given location
Port* GraphRendererState::FindPort(Graph* graph, int32 x, int32 y, Node** outNode, uint32* outPortNr, bool* outIsInputPort, bool includeInputPorts)
{
	// if there is no valid graph, return directly
	if (graph == NULL)
		return NULL;

	// get the number of nodes in the graph and iterate through them
	const uint32 numNodes = graph->GetNumNodes();
	for (uint32 n=0; n<numNodes; ++n)
	{
		// get a pointer to the graph node
		Node* graphNode = graph->GetNode(n);

		// skip the node in case it is collapsed
		if (graphNode->GetCollapsedState() == Node::COLLAPSE_ALL)
			continue;

		// check if we're in a port of the given node
		Port* result = FindPort( graph, graphNode, x, y, outPortNr, outIsInputPort, includeInputPorts );
		if (result != NULL)
		{
			*outNode =graphNode;
			return result;
		}
	}

	// failure, no port at the given coordinates
	return NULL;
}


// find the connection at the given mouse position
Connection* GraphRendererState::FindConnection(Graph* graph, const QPoint& mousePos)
{
	// if there is no valid graph, return directly
	if (graph == NULL)
		return NULL;

	// get the number of connections and iterate through them
	const uint32 numConnections = graph->GetNumConnections();
	for (uint32 c=0; c<numConnections; ++c)
	{
		Connection* connection = graph->GetConnection(c);
		if (IsCloseToConnection(graph, mousePos, connection) == true)
			return connection;
	}

	// failure, there is no connection at the given mouse position
	return NULL;
}


// select all nodes within a given rect::pain
void GraphRendererState::SelectNodesAndConnectionsInRect(Graph* graph, const QRect& selectionRect, bool overwriteCurSelection, bool select, bool toggleMode)
{
	if (overwriteCurSelection == true)
	{
		mShared->UnselectAllNodes();
		mShared->UnselectAllConnections();
	}

	// if there is no valid graph, return directly
	if (graph == NULL)
		return;

	StateMachine* stateMachine = static_cast<StateMachine*>(graph);

	// get the number of nodes and iterate through them
	const uint32 numNodes = graph->GetNumNodes();
	for (uint32 i=0; i<numNodes; ++i)
	{
		// get the current node and calculate its rect
		Node*	node		= graph->GetNode(i);
		QRect	nodeRect	= CalcNodeRect(graph, node);

		// check if the node rect is inside the selection rect, if yes select the node
		if (nodeRect.intersects(selectionRect) == true)
		{
			if (toggleMode == true)
				mShared->SetNodeSelected(node, !mShared->IsNodeSelected(node));
			else
				mShared->SetNodeSelected(node, select);
		}
		else
		{
			if (overwriteCurSelection)
				mShared->UnselectNode(node);
		}
	}

	// get the number of connections attached to the node and iterate through them
	const uint32 numConnections = graph->GetNumConnections();
	for (uint32 c=0; c<numConnections; ++c)
	{
		// get the current connection and calculate its rect
		Connection*			connection		= graph->GetConnection(c);
		/*const QRect			connectionRect	= */CalcConnectionRect( graph, connection );
		StateTransition*	transition		= static_cast<StateTransition*>(connection);

		// check if the connectionrect is inside the selection rect, if yes select the connection
		if (IntersectsTransition(stateMachine, transition, selectionRect, this) == true)
		{
			if (toggleMode == true)
				mShared->SetConnectionSelected( connection, !mShared->IsConnectionSelected(connection) );
			else
				mShared->SetConnectionSelected( connection, select );
		}
		else
		{
			if (overwriteCurSelection)
				mShared->UnselectConnection( connection );
		}
	}
}

bool GraphRendererState::IsCloseToHead(Connection* connection, const QPoint& point)
{
	if (connection->GetType() != StateTransition::TYPE_ID)
		return false;

	StateTransition* transition = static_cast<StateTransition*>(connection);
	StateMachine* stateMachine = transition->GetStateMachine();

	// calculate its start and end points
	QPoint start, end;
	CalcTransitionStartAndEndPoints( stateMachine, transition, start, end, this );

	Vector2 dir = Vector2( end.x()-start.x(), end.y()-start.y() );
	dir.Normalize();
	Vector2 newStart = Vector2(end.x(), end.y()) - dir * 5.0f;

	return (GraphHelpers::DistanceToLine(newStart.x, newStart.y, end.x(), end.y(), point.x(), point.y()) <= 7.0f);
}


bool GraphRendererState::IsCloseToTail(Connection* connection, const QPoint& point)
{
	if (connection->GetType() != StateTransition::TYPE_ID)
		return false;

	StateTransition* transition = static_cast<StateTransition*>(connection);
	StateMachine* stateMachine = transition->GetStateMachine();

	// calculate its start and end points
	QPoint start, end;
	CalcTransitionStartAndEndPoints( stateMachine, transition, start, end, this );

	Vector2 dir = Vector2( end.x()-start.x(), end.y()-start.y() );
	dir.Normalize();
	Vector2 newStart = Vector2(start.x(), start.y()) + dir * 6.0f;

	return (Vector2(newStart - Vector2(point.x(), point.y())).SafeLength() <= 6.0f);
}


// render the connection we're creating, if any
void GraphRendererState::RenderRelinkConnection(Graph* graph, QPainter& painter, GraphHelpers::RelinkConnectionInfo& relinkInfo, const QPoint& mousePos)
{
	// make sure the graph is valid
	if (graph == NULL)
		return;

	// make sure the graph is a state machine
	if (graph->GetType() != StateMachine::TYPE_ID)
	{
		LogWarning("GraphRendererState::RenderRelinkConnection(): Input graph is no state machine");
		CORE_ASSERT(false);
		return;
	}

	// down-cast to state machine
	StateMachine* stateMachine = static_cast<StateMachine*>(graph);

	QPen pen;
	QBrush brush;

	bool isDisabled = false;
	bool isTransitioning = false;
	bool isSelected = false;
	bool isHighlighted = false;
	bool isConnectedHighlighted = false;
	float progress = 0.0;
	bool isHeadHighlighted = relinkInfo.IsRelinkingHead();
	bool isTailHighlighted = relinkInfo.IsRelinkingTail();

	StateTransition* relinkTransition = NULL;

	if (relinkInfo.IsRelinkingHead() == true && relinkInfo.GetRelinkHeadConnection() != NULL && relinkInfo.GetRelinkHeadConnection()->GetType() == StateTransition::TYPE_ID)
	{
		relinkTransition = static_cast<StateTransition*>( relinkInfo.GetRelinkHeadConnection() );
	}
	else if (relinkInfo.IsRelinkingTail() == true && relinkInfo.GetRelinkTailConnection() != NULL && relinkInfo.GetRelinkTailConnection()->GetType() == StateTransition::TYPE_ID)
	{
		relinkTransition = static_cast<StateTransition*>( relinkInfo.GetRelinkTailConnection() );
	}

	if (relinkTransition == NULL)
		return;	

	QPoint start, end;
	CalcTransitionStartAndEndPoints( stateMachine, relinkTransition, start, end, this );

	// overwrite start or end with the current mouse position
	if (relinkInfo.IsRelinkingHead() == true)
		end = mousePos;
	if (relinkInfo.IsRelinkingTail() == true)
		start = mousePos;

	QColor baseColor = mShared->GetConnectionColor();
	RenderStateTransition( painter, pen, brush, start, end, isDisabled, isTransitioning, progress, baseColor, isSelected, isHighlighted, isConnectedHighlighted, isHeadHighlighted, isTailHighlighted );
}
