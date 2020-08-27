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

#ifndef __NEUROMORE_DEVICEINPUTNODE_H
#define __NEUROMORE_DEVICEINPUTNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../BciDevice.h"
#include "InputNode.h"


class ENGINE_API DeviceInputNode : public InputNode
{
	public:
		enum { NODE_TYPE			= 0x006 };

		enum 
		{ 
			ATTRIB_DEVICEINDEX = NUM_INPUTNODEATTRIBUTES,
			ATTRIB_RAWOUTPUT,
			NUM_DEVICENODEATTRIBUTES
		};

		enum EError
		{
			ERROR_DEVICE_NOT_FOUND	= GraphObjectError::ERROR_RUNTIME | 0x01,
		};

		enum EWarning
		{
			WARNING_DEVICE_DEVICE_BATTERY_LOW = GraphObjectWarning::WARNING_RUNTIME & 0x01,
		};

		// constructor & destructor
		DeviceInputNode(Graph* graph, uint32 deviceType);
		virtual ~DeviceInputNode();

		// type management
		virtual uint32 GetNodeType() const override						{ return NODE_TYPE; }

		// most devices are HW devices and thus time dependent
		virtual bool IsTimeIndependent() const override					{ return false; }

		uint32 GetPaletteCategory() const override						{ return CATEGORY_SENSORS; }

		// initialize & update
		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		void RegisterDeviceSensorsAsPorts(const Device* device);
		virtual Device* FindDevice();

		//Device* GetDevice()												{ return mCurrentDevice; }

		uint32 GetNumSensors() const override;
		Sensor* GetSensor(uint32 index) override;
		virtual bool IsSensorUsed(uint32 index);

		uint32 GetDeviceType() const { return mDeviceType; }

	protected:
		uint32	mDeviceType;
		Device* mCurrentDevice;

		bool	mRawOutputEnabled; // for detecting attribute changes

		virtual void UpdateOutputChannelColors() override;

};


#endif
