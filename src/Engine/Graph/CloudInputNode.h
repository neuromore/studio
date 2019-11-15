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

#ifndef __NEUROMORE_CLOUDINPUTNODE_H
#define __NEUROMORE_CLOUDINPUTNODE_H

// include the required headers
#include "../Config.h"
#include "../CloudParameters.h"
#include "../Core/StandardHeaders.h"
#include "../DSP/ClockGenerator.h"
#include "InputNode.h"


class ENGINE_API CloudInputNode : public InputNode
{
	public:
		enum { TYPE_ID = 0x0042 };
		static const char* Uuid () { return "b8f5f7b8-7cb0-11e5-8bcf-feff819cdc9f"; }
		
		enum { OUTPUTPORT_VALUE = 0 };
		enum
		{
			ATTRIB_DEFAULTVALUE = NUM_INPUTNODEATTRIBUTES,
			ATTRIB_SAMPLERATE,
			ATTRIB_STORAGETYPE,
			ATTRIB_REQUESTMODE,
			ATTRIB_TIMERANGETYPE,
			ATTRIB_TIMERANGE,
			ATTRIB_SAMPLERANGE
		};

		enum EStorageType
		{
			STORAGE_USERPARAMETER,
			STORAGE_CLASSIFIERPARAMETER
		};

		enum ERequestMode
		{
			REQUEST_CURRENT,
			REQUEST_COUNT,
			REQUEST_ALL,
			REQUEST_TIMERANGE
		};

		// time range settings
		enum ETimeRangeType
		{
			TIMERANGE_CUSTOM,
			TIMERANGE_ONEHOUR,
			TIMERANGE_ONEDAY,
			TIMERANGE_ONEWEEK,
			TIMERANGE_ONEMONTH,
			TIMERANGE_ONEYEAR
		};

		enum EError
		{
			ERROR_DUPLICATE_NAME	= GraphObjectError::ERROR_CONFIGURATION | 0x01,
		};

		enum EWarning
		{
			WARNING_NOT_UPDATED		= GraphObjectWarning::WARNING_RUNTIME | 0x01,
		};

		// constructor & destructor
		CloudInputNode(Graph* graph);
		~CloudInputNode();

		// initialize & update
		void Init() override; 
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		void OnAttributesChanged() override;

		// node information & helpers
		Core::Color GetColor() const override									{ return Core::RGBA(211, 211, 230); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Cloud Input"; }
		const char* GetRuleName() const override final							{ return "NODE_CloudInput"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_INPUT; }
		GraphObject* Clone(Graph* graph) override								{ CloudInputNode* clone = new CloudInputNode(graph); return clone; }

		uint32 GetNumSensors() const override									{ return 1; }
		Sensor* GetSensor(uint32 index)  override								{ return &mSensor; }
		void GenerateSamples() override;

		void AddInputSample(double value)										{ mInputSamples.AddSample(value); }
		bool HasInputSample() const												{ return mInputSamples.GetNumSamples() > 0; }
		
		EStorageType GetStorageType() const										{ return (EStorageType)GetInt32Attribute(ATTRIB_STORAGETYPE); }
		ERequestMode GetRequestMode() const										{ return mRequestMode; }
		double GetTimeRange() const												{ return mTimeRange; }
		double GetSampleRange() const											{ return mSampleRange; }
		
		// load values from parameters
		uint32 LoadCloudParameters(const CloudParameters& parameters);

	private:
		Sensor			mSensor;		// holds output channel
		ClockGenerator	mClock;			// for creating aequidistant samples in continuous mode

		Channel<double> mInputSamples;	// values from the backend (if updated);

		// request mode settings
		ERequestMode	mRequestMode;
		ETimeRangeType	mTimeRangeType;

		Core::String	mLastName;			// for detecting change in name attribute
		double			mAttribTimeRange;	// for detecting change in time range attribute
		double			mTimeRange;
		double			mSampleRange;
		
		double			CalculateTimeRange(ETimeRangeType timeRangeType, double customTimeRange);

		double			mSampleRate;
		bool			mContinuousOutput;	// true if samplerate > 0

		// check node name for uniqueness in classifier
		bool IsParameterNameUnique(const char* name);
};


#endif
