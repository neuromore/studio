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

#ifndef __NEUROMORE_VERSUSHEADSET_H
#define __NEUROMORE_VERSUSHEADSET_H

// include required headers
#include "../../Config.h"
#include "../../BciDevice.h"
#include "../../Core/AttributeString.h"

#ifdef INCLUDE_DEVICE_SENSELABS_VERSUS

// forward declaration
class VersusSystem;

// the SenseLabs Versus headset class
class ENGINE_API VersusDevice : public BciDevice
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_SENSELABS_VERSUS };
		
		// constructors & destructor
		VersusDevice(DeviceDriver* driver = NULL);
		virtual ~VersusDevice();

		Device* Clone() override								{ return new VersusDevice(); }

		// information
		uint32 GetType() const override							{ return TYPE_ID; }
		double GetSampleRate() const override					{ return 250; }
		const char* GetHardwareName() const override			{ return "Versus"; }
		const char* GetUuid() const override					{ return "246f0041-6662-11e4-9803-0800200c9a66"; }
		const char* GetTypeName() const override				{ return "versus"; }
		bool IsWireless() const override						{ return true; }
		double GetLatency() const override						{ return 0.1; }

		static const char* GetRuleName()						{ return "DEVICE_SenselabsVersus"; }

		void CreateElectrodes() override;


};

#endif

#endif
