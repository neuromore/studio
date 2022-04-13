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

// include precompiled header
#include <Engine/Precompiled.h>

// include required headers
#include "GraphObject.h"
#include "../Core/AttributeSettings.h"
#include "../Core/Counter.h"
#include "../EngineManager.h"


using namespace Core;

// default constructor
GraphObject::GraphObject(Graph* parentGraph) : AttributeSet()
{
	mParentGraph	= parentGraph;
	mID				= CORE_COUNTER.Next();
	mIsDirty		= false;
	mIsEnabled		= true;
	mIsLocked		= false;

	// allow everything on default, else we can't add nodes and connections at load time
	mCreud = Creud(true, true, true, true, true);

	SetUuid( GenerateRandomUuid() );
}


// destructor
GraphObject::~GraphObject()
{
}


// set the name
void GraphObject::SetName(const char* name)
{ 
	if (mName.Compare(name) != 0)
		mIsDirty = true;

	mName = name;

	// NOTE: this crashes the studio more often than not:
	//EMIT_EVENT( OnGraphModified(mParentGraph, this) );
}


// get the name
const char* GraphObject::GetName() const
{ 
	return mName.AsChar();
}


// get the name as string object
const String& GraphObject::GetNameString() const
{
	return mName;
}


// set the uuid
void GraphObject::SetUuid(const char* uuid)
{ 
	mUuid = uuid;
}


// get the uuid
const char* GraphObject::GetUuid() const
{ 
	return mUuid.AsChar();
}


// get the uuid as string object
const String& GraphObject::GetUuidString() const
{
	return mUuid;
}


void GraphObject::SetEnabled(bool enable)
{
	if (mIsEnabled != enable)
		mIsDirty = true;

	mIsEnabled = enable;

	// NOTE: no graph even neede yet, all use cases prefer node start/stop events
	//EMIT_EVENT( OnGraphModified(mParentGraph, this) );
}


void GraphObject::SetLocked(bool locked)
{
	if (mIsLocked != locked)
		mIsDirty = true;

	mIsLocked = locked;

	EMIT_EVENT( OnGraphModified(mParentGraph, this) );
}

bool GraphObject::OnAttributeChanged(Attribute* attribute)
{
	// remembers attribute changes 
	const uint32 numAttributes = GetNumAttributes();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		if (GetAttributeValue(i) != attribute)
			continue;

		AttributeSettings* settings = GetAttributeSettings(i);
		const char* internalName = settings->GetInternalName();

		// already containe in modified attributes list?
		const uint32 modifiedIndex = mChangedAttributes.FindAttributeIndexByInternalName(internalName);
		if (modifiedIndex == CORE_INVALIDINDEX32)
		{
			// add attribute to modified list
			AttributeSettings* settingsCopy = settings->Clone();
			Attribute* attributeCopy = attribute->Clone();
			mChangedAttributes.AddAttribute(settingsCopy, attributeCopy);
		}
		else
		{
			// update attribute
			mChangedAttributes.GetAttributeValue(modifiedIndex)->InitFrom(attribute);
		}
	}
	
	return true;
}


bool GraphObject::HasError(uint32 errorId) const
{ 
	if (errorId == 0)
		return mErrors.Size() > 0; 
	else
	{
		uint32 index = FindErrorByID(errorId);
		return (index != CORE_INVALIDINDEX32);
	}
}


void GraphObject::SetError(uint32 errorID, const char* message)
{
	// check if we already have an error of that ID
	uint32 index = FindErrorByID(errorID);
	if (index != CORE_INVALIDINDEX32)
		return;


	// add error
	mErrors.AddEmpty();
	mErrors.GetLast().mObjectType = GetType();
	mErrors.GetLast().mErrorID = errorID;
	mErrors.GetLast().mMessage = message;
}


void GraphObject::ClearError(uint32 errorID)
{
	// find error in list
	uint32 index = FindErrorByID(errorID);
	if (index == CORE_INVALIDINDEX32)
		return;

	// delete it
	mErrors.Remove(index);
}


Array<GraphObject::GraphObjectError*>& GraphObject::CollectErrors(Array<GraphObject::GraphObjectError*>& outList)
{
	const uint32 numErrors = mErrors.Size();
	for (uint32 i = 0; i < numErrors; ++i)
		outList.Add(&mErrors[i]);

	return outList;
}


uint32 GraphObject::FindErrorByID(uint32 errorID) const
{
	const uint32 numErrors = mErrors.Size();
	for (uint32 i = 0; i < numErrors; ++i)
	{
		if (mErrors[i].mErrorID == errorID)
			return i;
	}

	return CORE_INVALIDINDEX32;
}


void GraphObject::SetWarning(uint32 warningID, const char* message)
{
	// check if we already have an error of that ID
	uint32 index = FindWarningByID(warningID);
	if (index != CORE_INVALIDINDEX32)
		return;

	// add to list
	mWarnings.AddEmpty();
	mWarnings.GetLast().mObjectType = GetType();
	mWarnings.GetLast().mWarningID = warningID;
	mWarnings.GetLast().mMessage = message;
}


// remove warning of certain type (so the object can clear it if the problem is gone)
void GraphObject::ClearWarning(uint32 warningID)
{
	// check if we already have an error of that ID
	uint32 index = FindWarningByID(warningID);
	if (index == CORE_INVALIDINDEX32)
		return;

	mWarnings.Remove(index);
	return;
}


Array<GraphObject::GraphObjectWarning*>& GraphObject::CollectWarnings(Array<GraphObject::GraphObjectWarning*>& outList) 
{
	const uint32 numWarnings = mWarnings.Size();
	for (uint32 i = 0; i < numWarnings; ++i)
		outList.Add(&mWarnings[i]);

	return outList;
}


// look for warning of specified type
uint32 GraphObject::FindWarningByID(uint32 warningID) const
{
	const uint32 numWarnings = mWarnings.Size();
	for (uint32 i = 0; i < numWarnings; ++i)
	{
		if (mWarnings[i].mWarningID == warningID)
			return i;
	}

	return CORE_INVALIDINDEX32;
}


// add deprecated/unstable to warnings
void GraphObject::AddDefaultWarnings()
{
	// unstable objects
	if (IsUnstable() == true)
		SetWarning(GraphObjectWarning::WARNING_UNSTABLE_OBJECT, "Object is unstable.");

	// deprecated objects
	if (IsDeprecated() == true)
		SetWarning(GraphObjectWarning::WARNING_DEPRECATED_OBJECT, "Object type is deprecated.");

	// TODO can add more here
}


Json::Item GraphObject::Save(Json& json, Json::Item& item)
{
	// add the node item
	Json::Item objectItem = item.AddObject();
	if (objectItem.IsNull() == true)
		return json.NullItem();

	// graph object properties
	objectItem.AddString( "name", GetName() );
	objectItem.AddString( "uuid", GetUuid() );
	objectItem.AddString( "type", GetTypeUuid() );

	// skip saving default values if we can
	if (IsEnabled() == false)
		objectItem.AddBool( "enabled", IsEnabled() );
	if (IsLocked() == true)
		objectItem.AddBool( "locked", IsLocked() );

	// add attributes from the attribute set
	Write(json, objectItem);

	return objectItem;
}


bool GraphObject::Load(const Json& json, const Json::Item& item)
{
	SetEmitEvents(false);

	// node name
	Json::Item nameItem = item.Find("name");
	if (nameItem.IsString() == true)
		SetName( nameItem.GetString() );

	// uuid
	Json::Item uuidItem = item.Find("uuid");
	if (uuidItem.IsString() == true)
		SetUuid( uuidItem.GetString() );

	// enabled?
	Json::Item enabledItem = item.Find("enabled");
	if (enabledItem.IsBool() == true)
		SetEnabled( enabledItem.GetBool() );
	else
		SetEnabled(true); // default: enabled 

	// locked?
	Json::Item lockedItem = item.Find("locked");
	if (lockedItem.IsBool() == true)
		SetLocked( lockedItem.GetBool() );
	else
		SetLocked(false);	// default: unlocked

	// read the attributes
	if (Read(json, item, true) == false)
	{
		LogError("Node::Load(): Error reading graph object attributes in '%s'.", GetName());
		
		// Note: attributes don't change, i think. 
		// TODO think about this more, it should change if name changes because some nodes use it?

		return false;
	}

	// update attributes
	OnAttributesChanged();

	mIsDirty = false;

	SetEmitEvents(true);

	return true;
}
