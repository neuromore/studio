/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "ColorMappingWidget.h"
#include <Core/LogManager.h>
#include <QPushButton>
#include <QHBoxLayout>
#include <QPainter>
#include <QPen>


using namespace Core;

// constructor
ColorMappingWidget::ColorMappingWidget(ColorMapper* colorMapper, QWidget* parent) : QWidget(parent)
{
	mColorMapper = colorMapper;

	setMinimumHeight( 30 );
}


// destructor
ColorMappingWidget::~ColorMappingWidget()
{
}


// overload the paintevent
void ColorMappingWidget::paintEvent(QPaintEvent* event)
{
	QRect rect = this->rect();

	// init the painter
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	painter.setPen( Qt::NoPen );

	QLinearGradient gradient( QPointF(0, 0), QPointF(rect.width(), 0) );
	
	// get the number of colors in the color mapping and iterate through them
	const uint32 numColors = mColorMapper->GetNumColors();
	for (uint32 i=0; i<numColors; ++i)
	{
		double		position	= mColorMapper->GetColorPos(i);
		Color	color		= mColorMapper->GetColor(i);

		QColor qColor;
		qColor.setRgbF( color.r, color.g, color.b );

		if (isEnabled() == false)
			qColor = qColor.darker(300);

		gradient.setColorAt( position, qColor );
	}

	painter.setBrush( gradient );

	painter.drawRect( rect);
}
