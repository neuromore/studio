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

#ifndef __NEUROMORE_GRAPHRENDERERSTATE_H
#define __NEUROMORE_GRAPHRENDERERSTATE_H

// include required headers
#include "../../Config.h"
#include "GraphRenderer.h"
#include <Core/StandardHeaders.h>
#include <Graph/StateMachine.h>
#include <Graph/State.h>
#include <Graph/EntryState.h>
#include <Graph/ExitState.h>
#include <Graph/StateTransition.h>
#include <Graph/StateTransitionCondition.h>
#include "GraphHelpers.h"
#include "GraphShared.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>


class GraphRendererState : public GraphRenderer
{
	public:
		GraphRendererState(GraphShared* shared);
		virtual ~GraphRendererState();

		void RenderNodes(Graph* graph, QPainter& painter, const GraphHelpers::CreateConnectionInfo& createConnectionInfo, bool isWidgetEnabled, const QRect& visibleRect, const QPoint& mousePos) override final;
		void RenderConnections(Graph* graph, QPainter& painter, const GraphHelpers::RelinkConnectionInfo& relinkConnectionInfo, Node* onMouseOverNode, const QPoint& globalMousePos, bool isWidgetEnabled, const QRect& visibleRect) override final;

		QRect CalcNodeRect(Graph* graph, Node* node, int32* outMaxInputWidth=NULL, int32* outMaxOutputWidth=NULL) override final;
		QRect CalcConnectionRect(Graph* graph, Connection* connection) override final;

		bool AllowCreateConnectionsStartingFromInputPorts() const override final										{ return false; }
		bool IsPointOverNodeIcon(Graph* graph, Node* node, const QPoint& globalPoint) override final                    { return false; }
		
		bool IsPointOnNode(Graph* graph, Node* node, const QPoint& globalPoint) override;
		bool IsCloseToConnection(Graph* graph, const QPoint& point, Connection* connection) override final;

		Connection* FindConnection(Graph* graph, const QPoint& mousePos) override final;
		Port* FindPort(Graph* graph, Node* node, int32 x, int32 y, uint32* outPortNr, bool* outIsInputPort, bool includeInputPorts) override final;
		Port* FindPort(Graph* graph, int32 x, int32 y, Node** outNode, uint32* outPortNr, bool* outIsInputPort, bool includeInputPorts=true) override final;

		// TODO: not a good place here
		void SelectNodesAndConnectionsInRect(Graph* graph, const QRect& selectionRect, bool overwriteCurSelection, bool select, bool toggleMode) override final;

		void RenderCreateConnection(QPainter& painter, Graph* graph, const GraphHelpers::CreateConnectionInfo& createConnectionInfo, const QPoint& globalMousePos) override final;

		virtual bool AllowCreateConnectionsFromConnections() const override															{ return false; }

		bool IsCloseToHead(Connection* connection, const QPoint& point) override;
		bool IsCloseToTail(Connection* connection, const QPoint& point) override;
		void RenderRelinkConnection(Graph* graph, QPainter& painter, GraphHelpers::RelinkConnectionInfo& relinkInfo, const QPoint& mousePos) override final;

	private:
		void RenderState(StateMachine* stateMachine, State* state, QPainter& painter, const GraphHelpers::CreateConnectionInfo& createConnectionInfo, bool isWidgetEnabled, const QRect& visibleRect, const QPoint& mousePos);
		void RenderStateTransition(StateTransition* transition, QPainter& painter, const QColor& baseColor, bool isSelected, bool isHighlighted, bool isConnectedHighlighted, bool isHeadHighlighted, bool isTailHighlighted);
		void RenderStateConditions(StateTransition* transition, QPainter* painter, QPen* pen, QBrush* brush, QPoint& start, QPoint& end);
		void RenderArrow(QPainter& painter, QPen& pen, QBrush& brush, const QPoint& start, const QPoint& end, const QColor& baseColor, bool isSelected, bool isHighlighted, bool isConnectedHighlighted, bool isHeadHighlighted, bool isTransitioning, bool isDisabled, float progress);

		void RenderStateTransition(QPainter& painter, QPen& pen, QBrush& brush, const QPoint& startPos, const QPoint& endPos, bool isDisabled, bool isTransitioning, float progress, const QColor& baseColor, bool isSelected, bool isHighlighted, bool isConnectedHighlighted, bool isHeadHighlighted, bool isTailHighlighted);

		static int32 CalcRequiredStateWidth(Node* node, const QFontMetrics& headerFontMetrics);
		static int32 CalcRequiredStateHeight();

		bool IsPointInInputPort(Graph* graph, Node* node, const QPoint& point);
		bool IsPointInOutputPort(Graph* graph, Node* node, const QPoint& point);

		// state transitions
		static StateTransitionCondition* FindCondition(StateMachine* stateMachine, StateTransition* transition, QPoint mousePos, GraphRenderer* renderer);
		static void CalcTransitionStartAndEndPoints(StateMachine* stateMachine, StateTransition* transition, QPoint& outStart, QPoint& outEnd, GraphRenderer* renderer);
		static bool IsCloseToTransitionTail(StateMachine* stateMachine, StateTransition* transition, const QPoint& point, GraphRenderer* renderer);
		static bool IsCloseToTransitionHead(StateMachine* stateMachine, StateTransition* transition, const QPoint& point, GraphRenderer* renderer);
		static bool IsCloseToTransition(StateMachine* stateMachine, StateTransition* transition, const QPoint& point, GraphRenderer* renderer);
		static bool IntersectsTransition(StateMachine* stateMachine, StateTransition* transition, const QRect& rect, GraphRenderer* renderer);
};


#endif
