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
#include <Engine/Precompiled.h>

#include "TobiiEyeXNode.h"


#ifdef INCLUDE_DEVICE_TOBIIEYEX

using namespace Core;

TobiiEyeXNode::~TobiiEyeXNode()
{
}

void TobiiEyeXNode::Init()
{
	DeviceInputNode::Init();

	// hide unused attributes
	GetAttributeSettings(ATTRIB_RAWOUTPUT)->SetVisible(false);
	GetAttributeSettings(ATTRIB_UPLOAD)->SetVisible(false);
}

void TobiiEyeXNode::ReInit(const Core::Time& elapsed, const Core::Time& delta)
{
	// Note: Skip InputNode::ReInit()
	SPNode::ReInit(elapsed, delta);
	// get device and stop node if there is not a running one
	mCurrentDevice = NULL;
	Device* device = FindDevice();
	if (device != NULL && device->IsEnabled())
	{
		TobiiEyeXDevice* tobiiEyeXDevice = static_cast<TobiiEyeXDevice*>(device);

		// check if we are connected now
		if (tobiiEyeXDevice->IsConnected() == true)
			mCurrentDevice = tobiiEyeXDevice;
		else
		{
			SetError(ERROR_DEVICE_NOT_FOUND, "Device not connected");
		}
	}

	if (mCurrentDevice == NULL)
		mIsInitialized = false;

	PostReInit(elapsed, delta);
}

void TobiiEyeXNode::OnAttributesChanged()
{
	DeviceInputNode::OnAttributesChanged();
}

#endif
