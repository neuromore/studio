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

#ifndef __NEUROMORE_GRAPHSETTINGS_H
#define __NEUROMORE_GRAPHSETTINGS_H

// include the required headers
#include "../Config.h"
#include "../Core/Json.h"
#include "../CloudParameters.h"
#include "GraphObject.h"


class ENGINE_API GraphSettings
{
	public:
		// Note: uses default constructor
		
		void Clear()										{ mSettings.Clear(); }
		
		uint32 Size() const									{ return mSettings.Size(); }
		
		bool HasObjectUuid(uint32 index) const				{ return mSettings[index].mObjectUuid.GetLength() > 0; }
		const char* GetObjectUUid(uint32 index) const		{ return mSettings[index].mObjectUuid.AsChar(); }

		bool HasObjectName(uint32 index) const				{ return mSettings[index].mObjectName.GetLength() > 0; }
		const char* GetObjectName(uint32 index) const		{ return mSettings[index].mObjectName.AsChar(); }
	
		bool HasObjectType(uint32 index) const				{ return mSettings[index].mObjectType != 0; }
		uint32 GetObjectType(uint32 index) const			{ return mSettings[index].mObjectType; }
		
		const char* GetName(uint32 index) const				{ return mSettings[index].mName.AsChar(); }
		const char* GetValue(uint32 index) const			{ return mSettings[index].mValue.AsChar(); }

		void Add(GraphObject* object, const char* settingsName, const char* settingsValue);
		void Add(const GraphSettings& settings)				{ mSettings.Add(settings.mSettings); }

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Serialization
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		virtual void Save(Core::Json::Item& item);
		virtual bool Load(const Core::Json::Item& item);

		bool LoadCloudParameter(const CloudParameters& parameters, const char* uuid, const char* parameterName);

	protected:
	
		struct GraphObjectSettings
		{
			Core::String mObjectUuid;
			uint32 mObjectType;
			Core::String mObjectName;
			Core::String mName;
			Core::String mValue;
		};

		Core::Array<GraphObjectSettings> mSettings;

};


#endif
