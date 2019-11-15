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

#ifndef ABMTHREADHANDLER_H
#define ABMTHREADHANDLER_H

// include required headers
#include <DeviceDriver.h>
#include <Devices/ABM/AbmDevices.h>
#include <Core/ThreadHandler.h>

#ifdef INCLUDE_DEVICE_ADVANCEDBRAINMONITORING

// forward declaration
class AbmDriver;

class AbmThreadHandler : public Core::ThreadHandler
{
	public:
		// constructor & destructor
		AbmThreadHandler(AbmDriver* driver);
		virtual ~AbmThreadHandler();

		void Execute();
		void Terminate();

		void DetectDevices();
		void DetectDevicesOnce();

		void DataAcquisition(AbmDevice* device);
		
		enum EMode
		{
			MODE_DETECTDEVICES = 0,
			MODE_DATAACQUISITION,
			MODE_IMPEDANCECHECK,
			MODE_TECHNICALMONITORING
		};

		EMode		mMode;

	private:
		
		AbmDriver*		mDriver;
		bool			mDetectOnce;
		bool			mBreak;



};

#endif

#endif
