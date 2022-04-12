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
#include "NoDeviceWidget.h"
#include <EngineManager.h>
#include <QLabel>
#include <QMovie>
#include <QVBoxLayout>


using namespace Core;

#define NODEVICEWIDGETTEXT_CLICK		"Click to search for devices"
#define NODEVICEWIDGETTEXT_SEARCHING	"Searching ..."
#define NODEVICEWIDGETTEXT_AUTO			"Auto detection enabled"

// constructor
NoDeviceWidget::NoDeviceWidget(QWidget* parent) : QWidget(parent)
{
	// create a vertical layout
	QVBoxLayout* vLayout = new QVBoxLayout(this);
	setLayout(vLayout);

	// top spacer widget
	QWidget* topSpacerWidget = new QWidget();
	topSpacerWidget->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	vLayout->addWidget(topSpacerWidget, 0, Qt::AlignHCenter );

	// add search button
	mSearchButton = new ImageButton( "/Images/Icons/Search.png", IMAGEBUTTONSIZE_BIG, "Search for devices");
	mSearchButton->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	connect(mSearchButton, SIGNAL(released()), this, SLOT(OnSearchButton()));
	vLayout->addWidget( mSearchButton, 0, Qt::AlignHCenter );

	// add waiting animation
	const QSize size = mSearchButton->size();
	mWaitingMovie = new QMovie(":/Images/Icons/Loader.gif");
	mWaitingMovie->setScaledSize(size - QSize(4, 4));
	mWaitingMovieLabel = new QLabel();
	mWaitingMovieLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	mWaitingMovieLabel->setMinimumSize(size);
	mWaitingMovieLabel->setMaximumSize(size);
	mWaitingMovieLabel->setAlignment(Qt::AlignCenter);
	mWaitingMovieLabel->setStyleSheet("background:transparent;");
	mWaitingMovieLabel->setAttribute(Qt::WA_TranslucentBackground, true);
	mWaitingMovieLabel->setMovie(mWaitingMovie);
	mWaitingMovieLabel->setVisible(false);
	vLayout->addWidget( mWaitingMovieLabel, 0, Qt::AlignHCenter );

	// search status label
	mLabel = new QLabel();
	mLabel->setText(NODEVICEWIDGETTEXT_CLICK);
	vLayout->addWidget( mLabel, 0, Qt::AlignHCenter );

	// bottom spacer widget
	QWidget* bottomSpacerWidget = new QWidget();
	bottomSpacerWidget->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	vLayout->addWidget(bottomSpacerWidget, 0, Qt::AlignHCenter );
}


// destructor
NoDeviceWidget::~NoDeviceWidget()
{
}


// reinitialize device list
void NoDeviceWidget::ReInit()
{

}


// update interface information
void NoDeviceWidget::UpdateInterface()
{
	const uint32 numDevices = GetDeviceManager()->GetNumDevices();

	// update top row buttons
	if (GetEngine()->GetAutoDetectionSetting() == true)
	{
		// hide manual search button if autodetection is enabled
		mSearchButton->setVisible(false);
		mLabel->setText(NODEVICEWIDGETTEXT_AUTO);
		mWaitingMovieLabel->setVisible(false);
	}
	else
	{
		// detection is running
		if (GetDeviceManager()->IsDetectionRunning() == true)
		{
			mSearchButton->setVisible(false);
			mLabel->setText(NODEVICEWIDGETTEXT_SEARCHING);
			mWaitingMovieLabel->setVisible(true);

			// start movie if not already
			if (mWaitingMovie->state() != QMovie::Running)
				mWaitingMovie->start();
		}
		// detection not active
		else if (GetDeviceManager()->IsDetectionRunning() == false)  
		{
			mSearchButton->setVisible(true);
			mLabel->setText(NODEVICEWIDGETTEXT_CLICK);
			mWaitingMovieLabel->setVisible(false);

			// stop movie if not already
			if (mWaitingMovie->state() == QMovie::Running)
				mWaitingMovie->stop();
		}
	}
}


// search for devices
void NoDeviceWidget::OnSearchButton()
{
	GetDeviceManager()->DetectDevices();
}
