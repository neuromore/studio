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
#include "MetaInfoNode.h"
#include "../Core/Math.h"
#include "../EngineManager.h"
#include "Classifier.h"


using namespace Core;

// constructor
MetaInfoNode::MetaInfoNode(Graph* graph) : InputNode(graph)
{
	// configure sensor
	mSensor.GetChannel()->SetSampleRate(128);
	mSensor.GetChannel()->SetBufferSize(10);
	mSensor.SetDriftCorrectionEnabled(false);

	mInfoType = INFOTYPE_CHANNEL_SAMPLERATE;

	// color the output channel same as the node
	UseChannelColoring();
}


// destructor
MetaInfoNode::~MetaInfoNode()
{
}


// initialize the node
void MetaInfoNode::Init()
{
	// PORTS

	// setup the input ports
	InitInputPorts(1);
	GetInputPort(INPUTPORT_VALUE).Setup("In", "x1", AttributeChannels<double>::TYPE_ID, INPUTPORT_VALUE);

	InitOutputPorts(1);
	GetOutputPort(OUTPUTPORT_VALUE).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_VALUE);
	GetOutputPort(OUTPUTPORT_VALUE).GetChannels()->AddChannel(mSensor.GetChannel());

	// ATTRIBUTES

	// baseclass attributes
	InputNode::Init();

	Core::AttributeSettings* functionParam = RegisterAttribute("Info Type", "infoType", "Select the type of information to output.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	functionParam->ResizeComboValues((uint32)NUM_INFOTYPES);
	for (uint32 i = 0; i<NUM_INFOTYPES; ++i)
		functionParam->SetComboValue(i, GetInfoTypeString((EInfoType)i));
	functionParam->SetDefaultValue(Core::AttributeInt32::Create(mInfoType));

	// sample rate
	Core::AttributeSettings* attributeSettings = RegisterAttribute("Sample Rate", "sampleRate", "Sample rate of the output channel.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeSettings->SetDefaultValue( Core::AttributeFloat::Create(128) );
	attributeSettings->SetMinValue( Core::AttributeFloat::Create(0) );
	attributeSettings->SetMaxValue( Core::AttributeFloat::Create(DBL_MAX) );

	UpdateSensorName();
}


void MetaInfoNode::Reset()
{
	InputNode::Reset();

	mClock.Stop();
	mClock.Reset();
}


void MetaInfoNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;
	
	InputNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


void MetaInfoNode::Start(const Time& elapsed)
{
	// configure output channel
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	mSensor.GetChannel()->SetSampleRate(sampleRate);

	// call baseclass start
	InputNode::Start(elapsed);

	// set sensor / channel name
	UpdateSensorName();
	
	// configure clock and start it at current elapsed time
	mClock.Reset();
	mClock.SetFrequency(sampleRate);
	mClock.SetStartTime(elapsed);
	mClock.Start();
}


// update the node
void MetaInfoNode::Update(const Time& elapsed, const Time& delta)
{	
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	mClock.Update(elapsed, delta);

	InputNode::Update(elapsed, delta);
}


// update the data
void MetaInfoNode::OnAttributesChanged()
{
	// in case the sample rate changed, reset the node and channels
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	if (mSensor.GetChannel()->GetSampleRate() != sampleRate)
		ResetAsync();

	mInfoType = (EInfoType)GetInt32Attribute(ATTRIB_INFOTYPE);

	UpdateSensorName();
}


// the function that fills the sensors with samples
void MetaInfoNode::GenerateSamples()
{
	const uint32 numNewSamples = mClock.GetNumNewTicks();			// number of samples in this interval
	mClock.ClearNewTicks();

	const double value = GetInfoValue(mInfoType);

	// add the samples
	for (uint32 i = 0; i<numNewSamples; ++i)
		mSensor.AddQueuedSample(value);

	// if clock is not running (non-const samplerate), output sample only if value has changed
	ChannelBase* channel = mSensor.GetChannel();
	if (channel->GetSampleRate() == 0)
	{
		if (channel->GetNumSamples() == 0)
			mSensor.AddQueuedSample(value);
		else if (channel->AsType<double>()->GetLastSample() != value)
			mSensor.AddQueuedSample(value);
	}
}


void MetaInfoNode::UpdateSensorName()
{
	// use parameter value as channel name
	const char* name = GetShortInfoTypeString(mInfoType);
	mSensor.SetName(name);
	GetOutputPort(OUTPUTPORT_VALUE).SetName(name);
}


const char* MetaInfoNode::GetInfoTypeString(EInfoType type)
{
	switch (type)
	{
		case INFOTYPE_MULTICHANNEL_NUMCHANNELS:		{ return "Number of Channels in Multichannel";	}
	
		case INFOTYPE_CHANNEL_SAMPLERATE:			{ return "Channel: Sample Rate";				}
		case INFOTYPE_CHANNEL_SAMPLECOUNTER:		{ return "Channel: Sample Counter";				}
		case INFOTYPE_CHANNEL_NUMSAMPLES:			{ return "Channel: Number of Samples";			}
		case INFOTYPE_CHANNEL_NUMNEWSAMPLES:		{ return "Channel: Number of added samples";	}
		case INFOTYPE_CHANNEL_BUFFERSIZE:			{ return "Channel: Buffer Size (bytes)";		}
		case INFOTYPE_CHANNEL_ELAPSEDTIME:			{ return "Channel: Elapsed Time (seconds)";		}
		case INFOTYPE_CHANNEL_LASTSAMPLETIME:		{ return "Channel: Last Sample Time (seconds)"; }
		case INFOTYPE_CHANNEL_STARTTIME:			{ return "Channel: Start Time (seconds)";		}
		case INFOTYPE_CHANNEL_ISINDEPENDENT:		{ return "Channel: Is Independent?";			}
		case INFOTYPE_CHANNEL_LATENCY:				{ return "Channel: Latency (seconds)";			}

		case INFOTYPE_CLASSIFIER_BUFFERUSAGE:		{ return "Classifier Buffer Usage (bytes)";	}
		case INFOTYPE_CLASSIFIER_MAXLATENCY:		{ return "Classifier Maximum Output Latency (seconds)";	}

		case INFOTYPE_ENGINE_UPDATE_MS:				{ return "Engine Update Loop Performance (ms)";	}
		case INFOTYPE_ENGINE_CLASSIFIER_MS:			{ return "Engine Classifier Performance (ms)";	}
		default:							  return "";
	}
}



const char* MetaInfoNode::GetShortInfoTypeString(EInfoType type)
{
	switch (type)
	{
		case INFOTYPE_MULTICHANNEL_NUMCHANNELS:		{ return "Channel Count";			}
	
		case INFOTYPE_CHANNEL_SAMPLERATE:			{ return "Sample Rate";				}
		case INFOTYPE_CHANNEL_SAMPLECOUNTER:		{ return "Sample Counter";			}
		case INFOTYPE_CHANNEL_NUMSAMPLES:			{ return "Available Samples";		}
		case INFOTYPE_CHANNEL_NUMNEWSAMPLES:		{ return "Added samples";			}
		case INFOTYPE_CHANNEL_BUFFERSIZE:			{ return "Buffer Size";				}
		case INFOTYPE_CHANNEL_ELAPSEDTIME:			{ return "Elapsed Time";			}
		case INFOTYPE_CHANNEL_LASTSAMPLETIME:		{ return "Last Sample Time";		}
		case INFOTYPE_CHANNEL_STARTTIME:			{ return "Start Time";				}
		case INFOTYPE_CHANNEL_ISINDEPENDENT:		{ return "Is Independent?";			}
		case INFOTYPE_CHANNEL_LATENCY:				{ return "Latency";					}

		case INFOTYPE_CLASSIFIER_BUFFERUSAGE:		{ return "Buffers";					}
		case INFOTYPE_CLASSIFIER_MAXLATENCY:		{ return "Output Latency";			}

		case INFOTYPE_ENGINE_UPDATE_MS:				{ return "Engine Performance";		}
		case INFOTYPE_ENGINE_CLASSIFIER_MS:			{ return "Classifier Performance";	}
		default:							  return "";
	}
}

double MetaInfoNode::GetInfoValue(EInfoType type)
{
	double value = 0.0;

	ChannelBase* channel = NULL;
	if (mInputReader.GetNumChannels() > 0)
		channel = mInputReader.GetChannel(0);

	Classifier * classifier = NULL;
	if (mParentGraph->GetType() == Classifier::TYPE_ID)
		classifier = static_cast<Classifier*>(mParentGraph);

	switch (type)
	{
		// multichannels
		case INFOTYPE_MULTICHANNEL_NUMCHANNELS: { value = mInputReader.GetNumChannels();										} break;

		// channels
		case INFOTYPE_CHANNEL_SAMPLERATE:		{ value = (channel == NULL ? 0.0 : channel->GetSampleRate());					} break;
		case INFOTYPE_CHANNEL_SAMPLECOUNTER:	{ value = (channel == NULL ? 0.0 : channel->GetSampleCounter());				} break;
		case INFOTYPE_CHANNEL_NUMSAMPLES:		{ value = (channel == NULL ? 0.0 : channel->GetNumSamples());					} break;
		case INFOTYPE_CHANNEL_NUMNEWSAMPLES:	{ value = (channel == NULL ? 0.0 : channel->GetNumNewSamples());				} break;
		case INFOTYPE_CHANNEL_BUFFERSIZE:		{ value = (channel == NULL ? 0.0 : channel->GetBufferSize());					} break;
		case INFOTYPE_CHANNEL_ELAPSEDTIME:		{ value = (channel == NULL ? 0.0 : channel->GetElapsedTime().InSeconds());		} break;
		case INFOTYPE_CHANNEL_LASTSAMPLETIME:	{ value = (channel == NULL ? 0.0 : channel->GetLastSampleTime().InSeconds());	} break;
		case INFOTYPE_CHANNEL_STARTTIME:		{ value = (channel == NULL ? 0.0 : channel->GetStartTime().InSeconds());		} break;
		case INFOTYPE_CHANNEL_ISINDEPENDENT:	{ value = (channel == NULL ? 0.0 : (channel->IsIndependent() ? 1.0 : 0.0));		} break;
		case INFOTYPE_CHANNEL_LATENCY:			{ value = (channel == NULL ? 0.0 : channel->GetLatency());						} break;

		// classifier
		case INFOTYPE_CLASSIFIER_BUFFERUSAGE:	{ value = (classifier == NULL ? 0.0 : classifier->CalculateBufferMemoryUsed());	} break;
		case INFOTYPE_CLASSIFIER_MAXLATENCY:	{ value = (classifier == NULL ? 0.0 : classifier->FindMaximumLatency());		} break;
		
		// performance
		case INFOTYPE_ENGINE_UPDATE_MS:			{ value = GetEngine()->GetFpsCounter().GetWorstCaseTiming() * 1000.0 ;			} break;
		case INFOTYPE_ENGINE_CLASSIFIER_MS:		{ value = (GetEngine()->GetActiveClassifier() == NULL ? 0.0 : GetEngine()->GetActiveClassifier()->GetFpsCounter().GetWorstCaseTiming() * 1000.0 );		} break;

        default:                                { LogError("MetaInfoNode::GetInfoValue(): Unsupported type"); break; }
	}

	return value;
}
