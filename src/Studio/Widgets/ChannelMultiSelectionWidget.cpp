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
#include "ChannelMultiSelectionWidget.h"
#include <Core/LogManager.h>

#include <QHBoxLayout>

using namespace Core;


// constructor
ChannelMultiSelectionWidget::ChannelMultiSelectionWidget(QWidget* parent) : QWidget(parent)
{
	mShowUsedCheckbox = new QCheckBox();
	mShowUsedCheckbox->setText("Used");
	mShowUsedCheckbox->setToolTip("Show only channels used by classifier.");
	mShowUsedCheckbox->setChecked(false);
	connect(mShowUsedCheckbox, SIGNAL(stateChanged(int)), this, SLOT(OnShowUsedCheckboxToggled(int)));

	mChannelMultiCheckbox = new HMultiCheckboxWidget();
	connect(mChannelMultiCheckbox, SIGNAL(SelectionChanged()), this, SLOT(OnChannelSelectionChanged()));
	
	mDeviceSelectionWidget = new DeviceSelectionWidget();
	connect(mDeviceSelectionWidget, SIGNAL(DeviceSelectionChanged(Device*)), this, SLOT(OnDeviceSelectionChanged(Device*)));

	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->addWidget(mDeviceSelectionWidget);
	hLayout->addWidget(mShowUsedCheckbox);
	hLayout->addWidget(mChannelMultiCheckbox);
	hLayout->setMargin(0);
	hLayout->setSpacing(0);

	setLayout(hLayout);

	mAutoSelectType = SELECT_NONE;
	mShowOnlyEEGChannels = false;
}


// destructor
ChannelMultiSelectionWidget::~ChannelMultiSelectionWidget()
{
}


void ChannelMultiSelectionWidget::Init()
{
	// init device selection widget
	mDeviceSelectionWidget->Init();

	if (mDeviceSelectionWidget->GetSelectedDevice() != NULL)
		ReInit(mDeviceSelectionWidget->GetSelectedDevice());
}


// initialize for a new device : get sensors, fill checkbox widget
void ChannelMultiSelectionWidget::ReInit(Device* device)
{
	// if device not specified, automatically use the first one
	if (device == NULL)
		device = mDeviceSelectionWidget->GetSelectedDevice();

	// prepare the arrays for the reinitialization
	mAvailableChannels.Clear();
	Array<String> names;
	Array<String> tooltips;
	Array<Color> colors;

	// in case there is no device the pointer will be NULL and we don't add any checkboxes
	BciDevice* headset = NULL;
	if (device != NULL)
	{
		//TODO for now, we only allow neuro headset and EEG channels
		if (device->GetBaseType() == BciDevice::BASE_TYPE_ID)
		{
			headset = static_cast<BciDevice*>(device);
			
			// add all EEG channels
			const uint32 numRawSensors = headset->GetNumNeuroSensors(); 
			for (uint32 i = 0; i < numRawSensors; i++)
			{
				Sensor* sensor = headset->GetNeuroSensor(i);
				Channel<double>* channel = sensor->GetChannel();
				
				// skip channel, if it is not used
				if (IsShowUsedChecked() == true && mUsedChannels.Contains(channel) == false)
					continue;
			
				mAvailableChannels.Add(channel);
				names.Add(channel->GetName());
				tooltips.Add(channel->GetName());
				colors.Add(channel->GetColor());
			}
			// add all other non-EEG sensors
			if (mShowOnlyEEGChannels == false)
			{
				const uint32 numSensors = headset->GetNumSensors(); 
				for (uint32 i=0; i<numSensors; ++i)
				{
					Sensor* sensor = headset->GetSensor(i);
					Channel<double>* channel = sensor->GetChannel();

					// skip channel, if it is not used
					if (IsShowUsedChecked() == true && mUsedChannels.Contains(channel) == false)
						continue;
			
					// check if this a neuro sensor (there is no other way to do it right now)
					bool isNeuroSensor = false;
					/*const uint32 numRawSensors = */headset->GetNumNeuroSensors();
					for (uint32 j=0; j<numSensors && isNeuroSensor == false; ++j)
					{
						if (headset->GetSensor(i) == headset->GetNeuroSensor(j))	
							isNeuroSensor = true;
					}
				
					// don't add this sensor if its a neurosensor
					if (isNeuroSensor == false)
					{

						mAvailableChannels.Add(channel);
						names.Add(channel->GetName());
						tooltips.Add(channel->GetName());
						colors.Add(channel->GetColor());
					}
				}
			}

		}

		

	}

	// hide "used" checkbox is no device is present
	mShowUsedCheckbox->setVisible(device != NULL);

	// reinit the multi checkbox widget
	mChannelMultiCheckbox->ReInit(names, tooltips, colors, "All");

	// auto-select channels
	const uint32 numCheckBoxes = mChannelMultiCheckbox->GetNumCheckboxes();
	uint32 numBoxesToSelect = 0;
	switch (mAutoSelectType)
	{
		case AutoSelectType::SELECT_NONE: numBoxesToSelect = 0; break;
		case AutoSelectType::SELECT_FIRST: numBoxesToSelect = 1; break;
		
		case AutoSelectType::SELECT_ALL: 
		default:
			numBoxesToSelect = numCheckBoxes; break;
	}
	mChannelMultiCheckbox->CheckXCheckboxes(numBoxesToSelect);

	// disable REF and GND by default
	for (uint32 i = 0; i < names.Size(); i++)
	{
		if (i >= mChannelMultiCheckbox->GetNumCheckboxes())
			return;

		if (names[i] == "REF" || names[i] == "GND")
			mChannelMultiCheckbox->GetCheckbox(i)->setChecked(false);
	}
}


void ChannelMultiSelectionWidget::OnChannelSelectionChanged()
{
	const uint32 numSensors = mAvailableChannels.Size();
	
	mSelectedChannels.Clear();

	for (uint32 i = 0; i < numSensors; ++i)
	{
		if (mChannelMultiCheckbox->IsChecked(i))
			mSelectedChannels.Add(mAvailableChannels[i]);
	}

	emit ChannelSelectionChanged();
}


void ChannelMultiSelectionWidget::OnDeviceSelectionChanged(Device* device)
{
	ReInit(device);
}


void ChannelMultiSelectionWidget::OnShowUsedCheckboxToggled(int state)
{
	// reinit with current device
	ReInit();

	emit ShowUsedCheckboxToggled(state);
}


void ChannelMultiSelectionWidget::SetChannelAsUsed(Channel<double>* channel, bool used)
{
	// not in list, add it 
	const bool inList = mUsedChannels.Contains(channel);
	if (used == true && inList == false)
		mUsedChannels.Add(channel);

	// in list, remove it
	else if (used == false && inList == true)
		mUsedChannels.RemoveByValue(channel);
}

void ChannelMultiSelectionWidget::ClearUsedChannels()
{
	mUsedChannels.Clear(); 
	mShowUsedCheckbox->setVisible(false);
}


void ChannelMultiSelectionWidget::SetChecked(Channel<double>* channel, bool checked)
{
	const uint32 channelIndex = mSelectedChannels.Find(channel);
	if (channelIndex != CORE_INVALIDINDEX32)
		SetChecked(channelIndex, checked);
}


void ChannelMultiSelectionWidget::SetChecked(uint32 index, bool checked)
{
	mChannelMultiCheckbox->SetChecked(index, checked);
}


void ChannelMultiSelectionWidget::SetVisible(uint32 index, bool visible)
{
	if (index >= mChannelMultiCheckbox->GetNumCheckboxes())
		return;

	mChannelMultiCheckbox->GetCheckbox(index)->setVisible(visible);
}
