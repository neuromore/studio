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

// include required headers
#include "HeartRateWidget.h"
#include "../AppManager.h"
#include <Core/LogManager.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPen>


using namespace Core;

// constructor
HeartRateWidget::HeartRateWidget(QWidget* parent) : QWidget(parent)
{
	// main layout
	QHBoxLayout* mainLayout = new QHBoxLayout();
	mainLayout->setMargin(0);
	//mainLayout->setSpacing(0);
	setLayout( mainLayout );


	QVBoxLayout* leftLayout	= new QVBoxLayout();
	QVBoxLayout* rightLayout= new QVBoxLayout();

	leftLayout->setSpacing(0);
	rightLayout->setSpacing(0);

	mainLayout->addLayout(leftLayout);
	mainLayout->addLayout(rightLayout);

	////////////////////////
	// heart rate (left)

	// add the header label
	QLabel* headerLabel = new QLabel( "Heart Rate" );
	headerLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	leftLayout->addWidget( headerLabel );

	// add the main widget
	QWidget* mainWidget = new QWidget();
	mainWidget->setObjectName("TransparentWidget");
	leftLayout->addWidget( mainWidget );


	QHBoxLayout* hLayout = new QHBoxLayout();
	//hLayout->setMargin(0);
	hLayout->setSpacing(0);
	mainWidget->setLayout( hLayout );

	// heart rate label
	mHeartRateLabel = new QLabel();
	mHeartRateLabel->setStyleSheet("QLabel { color : white; }");
	QFont heartRateFont = mHeartRateLabel->font();
	heartRateFont.setPixelSize(50);
	mHeartRateLabel->setFont(heartRateFont);
	mHeartRateLabel->setText( "" );
	hLayout->addWidget( mHeartRateLabel, 0, Qt::AlignCenter );

	QVBoxLayout* rightVLayout = new QVBoxLayout();
	rightVLayout->setMargin(0);
	rightVLayout->setSpacing(0);
	hLayout->addLayout( rightVLayout );

	// load an image
	QString imageA = ":/Images/Icons/Heart_Key1.png";
	QString imageB = ":/Images/Icons/Heart_Key2.png";
	mHeart = new SwitchingImage( imageA, imageB, 64, 64 );
	rightVLayout->addWidget( mHeart, 0, Qt::AlignHCenter );

	// BPM label
	QLabel* bpmLabel = new QLabel();
	bpmLabel->setStyleSheet("QLabel { color : white; }");
	QFont bpmFont = bpmLabel->font();
	bpmFont.setPixelSize(30);
	bpmLabel->setFont(bpmFont);
	bpmLabel->setText( "BPM" );
	rightVLayout->addWidget( bpmLabel, 0, Qt::AlignCenter );

	///////////////////////////
	// HRV (right layout)

	// add the header label
	headerLabel = new QLabel( "Heart Rate Variability" );
	headerLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	rightLayout->addWidget( headerLabel );

	// add the main widget
	QWidget* mainRightWidget = new QWidget();
	mainRightWidget->setObjectName("TransparentWidget");
	rightLayout->addWidget( mainRightWidget );

	rightVLayout = new QVBoxLayout();
	rightVLayout->setMargin(0);
	rightVLayout->setSpacing(0);
	mainRightWidget->setLayout( rightVLayout );

	mainRightWidget->setMaximumWidth(105);

	// heart rate variability label
	mHRVLabel = new QLabel();
	mHRVLabel->setStyleSheet("QLabel { color : white; }");
	mHRVLabel->setFont(bpmFont);
	mHRVLabel->setText( "" );
	rightVLayout->addWidget( mHRVLabel, 0, Qt::AlignHCenter | Qt::AlignBottom );

	// milliseconds label
	QLabel* msLabel = new QLabel();
	msLabel->setStyleSheet("QLabel { color : white; }");
	msLabel->setFont(bpmFont);
	msLabel->setText( "ms" );
	rightVLayout->addWidget( msLabel, 0, Qt::AlignHCenter | Qt::AlignTop );

	setMaximumHeight(135);
}


// destructor
HeartRateWidget::~HeartRateWidget()
{
}


// change the heart rate
void HeartRateWidget::SetHeartRate(int heartRateBPM, int HRVinMS)
{
	// set the heart rate label
	if (heartRateBPM < 100)
	{
		if (heartRateBPM < 0)
			mHeartRateString = "---";
		else if (heartRateBPM < 10)
			mHeartRateString.Format( "00%i", heartRateBPM );
		else
			mHeartRateString.Format( "0%i", heartRateBPM );
	}
	else
		mHeartRateString.Format( "%i", heartRateBPM );

	mHeartRateLabel->setText( mHeartRateString.AsChar() );

	// animate the heart icon
	const float beatsPerSecond = (float)heartRateBPM / 60.0f;
	const float interval = 1.0f / beatsPerSecond;

	// set the heart switching rate
	mHeart->SetSwitchingCountDown( interval * 0.5f );

	// set the HRV
	if (HRVinMS < 100)
	{
		if (HRVinMS < 0)
			mHRVString = "---";
		else if (HRVinMS < 10)
			mHRVString.Format( "00%i", HRVinMS );
		else
			mHRVString.Format( "0%i", HRVinMS );
	}
	else
		mHRVString.Format( "%i", HRVinMS );

	mHRVLabel->setText( mHRVString.AsChar() );
}
