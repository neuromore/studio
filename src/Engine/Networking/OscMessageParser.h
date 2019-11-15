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

#ifndef __NEUROMORE_OSCMESSAGEPARSER_H
#define __NEUROMORE_OSCMESSAGEPARSER_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/String.h"
#include "../Core/StringIterator.h"
#include "../Core/Array.h"

#include <oscpack/OscOutboundPacketStream.h>
#include <oscpack/OscReceivedElements.h>
#include <oscpack/OscTypes.h>
#include <oscpack/OscReceivedElements.h>

// osc message parser
class ENGINE_API OscMessageParser
{
	public:
		OscMessageParser(const osc::ReceivedMessage& message);

		const char* GetAddress() const;
		uint32 GetNumArguments() const;
		const char* GetTypeTags() const;
		
		// match an address against an adress pattern (may contain wildcards)
		static bool MatchAddress(const char* messageAddress, const char* addressPattern);
		bool MatchAddress(const char* addressPattern)		{ return MatchAddress(GetAddress(), addressPattern); } 

		// read arguments
		OscMessageParser& operator>>(bool& value);
		OscMessageParser& operator>>(float& value);
		OscMessageParser& operator>>(char& value);
		OscMessageParser& operator>>(int32& value);
		OscMessageParser& operator>>(uint32& value);
		OscMessageParser& operator>>(int64& value);
		OscMessageParser& operator>>(uint64& value);
		OscMessageParser& operator>>(double& value);
		OscMessageParser& operator>>(const char*& value);

		osc::ReceivedMessage				mMessage;
		osc::ReceivedMessageArgumentStream* mOscPackStream;
		bool								mIsReady;
};

#endif
