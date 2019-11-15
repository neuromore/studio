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

#ifndef __NEUROMORE_GRAPHHELPERS_H
#define __NEUROMORE_GRAPHHELPERS_H

// include required headers
#include "../../Config.h"
#include <Core/StandardHeaders.h>
#include <Core/Array.h>
#include <Graph/Graph.h>
#include <Graph/Port.h>
#include <Graph/StateMachine.h>
#include <Graph/StateTransition.h>
#include <Graph/StateTransitionCondition.h>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>


#ifndef PRODUCTION_BUILD
	#define GRAPH_PERFORMANCESTATS
#endif


// forward declaration
class GraphShared;
class GraphRenderer;

class GraphHelpers
{
	public:
		GraphHelpers();
		~GraphHelpers();

		// rendering helpers
		void RenderGrid(GraphShared* shared, QPainter& painter, const QTransform& transform, float scale, const QColor& gridColor, const QColor& subGridColor, int32 width, int32 height);
		static void RenderPortArrow(QPainter& painter, const QRect& portRect);

		// mouse coordinate conversion helpers
		static QPoint LocalToGlobal(const QTransform& transform, const QPoint& inPoint);
		static QPoint GlobalToLocal(const QTransform& transform, const QPoint& inPoint);

		static bool IsInCircle(int32 centerX, int32 centerY, float radius, int32 pointX, int32 pointY);

		static Creud GetCreud(Node* node);

		// line intersection helpers
		static bool LineIntersectsRect(const QRect& b, float x1, float y1, float x2, float y2, double* outX=NULL, double* outY=NULL);
		static bool LinesIntersect( double Ax, double Ay, double Bx, double By, double Cx, double Cy, double Dx, double Dy, double* X, double* Y);
		static float DistanceToLine(float x1, float y1, float x2, float y2, float px, float py);
		static bool IsPointCloseToSmoothedLine(int32 x1, int32 y1, int32 x2, int32 y2, int32 px, int32 py);

		static bool LineIntersectsCircle(float centerX, float centerY, float radius, float x1, float y1, float x2, float y2, bool startPoint, double* outX=NULL, double* outY=NULL);

		static bool PathIntersectsRect(const QPainterPath& path, const QRect& rect);
		static float DistanceToPath(QPoint point, const QPainterPath& path);


		// node alignment
		enum ENodeAlignment
		{
			ALIGN_LEFT		= 0,
			ALIGN_RIGHT		= 1,
			ALIGN_TOP		= 2,
			ALIGN_BOTTOM	= 3
		};
		static void AlignSelectedNodes(Graph* graph, ENodeAlignment mode, GraphShared* shared, GraphRenderer* renderer);

		// create connection helpers
		class CreateConnectionInfo
		{
			public:
				CreateConnectionInfo();
				~CreateConnectionInfo();
				inline bool IsCreatingConnection() const													{ return mConNode != NULL; }
				void StartCreateConnection(uint32 portNr, bool isInputPort, Node* portNode, Port* port, const QPoint& startOffset);
				void StopCreateConnection();

				QPoint		mConStartOffset;
				QPoint		mConEndOffset;
				uint32		mConPortNr;
				bool		mConIsInputPort;
				Node*		mConNode;			// NULL when no connection is being created
				Port*		mConPort;
				bool		mConIsValid;
		};

		class RelinkConnectionInfo
		{
			public:
				RelinkConnectionInfo()
				{
					mRelinkHeadConnection = NULL;
					mRelinkTailConnection = NULL;
				}

				bool IsRelinkingHead() const					{ return mRelinkHeadConnection != NULL; }
				bool IsRelinkingTail() const					{ return mRelinkTailConnection != NULL; }
				bool IsRelinking() const						{ return (mRelinkHeadConnection != NULL || mRelinkTailConnection != NULL); }
				Connection* GetRelinkHeadConnection() const		{ return mRelinkHeadConnection; }
				Connection* GetRelinkTailConnection() const		{ return mRelinkTailConnection; }

				void StartRelinkHead(Connection* connection);
				void StartRelinkTail(Connection* connection);
				void StopRelinking();

			private:
				Connection*					mRelinkHeadConnection; // NULL when not replacing a  head
				Connection*					mRelinkTailConnection; // NULL when not replacing a  tail
		};

	private:
		QVector<QLine> mSubGridLines;
		QVector<QLine> mMainGridLines;
};


#endif
