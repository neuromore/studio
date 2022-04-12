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
#include "ChannelSelectionWidget.h"
#include <Core/LogManager.h>
#include <BciDevice.h>

#include <QHBoxLayout>

using namespace Core;

// constructor
ChannelSelectionWidget::ChannelSelectionWidget(QWidget* parent) : QWidget(parent)
{
	mSelectedChannel = NULL;
	mShowOnlyRawChannels = false;

	mChannelComboBox = new QComboBox();
	connect(mChannelComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnChannelSelectionIndexChanged(int)));
	
	mDeviceSelectionWidget = new DeviceSelectionWidget();
	connect(mDeviceSelectionWidget, SIGNAL(DeviceSelectionChanged(Device*)), this, SLOT(OnDeviceSelectionChanged(Device*)));

	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->addWidget(mDeviceSelectionWidget);
	hLayout->addWidget(mChannelComboBox);
	hLayout->setMargin(0);
	hLayout->setSpacing(0);
	setLayout(hLayout);
}

// destructor
ChannelSelectionWidget::~ChannelSelectionWidget()
{
}


void ChannelSelectionWidget::Init()
{
	// init device selection widget
	mDeviceSelectionWidget->Init();

	// if we have a device, init the channel list
	if (mDeviceSelectionWidget->GetSelectedDevice() != NULL)
	{

		ReInit(mDeviceSelectionWidget->GetSelectedDevice());
	}
}


// initialize for a new device
void ChannelSelectionWidget::ReInit(Device* device)
{
	mChannelComboBox->blockSignals(true);
	mChannelComboBox->clear();
	mAvailableChannels.Clear();

	uint32 numChannels = 0;

	//TODO for now, we only allow neuro headset and EEG channels
	if (device->GetBaseType() == BciDevice::BASE_TYPE_ID)
	{
		// add all EEG channels

		BciDevice* headset = static_cast<BciDevice*>(device);
		numChannels = headset->GetNumNeuroSensors(); 
		for (uint32 i = 0; i < numChannels; i++)
		{
			Sensor* sensor = headset->GetNeuroSensor(i);
			Channel<double>* channel = sensor->GetChannel();
			mAvailableChannels.Add(channel);
			mChannelComboBox->addItem(channel->GetName());
		}
	}

	// select the first channel as the active one
	if (numChannels > 0)
	{
		mSelectedChannel = mAvailableChannels[0];
		OnChannelSelectionIndexChanged(0);
	}

	mChannelComboBox->blockSignals(false);
}


// called when selecting a new element in the combobox
void ChannelSelectionWidget::OnChannelSelectionIndexChanged(int index)
{
	mSelectedChannel = mAvailableChannels[index];
	emit ChannelSelectionChanged(mSelectedChannel);
}


void ChannelSelectionWidget::OnDeviceSelectionChanged(Device* device)
{
	ReInit(device);
}
