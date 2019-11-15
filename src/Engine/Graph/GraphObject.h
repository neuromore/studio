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

#ifndef __NEUROMORE_GRAPHOBJECT_H
#define __NEUROMORE_GRAPHOBJECT_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/String.h"
#include "../Core/Time.h"
#include "../Core/Attribute.h"
#include "../Core/AttributeSet.h"
#include "../Core/AttributeSettings.h"
#include "../Core/AttributeBool.h"
#include "../Core/AttributeInt32.h"
#include "../Core/AttributeFloat.h"
#include "../Core/AttributeString.h"
#include "../Core/AttributeStringArray.h"
#include "../Core/AttributeColor.h"
#include "../Core/Color.h"
#include "../Core/EventSource.h"
#include "../Creud.h"

class Graph;

class ENGINE_API GraphObject : public Core::AttributeSet, public Core::EventSource
{
	public:
		GraphObject(Graph* parentGraph = NULL);
		virtual ~GraphObject();

		virtual uint32 GetBaseType() const						= 0;
		virtual uint32 GetType() const							= 0;
		virtual const char* GetTypeUuid() const					= 0;
		virtual const char* GetReadableType() const				= 0;

		// parent graph
		void SetParent(Graph* graph)							{ mParentGraph = graph; }
		Graph* GetParent() const								{ return mParentGraph; }

		// clone objects
		virtual GraphObject* Clone(Graph* parentGraph)			{ return NULL; }

		// initialize, reset and update
		virtual void Update(const Core::Time& elapsed, const Core::Time& delta)= 0;
		virtual void Init()										{ AddDefaultWarnings(); }
		virtual void Reset()									{ ClearErrors(); ClearWarnings(); }
		virtual void CollectObjects()							{}

		virtual bool AllowEditing()								{ return mIsLocked == false; }

		// attributes
		virtual void OnAttributesChanged()						{}
		virtual bool OnAttributeChanged(Core::Attribute* attribute);
		virtual bool ContainsAttribute(Core::Attribute* attribute) const				{ return this->HasAttribute(attribute); }

		// enabled/disable object
		bool IsEnabled() const									{ return mIsEnabled; }
		void SetEnabled(bool enable = true);

		// lock/unlock object
		bool IsLocked() const									{ return mIsLocked; }
		void SetLocked(bool lock = false);

		// object name
		virtual void SetName(const char* name);
		const char* GetName() const;
		const Core::String& GetNameString() const;

		// real-time identification (regenerated at object construction time and NOT serialization consistent)
		inline uint32 GetID() const								{ return mID; }

		// lifetime and serialization consistent UUID for object identification
		void SetUuid(const char* uuid);
		const char* GetUuid() const;
		const Core::String& GetUuidString() const;

		void SetCreud(const Creud& creud)						{ mCreud = creud; }
		const Creud& GetCreud() const							{ return mCreud; }

		// deprecated nodes (not to be used anymore)
		virtual bool IsDeprecated() const						{ return false; }

		// unstable nodes (not to be used in production)
		virtual bool IsUnstable() const							{ return false; }

		// errors
		class GraphObjectError
		{
		  public:
			enum EError
			{
				ERROR_UNDEFINED				= 0,
				ERROR_CONFIGURATION			= 0x010000,		// bad configuration (attributes or inputs etc)
				ERROR_RUNTIME				= 0x020000,		// runtime error, like filter bekame unstable or device not found
				ERROR_LOADING				= 0x030000,		// error while load a (child) object from JSON
				ERROR_UNKNOWN_OBJECT		= 0x040000,		// a child object was not registered and not loaded
			};


			uint32			mErrorID;
			uint32			mObjectType;
			Core::String	mMessage;
		};

		virtual bool HasError(uint32 errorId = 0) const;
		virtual uint32 GetNumErrors() const							{ return mErrors.Size(); }
		const GraphObjectError& GetError(uint32 index) const		{ return mErrors[index]; }

		void SetError(uint32 errorID, const char* message);
		void ClearError(uint32 errorID);
		void ClearErrors()											{ mErrors.Clear(); }

		virtual Core::Array<GraphObject::GraphObjectError*>& CollectErrors(Core::Array<GraphObject::GraphObjectError*>& outList);


		//
		// Object Warnings
		//
		class GraphObjectWarning
		{
		  public:
		    enum EWarning
			{
				WARNING_UNDEFINED			= 0,
				WARNING_DEPRECATED_OBJECT	= 0x010000,
				WARNING_UNSTABLE_OBJECT		= 0x020000,	
				WARNING_RUNTIME				= 0x030000,	
				WARNING_CONFIGURATION		= 0x040000,	
				WARNING_CUSTOM				= 0xFF0000,	

			};

			bool IsOfClass(uint32 warningID, EWarning warningClass)		{ return (warningID & 0xFF0000) == (uint32)warningClass; }	

			uint32			mWarningID;		// type of warning (combination of EWarning and low-number ids used locally in the objects)
			uint32			mObjectType;	// object type that caused the warning, for propagation through the graphs
			Core::String	mMessage;
		};

		virtual bool HasWarning() const										{ return mWarnings.Size() > 0; }
		virtual uint32 GetNumWarnings() const								{ return mWarnings.Size(); }
		const GraphObjectWarning& GetWarning(uint32 index) const			{ return mWarnings[index]; }

		void SetWarning(uint32 warningID, const char* message);
		void ClearWarning(uint32 warningID);
		void ClearWarnings()												{ mWarnings.Clear(); AddDefaultWarnings(); }

		virtual Core::Array<GraphObject::GraphObjectWarning*>& CollectWarnings(Core::Array<GraphObject::GraphObjectWarning*>& outList);

		// dirty flag
		virtual bool IsDirty() const										{ return mIsDirty; }
		virtual void SetIsDirty(bool isDirty)								{ mIsDirty = isDirty; }
	
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Serialization
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		// TODO (de)serialize graph object attributes like 'isEnabled'
		virtual Core::Json::Item Save(Core::Json& json, Core::Json::Item& item);
		virtual bool Load(const Core::Json& json, const Core::Json::Item& item);

		// changed attributes
		const Core::AttributeSet& GetChangedAttributes()					{ return mChangedAttributes; }

	protected:
		Graph*								mParentGraph;

		Core::String						mName;
		Core::String						mUuid;

		uint32								mID;

		bool								mIsEnabled;

		bool								mIsLocked;

		bool								mIsDirty;

		Creud								mCreud;

		Core::AttributeSet					mChangedAttributes;

		Core::Array<GraphObjectError>		mErrors;
		Core::Array<GraphObjectWarning>		mWarnings;

		uint32 FindErrorByID(uint32 errorID) const;
		uint32 FindWarningByID(uint32 warningID) const;
		void AddDefaultWarnings();
};


#endif
