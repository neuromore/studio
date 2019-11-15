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

#ifndef __NEUROMORE_TOBIISYSTEM_H
#define __NEUROMORE_TOBIISYSTEM_H

// include headers
#include <DeviceDriver.h>
#include <EngineManager.h>
#include <Devices/EyeX/TobiiEyeXDevice.h>
#include <Devices/EyeX/TobiiEyeXNode.h>
#include <Core/LogManager.h>
#include <QMessageBox>
#include <Core/LogManager.h>

#ifdef INCLUDE_DEVICE_TOBIIEYEX

// include EyeX SDK
#include <eyex/EyeX.h>

class TobiiEyeXDriver : public DeviceDriver, Core::EventHandler
{
	public:
		enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_TOBII_EYEX };

		// constructor and destructor
		TobiiEyeXDriver();
		virtual ~TobiiEyeXDriver();

		const char* GetName() const override								{ return "Tobii Devices"; }
		uint32		GetType() const override								{ return TYPE_ID; }
		bool		HasAutoDetectionSupport() const override final			{ return true; }
		// initialization
		bool		Init() override;
		bool		InitDevice(TX_CONTEXTHANDLE context);
		// update process
		void		Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		Device* CreateDevice(uint32 deviceTypeID) override				{ return NULL;  }
		
		static void TX_CALLCONVENTION OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam);
		static void TX_CALLCONVENTION OnSnapshotCommitted(TX_CONSTHANDLE asyncData, TX_USERPARAM param);
		static void TX_CALLCONVENTION HandleEvent(TX_CONSTHANDLE asyncData, TX_USERPARAM userParam);
		static void OnEyePositionDataEvent(TX_HANDLE eyePositionDataBehavior);
		static void OnGazeDataEvent(TX_HANDLE gazePositionDataBehavior);

		void DoDataUpdateEyeX(TobiiEyeXDevice* tobiiEyeXDevice, int32 userid);

	private:
		bool			 mSuccess;
		TX_CONTEXTHANDLE mContext;
		TobiiEyeXDevice* mTobiiEyeXDevice;
};

#endif
#endif
