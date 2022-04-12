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
#include "BreathingRateWidget.h"
#include "../AppManager.h"
#include <Core/LogManager.h>
#include <QHBoxLayout>
#include <QPainter>
#include <QPen>
#include <QLabel>


using namespace Core;

// constructor
BreathingRateWidget::BreathingRateWidget(QWidget* parent) : QWidget(parent)
{
	// main layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	setLayout( mainLayout );

	// add the header label
	QLabel* headerLabel = new QLabel( "Breathing Rate" );
	headerLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	mainLayout->addWidget( headerLabel );

	// add the main widget
	QWidget* mainWidget = new QWidget();
	mainWidget->setObjectName("TransparentWidget");
	mainLayout->addWidget( mainWidget );

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->setMargin(0);
	vLayout->setSpacing(0);
	mainWidget->setLayout( vLayout );

	// breathing rate label
	mBreathingRateLabel = new QLabel();
	mBreathingRateLabel->setStyleSheet("QLabel { color : white; }");
	QFont heartRateFont = mBreathingRateLabel->font();
	heartRateFont.setPixelSize(30);
	mBreathingRateLabel->setFont(heartRateFont);
	vLayout->addWidget( mBreathingRateLabel, 0, Qt::AlignCenter );

	// load an image
	QPixmap pixmap( ":/Images/Icons/Lungs.png" );
	//pixmap = pixmap.scaledToWidth( 90 );

	QLabel* imageLabel = new QLabel(this);
	imageLabel->setPixmap( pixmap );
	vLayout->addWidget(imageLabel);

	// right side
	/*QVBoxLayout* rightVLayout = new QVBoxLayout();
	rightVLayout->setMargin(0);
	rightVLayout->setSpacing(0);
	hLayout->addLayout( rightVLayout );*/

	// BPM label
	/*QLabel* bpmLabel = new QLabel();
	bpmLabel->setStyleSheet("QLabel { color : white; }");
	QFont bpmFont = bpmLabel->font();
	bpmFont.setPixelSize(30);
	bpmLabel->setFont(bpmFont);
	bpmLabel->setText( "BPM" );
	rightVLayout->addWidget( bpmLabel, 0, Qt::AlignCenter );*/
	
	//hLayout->addLayout( rightVLayout );
}


// destructor
BreathingRateWidget::~BreathingRateWidget()
{
}


// change the breathing rate
void BreathingRateWidget::SetBreathingRate(float bpm)
{
	if (bpm < 0.0f)
		mTempString = "--.- BPM";
	else
		mTempString.Format( "%.1f BPM", bpm );

	mBreathingRateLabel->setText( mTempString.AsChar() );
}
