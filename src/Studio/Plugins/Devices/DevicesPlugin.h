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

#ifndef __NEUROMORE_DEVICESPLUGIN_H
#define __NEUROMORE_DEVICESPLUGIN_H

// include required headers
#include "../../Config.h"
#include <Core/EventHandler.h>
#include <PluginSystem/Plugin.h>
#include "DevicesWidget.h"
#include "NoDeviceWidget.h"


// lists active devices
class DevicesPlugin : public Plugin, public Core::EventHandler
{
	Q_OBJECT
	public:
		DevicesPlugin();
		virtual ~DevicesPlugin();

		// overloaded
		const char* GetName() const override											{ return "Devices"; }
		static const char* GetStaticTypeUuid()											{ return "c38f75e1-7eca-11e4-b4a9-0800200c9a66"; }
		const char* GetRuleName() const override										{ return "PLUGIN_Devices"; }
		Plugin* Clone() override														{ return new DevicesPlugin(); }
		bool NeedRegularUpdateInterface() override										{ return true; }

		// overloaded main init function
		bool Init() override;

		// reinit in case devices have changed
		void ReInit();
		
		// called regularily to update the information on the interface
		void UpdateInterface() override;

		// event callbacks
		void OnDeviceAdded(Device* device) override;
		void OnRemoveDevice(Device* device) override;

	private:
		DevicesWidget*		mDevicesWidget;
		NoDeviceWidget*		mNoDeviceWidget;
};


#endif
