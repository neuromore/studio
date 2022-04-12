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
#include "SignalQualityWidget.h"
#include "../AppManager.h"
#include <Core/LogManager.h>
#include <QHBoxLayout>


using namespace Core;

// constructor
SignalQualityWidget::SignalQualityWidget(QWidget* parent) : QWidget(parent)
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

	AddPixmap( ":/Images/Icons/SignalQuality0.png" );
	AddPixmap( ":/Images/Icons/SignalQuality20.png" );
	AddPixmap( ":/Images/Icons/SignalQuality40.png" );
	AddPixmap( ":/Images/Icons/SignalQuality60.png" );
	AddPixmap( ":/Images/Icons/SignalQuality80.png" );
	AddPixmap( ":/Images/Icons/SignalQuality100.png" );

	// initialize the signal quality widget
	mSignalQuality = 1.0; // set it to full so that it updates correctly with 0.0 in the set call
	SetSignalQuality( 0.0 );
}


// destructor
SignalQualityWidget::~SignalQualityWidget()
{
}



void SignalQualityWidget::AddPixmap(QString filename)
{
	QPixmap pixmap(filename);
	pixmap = pixmap.scaledToHeight( mPixmapHeight, Qt::TransformationMode::SmoothTransformation );

	mPixmaps.Add(pixmap);
}


// set the battery status
void SignalQualityWidget::SetSignalQuality(double normalizedSignalQuality)
{
	// make sure the given battery status is inside the range
	normalizedSignalQuality = Core::Clamp<double>( normalizedSignalQuality, 0.0, 1.0 );

	// check if the signal quality actually changed, if not skip directly
	if (Core::IsClose<double>(mSignalQuality, normalizedSignalQuality, 0.01f) == true)
		return;

	// update the signal quality
	mSignalQuality = normalizedSignalQuality;

	mTempString.Format( "%.0f %%", normalizedSignalQuality * 100.0 );
	mTextLabel->setText( mTempString.AsChar() );

	const int32 pixmapIndex = normalizedSignalQuality * mPixmaps.Size() - Math::epsilon;
	mPixmapLabel->setPixmap( mPixmaps[pixmapIndex] );
}
