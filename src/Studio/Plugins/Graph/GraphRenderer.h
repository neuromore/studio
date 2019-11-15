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

#ifndef __NEUROMORE_GRAPHRENDERER_H
#define __NEUROMORE_GRAPHRENDERER_H

// include required headers
#include "../../Config.h"
#include <Core/StandardHeaders.h>
#include <Graph/Node.h>
#include <Graph/Graph.h>
#include <Graph/Port.h>
#include "GraphHelpers.h"
#include "GraphShared.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include "GraphTextPixmapCache.h"
#include <PainterStaticTextCache.h>
#include <Graph/AnnotationNode.h>


class GraphRenderer
{
	public:
		GraphRenderer(GraphShared* shared);
		virtual ~GraphRenderer();

		virtual void RenderNodes(Graph* graph, QPainter& painter, const GraphHelpers::CreateConnectionInfo& createConnectionInfo, bool isWidgetEnabled, const QRect& visibleRect, const QPoint& mousePos);
		virtual void RenderNodeDebugInfo(Graph* graph, QPainter& painter, const GraphHelpers::CreateConnectionInfo& createConnectionInfo, bool isWidgetEnabled, const QRect& visibleRect, const QPoint& mousePos);
		virtual void RenderConnections(Graph* graph, QPainter& painter, const GraphHelpers::RelinkConnectionInfo& relinkConnectionInfo, Node* onMouseOverNode, const QPoint& globalMousePos, bool isWidgetEnabled, const QRect& visibleRect);

		// rect calculation helpers
		virtual QRect CalcNodeRect(Graph* graph, Node* node, int32* outMaxInputWidth=NULL, int32* outMaxOutputWidth=NULL);
		virtual QRect CalcConnectionRect(Graph* graph, Connection* connection);

		virtual QPainterPath& CalcConnectionPath(QPainterPath& inOutPath, Graph* graph, Connection* connection);
		virtual QPainterPath& CalcConnectionPath(QPainterPath& inOutPath, int startX, int startY, int endX, int endY);

		virtual Connection* FindConnection(Graph* graph, const QPoint& mousePos);
		virtual Port* FindPort(Graph* graph, Node* node, int32 x, int32 y, uint32* outPortNr, bool* outIsInputPort, bool includeInputPorts);
		virtual Port* FindPort(Graph* graph, int32 x, int32 y, Node** outNode, uint32* outPortNr, bool* outIsInputPort, bool includeInputPorts=true);

		virtual bool IsPointOverNodeIcon(Graph* graph, Node* node, const QPoint& globalPoint);
		virtual bool IsCloseToConnection(Graph* graph, const QPoint& point, Connection* connection);

		virtual bool IsPointOnNode(Graph* graph, Node* node, const QPoint& globalPoint);

		// TODO: not a good place here
		virtual void SelectNodesAndConnectionsInRect(Graph* graph, const QRect& selectionRect, bool overwriteCurSelection, bool select, bool toggleMode);

		// render create connection
		virtual void RenderCreateConnection(QPainter& painter, Graph* graph, const GraphHelpers::CreateConnectionInfo& createConnectionInfo, const QPoint& globalMousePos);

		// TODO deprecate this? or fix all problems: 1) cyclic graph detection missbehaves when creating a reverse connection 2) connection bezier is drawn wrong 3) other bugs may exist?
		virtual bool AllowCreateConnectionsStartingFromInputPorts() const													{ return false; }

		virtual bool AllowCreateConnectionsFromConnections() const															{ return true; }

		virtual void RenderRelinkConnection(Graph* graph, QPainter& painter, GraphHelpers::RelinkConnectionInfo& relinkInfo, const QPoint& mousePos)						{}

		virtual bool IsCloseToHead(Connection* connection, const QPoint& point)												{ return false; }
		virtual bool IsCloseToTail(Connection* connection, const QPoint& point)												{ return false; }

		// rendering helpers
		void RenderText(bool useCache, QPainter& painter, const Core::String& text, const QColor& textColor, const QRect& rect, const QFont& font, const QFontMetrics& fontMetrics, Qt::Alignment textAlignment=Qt::AlignCenter);

		// node chaches
		struct NodeCache
		{
			QRect						mRect;
			int32						mMaxInputWidth;
			int32						mMaxOutputWidth;
			bool						mIsValid;

			NodeCache()					{ Reset(); }
			inline void Reset()			{ mIsValid = false; }
			inline bool IsValid() const	{ return mIsValid; }
		};

		void ResetNodeCaches(Graph* graph);
		void UpdateNodeCaches(Graph* graph);

		const QPixmap& FindNodeIcon(Node* node, uint32 pixmapSize);

		GraphTextPixmapCache* GetPixmapTextCache()								{ return mPixmapTextCache; }
		PainterStaticTextCache* GetStaticTextCache()							{ return &mTextRenderingCache; }

	protected:
		GraphShared*				mShared;
		Core::String				mTempString;
		QPainterPath				mPainterPath;
		Core::Array<NodeCache>		mNodeCaches;

	private:
		void RenderConnection(Graph* graph, Connection* connection, bool isWidgetEnabled, QPainter& painter, QPen& pen, QBrush& brush, const QRect& visibleRect, float opacity, bool isSelected, bool isHighlighted, bool isProcessed);
		void RenderNode(Graph* graph, Node* node, QPainter& painter, const GraphHelpers::CreateConnectionInfo& createConnectionInfo, bool isWidgetEnabled, const QRect& visibleRect, const QPoint& mousePos);
		void RenderAnnotationNode(Graph* graph, AnnotationNode* node, QPainter& painter, bool isWidgetEnabled, const QRect& visibleRect, const QPoint& mousePos);
		
		static int32 CalcRequiredNodeWidth(GraphShared* shared, Node* node, const QFontMetrics& portFontMetrics, const QFontMetrics& infoFontMetrics, const QFontMetrics& headerFontMetrics, const QFontMetrics& subtitleFontMetrics, int32* outMaxInputWidth, int32* outMaxOutputWidth);
		static int32 CalcRequiredNodeHeight(GraphShared* shared, Node* node);
		QRect CalcSourceRect(Node* node, uint32 sourcePort, const QRect& nodeRect);
		QRect CalcTargetRect(Node* node, uint32 targetPort, const QRect& nodeRect);

		QRect CalcInputPortRect(Node* node, uint16 inputPortNr, const QRect& nodeRect, bool isPortHighlighted = false);
		QRect CalcOutputPortRect(Node* node, uint16 outputPortNr, const QRect& nodeRect, bool isPortHighlighted = false);
	
		QRect CalcInputPortTextRect(Node* node, uint16 inputPortNr, const QRect& nodeRect);
		QRect CalcOutputPortTextRect(Node* node, uint16 outputPortNr, const QRect& nodeRect);
	
		QRect CalcInputPortInteractionRect(Node* node, uint16 inputPortNr, const QRect& nodeRect);
		QRect CalcOutputPortInteractionRect(Node* node, uint16 outputPortNr, const QRect& nodeRect);

		static uint32 CalcMaxNodeInputPortWidth(Node* node, const QFontMetrics& fontMetrics);
		static uint32 CalcMaxNodeOutputPortWidth(Node* node, const QFontMetrics& fontMetrics);
		
		QRect CalcInfoAreaRect(Node* node, uint32 targetPort, const QRect& nodeRect);

		struct NodeIconCache
		{
			NodeIconCache(QPixmap pixmap, uint32 nodeType);
			~NodeIconCache();
			QPixmap		mPixmap;
			uint32		mNodeType;
		};

		Core::Array<NodeIconCache*>		mNodeIcons;

		GraphTextPixmapCache*			mPixmapTextCache;
		PainterStaticTextCache			mTextRenderingCache;
};


#endif
