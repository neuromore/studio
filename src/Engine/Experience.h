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

#ifndef __NEUROMORE_EXPERIENCE_H
#define __NEUROMORE_EXPERIENCE_H

// include required headers
#include "Config.h"
#include "Core/StandardHeaders.h"
#include "Core/AttributeSet.h"
#include "Graph/Classifier.h"
#include "Graph/StateMachine.h"
#include "Device.h"


// TODO rename Experience to Design

class ENGINE_API Experience : public Core::AttributeSet
{
	public:
		// constructor & destructor
		Experience();
		virtual ~Experience();

		// attributes
		enum
		{
			ATTRIB_CLASSIFIERUUID = 0,
			ATTRIB_STATEMACHINEUUID,
			ATTRIB_LAYOUTUUID,
			ATTRIB_ALLOW_LAYOUTSWITCH,
		};

		// experience UUID
		void SetUuid(const char* uuid)										{ mUuid = uuid; }
		const char* GetUuid() const											{ return mUuid.AsChar(); }
		const Core::String& GetUuidString() const							{ return mUuid; }

		// name
		void SetName(const char* name)										{ mName = name; }
		const char* GetName() const											{ return mName.AsChar(); }
		const Core::String& GetNameString() const							{ return mName; }

		// revision
		void SetRevision(uint32 revision)									{ mRevision = revision; }
		uint32 GetRevision() const											{ return mRevision; }

		// design permissions
		void SetCreud(const Creud& creud)									{ mCreud = creud; }
		const Creud& GetCreud() const										{ return mCreud; }
		
		// dirty flag
		bool IsDirty() const												{ return mIsDirty; }
		void SetIsDirty(bool isDirty = true)								{ mIsDirty = isDirty; }
		
		// classifier
		const char* GetClassifierUuid() const								{ return GetStringAttribute(ATTRIB_CLASSIFIERUUID); }
		void SetClassifierUuid(const char* uuid);
		
		const GraphSettings& GetClassifierSettings() 						{ return mClassifierSettings; }
		void SetClassifierSettings(const GraphSettings& settings)			{ mClassifierSettings = settings; mIsDirty = true; }

		void AttachClassifier(Classifier* classifier);

		// state machine
		void SetStateMachineUuid(const char* uuid);
		const char* GetStateMachineUuid() const								{ return GetStringAttribute(ATTRIB_STATEMACHINEUUID); }

		void AttachStateMachine(StateMachine* stateMachine);
		
		const GraphSettings& GetStateMachineSettings()						{ return mStateMachineSettings; }
		void SetStateMachineSettings(const GraphSettings& settings)			{ mStateMachineSettings = settings; mIsDirty = true; }

		// layout
		void SetLayoutUuid(const char* uuid)								{ SetStringAttribute("layoutUuid", uuid); }
		const char* GetLayoutUuid() const									{ return GetStringAttribute(ATTRIB_LAYOUTUUID); }
	
		// device settings
		void AddDeviceSettings(const Device::DeviceConfig& config)			{ mDeviceSettings.Add(config); }
		Core::Array<Device::DeviceConfig>& GetDeviceSettings()				{ return mDeviceSettings; }

		// remove all settings
		void RemoveSettings();
		bool HasSettings();

		// log xp configuration
		void Log();

		// (de)serialization
		virtual void Save(Core::Json& json, Core::Json::Item& item, bool attachClassifier = false, bool attachStateMachine = false, bool attachLayout = false);
		virtual bool Load(const Core::Json& json, const Core::Json::Item& item);
		
		// cloud parameter
		uint32 LoadCloudParameters(const CloudParameters& parameters);
		uint32 SaveCloudParameters(CloudParameters& parameters) const;

	private:
		Core::String		mName;
		Core::String		mUuid;
		uint32				mRevision;
		Creud				mCreud;
		bool				mIsDirty;

		Core::String		mDescription;

		Classifier*			mAttachedClassifier;
		StateMachine*		mAttachedStateMachine;

		GraphSettings		mClassifierSettings;
		GraphSettings		mStateMachineSettings;
		Core::Array<Device::DeviceConfig>	mDeviceSettings;

		bool LoadGraphSettings(const CloudParameters& parameters, const char* uuid, const char* parameterName, GraphSettings& graphSettings);


};


#endif
