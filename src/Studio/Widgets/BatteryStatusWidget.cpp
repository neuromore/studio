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
#include "BatteryStatusWidget.h"

using namespace Core;

// constructor
BatteryStatusWidget::BatteryStatusWidget(QWidget* parent) : QWidget(parent)
{
	mPixmapHeight = 24;

	setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

	// add the main widget
	QWidget* mainWidget = new QWidget();
	mainWidget->setObjectName("TransparentWidget");
	QVBoxLayout* vMainLayout = new QVBoxLayout();
	vMainLayout->setMargin(0);
	vMainLayout->addWidget(mainWidget);
	setLayout(vMainLayout);

	// main layout
	QHBoxLayout* mainLayout = new QHBoxLayout();
	mainLayout->setMargin(0);
	mainWidget->setLayout( mainLayout );

	// add the text label
	mPixmapLabel = new QLabel();
	mainLayout->addWidget( mPixmapLabel );

	// add the text label
	mTextLabel = new QLabel();
	mTextLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	QFont timeFont = mTextLabel->font();
	timeFont.setPixelSize(20);
	mTextLabel->setFont(timeFont);
	mainLayout->addWidget( mTextLabel, 0, Qt::AlignLeft | Qt::AlignHCenter );

	// note: same order as the enum EPixmaps
	AddPixmap( ":/Images/Icons/BatteryUnknown.png" );
	AddPixmap( ":/Images/Icons/BatteryEmpty.png" );
	AddPixmap( ":/Images/Icons/Battery0.png" );
	AddPixmap( ":/Images/Icons/Battery25.png" );
	AddPixmap( ":/Images/Icons/Battery50.png" );
	AddPixmap( ":/Images/Icons/Battery75.png" );
	AddPixmap( ":/Images/Icons/Battery100.png" );

	// animation timer for EMPTY state
	mAnimationTimer = new QTimer(this);
	mAnimationTimer->setInterval(500);
	connect( mAnimationTimer, &QTimer::timeout, this, &BatteryStatusWidget::OnTimerEvent );

	// initialize the battery status widget
	mBatteryLevel = -1;  // forces change in SetStatus
	mBatteryStatus = STATUS_UNKNOWN;
	SetStatus(0.0, STATUS_UNKNOWN);
}


// destructor
BatteryStatusWidget::~BatteryStatusWidget()
{
}


void BatteryStatusWidget::AddPixmap(QString filename)
{
	QPixmap pixmap(filename);
	pixmap = pixmap.scaledToHeight( mPixmapHeight, Qt::TransformationMode::SmoothTransformation );

	mPixmaps.Add(pixmap);
}


// set the battery status
void BatteryStatusWidget::SetStatus(double normalizedLevel, EStatus status)
{
	// make sure the given battery status is inside the range
	normalizedLevel = Core::Clamp<double>( normalizedLevel, 0.0, 1.0 );
	
	// check if the battery status actually changed, if not skip directly
	if (mBatteryLevel == normalizedLevel && mBatteryStatus == status)
		return;

	// select the image
	int32 pixmapIndex = CORE_INVALIDINDEX32;
	switch (status)
	{
		case STATUS_EMPTY: // blinking empty

			// force the charge level to 0
			normalizedLevel = 0.0;

			if (mBatteryStatus != STATUS_EMPTY) // don't disturb the flasing image when updating to the same state
				pixmapIndex = PIXMAP_EMPTY1;

			break;

		case STATUS_CRITICAL: // blinking red bar
			if (mBatteryStatus != STATUS_CRITICAL) // don't disturb the flasing image when updating to the same state
				pixmapIndex = PIXMAP_CRITICAL1;

			break;

		case STATUS_OK: // select one of the images with 2-4 bars

			if (normalizedLevel > 0.75)
				pixmapIndex = PIXMAP_100;
			else if (normalizedLevel > 0.5)
				pixmapIndex = PIXMAP_75;
			else
				pixmapIndex = PIXMAP_50;

			break;

		default:
		case STATUS_UNKNOWN:
			pixmapIndex = PIXMAP_UNKNOWN;
			break;

	}
	
	// set the status
	mBatteryLevel = normalizedLevel;
	mBatteryStatus = status;

	// set the label text
	if (mBatteryStatus == STATUS_UNKNOWN)
		mTempString = "";
	else
		mTempString.Format( "%.0f %%", mBatteryLevel * 100.0 );

	mTextLabel->setText( mTempString.AsChar() );

	// set the pixmap
	if (pixmapIndex != CORE_INVALIDINDEX32)
		mPixmapLabel->setPixmap( mPixmaps[pixmapIndex] );

	// start/stop blink timer
	if (mBatteryStatus != STATUS_OK)
		mAnimationTimer->start();
	else
		mAnimationTimer->stop();
}


// set status using a critical threshold
void BatteryStatusWidget::SetStatus(double normalizedLevel, double criticalLevel)
{
	normalizedLevel = Core::Clamp<double>( normalizedLevel, 0.0, 1.0 );
	criticalLevel = Core::Clamp<double>( criticalLevel, 0.0, 1.0 );
	
	// select on of the three states
	EStatus status = STATUS_OK;		// OK
	if (normalizedLevel <= 0.0)
		status = STATUS_EMPTY;		// EMPTY
	else if (normalizedLevel <= criticalLevel)
		status = STATUS_CRITICAL;	// CRITICAL

	SetStatus(normalizedLevel, status);
}


void BatteryStatusWidget::OnTimerEvent()
{
	// toggle images or stop timer
	if (mBatteryStatus == STATUS_EMPTY)
		mPixmapLabel->setPixmap( mPixmaps[ mAnimationState ? PIXMAP_EMPTY1 : PIXMAP_EMPTY2 ] );
	else if (mBatteryStatus == STATUS_CRITICAL)
		mPixmapLabel->setPixmap( mPixmaps[ mAnimationState ? PIXMAP_CRITICAL1 : PIXMAP_CRITICAL2 ]);
	else // stop blinking if status is OK
		mAnimationTimer->stop();
		
	// flip state
	mAnimationState = !mAnimationState;
}
