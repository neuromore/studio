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

#ifndef __NEUROMORE_ESENSESKINRESPONSEDEVICE_H
#define __NEUROMORE_ESENSESKINRESPONSEDEVICE_H

// include required headers
#include "../../Config.h"
#include "../../Device.h"

#ifdef INCLUDE_DEVICE_ESENSESKINRESPONSE


class ENGINE_API eSenseSkinResponseDevice : public Device
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_MINDFIELD_ESENSEGSR };

		// constructor & destructor
		eSenseSkinResponseDevice(DeviceDriver* driver=NULL);
		~eSenseSkinResponseDevice();

		Device* Clone() override								{ return new eSenseSkinResponseDevice(); }

		// overloaded
		uint32 GetType() const override							{ return TYPE_ID; }
		const char* GetHardwareName() const override			{ return "eSense GSR"; }
		static const char* GetRuleName()						{ return "DEVICE_eSenseSkinResponse"; }
		const char* GetUuid() const override					{ return "f2b470e6-ffc9-11e5-86aa-5e5517507c66"; }
		const char* GetTypeName() const override				{ return "esensegsr"; }
		double GetTimeoutLimit() const override					{ return 3.0; }
		double GetLatency() const override						{ return 0.2; }
		double GetExpectedJitter() const override				{ return 0.2; }
		bool IsWireless() const override						{ return false; }
		bool HasWirelessIndicator() const override				{ return false; }

		void CreateSensors() override;

	protected:
		Sensor* mGsrSensor;
};

#endif

#endif
