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
#include "FileReaderNode.h"
#include "../Core/Math.h"
#include "../EngineManager.h"
#include "../DSP/ChannelFileReader.h"

using namespace Core;

// constructor
FileReaderNode::FileReaderNode(Graph* graph) : InputNode(graph)
{
	mHasLoadError = false;

	// color output channels automatically
	UseChannelColoring();
}



// destructor
FileReaderNode::~FileReaderNode()
{
	// remove loaded data
	const uint32 numChannels = mFileChannels.Size();
	for (uint32 i = 0; i < numChannels; ++i)
		delete mFileChannels[i];
	mFileChannels.Clear();
}


// initialize the node
void FileReaderNode::Init()
{
	// PORTS

	// start with one single output
	InitOutputPorts(1);
	GetOutputPort(OUTPUTPORT_VALUE).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, OUTPUTPORT_VALUE);

	// ATTRIBUTES

	// baseclass attributes
	InputNode::Init();

	// file url
	Core::AttributeSettings* attributeUrl = RegisterAttribute("File", "File", "The local filename of the data file.", Core::ATTRIBUTE_INTERFACETYPE_STRING);
	attributeUrl->SetDefaultValue( Core::AttributeString::Create() );

	// file formats
	Core::AttributeSettings* attributeFormat = RegisterAttribute("Format", "Format", "The data format of the input file.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	const uint32 numFormats = ChannelFileReader::EFormat::NUM_FORMATS;
	for (uint32 i = 0; i < numFormats; ++i)
	{
		ChannelFileReader::EFormat format = (ChannelFileReader::EFormat)i;
		mTempString.Format("%s (*.%s)", ChannelFileReader::GetFormatName(format), ChannelFileReader::GetFormatExtension(format));
		attributeFormat->AddComboValue(mTempString.AsChar());
	}
	attributeFormat->SetDefaultValue(Core::AttributeInt32::Create(0));

	// override sample rate
	Core::AttributeSettings* attributeSettings = RegisterAttribute("Sample Rate", "SampleRate", "Override the sample rate of the output channels.", Core::ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	attributeSettings->SetDefaultValue(Core::AttributeFloat::Create(0));
	attributeSettings->SetMinValue(Core::AttributeFloat::Create(0));
	attributeSettings->SetMaxValue(Core::AttributeFloat::Create(DBL_MAX));

	// hide upload attribute
	GetAttributeSettings(ATTRIB_UPLOAD)->SetVisible(false);
}


void FileReaderNode::Reset()
{
	InputNode::Reset();

	// remove references from output port multichannel
	MultiChannel* channels = GetOutputPort(OUTPUTPORT_VALUE).GetChannels();
	channels->Clear();

	// remove loaded data
	const uint32 numChannels = mFileChannels.Size();
	for (uint32 i = 0; i < numChannels; ++i)
		delete mFileChannels[i];
	mFileChannels.Clear();

	mNumSamples = 0;
	mHasData = false;

	// reset load error, so ReInit() will try again even if it failed earlier
	mHasLoadError = false;

	// stop clock
	mClock.Stop();
	mClock.Reset();
}



void FileReaderNode::ReInit(const Time& elapsed, const Time& delta)
{
	InputNode::ReInit(elapsed, delta);

	// only try to load if there is no data
	if (mHasData == false)
	{
		FILE* file = NULL;

		mFileName = GetStringAttribute(ATTRIB_URL);
		mFileName.Trim();

		// HACKFIX backslash escape sequences are interpreted somewhere in the attribute stack 
		mFileName.Replace(StringCharacter::backSlash, StringCharacter::forwardSlash);	
		SetStringAttribute("File", mFileName);

		// skip immediately if the last ReInit failed to load the file
		if (mHasLoadError == true)
		{
			mIsInitialized = false;
		}
		// skip empty filename
		else if (mFileName.IsEmpty() == true)
		{
			SetError(ERROR_FILE_NOT_READABLE, "No file specified.");
			mIsInitialized = false;
		}
		// cannot open file
		else if ((file = fopen(mFileName.AsChar(), "rt\0")) == NULL)
		{
			mIsInitialized = false;
			mHasLoadError = true;
			SetError(ERROR_FILE_NOT_READABLE, "Cannot open file.");
		}
		else
		{
			ClearError(ERROR_FILE_NOT_READABLE);

			// try to read the file into the mFileChannels channels array
			mFileFormat = GetInt32Attribute(ATTRIB_FORMAT);
			const bool success = mFileReader.Read(file, mFileName, (ChannelFileReader::EFormat)mFileFormat, mFileChannels);
			const uint32 numChannels = mFileChannels.Size();
			if (success == false)
			{
				mIsInitialized = false;
				mHasLoadError = true;
				SetError(ERROR_FORMAT_NOT_READABLE, "Can't parse the file. Wrong format?");
			}
			else
			{
				ClearError(ERROR_FORMAT_NOT_READABLE);
			}


			// check if 
			bool channelsEmpty = true;
			for (uint32 i=0; i<numChannels; ++i)
			{
				ChannelBase* channel = mFileChannels[i];
				if (channel != NULL && channel->GetNumSamples() > 0)
				{
					channelsEmpty = false;
					break;
				}
			}

			if (numChannels == 0 || (numChannels > 0 && channelsEmpty) == true)
			{
				mIsInitialized = false;
				mHasLoadError = true;
				SetWarning(WARNING_FILE_EMPTY, "File is empty.");
			}
			else  // reading succeeded
			{
				ClearWarning(WARNING_FILE_EMPTY);

				// overwrite channel sample rate if attribute is set
				const uint32 attribSampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
				if (attribSampleRate > 0)
				{
					mSampleRate = attribSampleRate;
					for (uint32 i = 0; i < numChannels; ++i)
						mFileChannels[i]->SetSampleRate(mSampleRate);
				}
				else
				{
					// make sure all samples have the same samplerate (makes everything easier)
					mSampleRate = mFileChannels[0]->GetSampleRate();  // Note: we already ensured that first element exists
					bool missmatch = false;
					for (uint32 i = 1; i < numChannels; ++i)
					{
						ChannelBase* channel = mFileChannels[i];
						if (channel->GetSampleRate() != mSampleRate)
							missmatch = true;
					}

					if (missmatch == true)
						mIsInitialized = false;
				}

				// number of samples that can be read from all channels (assuming identical samplerates)
				uint32 maxNumSamples = mFileChannels[0]->GetNumSamples();
				for (uint32 i = 1; i < numChannels; ++i)
				{
					ChannelBase* channel = mFileChannels[i];
					maxNumSamples = Min<uint32>(maxNumSamples, channel->GetNumSamples());
				}

				// maximum number of sampels that we can read from all channels
				mNumSamples = maxNumSamples;

				// we're done, data can now be played back
				if (mNumSamples > 0)
					mHasData = true;
			}

			fclose(file);
		}

		// FIXME we set load error _after_ PostReInit(), otherwise it could be cleared by Reset()..
		//  this can be fixed by adding some kind of 'cooldown timer', that ensures a node's Reinit is not called with every Classifier::Reinit(), but less often (e.g. every 5 seconds at max).
		// This is very useful for nodes that have high ReInit costs, like FileReader.
	}

	const bool hasLoadError = mHasLoadError;
	PostReInit(elapsed, delta);
	mHasLoadError = hasLoadError;
}


void FileReaderNode::Start(const Time& elapsed)
{

	// create sensors, if not already
	const uint32 numChannels = mFileChannels.Size();
	mSensors.Resize(numChannels);

	// multichannel holds references to all sensors
	MultiChannel* channels = GetOutputPort(OUTPUTPORT_VALUE).GetChannels();
	channels->Clear();

	//  and configure sensors
	for (uint32 i = 0; i < numChannels; i++)
	{
		// take over config from loaded channels
		Sensor* sensor = &mSensors[i];
		sensor->Reset();
		sensor->SetName(mFileChannels[i]->GetName());
		sensor->SetDriftCorrectionEnabled(false);
		sensor->SetSampleRate(mFileChannels[i]->GetSampleRate());
		sensor->GetChannel()->SetBufferSize(100);	// arbitrary start buffer size 
		channels->AddChannel(sensor->GetChannel());
	}

	// set output port name
	GetOutputPort(OUTPUTPORT_VALUE).SetName(mFileName.AsChar());

	// configure clock and start it at current elapsed time
	mClock.Reset();
	mClock.SetFrequency(mSampleRate);
	mClock.SetStartTime(elapsed);
	mClock.Start();

	// call baseclass start after channels were created
	InputNode::Start(elapsed);
}


// update the node
void FileReaderNode::Update(const Time& elapsed, const Time& delta)
{	
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	mClock.Update(elapsed, delta);

	InputNode::Update(elapsed, delta);
}


// update the data
void FileReaderNode::OnAttributesChanged()
{
	// get current attribute values
	const double sampleRate = GetFloatAttribute(ATTRIB_SAMPLERATE);
	const char* fileName = GetStringAttribute(ATTRIB_URL);
	const uint32 fileFormat = GetInt32Attribute(ATTRIB_FORMAT);

	// rset node if one of the attributes was changed
	if (mSampleRate != sampleRate ||
		mFileFormat != fileFormat ||
		mFileName.Compare(fileName) != 0)
	{
		// reset load error
		mHasLoadError = false;
		ResetAsync();
	}
}


// the function that fills the sensors with samples
void FileReaderNode::GenerateSamples()
{
	// modulo must not fail
	if (mNumSamples == 0)
		return;

	const uint32 numNewSamples = mClock.GetNumNewTicks();
	const uint32 numChannels = GetNumSensors();			
	
	// push samples from data channels into sensor queue
	for (uint32 i = 0; i < numNewSamples; ++i)
	{
		const uint32 sampleIndex = mClock.PopOldestTick() % (uint64)mNumSamples;	// index loop happens here
		for (uint32 c = 0; c < numChannels; ++c)
		{
			const double sampleValue = mFileChannels[c]->GetSample(sampleIndex);
			mSensors[c].AddQueuedSample(sampleValue);
		}
	}
	
}
