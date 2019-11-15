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

#ifndef __NEUROMORE_GRAPHSHARED_H
#define __NEUROMORE_GRAPHSHARED_H

// include required headers
#include "../../Config.h"
#include <Core/StandardHeaders.h>
#include <QtBaseConfig.h>
#include <Graph/Graph.h>
#include "GraphHelpers.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QVector>


class GraphShared : public QObject, public GraphHelpers
{
	Q_OBJECT
	public:
		GraphShared();
		virtual ~GraphShared();

		// main update method
		void Update(double timeDelta);

		// color accessors
		inline QColor GetReadOnlyColor() const						{ return mReadOnlyColor; }
		inline QColor GetBackgroundColor() const					{ return mBackgroundColor; }
		inline QColor GetGridColor() const							{ return mGridColor; }
		inline QColor GetSubGridColor() const						{ return mSubGridColor; }
		inline QColor GetActiveBorderColor() const					{ return mActiveBorderColor; }
		inline QColor GetPortColor() const							{ return mPortColor; }
		inline QColor GetConnectionBorderColor() const				{ return mConnectionBorderColor; }
		inline QColor GetSelectedColor() const						{ return mSelectedColor; }
		inline QColor GetConnectionColor() const					{ return mConnectionColor; }
		inline QColor GetHighlichtColor() const						{ return mHighlightFactor; }
		inline int32  GetHighlightFactor() const					{ return mHighlightFactor; }
		inline QColor GetErrorBlinkColor() const					{ int32 red = 160 + ((0.5f + 0.5f * Core::Math::Cos(mErrorBlinkOffset * 5.0f)) * 96); red = Core::Clamp<int32>(red, 0, 255); return QColor(red,0,0); }

		// color helpers
		void GetNodePortColors(bool isWidgetEnabled, const Port& nodePort, const QColor& borderColor, const QColor& headerBgColor, const QColor& textColor, QColor* outBrushColor, QColor* outPenColor, QColor* outTextColor, bool isNodeSelected, bool isPortHighlighted);
		inline QColor GetNodeColor(bool isWidgetEnabled, Node* node) const { if (isWidgetEnabled == false) return GetReadOnlyColor(); return ToQColor(node->GetColor()); }

		// font accessors
		inline const QFont& GetStandardFont() const					{ return mStandardFont; }
		inline const QFont& GetNodeHeaderFont() const				{ return mNodeHeaderFont; }
		inline const QFont& GetNodeInfoFont() const					{ return mNodeInfoFont; }
		inline const QFont& GetNodePortFont() const					{ return mNodePortFont; }
		inline const QFont& GetNodeSubTitleFont() const				{ return mNodeSubTitleFont; }
		inline const QFontMetrics& GetNodeHeaderMetrics() const		{ return *mNodeHeaderMetrics; }
		inline const QFontMetrics& GetNodeInfoMetrics() const		{ return *mNodeInfoMetrics; }
		inline const QFontMetrics& GetNodePortMetrics() const		{ return *mNodePortMetrics; }
		inline const QFontMetrics& GetNodeSubTitleMetrics() const	{ return *mNodeSubTitleMetrics; }
		inline const QFontMetrics& GetStandardFontMetrics() const	{ return *mStandardFontMetrics; }
		inline QTextOption& GetTextOption()							{ return mTextOption; }

		inline double GetErrorBlinkOffset() const					{ return mErrorBlinkOffset; }
		inline double GetConnectionDashOffset() const				{ return mConnectionDashOffset; }

		// node selection
		Node* GetSelectedNode(uint32 index) const																{ return mSelectedNodes[index]; }
		Node* GetSingleSelectedNode() const																		{ if (GetNumSelectedNodes() == 1) return mSelectedNodes[0]; return NULL; }
		uint32 GetNumSelectedNodes() const																		{ return mSelectedNodes.Size(); }
		bool IsNodeSelected(Node* node) const																	{ return mSelectedNodes.Contains(node); }
		void SelectNode(Node* node, bool emitSignal=true)														{ if (IsNodeSelected(node) == false) { mSelectedNodes.Add(node); if (emitSignal == true) emit SelectionChanged(); } }
		void UnselectNode(Node* node, bool emitSignal=true)														{ mSelectedNodes.RemoveByValue(node); emit SelectionChanged(); }
		void SetNodeSelected(Node* node, bool select, bool emitSignal=true)										{ if (select == true) SelectNode(node, emitSignal); else UnselectNode(node, emitSignal); }
		void UnselectAllNodes(bool emitSignal=true)																{ mSelectedNodes.Clear(); if (emitSignal == true) emit SelectionChanged(); }
		const Core::Array<Node*>& GetSelectedNodesArray() const													{ return mSelectedNodes; }
		
		// connection selection
		Connection* GetSelectedConnection(uint32 index) const													{ return mSelectedConnections[index]; }
		Connection* GetSingleSelectedConnection() const															{ if (GetNumSelectedConnections() == 1) return mSelectedConnections[0]; return NULL; }
		uint32 GetNumSelectedConnections() const																{ return mSelectedConnections.Size(); }
		Core::Array<Connection*> GetSelectedConnectionsArray() const											{ return mSelectedConnections; }
		bool IsConnectionSelected(Connection* connection) const													{ return mSelectedConnections.Contains(connection); }
		void SelectConnection(Connection* connection, bool emitSignal=true)										{ if (IsConnectionSelected(connection) == false) { mSelectedConnections.Add(connection); if (emitSignal == true) emit SelectionChanged(); } }
		void UnselectConnection(Connection* connection, bool emitSignal=true)									{ mSelectedConnections.RemoveByValue(connection); if (emitSignal == true) emit SelectionChanged(); }
		void SetConnectionSelected(Connection* connection, bool select, bool emitSignal=true)					{ if (select == true) SelectConnection(connection, emitSignal); else UnselectConnection(connection, emitSignal); }
		void UnselectAllConnections(bool emitSignal=true)														{ mSelectedConnections.Clear(); if (emitSignal == true) emit SelectionChanged(); }

		void EmitSelectionChangedSignal()																		{ emit SelectionChanged(); }

		// connection dash styles for line drawing
		inline QVector<qreal>&	GetConnectionDashPattern( uint32 index )										{ return mConnectionDashPatterns[index]; }

		// transformation
		void SetScale(float scale, bool animate=false);
		inline float GetScale() const																			{ return mScale; }
		void SetScrollOffset(QPoint scrollOffset)																{ mScrollOffset = scrollOffset; }
		inline QPoint GetScrollOffset() const																	{ return mScrollOffset; }
		void SetScalePivot(QPoint pivot)																		{ mScalePivot = pivot; }
		inline QPoint GetScalePivot() const																		{ return mScalePivot; }
		const QTransform& GetTransform() const																	{ return mTransform; }

		// active state animation
		double GetActiveStateAnimOffset() const																	{ return mActiveStateAnimationOffset; }
		bool IsActiveStateAnimationPhaseA() const																{ return mActiveStateAnimationPhaseA; }

		void ResetTransform();

		void SetScreenInfo(float physicalScreenWidthInMillimeters, float physicalScreenHeightInMillimeters, float dpi);
		float GetScreenScaling() const										{ return mScreenScale; }

		float GetBorderRadius() const										{ return 6.0f * GetScreenScaling(); }

		static float GetStateOutputPortSize()								{ return 15.0f; }
		static float GetStateRadius()										{ return 50.0f; }
		static float GetStateDiameter()										{ return 2.0f * GetStateRadius(); }

		static float GetPortDiameter()										{ return 10.0f; }
		static float GetPortDistance()										{ return 4.0f; }					// vertical dist between ports (border to border)
		static float GetPortTextDistance()									{ return 5.0f; }					// horizontal dist between ports and port text
		static float GetNodeHeaderHeight()									{ return 25.0f; }					// height of the node header
		static float GetPortTextNodeInfoDistance()							{ return 5.0f; }					// horizontal dist between port texts and node info texts


	signals:
		void SelectionChanged();

	private:
		// colors
		QColor							mReadOnlyColor;
		QColor							mBackgroundColor;
		QColor							mGridColor;
		QColor							mSubGridColor;
		QColor							mActiveBorderColor;
		QColor							mPortColor;
		QColor							mConnectionBorderColor;
		QColor							mSelectedColor;
		QColor							mConnectionColor;
		int32							mHighlightFactor;

		float							mScreenScale;
		float							mScreenDPI;

		// fonts and metrics
		QFont							mNodeHeaderFont;
		QFontMetrics*					mNodeHeaderMetrics;
		QFont							mNodeInfoFont;
		QFontMetrics*					mNodeInfoMetrics;
		QFont							mNodePortFont;
		QFontMetrics*					mNodePortMetrics;
		QFont							mNodeSubTitleFont;
		QFontMetrics*					mNodeSubTitleMetrics;
		QFont							mStandardFont;
		QFontMetrics*					mStandardFontMetrics;
		QTextOption						mTextOption;

		double							mActiveStateAnimationOffset;
		bool							mActiveStateAnimationPhaseA;

		// animated error
		double							mErrorBlinkOffset;

		// animated dashed connection helpers
		double							mConnectionDashOffset;

		// selection
		Core::Array<Node*>				mSelectedNodes;
		Core::Array<Connection*>		mSelectedConnections;

		// transformation information
		QTransform						mTransform;
		float							mScale;
		QPoint							mScalePivot;
		QPoint							mScrollOffset;


		bool							mAnimationScaleActive;
		float							mAnimationScaleCurrentStart;
		float							mAnimationScaleTargetStart;
		float							mAnimationScaleTarget;
		float							mAnimationScaleTime;

		// custom dash patterns for drawing connections
		Core::Array<QVector<qreal>>		mConnectionDashPatterns;
};


#endif
