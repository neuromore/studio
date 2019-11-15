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
#include "FileWriterNode.h"
#include "../Core/Math.h"
#include "../EngineManager.h"
#include "../DSP/ChannelFileWriter.h"

using namespace Core;

// constructor
FileWriterNode::FileWriterNode(Graph* graph) : SPNode(graph)
{
	mFile = NULL;
	mHasWriteError = false;
	mIsWriting = false;
	mFileFormat = ChannelFileWriter::FORMAT_CSV_TIMESTAMP;
	mWriteMode = WRITEMODE_KEEP;

	// configure write clock
	mClock.SetFrequency(0.5);	//write every 2 seconds
}


// destructor
FileWriterNode::~FileWriterNode()
{
}


// initialize the node
void FileWriterNode::Init()
{
	// NODE
	RequireMatchingSampleRates();

	// PORTS

	// start with one single output
	InitInputPorts(1);
	GetInputPort(INPUTPORT_VALUE).Setup("In", "y", AttributeChannels<double>::TYPE_ID, INPUTPORT_VALUE);

	// ATTRIBUTES

	// baseclass attributes
	SPNode::Init();

	// file url
	Core::AttributeSettings* attributeUrl = RegisterAttribute("File", "File", "The local filename of the output file.", Core::ATTRIBUTE_INTERFACETYPE_STRING);
	attributeUrl->SetDefaultValue( Core::AttributeString::Create() );

	// file formats
	Core::AttributeSettings* attributeFormat = RegisterAttribute("Format", "Format", "The file format of the output file.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	const uint32 numFormats = ChannelFileWriter::EFormat::NUM_FORMATS;
	for (uint32 i = 0; i < numFormats; ++i)
	{
		ChannelFileWriter::EFormat format = (ChannelFileWriter::EFormat)i;
		mTempString.Format("%s \t(*.%s)", ChannelFileWriter::GetFormatName(format), ChannelFileWriter::GetFormatExtension(format));
		attributeFormat->AddComboValue(mTempString.AsChar());
	}
	attributeFormat->SetDefaultValue(Core::AttributeInt32::Create(0));

	// write modes
	Core::AttributeSettings* attributeWriteMode = RegisterAttribute("Write Mode", "WriteMode", "Changes the write behaviour.", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attributeWriteMode->AddComboValue("Never Overwrite");
	attributeWriteMode->AddComboValue("Always Overwrite");
	attributeWriteMode->AddComboValue("Overwrite during Session");
	attributeWriteMode->AddComboValue("Append");
	attributeWriteMode->SetDefaultValue(Core::AttributeInt32::Create());
}


void FileWriterNode::Reset()
{
	SPNode::Reset();

	// stop clock
	mClock.Stop();
	mClock.Reset();

	// close file, if open
	if (mFile != NULL)
	{
		fclose(mFile);
		mFile = NULL;
	}

	mHasWriteError = false;
	mIsWriting = false;
}



void FileWriterNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	SPNode::ReInit(elapsed, delta);

	// empty channel -> dont initialize
	if (mInputReader.GetNumChannels() == 0)
	{
		mIsInitialized = false;
	}
	else if (mIsWriting == true && mHasWriteError == true)	// write error during last update
	{
		mIsInitialized = false;
		SetError(ERROR_FILE_NOT_WRITEABLE, "Cannot write to file.");
		ResetAsync();
	}
	else
	{
		ClearError(ERROR_FILE_NOT_WRITEABLE);
	}
	
	if (mIsWriting == true && mWriteMode == WRITEMODE_OVERWRITE_SESSION && GetSession()->IsRunning() == false)
	{
		// stop node if session was stopped
		mIsInitialized = false;
	}
	else if (mIsWriting == false)  // if we are not writing, try to open the file and begin writing
	{
		// collect array of write channels (can't reuse multichannel here until we have template free typesystem)
		mWriteChannels.Clear();
		const uint32 numChannels = mInputReader.GetNumChannels();
		for (uint32 i = 0; i < numChannels; ++i)
		{
			ChannelBase* channel = mInputReader.GetChannel(i);
			if (channel->GetType() == Channel<double>::TYPE_ID)
				mWriteChannels.Add(static_cast<Channel<double>*>(channel));
		}

		// generate filename
		mFileName = GetStringAttribute(ATTRIB_FILE);
		// HACKFIX backslash escape sequences are interpreted somewhere in the attribute stack 
		mFileName.Replace(StringCharacter::backSlash, StringCharacter::forwardSlash);
		SetStringAttribute("File", mFileName);

		// format string
		mTempString = Time::Now().Format(mFileName);
		mTempString.Replace("$starttime", GetSession()->GetStartTime().AsUtcString());
		mTempString.Trim();
		mTempString.ConvertToNativePath();

		LogDebug("FileWriterNode: Final filename is %s", mTempString.AsChar());

		// check if the file exists already
		FILE* file = fopen(mTempString.AsChar(), "rb\0");
		bool fileExists = file != NULL;
		if (file != NULL)
			fclose(file);

		// handle the different write modes
		if (mWriteMode == WRITEMODE_KEEP && fileExists == true)
		{
			// file exists -> abort early
			mIsInitialized = false;
			SetError(ERROR_FILE_ALREADY_EXISTS, "File already exists.");

		}
		else
		{
			ClearError(ERROR_FILE_ALREADY_EXISTS);

			if (mWriteMode == WRITEMODE_OVERWRITE_SESSION && GetSession()->IsRunning() == false)
			{
				// don't overwrite file if session is stopped (but do not handle it like an error)
				mIsInitialized = false;
			}
			else
			{
				// try to open file
				if (mWriteMode == WRITEMODE_APPEND)
					mFile = fopen( mTempString.AsChar(), "a+b\0" );
				else
					mFile = fopen( mTempString.AsChar(), "w+b\0" );

				if (mFile == NULL)
				{
					// file could not be opened
					SetError(ERROR_FILE_NOT_WRITEABLE, "Cannot open file for writing.");
					mIsInitialized = false;
				}
				// try to write file header
				else if (mFileWriter.WriteHeader(mFileFormat, mWriteChannels, mFile) == false)
				{
					// could not write
					SetError(ERROR_FILE_NOT_WRITEABLE, "Cannot write to file.");
					mHasWriteError = true;
					mIsInitialized = false;
					fclose( mFile );
				}
				else
				{
					ClearError(ERROR_FILE_NOT_WRITEABLE);

					// init successfull
					mIsWriting = true;
				}
			}
		}
	}

	PostReInit(elapsed, delta);
}


void FileWriterNode::Start(const Time& elapsed)
{
	// call baseclass start after channels were created
	SPNode::Start(elapsed);

	// configure clock and start it at current elapsed time
	mClock.Reset();
	mClock.SetStartTime(elapsed);
	mClock.Start();
}


// update the node
void FileWriterNode::Update(const Time& elapsed, const Time& delta)
{	
	// shared base update helper
	if (BaseUpdate(elapsed, delta) == false)
		return;

	SPNode::Update(elapsed, delta);

	// nothing to do
	if (mIsInitialized == false)
		return;

	mClock.Update(elapsed, delta);

	// write out data on every clock tick (may tick more than once during very long engine stalls, we ignore that)
	if (mClock.GetNumNewTicks() > 0)
	{
		mClock.ClearNewTicks();

		CORE_ASSERT(mWriteChannels.Size() > 0);
		const uint32 numNewSamples = mInputReader.GetMinNumNewSamples();
			
		if (numNewSamples == 0)
			return;

		// write to file
		if (mFileWriter.WriteSamples(mFileFormat, mWriteChannels, numNewSamples, mFile, mFileName) == false)
		{
			// write failed
			mHasWriteError = true;
		}

			// force write to disk
		if (mFile != NULL)
			fflush( mFile );

		// mark samples as processed
		mInputReader.Flush();
	}
}


// update the data
void FileWriterNode::OnAttributesChanged()
{
	SPNode::OnAttributesChanged();

	// get current attribute values
	const char* fileName = GetStringAttribute(ATTRIB_FILE);
	const int32 fileFormat = GetInt32Attribute(ATTRIB_FORMAT);
	const int32 writeMode = GetInt32Attribute(ATTRIB_WRITEMODE);

	// check if one of the attributes was changed and reset node
	if (mFileNameUnchanged.Compare(fileName) != 0 ||
		mFileFormat != fileFormat ||
		mWriteMode != writeMode)
	{
		mFileNameUnchanged = GetStringAttribute(ATTRIB_FILE);
		mFileFormat = (ChannelFileWriter::EFormat)fileFormat;
		mWriteMode = (EWriteMode)writeMode;

		// reset load error
		ResetAsync();
	}
}
