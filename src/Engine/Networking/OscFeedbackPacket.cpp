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
#include "OscFeedbackPacket.h"
#include "../Core/LogManager.h"
#include "../EngineManager.h"

#include "OscPacketParser.h"


using namespace Core;

// constructor
OscFeedbackPacket::OscFeedbackPacket() : OscPacket(2048)	// TODO not sure how to determine the size beforehand
{
	// initialize the state
	mElapsedTime	= 0.0;
	mSessionRunning  = false;
	mHavePoints	= false;
	mNumPoints = 0;
}


// destructor
OscFeedbackPacket::~OscFeedbackPacket()
{
}


void OscFeedbackPacket::Write()
{
	// gather newset data
	FillDatagram();

	int32 numCustomFeedbackNodes = 0;
	Classifier* activeClassifier = GetEngine()->GetActiveClassifier();

	try
	{
		// clear osc parser
		Clear();

		// begin packet
		BeginPacket();

		// get access to the active classifier
		if (activeClassifier != NULL)
		{
			// iterate over all feedback nodes and compose the osc message
			const uint32 numOutputNodes = activeClassifier->GetNumOutputNodes();
			for (uint32 i = 0; i < numOutputNodes; ++i)
			{
				OutputNode* outputNode = activeClassifier->GetOutputNode(i);

				if (outputNode->GetOutputNodeType() == FeedbackNode::OUTPUTNODE_TYPE)
				{
					FeedbackNode* node = static_cast<FeedbackNode*>(outputNode);

					// add the node's osc message to the osc package
					if (node->GetSendOscNetworkMessages() == true)
					{
						node->WriteOscMessage(this);
						numCustomFeedbackNodes++;
					}
				}
			}
		}

		// write game points (if we have any)
		if (mHavePoints == true)
		{
			BeginMessage("/session/points");
			WriteValue((float)mNumPoints);
			EndMessage();
		}

		// write elapsed time
		BeginMessage("/session/time");
		WriteValue((float)mElapsedTime);
		EndMessage();

		
		// write session running flag
		BeginMessage("/session/running");
		WriteValue((bool)mSessionRunning);
		EndMessage();

		// packet is complete
		EndPacket();

	}
	catch (osc::OutOfBufferMemoryException e)
	{
		LogError("OutOfBufferMemoryException while writing OSC Feedback Packet : it is too small (%i feedback nodes)", numCustomFeedbackNodes);
	}
	catch (osc::Exception e)
	{
		LogError("Exception while writing OSC Feedback Packet: %s", e.what());
	}
}


// fill UDP datagram
void OscFeedbackPacket::FillDatagram()
{
	Classifier* activeClassifier = static_cast<Classifier*>( GetEngine()->GetActiveClassifier() );
	if (activeClassifier == NULL)
		return;

	Session* session = GetEngine()->GetSession();
	
	// sesion information, if running
	mSessionRunning = session->IsRunning();

	if (mSessionRunning == true)
		mElapsedTime = session->GetElapsedTime().InSeconds();
	else
		mElapsedTime = 0;


	mHavePoints = session->HasPoints();
	mNumPoints = session->GetPoints();
}
