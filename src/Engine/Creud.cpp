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

// include the required headers
#include "Creud.h"
#include "Core/LogManager.h"


using namespace Core;

// constructor
Creud::Creud()
{
	mCreate		= false;
	mRead		= false;
	mExecute	= false;
	mUpdate		= false;
	mDelete		= false;
}


// constructor
Creud::Creud(bool create, bool read, bool update, bool del, bool execute)
{
	mCreate		= create;
	mRead		= read;
	mExecute	= execute;
	mUpdate		= update;
	mDelete		= del;
}


int Creud::ConvertToInt()
{
	int result = 0;
	result |= mCreate	<< 0;
	result |= mRead		<< 1;
	result |= mExecute	<< 2;
	result |= mUpdate	<< 3;
	result |= mDelete	<< 4;
	
	return result;
}


void Creud::ConvertFromInt(int creud)
{
	mCreate		= (creud & 1<<0);
	mRead		= (creud & 1<<1);
	mExecute	= (creud & 1<<2);
	mUpdate		= (creud & 1<<3);
	mDelete		= (creud & 1<<4); }


bool Creud::operator==(const Creud& creud) const
{
	return (mCreate==creud.mCreate && mRead==creud.mRead && mExecute==creud.mExecute && mUpdate==creud.mUpdate && mDelete==creud.mDelete );
}


String Creud::ToString() const
{
	String result;
	result.Format( "%i%i%i%i%i", (int)Create(), (int)Read(), (int)Execute(), (int)Update(), (int)Delete() );
	return result;
}


void Creud::Log() const
{
	LogDetailedInfo( "creud=%s", ToString().AsChar() );
}


void Creud::ReadFromJson(Json::Item item, bool includeExecute)
{
	// disable everything
	mCreate	= false;
	mRead	= false;
	mUpdate	= false;
	mDelete	= false;
	mExecute= false;

	// find the creud item
	if (includeExecute == true)
	{
		Json::Item creudItem = item.Find("creud");
		if (creudItem.IsNull())
		{
			LogWarning( "JSON item '%s' does not contain a 'creud' child item.", item.GetName() );
			return;
		}

		if (creudItem.IsArray() == true)
		{
			// check size
			const int32 size = creudItem.Size();
			if (size != 5)
			{
				LogWarning( "Cannot read creud JSON item '%s'. Invalid number of items '%i'.", item.GetName(), size );
				return;
			}

			mCreate	= creudItem[0].GetBool();
			mRead	= creudItem[1].GetBool();
			mExecute= creudItem[2].GetBool();
			mUpdate	= creudItem[3].GetBool();
			mDelete	= creudItem[4].GetBool();
		}
		else if (creudItem.IsString() == true)
		{
			String creudString = creudItem.GetString();

			// check size
			const int32 numCharacters = creudString.GetLength();
			if (numCharacters != 5)
			{
				LogWarning( "Cannot read creud JSON item '%s'. Invalid number of characters '%i'.", item.GetName(), numCharacters );
				return;
			}

			mCreate	= String(creudString[0]).ToBool();
			mRead	= String(creudString[1]).ToBool();
			mExecute= String(creudString[2]).ToBool();
			mUpdate	= String(creudString[3]).ToBool();
			mDelete	= String(creudString[4]).ToBool();
		}
		else
		{
			LogWarning( "Cannot read creud JSON item '%s'. Item is neither an array nor a string.", item.GetName() );
			return;
		}
	}
	else
	{
		Json::Item creudItem = item.Find("crud");
		if (creudItem.IsNull())
		{
			LogWarning( "JSON item '%s' does not contain a 'crud' child item.", creudItem.GetName() );
			return;
		}

		// check size
		const int32 size = creudItem.Size();
		if (size != 4)
		{
			LogWarning( "Cannot read crud JSON item '%s'. Invalid number of items '%i'.", item.GetName(), size );
			return;
		}

		mCreate	= creudItem[0].GetBool();
		mRead	= creudItem[1].GetBool();
		mExecute= creudItem[2].GetBool();
		mUpdate	= creudItem[3].GetBool();
	}
}


bool Creud::Create() const			{ return mCreate; }
bool Creud::Read() const			{ return mRead; }
bool Creud::Execute() const			{ return mExecute; }
bool Creud::Update() const			{ return mUpdate; }
bool Creud::Delete() const			{ return mDelete; }

void Creud::RevokeCreate() 			{ mCreate = false; }
void Creud::RevokeRead() 			{ mRead = false; }
void Creud::RevokeExecute() 		{ mExecute = false; }
void Creud::RevokeUpdate() 			{ mUpdate = false; }
void Creud::RevokeDelete() 			{ mDelete = false; }
