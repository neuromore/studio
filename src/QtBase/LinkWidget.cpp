/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "LinkWidget.h"


// constructor
LinkWidget::LinkWidget(const char* text, QWidget* parent) : QToolButton(parent)
{
	setObjectName("LinkWidget");
	setText( text );
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setCursor(Qt::PointingHandCursor);

	connect (this, &LinkWidget::clicked, this, &LinkWidget::OnClicked);
}


// destructor
LinkWidget::~LinkWidget()
{
}


// handle click event
void LinkWidget::OnClicked()
{
	emit Clicked();
}