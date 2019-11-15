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

#ifndef __NEUROMORE_BRAINQUIRYDEVICE_H
#define __NEUROMORE_BRAINQUIRYDEVICE_H

// include required headers
#include "../../Config.h"
#include "../../BciDevice.h"
#include "../../Core/Thread.h"
#include "../../Core/AttributeString.h"

#ifdef INCLUDE_DEVICE_BRAINQUIRY

// the Brainquiry neuro headset class
class ENGINE_API BrainquiryDevice : public BciDevice
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_BRAINQUIRY_PET4 };

		// constructor & destructor
		BrainquiryDevice(DeviceDriver* driver = NULL);
		~BrainquiryDevice();

		Device* Clone() override								{ return new BrainquiryDevice(); }

		// overloaded
		uint32 GetType() const override							{ return TYPE_ID; }
		double GetSampleRate() const override					{ return 256; }
		const char* GetHardwareName() const override			{ return "Brainquiry PET 4.0"; }
		static const char* GetRuleName()						{ return "DEVICE_Brainquiry"; }
		const char* GetUuid() const override					{ return "f9a3a9e6-c4e9-11e5-9912-ba0be0483c18"; }
		const char* GetTypeName() const override				{ return "pet4"; }
		bool IsWireless() const override						{ return true; }
		bool HasWirelessIndicator() const override				{ return false; }
		bool HasEegContactQualityIndicator() override			{ return false; }

		double GetLatency() const override						{ return 0.1; }
		double GetExpectedJitter() const override				{ return 0.2; }

		void CreateElectrodes() override;

	private:
};

#endif

#endif
