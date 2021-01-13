/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "ColorLabel.h"


using namespace Core;

ColorLabel::ColorLabel(QWidget* parent, const QColor& defaultColor) : QLabel(parent) 
{
	setFixedHeight(20);

	mColorDialog = new QColorDialog( defaultColor );
	connect( mColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(OnColorChanged(QColor)) );
	
	UpdateInterface();
}


ColorLabel::~ColorLabel()
{
	delete mColorDialog;
}


void ColorLabel::UpdateInterface()
{
	QColor color = GetColor();

	int r, g, b;
	color.getRgb(&r, &g, &b);
	setStyleSheet( Core::String().Format("color: rgb(%i, %i, %i);background-color: rgb(%i, %i, %i); border: 1px solid rgb(0,0,0);", r, g, b, r, g, b).AsChar() );
}


void ColorLabel::OnColorChanged(const QColor& color)
{
	UpdateInterface();
	emit ColorChangeEvent(color);
	emit ColorChangeEvent();
}