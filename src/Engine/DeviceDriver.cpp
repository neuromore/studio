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

// include required files
#include "DeviceDriver.h"
#include "Core/LogManager.h"
#include "Core/Thread.h"


using namespace Core;

// constructor
DeviceDriver::DeviceDriver(const bool enabled, const bool autodetect) : mIsEnabled(enabled), mUseAutoDetection(autodetect)
{
}


// destructor
DeviceDriver::~DeviceDriver()
{
}


void DeviceDriver::SetEnabled(bool enabled)
{
   if (mIsEnabled == enabled)
      return;

   mIsEnabled = enabled;
   StartStopAutoDetection();
}

void DeviceDriver::SetAutoDetectionEnabled(bool enable)
{
   if (mUseAutoDetection == enable)
      return;

   mUseAutoDetection = enable;
   StartStopAutoDetection();
}

void DeviceDriver::StartStopAutoDetection()
{
   // device must have auto detect support
   if (HasAutoDetectionSupport())
   {
      // start
      if (mUseAutoDetection && mIsEnabled && !IsDetectionRunning())
         StartAutoDetection();

      // stop
      else if (!mIsEnabled || !mUseAutoDetection)
         StopAutoDetection();
   }
}
