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

// include required headers
#include "OscPacketParser.h"
#include "../Core/LogManager.h"
#include "../Core/StringCharacter.h"
#include <oscpack/OscReceivedElements.h>


void OscPacketParser::Clear()
{
	// get rid of all messages
	mMessages.Clear(false);
}

	
// parse a packet (consist of messages and/or bundles)
void OscPacketParser::ParsePacket(const char *data, uint32 size)
{
	Clear();

    osc::ReceivedPacket packet(data, (osc::osc_bundle_element_size_t)size);
	if( packet.IsBundle() )
		ProcessBundle( osc::ReceivedBundle(packet) );
	else
		ProcessMessage( osc::ReceivedMessage(packet) );
}


// parse a bundle (contains messages/and or bundles)
void OscPacketParser::ProcessBundle(const osc::ReceivedBundle& bundle)
{
	// ignore bundle time tag for now
	for( osc::ReceivedBundle::const_iterator i = bundle.ElementsBegin(); i != bundle.ElementsEnd(); ++i )
	{
		if (i->IsBundle() == true)
			ProcessBundle( osc::ReceivedBundle(*i) );
		else
			ProcessMessage( osc::ReceivedMessage(*i) );
	}
}


// parse one message
void OscPacketParser::ProcessMessage(const osc::ReceivedMessage& message)
{
	mMessages.Add(OscMessageParser(message));
}
