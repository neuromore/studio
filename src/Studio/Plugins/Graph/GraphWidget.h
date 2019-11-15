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

#ifndef __NEUROMORE_GRAPHWIDGET_H
#define __NEUROMORE_GRAPHWIDGET_H

// include required headers
#include "../../Config.h"
#include <Graph/Graph.h>
#include "GraphRenderer.h"
#include <Core/EventHandler.h>
#include <Core/FpsCounter.h>
#include <Graph/StateMachine.h>
#include <Graph/StateTransition.h>
#include <Graph/StateTransitionCondition.h>
#include <Core/Timer.h>
#include <QtBaseManager.h>
#include "GraphHelpers.h"
#include "GraphShared.h"
#include <QWidget>
#include <QFontMetrics>
#include <QPainter>
#include <QTransform>
#include <QPainterPath>
#include <QColor>
#include <QDropEvent>
#include <QMimeData>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGestureEvent>
#include <QPanGesture>
#include <QPinchGesture>
#include <QSwipeGesture>


// forward declaration
class GraphPlugin;

class GraphWidget : public QOpenGLWidget, Core::EventHandler, protected QOpenGLFunctions
{
	Q_OBJECT
	public:
		enum Type
		{
			CLASSIFIER		= 0,
			STATEMACHINE	= 1
		};

		GraphWidget(Type type, GraphPlugin* plugin, QWidget* parent=NULL);
		virtual ~GraphWidget();

		// opengl widget specific implemenations
		void initializeGL() override;
		void resizeGL(int width, int height) override;
		void paintGL() override;

		void InitForGraph(Graph* graph);
		void ShowGraph(Graph* graph, bool updateHistory=true);
		//void ShowGraph(const char* graphNodeName, bool updateHistory=true);
		Graph* GetShownGraph() const												{ return mShownGraph; }
		Graph* GetGraph() const														{ return mGraph; }
		
        void CenterGraph(Graph* graph);
        void FitActiveGraphOnScreen()												{ FitGraphOnScreen(mShownGraph, width(), height()); }

		void SetAllowInteraction(bool flag)											{ mAllowInteraction = flag; }
		bool GetAllowInteraction() const											{ return mAllowInteraction; }

		// events
		void mouseMoveEvent(QMouseEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;
		void mouseDoubleClickEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;
		void wheelEvent(QWheelEvent* event) override;
		void keyPressEvent(QKeyEvent* event) override;
		void keyReleaseEvent(QKeyEvent* event) override;

		GraphShared* GetShared()													{ return &mShared; }

		void ApplyScale(float scaleDelta);
        void ApplyScroll(int deltaX, int deltaY);

	signals:
		void SelectionChanged();

	private slots:
		void OnContextMenuCreateNode();
		void OnEmitSelectionChangedSignal()											{ emit SelectionChanged(); }
		void OnSelectionChanged();

		void OnRemoveSelected();
		void OnResetSelected();
		void OnEnableDisableSelected();
		void OnLockUnlockSelected();

		// node alignment
		void OnAlignLeft()															{ GraphHelpers::AlignSelectedNodes( mShownGraph, GraphHelpers::ALIGN_LEFT, &mShared, mRenderer); }
		void OnAlignRight()															{ GraphHelpers::AlignSelectedNodes( mShownGraph, GraphHelpers::ALIGN_RIGHT, &mShared, mRenderer); }
		void OnAlignTop()															{ GraphHelpers::AlignSelectedNodes( mShownGraph, GraphHelpers::ALIGN_TOP, &mShared, mRenderer); }
		void OnAlignBottom()														{ GraphHelpers::AlignSelectedNodes( mShownGraph, GraphHelpers::ALIGN_BOTTOM, &mShared, mRenderer); }

		
		// simple copy/paste
		void OnCutObjects();
		void OnCopyObjects();
		void OnPasteObjects();

		// state machine specific
		void OnForceStateActivate();

	private:
		
		// EVENTS ----------------------------------------------
		void OnRemoveNode(Graph* graph, Node* nodeToRemove) override						{ GetShared()->UnselectNode(nodeToRemove); }
		void OnNodeAdded(Graph* graph, Node* addedNode) override							{}
		void OnRemoveConnection(Graph* graph, Connection* connection) override				{ GetShared()->UnselectConnection(connection); }
		void OnConnectionAdded(Graph* graph, Connection* connection) override				{}
		// -----------------------------------------------------
		
		
		// context menu
		void OnContextMenuEvent();

		// drag & drop
		void dropEvent(QDropEvent* event) override;
		void dragEnterEvent(QDragEnterEvent* event) override;
		void dragLeaveEvent(QDragLeaveEvent* event) override			{ event->accept(); }
		void dragMoveEvent(QDragMoveEvent* event) override				{ event->accept(); }

		// gestures
		/*bool gestureEvent(QGestureEvent* event);
		void OnPanGesture(QGestureEvent* event, QPanGesture* gesture);
		void OnPinchGesture(QGestureEvent* event, QPinchGesture* gesture);
		void OnSwipeGesture(QGestureEvent* event, QSwipeGesture* gesture);*/


		// simple copy/paste
		void CopySelection();
		void ClearCopyBuffer();
		void PasteAt(QPoint pos);
		Core::Array<Node*>				mCopiedNodes;
		Core::Array<Connection*>		mCopiedConnections;


		// rect calculation helpers
		QRect CalcRectFromGraph(Graph* graph);
		QRect CalcRectFromNodes(const Core::Array<Node*>& nodes);
		
		// zooming helpers
		void FitGraphOnScreen(Graph* graph, int32 width, int32 height);
		void ZoomOnRect(const QRect& rect, int32 width, int32 height);
		void ScrollRectCentered(const QRect& rect, int32 width, int32 height);

		// base event
		bool event(QEvent* event) override;

		GraphPlugin*				mGraphPlugin;

		// mouse handling
		QPoint					mLocalMousePos;
		QPoint					mGlobalMousePos;
		bool					mLeftButtonPressed;
		bool					mMidButtonPressed;	
		bool					mRightButtonPressed;
		bool					mAltPressed;
		bool					mShiftPressed;
		bool					mControlPressed;
		int32					mPixelsMovedSinceRightClick;
		int32					mPixelsMovedSinceClick;
		Node*					mMousePressEventNode;

		GraphHelpers::CreateConnectionInfo mCreateConnectionInfo;
		GraphHelpers::RelinkConnectionInfo mRelinkConnectionInfo;

		// find nodes and connections under mouse position
		Node* FindNode(const QPoint& globalPoint);
		
		// selection
		void CalcSelectRect(QRect& outRect);

		Type							mType;

		QPoint							mSelectStart;
		QPoint							mSelectEnd;
		bool							mRectSelecting;
		bool							mAllowInteraction;
		bool							mGraphProtectionMode;
		bool							mDrawDebugInfo;

		// current graph infos
		Graph*							mGraph;
		Graph*							mShownGraph;
		Node*							mOnMouseOverNode;

		Core::String					mNoGraphShownError;
		Core::String					mGraphProtectionModeError;

		GraphShared						mShared;
		GraphRenderer*					mRenderer;

		// helpers
		Core::FpsCounter				mFpsCounter;
		Core::String					mTempString;
		Core::Timer						mTimer;
};


#endif
