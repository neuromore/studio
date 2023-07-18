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
#include "GraphRenderer.h"
#include "GraphPaletteWidget.h"
#include <DSP/AttributeChannels.h>

using namespace Core;

// constructor
GraphRenderer::GraphRenderer(GraphShared* shared)
{
	mShared = shared;

	// initialize the pixmap text rendering cache
	mPixmapTextCache = new GraphTextPixmapCache(mShared);

	// initialize the text rendering cache
	mTextRenderingCache.Init();
}


// destructor
GraphRenderer::~GraphRenderer()
{
	// get rid of the text cache
	delete mPixmapTextCache;

	// get rid of the node icons
	const uint32 numNodeIcons = mNodeIcons.Size();
	for (uint32 i=0; i<numNodeIcons; ++i)
		delete mNodeIcons[i];
	mNodeIcons.Clear();
}


// render text
void GraphRenderer::RenderText(bool useCache, QPainter& painter, const String& text, const QColor& textColor, const QRect& rect, const QFont& font, const QFontMetrics& fontMetrics, Qt::Alignment textAlignment)
{
	// use direct text rendering, no caching to pixmaps
	if (useCache == false)
	{
		// NOTE: DOES NOT WORK WELL ON ALL GRAPHICS CARDS
		// direct text drawing
       /*mShared->GetTextOption().setAlignment(textAlignment);

		painter.setFont( font );
		painter.setPen( textColor );
		painter.setBrush( Qt::NoBrush );

		painter.drawText( rect, text.AsChar(), mShared->GetTextOption() );*/

	    // uncached static text drawing
		//mTextRenderingCache.RenderTextUncached( painter, text, textColor, rect, font, fontMetrics, textAlignment );

		// new ring buffer based pre-texture rendered text rendering
		mPixmapTextCache->RenderTextUncached( painter, text, textColor, rect, font, fontMetrics, textAlignment );

		return;
	}

	// use cached text rendering
	// 25% faster than static text method
	mPixmapTextCache->RenderText( painter, text, textColor, rect, font, fontMetrics, textAlignment );
	//mTextRenderingCache.RenderText( painter, text, textColor, rect, font, fontMetrics, textAlignment );
}


// render all nodes
void GraphRenderer::RenderNodes(Graph* graph, QPainter& painter, const GraphHelpers::CreateConnectionInfo& createConnectionInfo, bool isWidgetEnabled, const QRect& visibleRect, const QPoint& mousePos)
{
	// make sure the graph is valid
	if (graph == NULL)
		return;

	// render all annotation first (so they are on the bottom layer)
	// TODO in the future we can replace this with correct Z sorting
	const uint32 numNodes = graph->GetNumNodes();
	for (uint32 i = 0; i < numNodes; ++i)
	{
		Node* node = graph->GetNode(i);
		if (node->GetType() != AnnotationNode::TYPE_ID)
			continue;

		AnnotationNode* annotationNode = static_cast<AnnotationNode*>(node);
		RenderAnnotationNode(graph, annotationNode, painter, isWidgetEnabled, visibleRect, mousePos);
	}


	// now draw all nodes that are not annotation nodes
	for (uint32 i=0; i<numNodes; ++i)
	{
		Node* node = graph->GetNode(i);
		if (node->GetType() == AnnotationNode::TYPE_ID)
			continue;

		RenderNode(graph, node, painter, createConnectionInfo, isWidgetEnabled, visibleRect, mousePos);
	}
}


// render a normal graph node
void GraphRenderer::RenderNode(Graph* graph, Node* node, QPainter& painter, const GraphHelpers::CreateConnectionInfo& createConnectionInfo, bool isWidgetEnabled, const QRect& visibleRect, const QPoint& mousePos)
{
	const float solidOpacity		 = 1.0f;
	const float halfTransOpacity	 = 0.60f;
	const float inactiveTransOpacity = 0.2f;

	const bool isSelected  = mShared->IsNodeSelected(node);
	
	const double borderWidth  = 2.0 * mShared->GetScreenScaling();
	const double headerHeight = mShared->GetNodeHeaderHeight() * mShared->GetScreenScaling();
	const double padding	  = 2.0f * mShared->GetScreenScaling();
	const double iconSize	  = headerHeight - 2*padding;

	// calc node dimensions
	int32 maxInputWidth, maxOutputWidth;
	QRect rect = CalcNodeRect( graph, node, &maxInputWidth, &maxOutputWidth );

	// calc the rect in screen space (after scrolling and zooming)
	QRect finalRect = mShared->GetTransform().mapRect( rect );

	// check if the node is visible, skip directly if it is not
	bool isVisible = finalRect.intersects( visibleRect );
	if (isVisible == false)
		return;

	// check if the mouse is over the node, if yes highlight the node
	const bool isHighlighted = IsPointOnNode(graph, node, mousePos);

	// border and background colors
	QColor nodeColor = mShared->GetNodeColor( isWidgetEnabled, node );
	QColor bgColor;
	QColor borderColor = nodeColor;
	
	if (isSelected == true)
	{
		borderColor = mShared->GetSelectedColor();
		bgColor		= mShared->GetSelectedColor();
	}
	else // not selected
	{
		if (isWidgetEnabled == true)
			bgColor = nodeColor;
		else
			bgColor.setRgbF(0.3f, 0.3f, 0.3f, 1.0f);
	}

	// locked node: grey and more transparency
	if (node->IsLocked() == true)
	{
		bgColor.setRgbF(0.5, 0.5, 0.5);
		borderColor = bgColor;
		bgColor.setAlphaF(0.5);
		borderColor.setAlphaF(0.5);
	}

	// disabled node: more transparency
	if (node->IsEnabled() == false)
	{
		bgColor.setAlphaF(0.5);
		borderColor.setAlphaF(0.5);
	}

#ifndef SCREENSHOT_MODE
	// blinking error
	if (node->HasError() == true && isSelected == false && isWidgetEnabled == true)
	{
		borderColor = mShared->GetErrorBlinkColor();
		bgColor		= borderColor;
	}
#endif
	
	

	QColor bgColor2;
	QColor headerBgColor;
	bgColor2	  = bgColor.darker(200); // make darker actually, 30% of the old color, same as bgColor * 0.3f;
	headerBgColor = bgColor.darker(200);

	// is highlighted/hovered (on-mouse-over effect)
	if (isHighlighted == true)
	{
		headerBgColor = headerBgColor.lighter(mShared->GetHighlightFactor());
		bgColor2	  = bgColor2.lighter(130);
	}



	// text color
	QColor textColor;
	if (isSelected == false)
	{
		if (isWidgetEnabled == true)
			textColor = Qt::white;
		else
			textColor = QColor( 100, 100, 100 );
	}
	else
	{
		textColor = QColor( bgColor );
	}

	// header rect (icon+text)
	QRect headerRect(rect.left(), rect.top(), rect.width(), headerHeight);

	// header text rect (right area of icon)
	const int32 textRectLeft = rect.left() + 3 * padding + iconSize;
	const int32 textRectWidth = rect.width() - (3 * padding + iconSize);
	QRect headerTextRect(textRectLeft, rect.top(), textRectWidth, headerHeight - padding); // shift bottom higher because text is not perfectly centered vertically

	// do not draw this if the node is fully collapsed
	const Node::ECollapsedState collapsedState = node->GetCollapsedState();
	if (collapsedState != Node::COLLAPSE_ALL)
	{
		
		painter.setOpacity(solidOpacity);
		painter.setClipping(true);

		QColor portBrushColor, portPenColor;
		QPen portPen;
		QRect portRect;
		QRect portInteractionRect;
		QRect portClipRect;

		// setup port clip rect for clipping input ports
		portClipRect = rect;
		portClipRect.setLeft(rect.left() - mShared->GetPortDiameter() * mShared->GetScreenScaling() - 1.0);// HACK pixel adjust
		portClipRect.setRight(rect.left());
		painter.setClipRect(portClipRect);

		// get the number of input ports and iterate through them
		const uint32 numInputPorts = node->GetNumInputPorts();
		for (uint32 portNr = 0; portNr < numInputPorts; ++portNr)
		{
			const Port& port = node->GetInputPort(portNr);
			
			// skip if invisible
			if (port.IsVisible() == false)
				continue;

			// check if the mouse is over the port or text (left quarter of node, with same height as port) and set the highlight flag
			portInteractionRect = CalcInputPortInteractionRect(node, portNr, rect);
			portInteractionRect.setRight(rect.left() + rect.width() / 4.0);
			const bool isPortHighlighted = portInteractionRect.contains(mousePos);

			// get the input port and calculate the corresponding rect, using the highlight flag
			portRect = CalcInputPortRect(node, portNr, rect, isPortHighlighted);

			// draw the port rect
			QColor portTextColor;
			mShared->GetNodePortColors(isWidgetEnabled, port, borderColor, headerBgColor, textColor, &portBrushColor, &portPenColor, &portTextColor, isSelected, isPortHighlighted);
			painter.setBrush(portBrushColor);
			portPen.setColor(portPenColor);
			portPen.setWidth(mShared->GetScreenScaling());

			// render port with multi-channels with a thicker border
			uint32 numChannels = 0;
			MultiChannel* channels = port.GetChannels();
			if (channels != NULL)
				numChannels = channels->GetNumChannels();

			painter.setPen(portPen);

			// render different shape depending on channel type
			// TODO we only differntiate between single and multichannels for now until we have types
			if (numChannels <= 1)
			{
				painter.drawRect(portRect);
				//painter.drawEllipse(portRect);
			}
			else
			{
				//const double roundness = mShared->GetPortDiameter() * mShared->GetScreenScaling() / 4.0f;
				//painter.drawRoundedRect(portRect, roundness, roundness);
				painter.drawRect(portRect);
			}
		}

		// setup port clip rect for clipping input ports
		portClipRect = rect;
		portClipRect.setRight(rect.right() + mShared->GetPortDiameter() * mShared->GetScreenScaling());
		portClipRect.setLeft(rect.right() + 1.0);	// HACK pixel adjust
		painter.setClipRect(portClipRect);

		// get the number of output ports and iterate through them
		const uint32 numOutputPorts = node->GetNumOutputPorts();
		uint32 drawnPortIndex = 0;
		for (uint32 portNr = 0; portNr < numOutputPorts; ++portNr)
		{
			const Port& port = node->GetOutputPort(portNr);

			// skip if invisible
			if (port.IsVisible() == false)
				continue;

			// check if the mouse is over the port or text (left quarter of node, with same height as port) and set the highlight flag
			portInteractionRect = CalcOutputPortInteractionRect(node, portNr, rect);
			portInteractionRect.setLeft(rect.right() - rect.width() / 4.0);
			const bool isPortHighlighted = portInteractionRect.contains(mousePos);

			// get the input port and calculate the corresponding rect, using the highlight flag
			portRect = CalcOutputPortRect(node, portNr, rect, isPortHighlighted);

			// draw the port rect
			QColor portTextColor;
			mShared->GetNodePortColors(isWidgetEnabled, port, borderColor, headerBgColor, textColor, &portBrushColor, &portPenColor, &portTextColor, isSelected, isPortHighlighted);
			painter.setBrush(portBrushColor);
			portPen.setColor(portPenColor);
			portPen.setWidth(mShared->GetScreenScaling() );

			// render port with multi-channels with a thicker border
			uint32 numChannels = 0;
			MultiChannel* channels = port.GetChannels();
			if (channels != NULL)
				numChannels = channels->GetNumChannels();

			painter.setPen(portPen);

			// render different shape depending on channel type
			// TODO we only differntiate between single and multichannels for now until we have types
			if (numChannels <= 1)
			{
				painter.drawRect(portRect);
				//painter.drawEllipse(portRect);
			}
			else
			{
				//const double roundness = mShared->GetPortDiameter() * mShared->GetScreenScaling() / 4.0f;
				//painter.drawRoundedRect(portRect, roundness, roundness);
				painter.drawRect(portRect);
			}

			// increment drawn port index
			drawnPortIndex++;
		}

		painter.setClipping(false);

		// draw the main node rect
		QLinearGradient bgGradient(0, rect.top(), 0, rect.bottom());
		bgGradient.setColorAt(0.0f, bgColor2);
		bgGradient.setColorAt(1.0f, bgColor);
		painter.setBrush(bgGradient);
		
		QPen borderPen = QPen(borderColor);
		borderPen.setWidth(borderWidth * mShared->GetScreenScaling());

		// used dashed border and more transparencyif node is not uninitialized
		if (node->IsInitialized() == false || node->IsEnabled() == false || node->IsLocked() == true)
		{
			borderPen.setStyle(Qt::DotLine);
			painter.setOpacity(inactiveTransOpacity);
		}
		else
			painter.setOpacity(halfTransOpacity);

		painter.setPen(borderPen);
		painter.drawRoundedRect(rect, mShared->GetBorderRadius(), mShared->GetBorderRadius());
		
		// remove brush and draw border again, with solid opacity
		painter.setBrush(Qt::NoBrush);
		painter.setOpacity(solidOpacity);
		painter.drawRoundedRect(rect, mShared->GetBorderRadius(), mShared->GetBorderRadius());
	
		painter.setOpacity(solidOpacity);
		painter.setPen(Qt::NoPen); // border got removed here, to get it in again use borderColor

		// set clipping mask so ports are drawn behind the halftransparent node background redct
		//painter.setClipRect(rect, Qt::ClipOperation::IntersectClip);


		// draw the header line
		//QRect headerRect(rect.left(), rect.top(), rect.width(), headerHeight);
		//painter.setClipping(true);
		// FIXME we reuse portPen
		//portPen.setColor(textColor);
		//portPen.setWidth(mShared->GetScreenScaling());
		//painter.setPen(portPen);
		//painter.setClipRect( headerRect, Qt::ReplaceClip );
		//painter.setBrush( headerBgColor );
		//painter.drawRoundedRect( rect, mShared->GetBorderRadius(), mShared->GetBorderRadius() );
		//painter.drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());

		painter.setPen(Qt::NoPen);

		// render icon area: if hovered, change icon size to show that a collapsed/expand will happen by decreaseing/increasing the icon size
		QRect iconRect;
		if (IsPointOverNodeIcon(graph, node, mousePos) == true)
		{
			const double shift = 2.0 * mShared->GetScreenScaling();

			// Note: in this codepath, the node is always expanded :)
			iconRect.setRect(rect.left() + padding + shift, rect.top() + padding + shift, iconSize - 2 * shift, iconSize - 2 * shift);
		}
		else
		{
			// normal size
			iconRect.setRect(rect.left() + padding, rect.top() + padding, iconSize, iconSize);
		}
		
		// now draw the icon
		painter.drawPixmap(iconRect, FindNodeIcon(node, 2 * iconSize));
		painter.setClipping(false);

		// draw header text
		RenderText(true, painter, node->GetNameString(), textColor, headerTextRect, mShared->GetNodeHeaderFont(), mShared->GetNodeHeaderMetrics(), Qt::AlignLeft);

		// render info/port text, if scale is not too small
		//if (mShared->GetScale() > 0.3f)
		{
			QRect textRect;
			QRect portRect;
/*
			// draw the info text
			const int nodeInfoHeight = mShared->GetNodeInfoMetrics().height() * mShared->GetScreenScaling();
			const int topPadding = (mShared->GetNodeHeaderHeight()) * mShared->GetScreenScaling();
			const int leftPadding = (mShared->GetPortTextNodeInfoDistance() + mShared->GetPortTextDistance())* mShared->GetScreenScaling() + maxInputWidth;
			const int rightPadding = (mShared->GetPortTextNodeInfoDistance() + mShared->GetPortTextDistance())* mShared->GetScreenScaling() + maxOutputWidth;
			textRect = QRect(rect.left() + leftPadding, rect.top() + topPadding, rect.width() - rightPadding - leftPadding, nodeInfoHeight);

			RenderText( true, painter, node->GetNodeInfoString(), textColor, textRect, mShared->GetNodeInfoFont(), mShared->GetNodeInfoMetrics(), Qt::AlignCenter );
*/
			// draw the input ports
			QColor portBrushColor, portPenColor;
			QPen portPen;

			// get the number of input ports and iterate through them
			const uint32 numInputPorts = node->GetNumInputPorts();
			for (uint32 portNr = 0; portNr<numInputPorts; ++portNr)
			{
				// get the input port
				const Port& port = node->GetInputPort(portNr);

				// skip if port is invisible
				if (port.IsVisible() == false)
					continue;
				
				QColor portTextColor;
				mShared->GetNodePortColors(isWidgetEnabled, port, borderColor, headerBgColor, textColor, &portBrushColor, &portPenColor, &portTextColor, isSelected, false);
				
				textRect = CalcInputPortTextRect(node, portNr, rect);
				RenderText(true, painter, port.GetNameString(), portTextColor, textRect, mShared->GetNodePortFont(), mShared->GetNodePortMetrics(), Qt::AlignLeft);
			}

			// get the number of output ports and iterate through them
			const uint32 numOutputPorts = node->GetNumOutputPorts();
			for (uint32 portNr = 0; portNr<numOutputPorts; ++portNr)
			{
				// get the output port
				const Port& port = node->GetOutputPort(portNr);

				// skip if port is invisible
				if (port.IsVisible() == false)
					continue;

				QColor portTextColor;
				mShared->GetNodePortColors(isWidgetEnabled, port, borderColor, headerBgColor, textColor, &portBrushColor, &portPenColor, &portTextColor, isSelected, false);

				textRect = CalcOutputPortTextRect(node, portNr, rect);
				RenderText(true, painter, port.GetNameString(), portTextColor, textRect, mShared->GetNodePortFont(), mShared->GetNodePortMetrics(), Qt::AlignRight);
			}
		}

		// render node error icon and hover text
		if (node->HasError() == true)
		{
			#ifndef SCREENSHOT_MODE
			
			// pixmap and position
			int pixmapSize = (32.0 * mShared->GetScreenScaling());
			//int halfPixmapSize = pixmapSize / 2;
			QPixmap pixmap = GetQtBaseManager()->FindIcon("Images/Icons/Error.png").pixmap(pixmapSize, pixmapSize);
			const int iconLeft = rect.left() + (rect.width() - pixmapSize) / 2;
			const int iconTop = rect.top() + (rect.height() - pixmapSize) / 2;
			const int iconBottom = rect.top() + (rect.height() + pixmapSize) / 2;

			// mouse over node: show error text
			if (rect.contains(mousePos) == true)
			{
				// TODO show all text messages, not just the first
				const String& message = node->GetError(0).mMessage;
				if (message.IsEmpty() == false)
				{
					painter.setOpacity(0.7);
					painter.setPen(Qt::NoPen);
					painter.setBrush(Qt::black);
					// 'dim' node background by overlaying 50% black rounded node rect without border
					//painter.drawRoundedRect(rect, mShared->GetBorderRadius(), mShared->GetBorderRadius());

					// render text (with additional background)
					const int textTop = iconBottom + 5 * mShared->GetScreenScaling();
					const int textWidth = mShared->GetNodeInfoMetrics().width(message.AsChar());
					const int textHeight = mShared->GetNodeInfoMetrics().height();
					const int textLeft = rect.left() + (rect.width() - textWidth) / 2;
					const int textBorder = 5 * mShared->GetScreenScaling();
					painter.drawRoundedRect(QRect(textLeft - textBorder, textTop - textBorder, textWidth + 2 * textBorder, textHeight + 2*textBorder), mShared->GetBorderRadius(), mShared->GetBorderRadius());
					painter.setOpacity(1.0);
					RenderText(true, painter, message, mShared->GetErrorBlinkColor(), QRect(textLeft, textTop, textWidth, textHeight), mShared->GetNodeInfoFont(), mShared->GetNodeInfoMetrics(), Qt::AlignCenter);
				}
			}

			// render error icon
			QRect pixmapRect(iconLeft, iconTop, pixmapSize, pixmapSize);
			painter.setBrush(Qt::black);
			painter.drawRoundedRect(pixmapRect, pixmapSize / 2, pixmapSize / 2);
			painter.drawPixmap( pixmapRect, pixmap );
			#endif
		}

		// TODO roll this into a general "node warning" that supports a list of warnings; then use it to show deprecated/unstable warnings
		// render deprecated / unstable warning sign only if there is no error
		else if (node->IsDeprecated() || node->IsUnstable())
		{

			// pixmap and position
			int pixmapSize = (32.0 * mShared->GetScreenScaling());
			//int halfPixmapSize = pixmapSize / 2;
			QPixmap pixmap = GetQtBaseManager()->FindIcon( node->IsDeprecated() ? "Images/Icons/Warning.png" : "Images/Icons/Bug.png").pixmap(pixmapSize, pixmapSize);
			const int iconLeft = rect.left() + (rect.width() - pixmapSize) / 2;
			const int iconTop = rect.top() + (rect.height() - pixmapSize) / 2;
			const int iconBottom = rect.top() + (rect.height() + pixmapSize) / 2;


			painter.setOpacity(0.7);
			painter.setPen(Qt::NoPen);
			painter.setBrush(Qt::black);

			// render icon on black circle
			QRect pixmapRect(iconLeft, iconTop, pixmapSize, pixmapSize);
			painter.drawRoundedRect(pixmapRect, pixmapSize / 2, pixmapSize / 2);
			painter.drawPixmap( pixmapRect, pixmap );

			// render text (with additional background)
			const char* message = (node->IsDeprecated() ? "Node is Deprecated" : "Node is Unstable");
			const int textTop = iconBottom + 5 * mShared->GetScreenScaling();
			const int textWidth = mShared->GetNodeInfoMetrics().width(message);
			const int textHeight = mShared->GetNodeInfoMetrics().height();
			const int textLeft = rect.left() + (rect.width() - textWidth) / 2;
			const int textBorder = 5 * mShared->GetScreenScaling();
			const QColor color = (node->IsDeprecated() ? Qt::yellow : Qt::red);
			painter.drawRoundedRect(QRect(textLeft - textBorder, textTop - textBorder, textWidth + 2 * textBorder, textHeight + 2*textBorder), mShared->GetBorderRadius(), mShared->GetBorderRadius());
			painter.setOpacity(1.0);
			RenderText(true, painter, message, color, QRect(textLeft, textTop, textWidth, textHeight), mShared->GetNodeInfoFont(), mShared->GetNodeInfoMetrics(), Qt::AlignCenter);

		
		}
	}

	// Render node in collapsed state
	else
	{
		// gradient
		QLinearGradient bgGradient(0, headerRect.top(), 0, headerRect.bottom());
		bgGradient.setColorAt(0.0f, bgColor2);
		bgGradient.setColorAt(1.0f, bgColor);
		painter.setBrush(bgGradient);

		QPen borderPen = QPen(borderColor);
		borderPen.setWidth(borderWidth * mShared->GetScreenScaling());
		// used dashed border and more transparencyif node is not uninitialized
		if (node->IsInitialized() == false)
		{
			borderPen.setStyle(Qt::DotLine);
			painter.setOpacity(inactiveTransOpacity);
		}
		else
			painter.setOpacity(halfTransOpacity);

		painter.setPen(borderPen);
		painter.drawRoundedRect(rect, mShared->GetBorderRadius(), mShared->GetBorderRadius());

		// remove brush and draw border again, with solid opacity
		painter.setBrush(Qt::NoBrush);
		painter.setOpacity(solidOpacity);
		painter.drawRoundedRect(rect, mShared->GetBorderRadius(), mShared->GetBorderRadius());

		painter.setOpacity(solidOpacity);
		painter.setClipping( true );
		painter.setClipRect( headerRect, Qt::ReplaceClip );

		// render icon area: if hovered, change icon size to show that a collapsed/expand will happen by decreaseing/increasing the icon size
		QRect iconRect;
		if (IsPointOverNodeIcon(graph, node, mousePos) == true)
		{
			const double shift = 2.0 * mShared->GetScreenScaling();

			// in this codepath, the node is always collapsed
			iconRect.setRect(rect.left() + padding - shift, rect.top() + padding - shift, iconSize + 2 * shift, iconSize + 2 * shift);
		}
		else
		{
			// normal size
			iconRect.setRect(rect.left() + padding, rect.top() + padding, iconSize, iconSize);
		}
		
		painter.drawPixmap(iconRect, FindNodeIcon(node, 2 * iconSize));
		painter.setClipping(false);

		// draw header and sub title
		RenderText(true, painter, node->GetNameString(), textColor, headerTextRect, mShared->GetNodeHeaderFont(), mShared->GetNodeHeaderMetrics(), Qt::AlignLeft);

		// avoid alloc
		//mTempString = node->GetReadableType();
		//RenderText( true, painter, mTempString, textColor, subHeaderRect, mShared->GetNodeSubTitleFont(), mShared->GetNodeSubTitleMetrics(), Qt::AlignCenter );
	}

	painter.setOpacity( 1.0f );
}



// render an  annotation graph node
void GraphRenderer::RenderAnnotationNode(Graph* graph, AnnotationNode* node, QPainter& painter, bool isWidgetEnabled, const QRect& visibleRect, const QPoint& mousePos)
{
	const float solidOpacity = 1.0f;
	const float halfTransOpacity = 0.60f;
	const float inactiveTransOpacity = 0.2f;

	const bool isSelected = mShared->IsNodeSelected(node);

	const double borderWidth = 2.0 * mShared->GetScreenScaling();
	const double headerHeight = mShared->GetNodeHeaderHeight() * mShared->GetScreenScaling();
	const double padding = 2.0f * mShared->GetScreenScaling();
	const double iconSize = headerHeight - 2 * padding;

	// get node dimensions
	int32 maxInputWidth, maxOutputWidth;
	QRect rect = CalcNodeRect(graph, node, &maxInputWidth, &maxOutputWidth);

	// calc the rect in screen space (after scrolling and zooming)
	QRect finalRect = mShared->GetTransform().mapRect(rect);

	// check if the node is visible, skip directly if it is not
	bool isVisible = finalRect.intersects(visibleRect);
	if (isVisible == false)
		return;

	// check if the mouse is over the node, if yes highlight the node
	bool isHighlighted = false;
	if (IsPointOnNode(graph, node, mousePos) == true)
		isHighlighted = true;

	// border and background colors
	QColor nodeColor = mShared->GetNodeColor(isWidgetEnabled, node);
	QColor bgColor;
	QColor borderColor = nodeColor;

	if (isSelected == true)
	{
		borderColor = mShared->GetSelectedColor();
		bgColor = mShared->GetSelectedColor();
	}
	else // not selected
	{
		if (isWidgetEnabled == true)
			bgColor = nodeColor;
		else
			bgColor.setRgbF(0.3f, 0.3f, 0.3f, 1.0f);
	}

	QColor bgColor2;
	QColor headerBgColor;
	bgColor2 = bgColor.darker(200); // make darker actually, 30% of the old color, same as bgColor * 0.3f;
	headerBgColor = bgColor.darker(200);

	// is highlighted/hovered (on-mouse-over effect)
	if (isHighlighted == true)
	{
		headerBgColor = headerBgColor.lighter(mShared->GetHighlightFactor());
		bgColor2 = bgColor2.lighter(130);
	}

	// text color
	QColor textColor;
	if (isSelected == false)
	{
		if (isWidgetEnabled == true)
			textColor = Qt::white;
		else
			textColor = QColor(100, 100, 100);
	}
	else
	{
		textColor = QColor(bgColor);
	}

	// header rect (icon+text)
	QRect headerRect(rect.left(), rect.top(), rect.width(), headerHeight);

	// header text rect (right area of icon)
	const int32 textRectLeft = rect.left() + 3 * padding + iconSize;
	const int32 textRectWidth = rect.width() - (3 * padding + iconSize);
	QRect headerTextRect(textRectLeft, rect.top(), textRectWidth, headerHeight - padding); // shift bottom higher because text is not perfectly centered vertically

	painter.setClipping(false);

	// draw the main node rect
	QLinearGradient bgGradient(0, rect.top(), 0, rect.bottom());
	bgGradient.setColorAt(0.0f, bgColor2);
	bgGradient.setColorAt(1.0f, bgColor);
	painter.setBrush(bgGradient);

	QPen borderPen = QPen(borderColor);
	borderPen.setWidth(borderWidth * mShared->GetScreenScaling());

	// used dashed border and more transparencyif node is not uninitialized
	if (node->IsInitialized() == false)
	{
		borderPen.setStyle(Qt::DotLine);
		painter.setOpacity(inactiveTransOpacity);
	}
	else
		painter.setOpacity(halfTransOpacity);

	painter.setPen(borderPen);
	painter.drawRoundedRect(rect, mShared->GetBorderRadius(), mShared->GetBorderRadius());

	// remove brush and draw border again, with solid opacity
	painter.setBrush(Qt::NoBrush);
	painter.setOpacity(solidOpacity);
	painter.drawRoundedRect(rect, mShared->GetBorderRadius(), mShared->GetBorderRadius());

	painter.setOpacity(solidOpacity);
	painter.setPen(Qt::NoPen); // border got removed here, to get it in again use borderColor

	// now finally render the text line-by-line
	const int32 lineSpacing = mShared->GetNodeHeaderMetrics().height() * 1.5;
	const int32 numLines = node->GetNumLines();
	const int32 vMargin = mShared->GetNodeHeaderMetrics().height() / 4;
	const int32 hMargin = mShared->GetNodeHeaderMetrics().height() / 2;
	
	// horizontal alignment (handled by QT)
	Qt::Alignment hAlignment = Qt::AlignCenter;
	switch (node->GetHorizontalAlignment())
	{
		case AnnotationNode::ALIGN_LEFT:	hAlignment = Qt::AlignLeft;		break;
		case AnnotationNode::ALIGN_CENTER:  hAlignment = Qt::AlignCenter;	break;
		case AnnotationNode::ALIGN_RIGHT:	hAlignment = Qt::AlignRight;	break;
	}

	// vertical alignment (handled by choosing the yStart)
	int32 yStart = rect.top();
	switch (node->GetVerticalAlignment())
	{
		case AnnotationNode::ALIGN_TOP:		yStart += vMargin;												break;
		case AnnotationNode::ALIGN_CENTER:  yStart += (rect.height() - numLines * lineSpacing) / 2;			break;
		case AnnotationNode::ALIGN_BOTTOM:	yStart += (rect.height() - vMargin) - numLines * lineSpacing;	break;
	}
	
	// set textRect starting position, we'll move it linewise downwards inside the loop
	QRect textRect = rect;
	textRect.setCoords(rect.x() + hMargin, yStart, rect.x() + rect.width() - hMargin, yStart + lineSpacing);

	for (int32 i = 0; i < numLines; ++i)
	{
		RenderText(true, painter, node->GetLine(i), textColor, textRect, mShared->GetNodeHeaderFont(), mShared->GetNodeHeaderMetrics(), hAlignment);
		textRect.adjust(0, lineSpacing, 0, lineSpacing);
	}

	painter.setOpacity(1.0f);
}


// draw node debug string over each node (black box, white text)
void GraphRenderer::RenderNodeDebugInfo(Graph* graph, QPainter& painter, const GraphHelpers::CreateConnectionInfo& createConnectionInfo, bool isWidgetEnabled, const QRect& visibleRect, const QPoint& mousePos)
{
	// make sure the graph is valid
	if (graph == NULL)
		return;

	Array<String> lineBuffer;

	// get the number of nodes in the graph and render them
	const uint32 numNodes = graph->GetNumNodes();
	for (uint32 i = 0; i < numNodes; ++i)
	{
		Node* node = graph->GetNode(i);

		// calc node dimensions
		int32 maxInputWidth, maxOutputWidth;
		QRect nodeRect = CalcNodeRect(graph, node, &maxInputWidth, &maxOutputWidth);

		// get the debug text
		mTempString.Clear();
		node->GetDebugString(mTempString);
		
		// number of lines (note: requires last line to have a newline!)
		lineBuffer = mTempString.Split(StringCharacter('\n'));
		const uint32 numLines = lineBuffer.Size();

		// calculate text box width and height
		QRect rect; 
		const QFontMetrics& fontMetrics = mShared->GetNodeInfoMetrics();
		int charHeight = fontMetrics.height();
		int maxWidth = 0;
		for (uint32 n = 0; n < numLines; ++n)
		{
			const uint32 numChars = lineBuffer[n].GetLength();
			int lineWidth = 0;
			for (uint32 c = 0; c < numChars; ++c)
				lineWidth += fontMetrics.width(QChar(lineBuffer[n].AsChar()[c]));
			maxWidth = Max(maxWidth, lineWidth);
		}
		rect.setHeight(numLines * charHeight + 5);
		rect.setWidth(maxWidth + 10);
		rect.moveCenter(nodeRect.center());

		painter.setOpacity(0.7);
		painter.fillRect(rect, Qt::black);

		// draw the lines of text
		const int top = rect.top();
		rect.setLeft(rect.left() + 5);
		for (uint32 n = 0; n < numLines; ++n)
		{
			rect.setTop(top + n * charHeight);
			rect.setBottom(rect.top() + charHeight);
			RenderText(false, painter, lineBuffer[n], Qt::white, rect, mShared->GetNodeInfoFont(), mShared->GetNodeInfoMetrics(), Qt::AlignLeft);
		}
	}
}


// render graph connections
void GraphRenderer::RenderConnections(Graph* graph, QPainter& painter, const GraphHelpers::RelinkConnectionInfo& relinkConnectionInfo, Node* onMouseOverNode, const QPoint& globalMousePos, bool isWidgetEnabled, const QRect& visibleRect)
{
	// render the connections
	QPen connectionPen;
	QBrush connectionBrush;
	
	// get the number of connections and iterate through them
	const uint32 numConnections = graph->GetNumConnections();
	for (uint32 i=0; i<numConnections; ++i)
	{
		// get the connection
		Connection* connection = graph->GetConnection(i);
		
		const bool isSelected = mShared->IsConnectionSelected(connection);
		const bool isProcessed= true;
				
		// highlight state of the connection
		bool isHighlighted = false;
		if (onMouseOverNode != NULL)
		{
			// highlight the connection in case we are currently hovering the node where the current connection is plugged into
			if (connection->GetSourceNode() == onMouseOverNode || connection->GetTargetNode() == onMouseOverNode)
				isHighlighted = true;
		}

		// in case the connection is not highlighted yet, check if we are over it with the mouse
		if (isHighlighted == false && IsCloseToConnection(graph, globalMousePos, connection) == true)
			isHighlighted = true;

		// render the connection
		RenderConnection( graph, connection, isWidgetEnabled, painter, connectionPen, connectionBrush, visibleRect, 1.0f, isSelected, isHighlighted, isProcessed );
	}
}


// render the connection
void GraphRenderer::RenderConnection(Graph* graph, Connection* connection, bool isWidgetEnabled, QPainter& painter, QPen& pen, QBrush& brush, const QRect& visibleRect, float opacity, bool isSelected, bool isHighlighted, bool isProcessed)
{
	if (isWidgetEnabled == false)
		opacity *= 0.35f;

	painter.setOpacity( opacity );

	// get the source and target nodes
	Node*			sourceNode		= connection->GetSourceNode();
	Node*			targetNode		= connection->GetTargetNode();
	const uint32	sourcePortNr	= connection->GetSourcePort();
	const uint32	targetPortNr	= connection->GetTargetPort();

	const QRect		sourceNodeRect	= CalcNodeRect( graph, sourceNode );
	const QRect		targetNodeRect	= CalcNodeRect( graph, targetNode );
	const QRect		sourceRect		= CalcSourceRect( sourceNode, sourcePortNr, sourceNodeRect );
	const QRect		targetRect		= CalcTargetRect( targetNode, targetPortNr, targetNodeRect );

	// get the start and end coordinates for the connections
	int32 startX	= sourceRect.center().x() + 2.0f * mShared->GetScreenScaling();
	int32 endX		= targetRect.center().x() - 0.5f * mShared->GetScreenScaling();	// pixel fiddling
	int32 startY	= sourceRect.center().y() + 1;		// HACK y coordinate is always off by one, no matter what screenscale or port diameter/distance
	int32 endY		= targetRect.center().y() + 1;

	QRect finalRect;
	finalRect.setCoords( startX, startY, endX, endY );

	// calc the rect in screen space (after scrolling and zooming)
	QRect transformedRect = mShared->GetTransform().mapRect( finalRect );

	// check if the node is visible, skip directly if it is not
	bool isVisible = transformedRect.intersects( visibleRect );
	if (isVisible == false)
		return;

	QColor penColor;
	QColor borderColor;

	//QColor textColor = penColor;
	QColor nodeColor = mShared->GetNodeColor(isWidgetEnabled, sourceNode);
	QColor textColor = nodeColor;
	// draw some small horizontal lines that go outside of the connection port
	if (isSelected == true)
	{
		penColor	= mShared->GetSelectedColor();
		textColor	= mShared->GetSelectedColor();
		borderColor	= mShared->GetSelectedColor().darker(200);
	}
	else // unselected
	{
		penColor	= textColor;
		borderColor = mShared->GetConnectionBorderColor();
	}

	// lighten the color in case the connection is highlighted
	if (isHighlighted == true)
	{
		penColor	= penColor.lighter(mShared->GetHighlightFactor());
		borderColor = borderColor.lighter(mShared->GetHighlightFactor());
	}

	// read only mode
	if (isWidgetEnabled == false)
		penColor = mShared->GetReadOnlyColor();

	// draw the curve
	mPainterPath = QPainterPath();
	CalcConnectionPath(mPainterPath, graph, connection);

	int				connectionWidth = (3.0 * mShared->GetScreenScaling());
	uint32			connectionDashPatternIndex = 0;
	Qt::PenStyle	connectionStyle = Qt::PenStyle::DashLine;

	if (sourceNode != NULL)
	{
		Port& port = sourceNode->GetOutputPort(sourcePortNr);

		// select size and dot style for each channel type
		if (port.GetValueAttribute() != NULL && port.GetValueAttribute()->GetType() == AttributeChannels<double>::TYPE_ID)
		{
			MultiChannel* channels = port.GetChannels();
			const uint32 width = (channels->GetNumChannels() > 1) ? 5 : 3;
			connectionWidth = (width * mShared->GetScreenScaling());
			connectionStyle = Qt::PenStyle::DotLine;
		}
		else if (port.GetValueAttribute() != NULL && port.GetValueAttribute()->GetType() == AttributeChannels<Spectrum>::TYPE_ID)
		{
			MultiChannel* channels = port.GetChannels();
			const uint32 width = (channels->GetNumChannels() > 1) ? 9 : 7;
			connectionWidth = (width * mShared->GetScreenScaling());
			connectionDashPatternIndex = 0;
			connectionStyle = Qt::PenStyle::CustomDashLine;
		}
	}

	//////////////////////////////////////////////////////////////////
	// draw tube around the connection
	//////////////////////////////////////////////////////////////////
	float oldOpacity = painter.opacity();
	painter.setOpacity(0.5f);
	pen.setStyle(Qt::PenStyle::SolidLine);
	pen.setWidth((11 * mShared->GetScreenScaling()));
	pen.setColor(borderColor);
	painter.setPen(pen);

	// set the brush
	brush.setColor(borderColor);
	painter.setBrush(brush);

	painter.drawPath(mPainterPath);
	painter.setOpacity(oldOpacity);
	//////////////////////////////////////////////////////////////////

	pen.setStyle( connectionStyle );
	pen.setCapStyle( Qt::PenCapStyle::FlatCap );
	pen.setWidth( connectionWidth );

	// apply custom dash pattern
	if (connectionStyle == Qt::PenStyle::CustomDashLine)
		pen.setDashPattern( mShared->GetConnectionDashPattern( connectionDashPatternIndex ) );

	// animate only actively processing AND active connections
	if (isProcessed == true)
	{
		// check channel activity
		Port& port = sourceNode->GetOutputPort(sourcePortNr);
		MultiChannel* channels = port.GetChannels();
	
		// animate only if channel is active (or if it is a deprecated float connection)
		if (channels != NULL && channels->IsActive() == true)
		{
			// modify channel speed using its sample rate (scale by ln(rate + 10) / 5)
			const double offset = mShared->GetConnectionDashOffset() * Math::LogD(channels->GetSampleRate() + 10) / 5.0;
			const double normalizedDashOffset = offset * 3 / (double)connectionWidth;
			pen.setDashOffset( normalizedDashOffset );
		}
	}

	// blinking red error color
#ifndef SCREENSHOT_MODE
	if (sourceNode != NULL && sourceNode->HasError() == true && isSelected == false && isWidgetEnabled == true)
	{
		penColor = mShared->GetErrorBlinkColor();
		textColor = mShared->GetErrorBlinkColor();
	}
#endif

	// set the pen
	pen.setColor(penColor);
	painter.setPen( pen );

	// set the brush
	brush.setColor(penColor);
	painter.setBrush( brush );
	
	painter.drawPath( mPainterPath );
	
	painter.setOpacity( 1.0f );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// render connection value
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SCREENSHOT_MODE
	if (isProcessed == true && sourceNode->IsUpdateReady() == true)
	{
		Core::Attribute* attribute = sourceNode->GetOutputPort(sourcePortNr).GetValueAttribute();
		bool useTextCaching = false;

		if (attribute != NULL)
		{

			// fill the string with data
			mTempString.Clear();
			switch (attribute->GetType())
			{
				// float attributes
				case Core::AttributeFloat::TYPE_ID:
				{
					Core::AttributeFloat* floatAttribute = static_cast<Core::AttributeFloat*>(attribute);
					mTempString.Format("%.2f", floatAttribute->GetValue());
					break;
				}

				// boolean attributes
				case AttributeBool::TYPE_ID:
				{
					AttributeBool* boolAttribute = static_cast<AttributeBool*>(attribute);
				
					mTempString.Format("%s", boolAttribute->GetValue() ? "true" : "false");
					useTextCaching = true;

					break;
				}

				// new channels
				case AttributeChannels<double>::TYPE_ID_DOUBLE:
				{
					AttributeChannels<double>* sampleAttribute = static_cast<AttributeChannels<double>*>(attribute);
					if (sampleAttribute == NULL)
						break;

					// only show value if the channelset has the width 1
					if (sampleAttribute->GetNumChannels() != 1)
						return;

					ChannelBase* channel = sampleAttribute->GetChannel(0);

		#ifdef CORE_DEBUG
					if (channel != NULL && channel->GetNumSamples() > 0)
						mTempString.Format("%.2f",channel->AsType<double>()->GetLastSample());
		#else
					if (channel == NULL)
					{
						useTextCaching = true;
						mTempString = "0.00";
					}
					else if (channel->GetNumSamples() == 0)
					{
						useTextCaching = true;
						mTempString = "0.00";
					}
					else
						mTempString.Format("%.2f",channel->AsType<double>()->GetLastSample());
		#endif
					break;
				}

				default:
					attribute->ConvertToString(mTempString);
			}

		}

		// only display the value in case it is not empty
		if (mTempString.IsEmpty() == false)
		{
			QPoint connectionAttachPoint = finalRect.center();

			int halfTextHeight = (6 * mShared->GetScreenScaling());
			int textWidth = mShared->GetNodePortMetrics().width( mTempString.AsChar() );
			int halfTextWidth = textWidth/2;

			QRect textRect(connectionAttachPoint.x()-halfTextWidth-(1.0 * mShared->GetScreenScaling()), connectionAttachPoint.y()-halfTextHeight, textWidth+(4.0 * mShared->GetScreenScaling()), halfTextHeight*2-(1.0 * mShared->GetScreenScaling()));
			QPoint textPosition = textRect.bottomLeft();
			textPosition.setY( textPosition.y()-(1.0 * mShared->GetScreenScaling()) );
			textPosition.setX( textPosition.x()+(2.0 * mShared->GetScreenScaling()) );

			// draw the background rect for the text
			painter.setBrush( mShared->GetGridColor() );

			QPen textPen;
			textPen.setColor(mShared->GetBackgroundColor() );
			textPen.setWidth(mShared->GetScreenScaling());
			painter.setPen( mShared->GetGridColor() );

			painter.drawRect( textRect );

			// draw the text
			RenderText( useTextCaching, painter, mTempString, textColor, textRect, mShared->GetNodePortFont(), mShared->GetNodePortMetrics(), Qt::AlignCenter );
		}
	}
#endif
}


// reset all node rects so that they get recalculated
void GraphRenderer::ResetNodeCaches(Graph* graph)
{
	// if there is no valid graph, return directly
	if (graph == NULL)
		return;

	// make sure the array sizes are all correct
	// get the number of nodes in the graph and make sure we have enough node caches
	UpdateNodeCaches(graph);

	// reset all node caches
	const uint32 numNodes = graph->GetNumNodes();
	for (uint32 i=0; i<numNodes; ++i)
		mNodeCaches[i].Reset();
}


// reset all node rects so that they get recalculated
void GraphRenderer::UpdateNodeCaches(Graph* graph)
{
	// if there is no valid graph, return directly
	if (graph == NULL)
		return;

	// make sure the array sizes are all correct
	// get the number of nodes in the graph and make sure we have enough node caches
	const uint32 numNodes = graph->GetNumNodes();
	if (mNodeCaches.Size() != numNodes)
	{
		mNodeCaches.Resize(numNodes);

		// reset all node caches
		for (uint32 i=0; i<numNodes; ++i)
			mNodeCaches[i].Reset();
	}
}


// calc rect around the node
QRect GraphRenderer::CalcNodeRect(Graph* graph, Node* node, int32* outMaxInputWidth, int32* outMaxOutputWidth)
{
	// make sure the array sizes are all correct
	UpdateNodeCaches(graph);

	// find the node index and check for a valid node cache
	const uint32 nodeIndex = graph->FindNodeIndex(node);
	if (nodeIndex != CORE_INVALIDINDEX32 && mNodeCaches[nodeIndex].IsValid() == true)
	{
		if (outMaxInputWidth != NULL)	*outMaxInputWidth = mNodeCaches[nodeIndex].mMaxInputWidth;
		if (outMaxOutputWidth != NULL)	*outMaxOutputWidth = mNodeCaches[nodeIndex].mMaxOutputWidth;

		return mNodeCaches[nodeIndex].mRect;
	}

	// calc node rect
	int32 maxInputWidth = 0;
	int32 maxOutputWidth = 0;
	int32 requiredWidth, requiredHeight;

	// annotation node has fixed size
	if (node->GetType() == AnnotationNode::TYPE_ID)
	{
		AnnotationNode* annotationNode = static_cast<AnnotationNode*>(node);
		requiredWidth = annotationNode->GetNodeWidth();
		requiredHeight = annotationNode->GetNodeHeight();
	}
	else
	{
		// other nodes are calculated here:
		requiredWidth = CalcRequiredNodeWidth( mShared, node, mShared->GetNodePortMetrics(), mShared->GetNodeInfoMetrics(), mShared->GetNodeHeaderMetrics(), mShared->GetNodeSubTitleMetrics(), &maxInputWidth, &maxOutputWidth );
		requiredHeight = CalcRequiredNodeHeight( mShared, node );
	}

	QRect rect( (node->GetVisualPosX() * mShared->GetScreenScaling()), (node->GetVisualPosY() * mShared->GetScreenScaling()), requiredWidth, requiredHeight );

	if (outMaxInputWidth != NULL)	*outMaxInputWidth = maxInputWidth;
	if (outMaxOutputWidth != NULL)	*outMaxOutputWidth = maxOutputWidth;

	if (nodeIndex != CORE_INVALIDINDEX32)
	{
		// update the node cache
		mNodeCaches[nodeIndex].mRect = rect;
		mNodeCaches[nodeIndex].mMaxInputWidth = maxInputWidth;
		mNodeCaches[nodeIndex].mMaxOutputWidth = maxOutputWidth;
		mNodeCaches[nodeIndex].mIsValid = true;
	}

	return rect;
}


// calc the input port rect
QRect GraphRenderer::CalcInputPortRect(Node* node, uint16 inputPortNr, const QRect& nodeRect, bool isPortHighlighted)
{
	// make sure this is never called for an invisible port (must be an error)
	const Port& port = node->GetInputPort(inputPortNr);
	CORE_ASSERT(port.IsVisible() == true);

	const int32 x = nodeRect.left() - (mShared->GetPortDiameter() * mShared->GetScreenScaling() * 0.6f);
	const int32 y		= nodeRect.top() + (mShared->GetNodeHeaderHeight() * mShared->GetScreenScaling()) + inputPortNr * ((mShared->GetPortDiameter() + mShared->GetPortDistance()) * mShared->GetScreenScaling());
	const int32 width	= mShared->GetPortDiameter() * mShared->GetScreenScaling();
	const int32 height	= mShared->GetPortDiameter() * mShared->GetScreenScaling();

	// port is highlighted, let it peek out a litte more by shifting it to the left
	if (isPortHighlighted == true)
		return QRect(x - 0.5f * mShared->GetPortDiameter() * mShared->GetScreenScaling(), y, width, height);
	else
		return QRect(x, y, width, height);
}


// calc the output port rect
QRect GraphRenderer::CalcOutputPortRect(Node* node, uint16 outputPortNr, const QRect& nodeRect, bool isPortHighlighted)
{
	// make sure this is never called for an invisible port (must be an error)
	const Port& port = node->GetOutputPort(outputPortNr);
	CORE_ASSERT(port.IsVisible() == true);

	const int32 x		= nodeRect.right() - (mShared->GetPortDiameter() * mShared->GetScreenScaling() * 0.3f); // HACK pixelfiddled, should be 0.6!!
	const int32 y		= nodeRect.top() + (mShared->GetNodeHeaderHeight() * mShared->GetScreenScaling()) + outputPortNr * ((mShared->GetPortDiameter() + mShared->GetPortDistance()) * mShared->GetScreenScaling());
	const int32 width	= mShared->GetPortDiameter() * mShared->GetScreenScaling();
	const int32 height	= mShared->GetPortDiameter() * mShared->GetScreenScaling();

	// port is highlighted, let it peek out a litte more by shifting it to the left
	if (isPortHighlighted == true)
		return QRect(x + 0.5f * mShared->GetPortDiameter() * mShared->GetScreenScaling(), y, width, height);
	else
		return QRect(x, y, width, height);
}



// calc the input port rect
QRect GraphRenderer::CalcInputPortInteractionRect(Node* node, uint16 inputPortNr, const QRect& nodeRect)
{
	// covers the area outside the node, and and 1/4 of node width
	QRect rect = CalcInputPortRect(node, inputPortNr, nodeRect);
	rect.setRight(nodeRect.left() + nodeRect.width() / 4.0f);
	rect.setLeft(nodeRect.left() - mShared->GetPortDiameter() * mShared->GetScreenScaling());
	return rect;
}


// calc the output port rect
QRect GraphRenderer::CalcOutputPortInteractionRect(Node* node, uint16 outputPortNr, const QRect& nodeRect)
{
	// covers the area outside the node, and and 1/4 of node width
	QRect rect = CalcOutputPortRect(node, outputPortNr, nodeRect);
	rect.setRight(nodeRect.right() + mShared->GetPortDiameter() * mShared->GetScreenScaling());
	rect.setLeft(nodeRect.right() - nodeRect.width() / 4.0f);
	return rect;
}


// calc the input port rect
QRect GraphRenderer::CalcInputPortTextRect(Node* node, uint16 inputPortNr, const QRect& nodeRect)
{
	const int32 x = nodeRect.left() + (mShared->GetPortTextDistance() * mShared->GetScreenScaling());
	const int32 y = nodeRect.top() + (mShared->GetNodeHeaderHeight() * mShared->GetScreenScaling()) + inputPortNr * ((mShared->GetPortDiameter() + mShared->GetPortDistance()) * mShared->GetScreenScaling());
	const int32 width = nodeRect.width() - 2 * (mShared->GetPortTextDistance() * mShared->GetScreenScaling()); 
	const int32 height = mShared->GetPortDiameter() * mShared->GetScreenScaling();

	return QRect(x, y, width, height);
}


// calc the output port rect
QRect GraphRenderer::CalcOutputPortTextRect(Node* node, uint16 outputPortNr, const QRect& nodeRect)
{
	const int32 x = nodeRect.left();
	const int32 y = nodeRect.top() + (mShared->GetNodeHeaderHeight() * mShared->GetScreenScaling()) + outputPortNr * ((mShared->GetPortDiameter() + mShared->GetPortDistance()) * mShared->GetScreenScaling());
	const int32 width = nodeRect.width() - (mShared->GetPortTextDistance() * mShared->GetScreenScaling());
	const int32 height = mShared->GetPortDiameter() * mShared->GetScreenScaling();

	return QRect(x, y, width, height);
}


QPainterPath& GraphRenderer::CalcConnectionPath(QPainterPath& inOutPath, Graph* graph, Connection* connection)
{

	// get the source and target nodes
	Node*			sourceNode = connection->GetSourceNode();
	Node*			targetNode = connection->GetTargetNode();
	const uint32	sourcePortNr = connection->GetSourcePort();
	const uint32	targetPortNr = connection->GetTargetPort();

	const QRect		sourceNodeRect = CalcNodeRect(graph, sourceNode);
	const QRect		targetNodeRect = CalcNodeRect(graph, targetNode);
	const QRect		sourceRect = CalcSourceRect(sourceNode, sourcePortNr, sourceNodeRect);
	const QRect		targetRect = CalcTargetRect(targetNode, targetPortNr, targetNodeRect);
	

	// get the start and end coordinates for the connections
	int32 startX = sourceRect.center().x() + 2.0f * mShared->GetScreenScaling();
	int32 endX = targetRect.center().x() - 0.5f * mShared->GetScreenScaling();	// pixel fiddling
	int32 startY = sourceRect.center().y() + 1;		// HACK y coordinate is always off by one, no matter what screenscale or port diameter/distance
	int32 endY = targetRect.center().y() + 1;

	const float width = Math::Abs((endX - (3.0 * mShared->GetScreenScaling())) - (startX + (3.0 * mShared->GetScreenScaling())));
	inOutPath.moveTo(startX, startY);
	inOutPath.lineTo(startX + (3.0 * mShared->GetScreenScaling()), startY);
	inOutPath.cubicTo(startX + (width / 2), startY, endX - (width / 2), endY, endX - 3, endY);
	inOutPath.lineTo(endX, endY);
	
	return inOutPath;
}


QPainterPath& GraphRenderer::CalcConnectionPath(QPainterPath& inOutPath, int startX, int startY, int endX, int endY)
{
	const float width = Math::Abs((endX - (3.0 * mShared->GetScreenScaling())) - (startX + (3.0 * mShared->GetScreenScaling())));
	inOutPath.moveTo(startX, startY);
	inOutPath.lineTo(startX + (3.0 * mShared->GetScreenScaling()), startY);
	inOutPath.cubicTo(startX + (width / 2), startY, endX - (width / 2), endY, endX - 3, endY);
	inOutPath.lineTo(endX, endY);

	return inOutPath;
}


// render the connection we're creating, if any
void GraphRenderer::RenderCreateConnection(QPainter& painter, Graph* graph, const GraphHelpers::CreateConnectionInfo& createConnectionInfo, const QPoint& globalMousePos)
{
	Node* dragStartNode = createConnectionInfo.mConNode;
	uint32 dragStartPortNr = createConnectionInfo.mConPortNr;

	if (dragStartNode == NULL)
		return;

	// calculate the start and end point
	QPoint start, end;
	QRect createConNodeRect = CalcNodeRect( graph, createConnectionInfo.mConNode );
	start = createConNodeRect.topLeft() + createConnectionInfo.mConStartOffset;
	end	  = globalMousePos;

	// get the port over which we are currently hovering
	Node*		portNode	= NULL;
	uint32		portNr		= CORE_INVALIDINDEX32;
	bool		isInputPort	= false;
	Port* port = FindPort( graph, globalMousePos.x(), globalMousePos.y(), &portNode, &portNr, &isInputPort );

	// get the mid point of the drag node starting port
	const QRect dragStartNodeRect	= CalcNodeRect( graph, dragStartNode );
	QRect dragStartPortRect = CalcSourceRect( dragStartNode, dragStartPortNr, dragStartNodeRect );
	
	int32 startX = dragStartPortRect.right();
	int32 startY = dragStartPortRect.center().y();
	int32 endX = end.x();
	int32 endY = end.y();

	QRect finalRect;
	finalRect.setCoords( startY, startY, endX, endY );

	// calc the rect in screen space (after scrolling and zooming)
	//QRect transformedRect = mShared->GetTransform().mapRect( finalRect );

	QColor borderColor;

	//QColor textColor = penColor;
	QColor nodeColor = mShared->GetNodeColor( true, dragStartNode );
	QColor textColor = nodeColor;

	borderColor = mShared->GetConnectionBorderColor();

	// get the color
	QColor penColor = textColor;
	if (portNode != NULL)
	{
		penColor = QColor( 0, 255, 0 );

		if (isInputPort == true)
		{
			if (portNode->GetInputPort(portNr).HasConnection() == true)
				penColor = mShared->GetErrorBlinkColor();
		}
		else
		{
			if (graph->HasInputConnection(createConnectionInfo.mConNode, createConnectionInfo.mConPortNr) == true)
				penColor = mShared->GetErrorBlinkColor();
		}

		if (createConnectionInfo.mConIsInputPort == isInputPort)
			penColor = mShared->GetErrorBlinkColor();

		if (createConnectionInfo.mConNode == portNode)
			penColor = mShared->GetErrorBlinkColor();

		if (createConnectionInfo.mConPort->IsCompatibleWith(*port) == false)
			penColor = mShared->GetErrorBlinkColor();
	}

	mPainterPath = QPainterPath();
	CalcConnectionPath(mPainterPath, startX, startY, endX, endY);

	//// draw the curve
	//mPainterPath = QPainterPath();
 //   const float width = Math::Abs((endX - (3.0 * mShared->GetScreenScaling())) - (startX + (3.0 * mShared->GetScreenScaling())));
	//mPainterPath.moveTo(startX, startY);
	//mPainterPath.lineTo(startX + (3.0 * mShared->GetScreenScaling()), startY);
	//mPainterPath.cubicTo(startX + (width / 2), startY, endX - (width / 2), endY, endX - 3, endY);
	//mPainterPath.lineTo(endX, endY);

	int				connectionWidth = (3.0 * mShared->GetScreenScaling());
	uint32			connectionDashPatternIndex = 0;
	Qt::PenStyle	connectionStyle = Qt::PenStyle::DashLine;

	if (dragStartNode != NULL)
	{
		Port& port = dragStartNode->GetOutputPort(dragStartPortNr);

		// select size and dot style for each channel type
		if (port.GetValueAttribute()->GetType() == AttributeChannels<double>::TYPE_ID)
		{
			MultiChannel* channels = port.GetChannels();
			const uint32 width = (channels->GetNumChannels() > 1) ? 5 : 3;
			connectionWidth = (width * mShared->GetScreenScaling());
			connectionStyle = Qt::PenStyle::DotLine;
		}
		else if (port.GetValueAttribute()->GetType() == AttributeChannels<Spectrum>::TYPE_ID)
		{
			MultiChannel* channels = port.GetChannels();
			const uint32 width = (channels->GetNumChannels() > 1) ? 9 : 7;
			connectionWidth = (width * mShared->GetScreenScaling());
			connectionDashPatternIndex = 0;
			connectionStyle = Qt::PenStyle::CustomDashLine;
		}
	}

	//////////////////////////////////////////////////////////////////
	// draw tube around the connection
	//////////////////////////////////////////////////////////////////
	float oldOpacity = painter.opacity();
	painter.setOpacity(0.5f);
	QPen pen;
	pen.setStyle(Qt::PenStyle::SolidLine);
	pen.setWidth((11 * mShared->GetScreenScaling()));
	pen.setColor(borderColor);
	painter.setPen(pen);

	// set the brush
	QBrush brush;
	brush.setColor(borderColor);
	painter.setBrush(brush);

	painter.drawPath(mPainterPath);
	painter.setOpacity(oldOpacity);
	//////////////////////////////////////////////////////////////////

	pen.setStyle( connectionStyle );
	pen.setCapStyle( Qt::PenCapStyle::FlatCap );
	pen.setWidth( connectionWidth );

	// apply custom dash pattern
	if (connectionStyle == Qt::PenStyle::CustomDashLine)
		pen.setDashPattern( mShared->GetConnectionDashPattern( connectionDashPatternIndex ) );

	// animate only actively processing AND active connections
	//if (isProcessed == true)
	{
		// check channel activity
		Port& port = dragStartNode->GetOutputPort(dragStartPortNr);
		MultiChannel* channels = port.GetChannels();
	
		// animate only if channel is active (or if it is a deprecated float connection)
		if (channels != NULL && channels->IsActive() == true)
		{
			// modify channel speed using its sample rate (scale by ln(rate + 10) / 5)
			const double offset = mShared->GetConnectionDashOffset() * Math::LogD(channels->GetSampleRate() + 10) / 5.0;
			const double normalizedDashOffset = offset * 3 / (double)connectionWidth;
			pen.setDashOffset( normalizedDashOffset );
		}
	}

	// set the pen
	pen.setColor(penColor);
	painter.setPen( pen );

	// set the brush
	brush.setColor(penColor);
	painter.setBrush( brush );
	
	painter.drawPath( mPainterPath );
	
	painter.setOpacity( 1.0f );
}


// calculate the width
int32 GraphRenderer::CalcRequiredNodeWidth(GraphShared* shared, Node* node, const QFontMetrics& portFontMetrics, const QFontMetrics& infoFontMetrics, const QFontMetrics& headerFontMetrics, const QFontMetrics& subtitleFontMetrics, int32* outMaxInputWidth, int32* outMaxOutputWidth)
{
	const int32 minNodeWidth = 50.0;

	// calc the maximum input port name widths
	const int32 maxInputWidth	= CalcMaxNodeInputPortWidth( node, portFontMetrics );
	const int32 maxOutputWidth	= CalcMaxNodeOutputPortWidth( node, portFontMetrics );
	*outMaxInputWidth			= maxInputWidth;
	*outMaxOutputWidth			= maxOutputWidth;

	// calculate the node info width and fix it so its never zero so the inpu/output port names have space between them
	//int32 infoWidth		= infoFontMetrics.width( node->GetNodeInfo() );
	//if (infoWidth == 0)
		int32 infoWidth = 15.0f * shared->GetScreenScaling();
	
	// total width: 2x max port text length + 2 * padding at borders + node info width
	const int32 totalPortWidth = maxInputWidth + maxOutputWidth + (2.0f * shared->GetPortTextDistance() + 2.0f * shared->GetPortTextNodeInfoDistance()) * shared->GetScreenScaling() + infoWidth;

	// calculate the title													        vv icon width      + vvv 4x padding of 2 units
	const int32 titleWidth = headerFontMetrics.width(node->GetName()) + (shared->GetNodeHeaderHeight() + 4*2) * shared->GetScreenScaling();
	
	// calculate the final width and make sure the node is at least 50 units in width
	int32 requiredWidth = Core::Max<int32>(titleWidth, totalPortWidth);
	requiredWidth = Core::Max<int32>(requiredWidth, (minNodeWidth * shared->GetScreenScaling()));
	return requiredWidth;
}


// calculate the height (including title and bottom)
int32 GraphRenderer::CalcRequiredNodeHeight(GraphShared* shared, Node* node)
{
	// calculate the node height based on the number of ports when not collapsed
	Node::ECollapsedState collapsedState = node->GetCollapsedState();
	if (collapsedState != Node::COLLAPSE_ALL)
	{
		// find number of visible ports left and right
		uint32 numVisibleInputPorts = 0;
		uint32 numVisibleOutputPorts = 0;

		// FIXME include COLLAPSE_VIP mode here, too
		// calculate how many ports are actually drawn, depending on their visibility state

		// inputs
		const uint32 numInputPorts = node->GetNumInputPorts();
		for (uint32 i=0; i<numInputPorts; ++i )
		{
			const Port& port = node->GetInputPort(i);
			if (port.IsVisible())	// is important if its a channel set
				numVisibleInputPorts++;
		}
		// outputs
		const uint32 numOutputPorts = node->GetNumOutputPorts();
		for (uint32 i = 0; i<numOutputPorts; ++i)
		{
			const Port& port = node->GetOutputPort(i);
			if (port.IsVisible())	// is important if its a channel set
				numVisibleOutputPorts++;
		}

		// Note: added border radius so the last port does not leave a gap due to the round node corners (factor 3 was determined by experimentation)
		const uint32 numPorts = Core::Max<uint32>(numVisibleInputPorts, numVisibleOutputPorts);
		return (numPorts * ((shared->GetPortDiameter() + shared->GetPortDistance()) * shared->GetScreenScaling())) + (shared->GetNodeHeaderHeight() * shared->GetScreenScaling()) + shared->GetBorderRadius() / 3.0;
	}

	// return static height for collapsed nodes
	return (shared->GetNodeHeaderHeight() * shared->GetScreenScaling());
}


// calculate the connection rect
QRect GraphRenderer::CalcConnectionRect(Graph* graph, Connection* connection)
{
	QRect sourceNodeRect = CalcNodeRect( graph, connection->GetSourceNode() );
	QRect targetNodeRect = CalcNodeRect( graph, connection->GetTargetNode() );

	QRect sourceRect = CalcSourceRect( connection->GetSourceNode(), connection->GetSourcePort(), sourceNodeRect );
	QRect targetRect = CalcTargetRect( connection->GetTargetNode(), connection->GetTargetPort(), targetNodeRect );

	return sourceRect.united( targetRect );
}


// calc the max input port width
uint32 GraphRenderer::CalcMaxNodeInputPortWidth(Node* node, const QFontMetrics& fontMetrics)
{
	// calc the maximum input port width
	uint32 maxInputWidth = 0;
	uint32 width;
	const uint32 numInputPorts = node->GetNumInputPorts();
	for (uint32 i=0; i<numInputPorts; ++i)
	{
		const Port& port = node->GetInputPort(i);

		// skip if invisible
		if (port.IsVisible() == false)
			continue;

		width			= fontMetrics.width( port.GetName() );
		maxInputWidth	= Core::Max<uint32>( maxInputWidth, width );
	}

	return maxInputWidth;
}


// calculate the max output port width
uint32 GraphRenderer::CalcMaxNodeOutputPortWidth(Node* node, const QFontMetrics& fontMetrics)
{
	// calc the maximum output port width
	uint32 width;
	uint32 maxOutputWidth = 0;
	const uint32 numOutputPorts = node->GetNumOutputPorts();
	for (uint32 i=0; i<numOutputPorts; ++i)
	{
		const Port& port = node->GetOutputPort(i);

		// skip if invisible
		if (port.IsVisible() == false)
			continue;

		width			= fontMetrics.width( port.GetName() );
		maxOutputWidth	= Core::Max<uint32>( maxOutputWidth, width );
	}

	return maxOutputWidth;
}


// check if the point is inside the arrow rect
bool GraphRenderer::IsPointOverNodeIcon(Graph* graph, Node* node, const QPoint& globalPoint)
{
	QRect iconRect;
	QRect nodeRect = CalcNodeRect( graph, node );
	
	// we shrink the icon hit area a little and move it more to the left and top
	const float iconWidth = (mShared->GetNodeHeaderHeight() - 2.0f) * mShared->GetScreenScaling();
	iconRect.setCoords(nodeRect.left(), nodeRect.top(), nodeRect.left() + iconWidth, nodeRect.top() + iconWidth);
	
	return iconRect.contains(globalPoint);
}


// is the given point close to the connection
bool GraphRenderer::IsCloseToConnection(Graph* graph, const QPoint& point, Connection* connection)
{
	QRect sourceNodeRect = CalcNodeRect( graph, connection->GetSourceNode() );
	QRect targetNodeRect = CalcNodeRect( graph, connection->GetTargetNode() );
	QRect sourceRect = CalcSourceRect( connection->GetSourceNode(), connection->GetSourcePort(), sourceNodeRect );
	QRect targetRect = CalcTargetRect( connection->GetTargetNode(), connection->GetTargetPort(), targetNodeRect );

	return GraphHelpers::IsPointCloseToSmoothedLine(sourceRect.center().x(), sourceRect.center().y(), targetRect.center().x(), targetRect.center().y(), point.x(), point.y());

	// Note: reverted this back to the old version above, its much more performant but less accurate (its done for every render step for every connection...)
	/*mPainterPath = QPainterPath();
	CalcConnectionPath(mPainterPath, graph, connection);
	const float distance = GraphHelpers::DistanceToPath(point, mPainterPath);
	if (distance < 5.0f)
		return true;
	else
		return false;
		*/
}


// get the source rect
QRect GraphRenderer::CalcSourceRect(Node* node, uint32 sourcePort, const QRect& nodeRect)
{
	if (node->GetCollapsedState() != Node::COLLAPSE_ALL)
		return CalcOutputPortRect(node, sourcePort, nodeRect);
	else
	{
		// calculate collapsed source rect
		QPoint a = QPoint(nodeRect.right(), nodeRect.top() + (13.0*mShared->GetScreenScaling()));
		return QRect(a - QPoint((1.0*mShared->GetScreenScaling()), (1.0*mShared->GetScreenScaling())), a);
	}
}


// get the target rect
QRect GraphRenderer::CalcTargetRect(Node* node, uint32 targetPort, const QRect& nodeRect)
{
	if (node->GetCollapsedState() != Node::COLLAPSE_ALL)
		return CalcInputPortRect(node, targetPort, nodeRect);
	else
	{
		// calculate collapsed target rect
		QPoint a = QPoint(nodeRect.left(), nodeRect.top() + (13.0*mShared->GetScreenScaling()));
		return QRect(a, a + QPoint((1.0*mShared->GetScreenScaling()), (1.0*mShared->GetScreenScaling())));
	}
}


bool GraphRenderer::IsPointOnNode(Graph* graph, Node* node, const QPoint& globalPoint)
{
	QRect nodeRect = CalcNodeRect( graph, node );

	// check if the node is under the current point
	return nodeRect.contains(globalPoint);
}


// find the port at a given location within a given node
Port* GraphRenderer::FindPort(Graph* graph, Node* node, int32 x, int32 y, uint32* outPortNr, bool* outIsInputPort, bool includeInputPorts)
{
	uint32 i;

	// if the node is collapsed we can skip directly, too
	if (node->GetCollapsedState() == Node::COLLAPSE_ALL)
		return NULL;

	// calculate the node rect
	QRect nodeRect = CalcNodeRect( graph, node );

	// check the input ports
	if (includeInputPorts == true)
	{
		const uint32 numInputPorts = node->GetNumInputPorts();
		for (i=0; i<numInputPorts; ++i)
		{
			// skip port if invisible
			if (node->GetInputPort(i).IsVisible() == false)
				continue;

			QRect rect = CalcInputPortInteractionRect( node, i, nodeRect );
			if (rect.contains( QPoint(x, y) ))
			{
				*outPortNr		= i;
				*outIsInputPort = true;
				return &(node->GetInputPort(i));
			}
		}
	}

	// check the output ports
	const uint32 numOutputPorts = node->GetNumOutputPorts();
	for (i=0; i<numOutputPorts; ++i)
	{
		// skip port if invisible
		if (node->GetOutputPort(i).IsVisible() == false)
			continue;

		QRect rect = CalcOutputPortInteractionRect( node, i, nodeRect );
		if (rect.contains( QPoint(x, y) ))
		{
			*outPortNr		= i;
			*outIsInputPort = false;
			return &(node->GetOutputPort(i));
		}
	}

	return NULL;
}


// find the port at a given location
Port* GraphRenderer::FindPort(Graph* graph, int32 x, int32 y, Node** outNode, uint32* outPortNr, bool* outIsInputPort, bool includeInputPorts)
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
Connection* GraphRenderer::FindConnection(Graph* graph, const QPoint& mousePos)
{
	// if there is no valid graph, return directly
	if (graph == NULL)
		return NULL;

	// get the number of connections and iterate through them
	const uint32 numCons = graph->GetNumConnections();
	for (uint32 i=0; i<numCons; ++i)
	{
		// get the connection
		Connection* connection = graph->GetConnection(i);
		
		// return it if if mouse is close
		if (IsCloseToConnection(graph, mousePos, connection) == true)
			return connection;
	}

	// failure, there is no connection at the given mouse position
	return NULL;
}


// select all nodes within a given rect::pain
void GraphRenderer::SelectNodesAndConnectionsInRect(Graph* graph, const QRect& selectionRect, bool overwriteCurSelection, bool select, bool toggleMode)
{
	if (overwriteCurSelection == true)
	{
		mShared->UnselectAllNodes(false);
		mShared->UnselectAllConnections(false);
	}

	// if there is no valid graph, return directly
	if (graph == NULL)
		return;

	// 1) select nodes

	// get the number of nodes and iterate through them
	const uint32 numNodes = graph->GetNumNodes();
	for (uint32 i=0; i<numNodes; ++i)
	{
		// get the current node and calculate its rect
		Node*	node		= graph->GetNode(i);
		QRect	nodeRect	= CalcNodeRect( graph, node );

		// check if the node rect is inside the selection rect, if yes select the node
		if (nodeRect.intersects(selectionRect) == true)
		{
			if (toggleMode == true)
				mShared->SetNodeSelected( node, !mShared->IsNodeSelected(node), false );
			else
				mShared->SetNodeSelected( node, select, false );
		}
		else
		{
			if (overwriteCurSelection)
				mShared->UnselectNode( node, false );
		}
	}


	// 2) select connections
	
	// get the number of connections and iterate through them
	const uint32 numCons = graph->GetNumConnections();
	for (uint32 i=0; i<numCons; ++i)
	{
		// get the connection
		Connection* connection = graph->GetConnection(i);
		
		// calc connection path
		mPainterPath = QPainterPath();
		CalcConnectionPath(mPainterPath, graph, connection);

		const bool isSelected = GraphHelpers::PathIntersectsRect(mPainterPath, selectionRect);

		if (isSelected == true)
		{
			if (toggleMode == true)
				mShared->SetConnectionSelected(connection, !mShared->IsConnectionSelected(connection), false);
			else
				mShared->SetConnectionSelected(connection, select, false);
		}
		else
		{
			if (overwriteCurSelection)
				mShared->UnselectConnection(connection, false);
		}

		/*const QRect		connectionRect	= CalcConnectionRect( graph, connection );

		// check if the connectionrect is inside the selection rect, if yes select the connection
		if (connectionRect.intersects(selectionRect) == true)
		{
			if (toggleMode == true)
				mShared->SetConnectionSelected( connection, !mShared->IsConnectionSelected(connection), false );
			else
				mShared->SetConnectionSelected( connection, select, false );
		}
		else
		{
			if (overwriteCurSelection)
				mShared->UnselectConnection( connection, false );
		}*/
	}

	mShared->EmitSelectionChangedSignal();
}


const QPixmap& GraphRenderer::FindNodeIcon(Node* node, uint32 pixmapSize)
{
	const uint32 nodeType = node->GetType();

	// get the number of node icons and iterate through them
	const uint32 numNodeIcons = mNodeIcons.Size();
	for (uint32 i=0; i<numNodeIcons; ++i)
	{
		if (mNodeIcons[i]->mNodeType == nodeType)
			return mNodeIcons[i]->mPixmap;
	}

	// the node icon is not loaded yet, load it!
	QString nodeIconFilename = GraphPaletteWidget::GetNodeIconFileName(node);
	QPixmap pixmap = QIcon( nodeIconFilename ).pixmap(pixmapSize, pixmapSize);

	// create the node icon cache object and add it to the node icons array
	NodeIconCache* nodeIconCache = new NodeIconCache(pixmap, node->GetType());
	mNodeIcons.Add(nodeIconCache);

	return nodeIconCache->mPixmap;
}


GraphRenderer::NodeIconCache::NodeIconCache(QPixmap pixmap, uint32 nodeType)				
{
	mPixmap		= pixmap; 
	mNodeType	= nodeType;
}


GraphRenderer::NodeIconCache::~NodeIconCache()			
{
}
