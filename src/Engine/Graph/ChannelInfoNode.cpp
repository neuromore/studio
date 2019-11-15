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
#include "ChannelInfoNode.h"
#include "../Core/Math.h"


using namespace Core;

// constructor
ChannelInfoNode::ChannelInfoNode(Graph* graph) : ProcessorNode(graph, new ChannelInfoNode::Processor())
{
	// default on addition
	mSettings.mInfoType = INFOTYPE_SAMPLERATE;
}


// destructor
ChannelInfoNode::~ChannelInfoNode()
{
}


// initialize the node
void ChannelInfoNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// SETUP PORTS

	// setup the input ports
	GetInputPort(INPUTPORT_DOUBLE).Setup("In", "x", AttributeChannels<double>::TYPE_ID, INPUTPORT_DOUBLE);

	// setup the output ports
	GetOutputPort(OUTPUTPORT_RESULT).Setup(GetInfoTypeString(mSettings.mInfoType), "y", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_RESULT);

	// ATTRIBUTES

	Core::AttributeSettings* functionParam = RegisterAttribute("Info Type", "infoType", "Select the type of information to output.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	functionParam->ResizeComboValues( (uint32)NUM_INFOTYPES );
	for (uint32 i = 0; i<NUM_INFOTYPES; ++i)
		functionParam->SetComboValue(i, GetInfoTypeString((EInfoType)i));
	functionParam->SetDefaultValue(Core::AttributeInt32::Create(mSettings.mInfoType));
}


void ChannelInfoNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


// update the node
void ChannelInfoNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update the baseclass
	ProcessorNode::Update(elapsed, delta);
}


// update the data
void ChannelInfoNode::OnAttributesChanged()
{
	EInfoType infoType = (EInfoType)GetInt32Attribute(ATTRIB_INFOTYPE);

	// if it didn't change, don't update anything
	if (infoType == mSettings.mInfoType)
		return;
	
	mSettings.mInfoType = (EInfoType)GetInt32Attribute(ATTRIB_INFOTYPE);
	
	// reset 
	ResetAsync();

	// output port name
	OutputPort& outputPort = GetOutputPort(OUTPUTPORT_RESULT);
	outputPort.SetName(GetInfoTypeString(infoType));
}


const char* ChannelInfoNode::GetInfoTypeString(EInfoType type)
{
	switch (type)
	{
		case INFOTYPE_SAMPLETIME:		{ return "Sample Time";				}
		case INFOTYPE_SAMPLEINDEX:		{ return "Sample Index";			}
		case INFOTYPE_SAMPLERATE:		{ return "Sample Rate";				}
		case INFOTYPE_NUMSAMPLES:		{ return "Available Samples";		}
		case INFOTYPE_NUMNEWSAMPLES:	{ return "New Samples"; }
		case INFOTYPE_BUFFERSIZE:		{ return "Buffer Size";				}
		case INFOTYPE_ELAPSEDTIME:		{ return "Elapsed Time";			}
		case INFOTYPE_LASTSAMPLETIME:	{ return "Last Sample Time";		}
		case INFOTYPE_STARTTIME:		{ return "Start Time";				}
		case INFOTYPE_ISINDEPENDENT:	{ return "Is Independent?";			}
		default:						  return "";
	}
}


//-----------------------------------------------
// the node's processor implementation
//-----------------------------------------------

void ChannelInfoNode::Processor::ReInit()					
{ 
	// ReInit baseclass
	ChannelProcessor::ReInit();

	mIsInitialized = false;
				
	ChannelBase* input = GetInput();
	ChannelBase* output = GetOutput();

	// nothing to do until a channel is connected
	if (input == NULL || output == NULL)
		return;

	double outputSampleRate = input->GetSampleRate();
	output->SetSampleRate(outputSampleRate);
				
	mIsInitialized = true;
}


void ChannelInfoNode::Processor::Update()
{
	if (mIsInitialized == false)
		return;
	
	// update base
	ChannelProcessor::Update();
	
	// channel input reader
	ChannelReader* input = GetInputReader(0);

	// input/output channels
	ChannelBase* output = GetOutput();
				
	// get number of samples
	uint32 numNewSamples = input->GetNumNewSamples();
	// process input samples
	for (uint32 i=0; i<numNewSamples; ++i)
	{
		const uint64 sampleIndex = input->GetSampleIndex(i);

		// add value to output
		const double value = GetInfoValue(mSettings.mInfoType, sampleIndex);
		output->AsType<double>()->AddSample(value);
	}

	input->Flush();
}


double ChannelInfoNode::Processor::GetInfoValue(EInfoType type, uint64 sampleIndex)
{
	ChannelBase* channel = GetInput();
	if (channel == NULL)
		return 0.0;

	switch (type)
	{
		case INFOTYPE_SAMPLETIME:	    { return channel->GetSampleTime(sampleIndex).InSeconds();	}
		case INFOTYPE_SAMPLEINDEX:		{ return sampleIndex;										}
		case INFOTYPE_SAMPLERATE:		{ return channel->GetSampleRate();							}
		case INFOTYPE_NUMSAMPLES:		{ return channel->GetNumSamples();							}
		case INFOTYPE_NUMNEWSAMPLES:	{ return channel->GetNumNewSamples();						}
		case INFOTYPE_BUFFERSIZE:		{ return channel->GetBufferSize();							}
		case INFOTYPE_ELAPSEDTIME:		{ return channel->GetElapsedTime().InSeconds();				}
		case INFOTYPE_LASTSAMPLETIME:	{ return channel->GetLastSampleTime().InSeconds();			}
		case INFOTYPE_STARTTIME:		{ return channel->GetStartTime().InSeconds();				}
		case INFOTYPE_ISINDEPENDENT:	{ return (channel->IsIndependent() ? 1.0 : 0.0);			}
		default:						  return 0.0;
	}
}
