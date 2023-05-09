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
#include "OscMessageParser.h"
#include "../Core/LogManager.h"


using namespace Core;

OscMessageParser::OscMessageParser(const osc::ReceivedMessage& message) : mMessage (message) 
{
	mIsReady		= false;
	mOscPackStream	= new osc::ReceivedMessageArgumentStream(message.ArgumentStream());
}


const char* OscMessageParser::GetAddress() const
{
	return mMessage.AddressPattern();
}


uint32 OscMessageParser::GetNumArguments() const
{
	return (uint32)mMessage.ArgumentCount();
}


const char* OscMessageParser::GetTypeTags() const
{
	return mMessage.TypeTags();
}

// read arguments
OscMessageParser& OscMessageParser::operator>>(bool& value)					
{ 
	try
	{
		*mOscPackStream >> value;
	}
	catch (osc::Exception e)
	{
		value = false;
	}
	return *this;
}


OscMessageParser& OscMessageParser::operator>>(float& value)				
{ 
	try
	{
		*mOscPackStream >> value;
	}
	catch (osc::Exception e)
	{
		value = 0.0;
	}
	return *this;
}


OscMessageParser& OscMessageParser::operator>>(char& value)					
{ 
	try
	{
		*mOscPackStream >> value;
	}
	catch (osc::Exception e)
	{
		value = 0;
	}
	return *this;
}


OscMessageParser& OscMessageParser::operator>>(int32& value)
{
	try
	{
		osc::int32 res;
		*mOscPackStream >> res;
		value = res;
	}
	catch (osc::Exception e)
	{
		value = 0;
	}
	
	return *this;
}


OscMessageParser& OscMessageParser::operator>>(uint32& value)
{
	try
	{
		osc::int32 res;
		*mOscPackStream >> res;
		value = (uint32)res;
	}
	catch (osc::Exception e)
	{
		value = 0;
	}
	return *this;
}


OscMessageParser& OscMessageParser::operator>>(int64& value)
{
	try
	{
		osc::int64 res;
		*mOscPackStream >> res;
		value = res;
	}
	catch (osc::Exception e)
	{
		value = 0;
	}
	return *this;
}


OscMessageParser& OscMessageParser::operator>>(uint64& value)
{
	try 
	{
		osc::int64 res;
		*mOscPackStream >> res;
		value = (uint64)res;
	}
	catch (osc::Exception e)
	{
		value = 0;
	}
	return *this;
}


OscMessageParser& OscMessageParser::operator>>(double& value)
{
	try
	{
		*mOscPackStream >> value;
	}
	catch (osc::Exception e)
	{
		value = 0.0;
	}
	return *this;
}


OscMessageParser& OscMessageParser::operator>>(const char*& value)	
{
	try
	{
		*mOscPackStream >> value;
	}
	catch (osc::Exception e)
	{
		value = 0;
	}
	return *this;
}




// match an address against an adress pattern (with simple wildcards)
bool OscMessageParser::MatchAddress(const char* address, const char* pattern)
{
	// supports the wildcards '*' and '?':
	//  '?' matches any character except '/' (NOT IMPLEMENTED right now)
	//  '*' matches any sequence of characters, up to the next '/' (excluding). Exception: if its the last char, accept immediately
	// all other chars have to match up perfectly

	const uint32 addrLen = (uint32)strlen(address);
	const uint32 pattLen = (uint32)strlen(pattern);
	uint32 addrPos = 0;
	uint32 pattPos = 0;
	
	// guard agains out-of-range
	while (addrPos < addrLen && pattPos < pattLen)
	{
		// get current  pattern char
		char c = pattern[pattPos];

		// process wildcards differently
		//if (c == '?')
		//{
		//	// match any char, except '/'
		//	if (address[addrPos] != '/')
		//	{
		//		addrPos++;
		//		pattPos++;
		//	}
		//	else
		//	{
		//		// chars do not match
		//		break;
		//	}
		//}
		//else
		if (c == '*')
		{
			// if * is the last char in the pattern, we can accept immediately
			if (pattPos == pattLen-1)
				return true;

			if (address[addrPos] != '/')
				addrPos++;
			else
				// otherwise, advance pattern char position
				pattPos++;
		
		}
		else
		{
			// compare the chars
			if (pattern[pattPos] == address[addrPos])
			{
				addrPos++;
				pattPos++;
			}
			else
			{
				// chars do not match
				break;
			}
		}
	}

	// we have a match if both character indices have reached the end of the string
	if (addrPos == addrLen && pattPos == pattLen)
		return true;
	else
		return false;

}
