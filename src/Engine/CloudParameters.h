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

#ifndef __NEUROMORE_CLOUDPARAMETERS_H
#define __NEUROMORE_CLOUDPARAMETERS_H

// include required headers
#include "Config.h"
#include "Core/StandardHeaders.h"
#include "Core/LogManager.h"
#include "Core/Json.h"
#include "Core/Array.h"
#include "Core/Time.h"
#include "User.h"

class Experience;
class Classifier;
class StateMachine;

class ENGINE_API CloudParameters
{
	public:
		CloudParameters(const User* user = NULL) : mUser(user)	{}
		virtual~CloudParameters()								{}

		// fixed cloud parameter names
		static const char* GetClassifierSettingsParameterName() 	{ return "classifierSettings"; }		// typeID is a classifier or state machine
		static const char* GetStateMachineSettingsParameterName() 	{ return "stateMachineSettings"; }		// typeID is a classifier or state machine

		// TODO more 'special' studio parameter names (for experiences or user parameters) go here
		
		
		// add values; itemId and timestamp is optional 
		//  1) no itemId makes it a user parameter 
		//  2) no timestamp makes it a history-free parameter that always overwrites the old value
		template <class T> void Add (const char* name, const T& value, Core::Time time = 0, const char* itemId = "")	
		{
			Parameter& param = mParameters.AddEmpty();
			
			param.mUserId = (mUser == NULL ? "" : mUser->GetId());
			param.mItemId = itemId;
			param.mTimeStamp = time;
			param.mName = name;
			param.SetValue<T>(value);
		}


	public:

		//
		//  Parameter
		//

		// types
		enum EType : int32
		{
			TYPE_INT		= 0,		// int32
			TYPE_FLOAT		= 1,		// double
			TYPE_STRING		= 2,		// Core::String
			TYPE_JSON		= 3,		// Core::Json
			TYPE_JSONARRAY  = 4,		// Core::Array<Json>
			NUM_TYPES		= 5,

			TYPE_UNKNOWN	= CORE_INT32_MAX,
		};

		// the Parameter value as represented in the database
		class Parameter
		{
			public:
				// simple generic parameter value base class
				class ValueBase  
				{
					public:
						virtual ~ValueBase()						{}
						virtual bool IsValid() const				{ return false; }
						virtual ValueBase* Copy() const		= 0;//	{ return new ValueBase(); }
				};

				// simple generic parameter value class
				template<class T> class Value : public ValueBase
				{
					public:
						Value(const T& value) : mValue(value)	{}
						virtual ~Value()						{}

						const T& GetValue() const				{ return mValue; }
						void SetValue(const T& value)			{ mValue = value; }
						bool IsValid() const override final		{ return true; }

						ValueBase* Copy() const	override		{ return new Value<T>(mValue); }

					private: 
						T mValue;
				};

				template <class T> EType GetType() const;		/* specialized for each type */


				
				// helper for getting and setting the value (use Ptr method only for JSON)
				template <class T> void SetValue(const T& value);

				bool HasValue() const											{ return mValue != NULL; }
				
				// TODO interesting problem: make following line work (allow only const ref access to values)  -> requires GetRootItem() to be const which is impossible due to the nature of the Json wrapper
				template <class T> const T& GetValue() const					{ return dynamic_cast<const Value<T>*>(mValue)->GetValue(); }

				// constructor
				Parameter() : mTypeId(CORE_INVALIDINDEX32), mValue(NULL)		{ }
				Parameter(const Parameter& other)								{ *this = other; }
				virtual ~Parameter()											{ delete mValue; mValue = NULL;}

				inline const Parameter& operator=(const Parameter& other) 
				{
					mUserId = other.mUserId;
					mItemId = other.mItemId;
					mTimeStamp = other.mTimeStamp;
					mName = other.mName;
					mTypeId = other.mTypeId;
					mValue = other.mValue->Copy();

					return *this;
				}

				// the fields contained in the database
				Core::String		mUserId;
				Core::String		mItemId;
				Core::Time			mTimeStamp;
				Core::String		mName;
				uint32				mTypeId;
				ValueBase*			mValue;
		};

		//
		// Cloud Parameter Methods
		// 
		
		const Core::Array<Parameter>& GetParameters() const		{ return mParameters; }

		Core::Array<const Parameter*> Find(const char* name, const char* itemId, bool requireItemId = true) const;
		Core::Array<const Parameter*> Find(const char* itemId) const;

		// Input: init from json, e.g. find parameters reply
		bool Load(const Core::Json::Item& arrayItem);

		// Input: add individual parameter objects to the parameter array item
		static uint32 CreateFindParametersJson(const Experience& experience, Core::Json::Item& arrayItem);
		static uint32 CreateFindParametersJson(const Classifier& classifier, Core::Json::Item& arrayItem);

		// Output: create set requests json
		void CreateSetRequestJson(Core::Json::Item& rootItem) const;

		void Log() const;

	private:
		// add parameter item to json
		static bool AddParameterValueItem(const char* name, const CloudParameters::Parameter& parameter, Core::Json::Item& rootItem);

		const User* mUser;						// allowed user (can be null to allow all users)
		Core::Array<Parameter> mParameters;		// the parameter array

};


template <> inline CloudParameters::EType CloudParameters::Parameter::GetType<int>() const							{ return TYPE_INT; }
template <> inline CloudParameters::EType CloudParameters::Parameter::GetType<double>() const						{ return TYPE_FLOAT; }
template <> inline CloudParameters::EType CloudParameters::Parameter::GetType<Core::String>() const				{ return TYPE_STRING; }
template <> inline CloudParameters::EType CloudParameters::Parameter::GetType<Core::Json>() const					{ return TYPE_JSON; }
//template <>  EType GetType<Core::Array<Core::Json>>() const												{ return TYPE_JSONARRAY; }


#endif
