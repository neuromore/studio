/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "PlotWidget.h"
#include <Core/LogManager.h>
#include <QPainter>
#include <QPen>


using namespace Core;

// constructor
PlotWidget::PlotWidget(QWidget* parent) : QWidget(parent)
{
}


// destructor
PlotWidget::~PlotWidget()
{
}


// render the background grid
void PlotWidget::RenderGridBackground(QPainter& painter, QPoint upperLeft, QPoint lowerRight, int32 spacing, int32 numSubGridLinesPerCell)
{
	// grid line color
	painter.setPen( QColor(40, 40, 40) );

	// calculate the start and end ranges in 'scrolled and zoomed out' coordinates
	// we need to render sub-grids covering that area
	const int32 startX		= upperLeft.x();
	const int32 startY		= upperLeft.y();
	const int32 endX		= lowerRight.x();
	const int32 endY		= lowerRight.y();
	const int32 width		= endX - startX;
	const int32 height		= endY - startY;
	const int32 numHLines	= height / spacing;
	const int32 numVLines	= width / spacing;

	QPen gridPen, subgridPen;
	gridPen.setColor( QColor(58, 58, 58) );
	subgridPen.setColor( QColor(55, 55, 55) );

	// draw subgridlines first
	painter.setPen( subgridPen );

	// draw vertical lines
	for (int32 i=0; i<=numVLines; ++i)
	{
		if (i % numSubGridLinesPerCell == 0)
			continue;

		const int32 x = startX + i * spacing;
		painter.drawLine(x, startY, x, endY);
	}

	// draw horizontal lines
	for (int32 i=0; i<=numHLines; ++i)
	{
		if (i % numSubGridLinesPerCell == 0)
			continue;

		const int32 y = startY + i * spacing;
		painter.drawLine(startX, y, endX, y);
	}

	// draw render grid lines
	painter.setPen( gridPen );

	// draw vertical lines
	for (int32 i=0; i<=numVLines; ++i)
	{
		if (i % numSubGridLinesPerCell != 0)
			continue;

		const int32 x = startX + i * spacing;
		painter.drawLine(x, startY, x, endY);
	}

	// draw horizontal lines
	for (int32 i=0; i<=numHLines; ++i)
	{
		if (i % numSubGridLinesPerCell != 0)
			continue;

		const int32 y = startY + i * spacing;
		painter.drawLine(startX, y, endX, y);
	}
}

//
//// overloaded paint event
//void PlotWidget::paintEvent(QPaintEvent* event)
//{
//	// TODO clean up plot code in FilterPlotWidget and make it possible to draw axis/grid/function/labels with one call each; right now it's not possible to extend the plot code with custom rendering without copying the whole function everything
//}
