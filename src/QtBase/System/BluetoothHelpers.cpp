/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

#include "BluetoothHelpers.h"
#include <Core/LogManager.h>
#ifdef NEUROMORE_PLATFORM_WINDOWS
	#include <stdlib.h>
	#include <stdio.h>
	// Link to ws2_32.lib
	//#include <Winsock2.h>
	//#include <Ws2bth.h>

	// Link to Bthprops.lib
	#include <BluetoothAPIs.h>


///

	#include <Setupapi.h>
	#include <Cfgmgr32.h>
#endif

using namespace Core;


// return a list of all (paired) devices that either contain or match the name
Array<BluetoothHelpers::DeviceInfo> BluetoothHelpers::FindDevicesByName(const char* name, bool exactMatch)
{
	// helper array
	Array<String> names;
	names.Add(name);

	return FindDevicesByName(names, exactMatch);
}


// return a list of all (paired) devices that either contain or match any of the device names
Array<BluetoothHelpers::DeviceInfo> BluetoothHelpers::FindDevicesByName(const Core::Array<Core::String>& names, bool exactMatch)
{
	Array<DeviceInfo> deviceInfos;

#ifdef NEUROMORE_PLATFORM_WINDOWS
    
    // get the number of names upfront
    const uint32 numNames = names.Size();

	// NOTE: this is mostly a copy-paste combination of the FindFirstRadio() / FindFirstDevice() examples in BluetoothAPIs.h
	
	// related to radio (the thing in your pc)
	HANDLE hRadio;
	BLUETOOTH_FIND_RADIO_PARAMS btfrp = { sizeof(btfrp) };
	
	// related to device
	HBLUETOOTH_DEVICE_FIND hFindDevice;
	BLUETOOTH_DEVICE_INFO btdi = { sizeof(btdi) };
	
	// device search parameters
	BLUETOOTH_DEVICE_SEARCH_PARAMS btsp = { sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS) };
	btsp.fReturnAuthenticated = TRUE;
	btsp.fReturnRemembered    = TRUE;
	
	// iterate over all devices of all radios
	HBLUETOOTH_RADIO_FIND hFindRadio = BluetoothFindFirstRadio( &btfrp, &hRadio );
	if ( NULL != hFindRadio )
	{
		// iterate over radios
		do
		{
			btsp.hRadio = hRadio;
			ZeroMemory(&btdi, sizeof(BLUETOOTH_DEVICE_INFO));
			btdi.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
			
			// find all devices connected to this radio
			hFindDevice = BluetoothFindFirstDevice(&btsp, &btdi);
			if ( NULL != hFindDevice )
			{
				// iterate over devices of radio
				do
				{
					deviceInfos.AddEmpty();
					DeviceInfo& info = deviceInfos.GetLast();

					// extract device info and copy it into our own struct
					info.mMacAddress = btdi.Address.ullLong;
					info.mMacAddressHex.Format("%02x%02x%02x%02x%02x%02x", btdi.Address.rgBytes[5], btdi.Address.rgBytes[4], btdi.Address.rgBytes[3], btdi.Address.rgBytes[2], btdi.Address.rgBytes[1], btdi.Address.rgBytes[0]);
					info.mMacAddressHex.ToUpper();

					QString unicodeDeviceName = QString::fromWCharArray( btdi.szName );
					info.mName = unicodeDeviceName.toUtf8().data();

					info.mDeviceClass = btdi.ulClassofDevice;
					info.mIsPaired = btdi.fAuthenticated;
					info.mIsConnected = btdi.fConnected;

					//LogInfo( "Device found: %s", unicodeDeviceName.toLatin1().data() );
					
					// match names (exact or partially)
					bool isMatch = false;
					if (exactMatch == true)
					{
						// check against bluetooth name
						for (uint32 i=0; i<numNames; ++i)
						{
							if (info.mName.Compare(names[i]) == 0)
							{
								isMatch = true;
								break;
							}
						}
					}
					else
					{
						// convert to low caps first
						info.mName.ToLower();

						// check against bluetooth name
						String lcName;
						for (uint32 i=0; i<numNames; ++i)
						{
							lcName = names[i];
							lcName.ToLower();

							if (info.mName.Contains(lcName) == true)
							{
								isMatch = true;
								break;
							}
						}
					}
					
					// remove element again
					if (isMatch == false)
						deviceInfos.RemoveLast();
					
				} while( BluetoothFindNextDevice( hFindDevice, &btdi ) );
				
				BluetoothFindDeviceClose( hFindDevice );
			}
			
	     } while( BluetoothFindNextRadio( hFindRadio, &hRadio ) );
	
	     BluetoothFindRadioClose( hFindRadio );
	 }

#endif

	// return found device infos
	return deviceInfos;
}

// find the first SPP serial port of a BT device (if it has one; if not it returns an empty string)
String BluetoothHelpers::FindSerialPort(const BluetoothHelpers::DeviceInfo& device)
{
	Array<String> ports = FindSerialPorts(device);
	if (ports.Size() == 0)
		return "";

	// return first port
	return ports[0];
}


//FIXME this is not able to find the second serial port of the brainquiry devices.. still only finds the first one even early-break code was removed
// find all serial ports of a BT device (if it has one; if not it returns an empty array)
Array<String> BluetoothHelpers::FindSerialPorts(const BluetoothHelpers::DeviceInfo& device)
{
	Array<String> devicePorts;

#ifdef NEUROMORE_PLATFORM_WINDOWS
	DWORD reqGuids = 16;
	GUID guids[16];
	HDEVINFO devs = INVALID_HANDLE_VALUE;
	DWORD devIndex;
	SP_DEVINFO_DATA devInfo;
	devInfo.cbSize = sizeof(devInfo);
	TCHAR hardware_id_string[300];
	DWORD propertyType;
	SP_DEVINFO_LIST_DETAIL_DATA devInfoListDetail;
	devInfoListDetail.cbSize = sizeof(devInfoListDetail);
	
	// for parsing the w-char strings we get from the api
	String hwParsingString;

	if (SetupDiClassGuidsFromNameEx(L"ports", guids, reqGuids, &reqGuids, NULL, NULL) == false)
		return devicePorts ;

	for (uint32 i = 0; i < reqGuids; ++i)
	{
		devs = SetupDiGetClassDevsEx(&guids[i],NULL,NULL,DIGCF_PRESENT,NULL,NULL,NULL);
		if (devs != INVALID_HANDLE_VALUE)
		{
			BOOL bSuccess = SetupDiGetDeviceInfoListDetail(devs, &devInfoListDetail);
			if (bSuccess == false)
				continue;
				
			devIndex = 0;
			while (SetupDiEnumDeviceInfo(devs,devIndex,&devInfo))
			{
				int status = CM_Get_Device_ID_Ex(devInfo.DevInst, hardware_id_string, 300, 0, devInfoListDetail.RemoteMachineHandle);
				if (status == CR_SUCCESS)
				{
					QString unicodeHwParsingString = QString::fromWCharArray( hardware_id_string );
					hwParsingString = unicodeHwParsingString.toUtf8().data();

					if (hwParsingString.Contains(device.mMacAddressHex.AsChar()) == true)
					{
						bool bSuccess = SetupDiGetDeviceRegistryProperty(devs, &devInfo, SPDRP_FRIENDLYNAME, &propertyType, (PBYTE) hardware_id_string, sizeof(hardware_id_string), NULL);
						if (bSuccess == false)
							continue;

						unicodeHwParsingString = QString::fromWCharArray( hardware_id_string );
						hwParsingString = unicodeHwParsingString.toUtf8().data();

						// hwParsingString has the form "Standard Serial over Bluetooth link (COM9)" -> extract the port name within the parenthesis
						const uint32 lastWordIndex = hwParsingString.CalcNumWords() - 1;
						String portName = hwParsingString.ExtractWord(lastWordIndex);
						portName.RemoveChars(")(");
									
						// cleanup
						SetupDiDestroyDeviceInfoList(devs);

						devicePorts.Add(portName);
					}
				}

				devIndex++;
					
			}

			// cleanup
			SetupDiDestroyDeviceInfoList(devs);
		}
	}
#endif

	// no serial port was found
	return devicePorts;
}




// return a list of all (paired) devices that either contain or match the name
Core::String& BluetoothHelpers::LogDevicesToString(Core::String& inOutString)
{
	Array<DeviceInfo> deviceInfos;

	String tempStr;
	uint32 numDevices = 0;

#ifdef NEUROMORE_PLATFORM_WINDOWS

	// NOTE: this is mostly a copy-paste combination of the FindFirstRadio() / FindFirstDevice() examples in BluetoothAPIs.h

	// related to radio (the thing in your pc)
	HANDLE hRadio;
	BLUETOOTH_FIND_RADIO_PARAMS btfrp = { sizeof(btfrp) };

	// related to device
	HBLUETOOTH_DEVICE_FIND hFindDevice;
	BLUETOOTH_DEVICE_INFO btdi = { sizeof(btdi) };

	// device search parameters
	BLUETOOTH_DEVICE_SEARCH_PARAMS btsp = { sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS) };
	btsp.fReturnAuthenticated = TRUE;
	btsp.fReturnRemembered = TRUE;

	// iterate over all devices of all radios
	HBLUETOOTH_RADIO_FIND hFindRadio = BluetoothFindFirstRadio(&btfrp, &hRadio);
	if (NULL != hFindRadio)
	{
		// iterate over radios
		do
		{
			btsp.hRadio = hRadio;
			ZeroMemory(&btdi, sizeof(BLUETOOTH_DEVICE_INFO));
			btdi.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);

			// find all devices connected to this radio
			hFindDevice = BluetoothFindFirstDevice(&btsp, &btdi);
			if (NULL != hFindDevice)
			{
				// iterate over devices of radio
				do
				{
					deviceInfos.AddEmpty();
					DeviceInfo& info = deviceInfos.GetLast();

					// extract device info and copy it into our own struct
					info.mMacAddress = btdi.Address.ullLong;
					info.mMacAddressHex.Format("%02x%02x%02x%02x%02x%02x", btdi.Address.rgBytes[5], btdi.Address.rgBytes[4], btdi.Address.rgBytes[3], btdi.Address.rgBytes[2], btdi.Address.rgBytes[1], btdi.Address.rgBytes[0]);
					info.mMacAddressHex.ToUpper();

					QString unicodeMacAddressHex = QString::fromWCharArray( btdi.szName );
					info.mName = unicodeMacAddressHex.toUtf8().data();

					info.mDeviceClass = btdi.ulClassofDevice;
					info.mIsPaired = btdi.fAuthenticated;
					info.mIsConnected = btdi.fConnected;


					tempStr.Format(" Name = %s\n", info.mName.AsChar());							inOutString += tempStr.AsChar();
					tempStr.Format(" MacAddressHex = %s\n", info.mMacAddressHex.AsChar());		inOutString += tempStr.AsChar();
					tempStr.Format(" DeviceClass = %i\n", info.mDeviceClass);						inOutString += tempStr.AsChar();
					tempStr.Format(" IsPaired = %s\n", info.mIsPaired ? "true" : "false");		inOutString += tempStr.AsChar();
					tempStr.Format(" IsConnected = %s\n", info.mIsConnected ? "true" : "false");	inOutString += tempStr.AsChar();
					
					inOutString += "\n\n";

					numDevices++;

				} while (BluetoothFindNextDevice(hFindDevice, &btdi));

				BluetoothFindDeviceClose(hFindDevice);
			}

		} while (BluetoothFindNextRadio(hFindRadio, &hRadio));

		BluetoothFindRadioClose(hFindRadio);
	}

#endif

	if (numDevices == 0)
	{
		inOutString += "No devices found.";
	}

	return inOutString;
}