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
#include "DeviceSelectionWidget.h"
#include <Core/LogManager.h>
#include <qcombobox.h>


using namespace Core;

// constructor
DeviceSelectionWidget::DeviceSelectionWidget(QWidget* parent) : QComboBox(parent)
{
	CORE_EVENTMANAGER.AddEventHandler(this);
	connect(this, SIGNAL(activated(int)), this, SLOT(OnCurrentIndexChanged(int)));

	// always switch to the active device
	mUseActiveDevice = true;
}


// destructor
DeviceSelectionWidget::~DeviceSelectionWidget()
{
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// combobox selection was changed
void DeviceSelectionWidget::OnCurrentIndexChanged(int index)
{
	// no index -> no device selected
	if (index == -1)
	{
		emit DeviceSelectionChanged(NULL);
		return;
	}

	// get the selected device
	Device* selectedDevice = GetSelectedDevice();

	// first entry is special and turns on the UseActiveDevices mode
	const bool enableActiveDeviceMode = (index == 0);
	SetUseActiveDeviceEnabled(enableActiveDeviceMode);

	// if the the active device mode was turned on by selecting the first entry, switch the selection to the device entry
	if (mUseActiveDevice == true)
		UpdateSelection(selectedDevice);
	else{
		emit DeviceSelectionChanged(selectedDevice);
	}
}

void DeviceSelectionWidget::SetUseActiveDeviceEnabled(bool enable)
{
	mUseActiveDevice = enable;

	if (enable == true)
	{
		//this->setItemText(0, "Use Active Device (on)");
		this->setItemData(0, QColor(0, 200, 0), Qt::TextColorRole);
	}
	else
	{
		//this->setItemText(0, "Use Active Device (off)");
		this->setItemData(0, QColor(100,100,100), Qt::TextColorRole);
	}

}


Device* DeviceSelectionWidget::GetSelectedDevice() const
{
	// index of selected entry
	int currentIndex = this->currentIndex();

	// if the first or no entry is selected, return the active device
	if (currentIndex <= 0)
		return GetEngine()->GetActiveBci();

	// select the device by index
	uint32 deviceIndex = currentIndex - 1;
	CORE_ASSERT(deviceIndex < GetDeviceManager()->GetNumDevices());
	return  GetDeviceManager()->GetDevice(deviceIndex);
}


// fill combobox with all devices
void DeviceSelectionWidget::Init()
{
	ReInit();
}


// fill combo box with devices, select mActiveDevice
void DeviceSelectionWidget::ReInit()
{
	// block signals because we change the combobox selection
	this->blockSignals(true);

	Device* activeDevice = GetEngine()->GetActiveBci();

	// clear combobox and add first entry
	this->clear();
	addItem("Use Active Device");
	QFont italicalizedFont = this->font();
	italicalizedFont.setItalic(true);
	this->setItemData(0, italicalizedFont, Qt::FontRole);

	bool foundActiveDevice = false;

	// iterate over devices, fill combobox
	uint32 numDevices = GetDeviceManager()->GetNumDevices();
	for (uint32 i = 0; i < numDevices; i++)
	{
		Device* device = GetDeviceManager()->GetDevice(i);
		addItem(device->GetHardwareName());

		// if the device is the active headset, select the entry we just added
		if (device == activeDevice)
		{
			setCurrentIndex(i+1);	//+1 due to first entry
			foundActiveDevice = true;
		}
	}

	// if no active headset is set, select "Use Active Device" entry
	if (foundActiveDevice == false)
		setCurrentIndex(0);

	// call once so the first entry gets the right color
	SetUseActiveDeviceEnabled(mUseActiveDevice);

	this->blockSignals(false);
}


// select the right entry by device and fire selection-change event
void DeviceSelectionWidget::UpdateSelection(Device* activeDevice)
{
	this->blockSignals(true);

	// select "Use Active Device" first entry first
	setCurrentIndex(0);

	// find which index corresponds to the given device 
	uint32 numDevices = GetDeviceManager()->GetNumDevices();
	if (activeDevice != NULL)
	{
		for (uint32 i = 0; i < numDevices; i++)
		{
			Device* device = GetDeviceManager()->GetDevice(i);

			// if the device is the active headset, select the entry
			if (device == activeDevice)
			{
				const uint32 itemIndex = i + 1;

				// supress signals because this is not a selection by the user
				setCurrentIndex(itemIndex);
				break;
			}
		}
	}

	this->blockSignals(false);

	// fire event
	emit DeviceSelectionChanged(activeDevice);
}



// EVENTS ---------------------------------------------

// select the active headset if mUseActiveDevice is true
void DeviceSelectionWidget::OnActiveBciChanged(BciDevice* device)
{
	// only change headset automatically if this mode is active
	if (mUseActiveDevice == false)
		return;

	UpdateSelection(device);
}


// append a new entry to the end of the list (requires that new devices are appended at the end in the device manager!)
void DeviceSelectionWidget::OnDeviceAdded(Device* device)
{
	addItem(device->GetHardwareName());
}


// pre-deletion event
void DeviceSelectionWidget::OnRemoveDevice(Device* device)
{
	// find the index of the device in the manager/list 
	uint32 deviceIndex = CORE_INVALIDINDEX32;

	uint32 numDevices = GetDeviceManager()->GetNumDevices();
	for (uint32 i = 0; i < numDevices; i++)
	{
		if (GetDeviceManager()->GetDevice(i) == device)
		{
			deviceIndex = i;
			break;
		}
	}

	// device is not managed by device manager -> do nothing
	if (deviceIndex == CORE_INVALIDINDEX32)
		return;

	// the entry to remove, shifted by one due to the first entry
	const uint32 itemToRemove = deviceIndex + 1;
	
	// remember selected index
	const int selected = currentIndex();

	// delete entry
	removeItem(itemToRemove);

	// fix selection
	if (selected < 0)
	{
		setCurrentIndex(0);
	}
	// check if the index of the currently selected element will change (if an elemente with a smaller index is deleted)
	else if (itemToRemove < (uint32)selected)
	{
		// entries shift one index down 
		setCurrentIndex(selected - 1);
	}
	else if (itemToRemove == selected)
	{
		setCurrentIndex(-1);
		setCurrentIndex(0);
		OnCurrentIndexChanged(0);
	}

	// failsafe: select first entry if nothing is selected
	if (currentIndex() < 0)
		setCurrentIndex(0);

}


// post-deletion event -> nothing to do?
void DeviceSelectionWidget::OnDeviceRemoved(Device* device)
{
}

// ----------------------------------------------------
