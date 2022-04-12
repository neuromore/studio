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
#include "DevicesPlugin.h"
#include <EngineManager.h>
#include <Core/EventManager.h>
#include <Core/LogManager.h>


using namespace Core;

// constructor
DevicesPlugin::DevicesPlugin() : Plugin(GetStaticTypeUuid())
{
	LogDetailedInfo("Constructing Devices plugin ...");
	mDevicesWidget = NULL;
}


// destructor
DevicesPlugin::~DevicesPlugin()
{
	LogDetailedInfo("Destructing Devices plugin ...");
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// init after the parent dock window has been created
bool DevicesPlugin::Init()
{
	LogDetailedInfo("Initializing Device plugin ...");

	// create the container widget
	QWidget* containerWidget = new QWidget(mDock);
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->setMargin(0);
	vLayout->setSpacing(0);
	containerWidget->setLayout(vLayout);
	mDock->SetContents(containerWidget);

	// create the devices widget and hide it by default
	mDevicesWidget = new DevicesWidget(containerWidget);
	mDevicesWidget->setVisible(false);
	vLayout->addWidget(mDevicesWidget);

	// create the no devices widget
	mNoDeviceWidget = new NoDeviceWidget(containerWidget);
	vLayout->addWidget(mNoDeviceWidget);

	// add event handler
	CORE_EVENTMANAGER.AddEventHandler(this);

	// reinit
	ReInit();

	LogDetailedInfo("Device Status plugin successfully initialized...");
	return true;
}


// reinitialize device list
void DevicesPlugin::ReInit()
{
	if (mDevicesWidget != NULL)
		mDevicesWidget->ReInit();

	if (mNoDeviceWidget != NULL)
		mNoDeviceWidget->ReInit();
}


// update interface information
void DevicesPlugin::UpdateInterface()
{
	bool hasDevices = false;

	if (mDevicesWidget != NULL)
	{
		hasDevices = mDevicesWidget->GetNumDevices() > 0;

		mDevicesWidget->UpdateInterface();
		mDevicesWidget->setVisible( hasDevices == true );
	}

	if (mNoDeviceWidget != NULL)
	{
		mNoDeviceWidget->UpdateInterface();
		mNoDeviceWidget->setVisible( hasDevices == false );
	}
}


void DevicesPlugin::OnDeviceAdded(Device* device)
{
	if (mDevicesWidget != NULL)
		mDevicesWidget->AddWidgetForDevice(device);

	UpdateInterface();
}


void DevicesPlugin::OnRemoveDevice(Device* device)
{
	if (mDevicesWidget != NULL)
		mDevicesWidget->RemoveWidgetByDevice(device);

	UpdateInterface();
}
