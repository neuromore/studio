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

// include required files
#include "DeviceHelpers.h"
#include <Device.h>
#include <EngineManager.h>
#include "System/SerialPort.h"


using namespace Core;

Core::Array<Core::String> DeviceHelpers::FindSerialPorts(const Device::DeviceConfig& config)
{
	Array<String> foundPorts;

	// try to get serial ports from config
	Json::Item rootItem = config.mJson.GetRootItem();


	// A) the defined serialPorts
	Json::Item serialItem = rootItem.Find("serialPort");
	if (serialItem.IsString() == true && String(serialItem.GetString()).IsEmpty() == false)
		foundPorts.Add(serialItem.GetString());


	// B) via usb vid/pid
	uint16 usbPid = 0;
	uint16 usbVid = 0;
	Json::Item pidItem = rootItem.Find("usbPid");
	Json::Item vidItem = rootItem.Find("usbVid");
		
	if (pidItem.IsNull() == false && vidItem.IsNull() == false)
	{
		bool hasError = false;
		if (pidItem.IsNull() == false && vidItem.IsNull() == false)
		{	
			// PID parse as integer or as 4 char hex string
			if (pidItem.IsInt() == true)
			{
				usbPid = pidItem.GetInt();
			}
			else if (pidItem.IsString() && String(pidItem.GetString()).GetLength() == 4)
			{
				const char* pidString = pidItem.GetString();
				if (sscanf(pidString, "%hx", &usbPid) != 1)
					hasError = true;
			}

			// VID: parse as integer or as 4 char hex string
			if (vidItem.IsInt() == true)
			{
				usbVid = vidItem.GetInt();
			}
			else if (vidItem.IsString() && String(vidItem.GetString()).GetLength() == 4)
			{
				const char* vidString = vidItem.GetString();
				if (sscanf(vidString, "%hx", &usbVid) != 1)
					hasError = true;
			}
		}

		// don't have complete PID/VID?
		if (hasError == true || usbPid == 0 || usbVid == 0)
		{
			// TODO we can't log the file path right now, the config doesn't know about it
			LogError("Error parsing device config. \"usbPid\" and \"usbVid\" must be both specified as either integer or four character hex string");
		}
		else
		{
			// detect serial port from vid/pid pairs
			foundPorts.Add( SerialPort::FindPortsByUsbID(usbVid, usbPid) );
		}

	}

	// TODO 
	// C) detect via bluetooth macs

	return foundPorts;
}

