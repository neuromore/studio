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

#ifndef __NEUROMORE_DEVICEINVENTORYTYPEIDS_H
#define __NEUROMORE_DEVICEINVENTORYTYPEIDS_H

// the device type IDs (separated from inventory class so we can expose them in the engine header)
class ENGINE_API DeviceTypeIDs
{
	public:

		// central list of all device IDs. Used by Devices and Nodes.
		enum EDeviceTypeID
		{
			INVALID_DEVICE_TYPEID					= 0,		// zero denotes invalid device type
			DEVICE_TYPEID_TEST						= 0x0023,	// 00XX = Test devices
			DEVICE_TYPEID_INTERAXON_MUSE			= 0x0101,	// 01XX = InteraXon
			DEVICE_TYPEID_NEUROSKY_MINDWAVE			= 0x0201,   // 02XX = Neurosky
			DEVICE_TYPEID_EMOTIV_EPOC				= 0x0301,	// 03XX = Emotiv
			DEVICE_TYPEID_EMOTIV_INSIGHT			= 0x0302,
			DEVICE_TYPEID_OPENBCI					= 0x0401,	// 04XX = OpenBCI
			DEVICE_TYPEID_OPENBCI_DAISY				= 0x0402,
			DEVICE_TYPEID_OPENBCI_GANGLION			= 0x0403,
			DEVICE_TYPEID_MITSAR_EEG201M			= 0x0501,	// 05XX = Mitsar
			DEVICE_TYPEID_ABM_BALERT_X24			= 0x0601,	// 06XX = ABM
			DEVICE_TYPEID_ABM_BALERT_X10			= 0x0602,
			DEVICE_TYPEID_ABM_BALERT_X4				= 0x0603,
			DEVICE_TYPEID_SENSELABS_VERSUS			= 0x0701,	// 07XX = Senselabs
			DEVICE_TYPEID_BRAINQUIRY_PET2			= 0x0801,	// 08XX = Brainquiry
			DEVICE_TYPEID_BRAINQUIRY_PET4			= 0x0802,	//
			DEVICE_TYPEID_MINDFIELD_ESENSEGSR		= 0x0901,	// 09XX = Mindfield
			DEVICE_TYPEID_BRAINPRODUCTS_ACTICHAMP	= 0x0911,   // 091X = BrainProducts
			DEVICE_TYPEID_EEMAGINE_8CH				= 0x0921,   // 092X = eemagine
			DEVICE_TYPEID_EEMAGINE_16CH				= 0x0922,
			DEVICE_TYPEID_EEMAGINE_32CH				= 0x0923,
			DEVICE_TYPEID_EEMAGINE_64CH				= 0x0924,
			DEVICE_TYPEID_NEUROSITY_NOTION			= 0x0931,	// 093X = neurosity
			DEVICE_TYPEID_BRAINMASTER_DISCOVERY20	= 0x0941,// 094X = BrainMaster
			DEVICE_TYPEID_NEUROSITY_CROWN			= 0x0951,	// 095X = neurosity crown
			DEVICE_TYPEID_BRAINFLOW					= 0x1001,   // 10XX = BrainFlow
			DEVICE_TYPEID_BRAINFLOW_CYTON			= 0x1002,

			// A0XX = Generic Vital Sensor (Sub)Devices
			DEVICE_TYPEID_GENERIC_HEARTRATE			= 0xA001,	// Heartrate sensors (Pulse + RR)
			DEVICE_TYPEID_GENERIC_RESPIRATION		= 0xA002,	// Respiration sensors (rate, volume)
			DEVICE_TYPEID_GENERIC_THERMOMETER		= 0xA003,	// Temperature sensors (single channel w/ Degree Celcius)
			DEVICE_TYPEID_GENERIC_BAROMETER			= 0xA004,	// Air pressure sensor // TODO define unit

			// B0XX = Generic Platform (Sub)Devices
			DEVICE_TYPEID_GENERIC_ACCELEROMETER		= 0xB001,	// 3axis Accelerometer
			DEVICE_TYPEID_GENERIC_GYROSCOPE			= 0xB002,	// 3axis Gyroscope 	
			DEVICE_TYPEID_GENERIC_AMBIENT_LIGHT		= 0xB003,	// ambient light in lux (//TODO or lumen??)
			DEVICE_TYPEID_GENERIC_PROXIMITY			= 0xB004,	// proximity detector 0.0 .. 1.0
			DEVICE_TYPEID_GENERIC_VIBRATOR			= 0xB005,	// single channel output (probably bool)
			DEVICE_TYPEID_GENERIC_GEOPOSITION		= 0xB006,	// lat, long, altitude
			DEVICE_TYPEID_GENERIC_GAMEPAD			= 0xB007,	// gamepad

			DEVICE_TYPEID_GENERIC_MICROPHONE		= 0xC001,	// one channel audio input	
			DEVICE_TYPEID_GENERIC_AUDIO_IN			= 0xC002,	// two channel audio input	
			DEVICE_TYPEID_GENERIC_AUDIO_OUT			= 0xC003,	// two channel audio output	

			DEVICE_TYPEID_GENERIC_CAMERA			= 0xD001,	// TODO probably need several types of camera

			// eyetracker
			DEVICE_TYPEID_TOBII_EYEX				= 0xE001

		};


		enum EDriverTypeID
		{
			DRIVER_TYPEID_TEST						= 0x0000,	// 00XX = Test devices
			DRIVER_TYPEID_NEUROSKY					= 0x0200,   // 02XX = Neurosky
			DRIVER_TYPEID_EMOTIV					= 0x0300,	// 03XX = Emotiv
			DRIVER_TYPEID_OPENBCI					= 0x0400,	// 04XX = OpenBCI
			DRIVER_TYPEID_MITSAR					= 0x0500,	// 05XX = Mitsar
			DRIVER_TYPEID_ABM_BALERT				= 0x0600,	// 06XX = ABM
			DRIVER_TYPEID_SENSELABS					= 0x0700,	// 07XX = Senselabs
			DRIVER_TYPEID_BRAINQUIRY				= 0x0800,	// 08XX = Brainquiry
			DRIVER_TYPEID_BRAINPRODUCTS				= 0x0900,   // 09xx = BrainProducts
			DRIVER_TYPEID_EEMAGINE					= 0x0A00,	// 0AXX = eemagine
			DRIVER_TYPEID_BRAINMASTER				= 0x0B00,	// 0BXX = BrainMaster
			DRIVER_TYPEID_BRAINFLOW					= 0x1000,	// 1000 = BrainFlow Driver
			DRIVER_TYPEID_AUDIO						= 0xC000,	// C000 = Audio Driver
			DRIVER_TYPEID_CAMERA					= 0xD000,	// D000 = Camera Driver
			DRIVER_TYPEID_BLUETOOTH					= 0xE000,	// E000 = Bluetooth Driver
		}; 
};

#endif
