/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_BLUETOOTHHELPERS_H
#define __NEUROMORE_BLUETOOTHHELPERS_H

// include required headers
#include "../QtBaseConfig.h"

// represents a single serial port
class QTBASE_API BluetoothHelpers
{
	public:
		BluetoothHelpers() {}
		virtual ~BluetoothHelpers() {}

		struct DeviceInfo
		{
			uint64		 mMacAddress;
			Core::String mMacAddressHex;
			Core::String mName;
			uint32		 mDeviceClass;
			bool		 mIsConnected;
			bool		 mIsPaired;
		};

		// return a list of all (paired) devices that either contain or match the name
		static Core::Array<DeviceInfo> FindDevicesByName(const char* name, bool exactMatch=true);
		static Core::Array<DeviceInfo> FindDevicesByName(const Core::Array<Core::String>& names, bool exactMatch=true);

		// find the first SPP serial port of a BT device (if it has one; if not it returns an empty string)
		static Core::String FindSerialPort(const DeviceInfo& device);
		static Core::Array<Core::String> FindSerialPorts(const DeviceInfo& device);

		static Core::String& LogDevicesToString(Core::String& inOutString);

};


#endif
