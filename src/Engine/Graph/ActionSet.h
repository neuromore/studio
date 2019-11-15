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

#ifndef __NEUROMORE_ACTIONSET_H
#define __NEUROMORE_ACTIONSET_H

// include the required headers
#include "../Config.h"
#include "../Core/Array.h"
#include "Action.h"


//
class ENGINE_API ActionSet
{
	public:
		// constructor & destructor
		ActionSet();
		ActionSet(const char* internalName);
		virtual ~ActionSet();

		void SetInternalName(const char* internalName)				{ mInternalName = internalName; }
		const char* GetInternalName() const							{ return mInternalName.AsChar(); }

		void Add(Action* action)									{ mActions.Add(action); }
		void RemoveByPointer(Action* action);
		void RemoveByIndex(uint32 index);
		void Clear();

		uint32 FindIndexByPointer(Action* action);

		Action* GetAction(uint32 index) const						{ return mActions[index]; }
		uint32 GetNumActions() const								{ return mActions.Size(); }

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Serialization
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		Core::Json::Item Save(Core::Json& json, Core::Json::Item& item);

	private:
		Core::Array<Action*>	mActions;
		Core::String			mInternalName;
};


#endif
