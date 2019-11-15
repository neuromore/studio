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

#ifndef __NEUROMORE_BCIDEVICE_H
#define __NEUROMORE_BCIDEVICE_H

// include required headers
#include "Config.h"
#include "Core/StandardHeaders.h"
#include "Core/String.h"
#include "Device.h"
#include "EEGElectrodes.h"


// the BCI device base class
class ENGINE_API BciDevice : public Device
{
	public:
		enum { BASE_TYPE_ID = 0x02 };

		// constructor & destructor
		BciDevice(DeviceDriver* driver = NULL);
		virtual ~BciDevice();
		
		uint32 GetBaseType() const override final							{ return BASE_TYPE_ID; }

		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		virtual void Configure(const DeviceConfig& config) override;

		// neuro sensor helpers
		uint32 GetNumNeuroSensors() const									{ return mNeuroSensors.Size(); }
		Sensor* GetNeuroSensor(uint32 index) const							{ return mNeuroSensors[index]; }
		Sensor* FindNeuroSensorByName(const char* electrodeName) const;
		virtual bool ShowNeuroChannels() const								{ return mNeuroSensors.Size() > 1; }
		
		// true if device provides the contact quality for a sensor
		virtual bool HasEegContactQualityIndicator()						{ return false; }

		// get/set position of electrodes
		const Core::Array<EEGElectrodes::Electrode>& GetElectrodes() const	{ return mElectrodes; }
		EEGElectrodes::Electrode GetElectrodePosition(uint32 neuroSensorIndex) const;
		void SetElectrodePosition(uint32 neuroSensorIndex, EEGElectrodes::Electrode electrode);

	
		// overload these
		virtual double GetSampleRate() const = 0;			// sample rate of the neuro sensors (same for all)
		virtual void CreateSensors() override;				// note: in the implementation, call the baseclass method before creating the other device sensors
		virtual void CreateElectrodes() = 0;				// add default positions of the neuro sensors to the mElectrodes array. Always implement this even if there are no default positions.

		// BCI impedance test
		virtual bool HasImpedanceTestMode() const							{ return false; }
		virtual double GetImpedance(uint32 neuroSensorIndex)				{ return 0.0; }

	protected:
		Core::Array<Sensor*>					mNeuroSensors;		// neurosensor = EEG electrode
		Core::Array<EEGElectrodes::Electrode>	mElectrodes;		// electrode positions for the neuro sensors
		Core::Array<EEGElectrodes::Electrode>	mDefaultElectrodes;	// default (original) electrode positions for the neuro sensors given by the device implementation

	private:
		// electrode configuration (subset of device definitions)
		void ConfigureElectrodes(const Core::Json& jsonParser);
		void WriteElectrodeConfig(Core::Json& jsonParser) const;
};


#endif
