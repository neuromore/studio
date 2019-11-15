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
#include "DevicesWidget.h"
#include <EngineManager.h>
#include "BciDeviceWidget.h"
#include <QLabel>
#include <QMovie>
#include <QVBoxLayout>


using namespace Core;

// constructor
DevicesWidget::DevicesWidget(QWidget* parent) : QWidget(parent)
{
	mVLayout = NULL;

	// create the vertical main layout
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	setLayout(mainLayout);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(1);

	// horizontal layout for buttons
	QHBoxLayout* buttonsLayout = new QHBoxLayout(this);
	//buttonsLayout->setSpacing(2);
	buttonsLayout->setAlignment(Qt::AlignLeft);
	mainLayout->addLayout(buttonsLayout);

	// add search button
	mSearchButton = new ImageButton( "/Images/Icons/Search.png", DEFAULT_ICONSIZE, "Search for devices");
	connect(mSearchButton, SIGNAL(released()), this, SLOT(OnSearchButton()));
	buttonsLayout->addWidget(mSearchButton);

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
	buttonsLayout->addWidget(mWaitingMovieLabel);

	// search status label
	mSearchLabel = new QLabel();
	mSearchLabel->setText(" Searching ...");
	mSearchLabel->setVisible(false);
	buttonsLayout->addWidget(mSearchLabel);

	// main widget
	QWidget* mainWidget = new QWidget();
	mainLayout->addWidget(mainWidget);

	mVLayout = new QVBoxLayout();
	mVLayout->setSpacing(50);
	mVLayout->setAlignment( Qt::AlignTop );
	mainWidget->setLayout(mVLayout);

	//setFocusPolicy(Qt::StrongFocus);
}


// destructor
DevicesWidget::~DevicesWidget()
{
}


void DevicesWidget::Clear()
{
	const uint32 numDeviceWidgets = mDeviceWidgets.Size();
	for (uint32 i=0; i<numDeviceWidgets; ++i)
	{
		mDeviceWidgets[i]->hide();
		mDeviceWidgets[i]->deleteLater();
	}

	mDeviceWidgets.Clear();
}


// reinitialize device list
void DevicesWidget::ReInit()
{
	if (mVLayout == NULL)
		return;

	// clear all widgets
	Clear();

	// fill dialog stack with devices
	String tmpString;
	Array<Device*> devices = GetDeviceManager()->GetDevices();
	const uint32 numDevices = devices.Size();

	// add one device status widget per device to stack
	for (uint32 i = 0; i < numDevices; ++i)
		AddWidgetForDevice(devices[i]);
}


// update interface information
void DevicesWidget::UpdateInterface()
{
	const uint32 numDevices = mDeviceWidgets.Size();

	// add one device status widget per device to stack
	for (uint32 i=0; i<numDevices; ++i)
	{
		mDeviceWidgets[i]->UpdateInterface();
	}

	// update top row buttons
	if (GetEngine()->GetAutoDetectionSetting() == true)
	{
		// hide manual search button if autodetection is enabled
		mSearchButton->setVisible(false);
		mSearchLabel->setVisible(false);
		mWaitingMovieLabel->setVisible(false);
	}
	else
	{
		// detection is running
		if (GetDeviceManager()->IsDetectionRunning() == true)
		{
			mSearchButton->setVisible(false);
			mSearchLabel->setVisible(true);
			mWaitingMovieLabel->setVisible(true);

			// start movie if not already
			if (mWaitingMovie->state() != QMovie::Running)
				mWaitingMovie->start();
		}
		// detection not active
		else if (GetDeviceManager()->IsDetectionRunning() == false)  
		{
			mSearchButton->setVisible(true);
			mSearchLabel->setVisible(false);
			mWaitingMovieLabel->setVisible(false);

			// stop movie if not already
			if (mWaitingMovie->state() == QMovie::Running)
				mWaitingMovie->stop();
		}
	}
}


// create and add a device status widget to the dialog stack
void DevicesWidget::AddWidgetForDevice(Device* device)
{
	// create appropriate device status widget
	DeviceWidget* widget;
	switch (device->GetBaseType())
	{
		case BciDevice::BASE_TYPE_ID:
		{
			BciDevice* headset = static_cast<BciDevice*>(device);
			widget = new BciDeviceWidget(headset, this);
			break;
		}

		default:
			widget = new DeviceWidget(device, this);
	}

	widget->Init();
	mDeviceWidgets.Add(widget);

	// add the widget
	mVLayout->addWidget( widget );
	//Add(widget, device->GetName().AsChar());
}


// remove a device status widget from the dialog stack
void DevicesWidget::RemoveWidgetByDevice(Device* device)
{
	// find widget that belongs to this device and remove it
	const uint32 numDeviceWidgets = mDeviceWidgets.Size();
	for (uint32 i=0; i<numDeviceWidgets; ++i)
	{
		DeviceWidget* widget = mDeviceWidgets[i];

		// remove widget if it belongs to the device
		if (widget->GetDevice() == device)
		{
			mDeviceWidgets[i]->hide();
			mDeviceWidgets[i]->deleteLater();
			mDeviceWidgets.Remove(i);

			// return directly
			return;
		}
	}
}


// search for devices
void DevicesWidget::OnSearchButton()
{
	GetDeviceManager()->DetectDevices();
}



// called when a keyboard button got pressed
/*void DevicesWidget::keyPressEvent(QKeyEvent* event)
{
	int32 key = event->key();

	// handle decreasing the value
	if (key == Qt::Key_Down)
	{
		const uint32 num = GetDeviceManager()->GetNumDevices();
		if (num <= 0)
			return;

		const uint32 randomIndex = (uint32)Math::RandF( 0.0f, (float)num-0.1f );
		if (num <= 0)
			return;

		Device* device = GetDeviceManager()->GetDevice(randomIndex);

		GetDeviceManager()->RemoveDeviceAsync( device );
	}

	// handle increasing the value
	if (key == Qt::Key_Up)
	{
		Device* device = GetDeviceManager()->CreateDeviceObjectByType( DeviceTypeIDs::DEVICE_TYPEID_TEST );
		GetDeviceManager()->AddDeviceAsync( device );
	}
	
	event->accept();
}*/
