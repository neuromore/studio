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

// include required files
#include "SessionExporter.h"
#include "Core/LogManager.h"
#include "EngineManager.h"


using namespace Core;

// save session
bool SessionExporter::Save(const char* folderPath, const char* userId, const char* dataChunkId)
{
	String filename, jsonFilename, nmdFilename;

	// get the active classifier
	Classifier* classifier = GetEngine()->GetActiveClassifier();

	// add all feedback channels
	const uint32 numFeedbackNodes = classifier->GetNumFeedbackNodes();
	for (uint32 i = 0; i<numFeedbackNodes; ++i)
	{
		FeedbackNode* feedbackNode = classifier->GetFeedbackNode(i);

		// skip node if data upload is disabled or node is not running
		if (feedbackNode->IsUploadEnabled() == false || feedbackNode->IsInitialized() == false)
			continue;


		// get the number of output channels and iterate through them
		const uint32 numChannels = feedbackNode->GetNumOutputChannels();
		for (uint32 j = 0; j<numChannels; ++j)
		{
			Channel<double>* channel = feedbackNode->GetOutputChannel(j);

			// generate random UUID based filename for the given channel
			filename = GetEngine()->GetCallback()->ConstructRandomFilename(folderPath, "");

			// save the channel info into a .json file
			jsonFilename = filename + ".json";
			SaveChannelJsonToDisk(jsonFilename.AsChar(), userId, dataChunkId, classifier->GetUuid(), feedbackNode->GetUuid(), channel);

			// save biodata to a binary file
			nmdFilename = filename + ".nmd";
			SaveSamplesToDisk(nmdFilename.AsChar(), channel);


			/*uint32 numSamplesInChunk = channel->GetSampleRate() * 10;

			NmdExporter::SaveSamplesToDisk("D:\\NmdTest_Float4Byte.nmd", channel, numSamplesInChunk, NmdFileFormat::FLOATINGPOINT_4BYTE, false);
			NmdExporter::SaveSamplesToDisk("D:\\NmdTest_Float4Byte_Compressed.nmd", channel, numSamplesInChunk, NmdFileFormat::FLOATINGPOINT_4BYTE, true);
			NmdExporter::SaveSamplesToDisk("D:\\NmdTest_Float8Byte.nmd", channel, numSamplesInChunk, NmdFileFormat::FLOATINGPOINT_8BYTE, false);
			NmdExporter::SaveSamplesToDisk("D:\\NmdTest_Float8Byte_Compressed.nmd", channel, numSamplesInChunk, NmdFileFormat::FLOATINGPOINT_8BYTE, true);
			NmdExporter::SaveSamplesToDisk("D:\\NmdTest_Quant1Byte.nmd", channel, numSamplesInChunk, NmdFileFormat::QUANTIZATION_1BYTE, false);
			NmdExporter::SaveSamplesToDisk("D:\\NmdTest_Quant1Byte_Compressed.nmd", channel, numSamplesInChunk, NmdFileFormat::QUANTIZATION_1BYTE, true);
			NmdExporter::SaveSamplesToDisk("D:\\NmdTest_Quant2Byte.nmd", channel, numSamplesInChunk, NmdFileFormat::QUANTIZATION_2BYTE, false);
			NmdExporter::SaveSamplesToDisk("D:\\NmdTest_Quant2Byte_Compressed.nmd", channel, numSamplesInChunk, NmdFileFormat::QUANTIZATION_2BYTE, true);

			NmdExporter::SaveSamplesToDisk("D:\\NmdTest_Temp.nmd", channel, numSamplesInChunk, NmdFileFormat::FLOATINGPOINT_4BYTE, false);
			NmdImporter importer;
			bool loadRes = importer.LoadFromDisk("D:\\NmdTest_Temp.nmd");
			LogInfo("Loading result = %i", loadRes);*/
		}
	}

	return true;
}


bool SessionExporter::GenerateDataChunkJson(Json& json, Json::Item& item, const char* userId, const char* supervisorId, const char* debitorId, const char* classifierId, uint32 classifierRevision, const String& stateMachineId, uint32 stateMachineRevision, const String& experienceId, uint32 experienceRevision, const String& startDateTime, const String& stopDateTime)
{
	item.AddString( "userId", userId);
	item.AddString( "supervisorId", supervisorId );
	item.AddString( "debitorId", debitorId );
	item.AddString( "classifierId", classifierId );
	item.AddInt( "classifierRevision", classifierRevision );

	if (stateMachineId.IsEmpty() == false)
	{
		item.AddString( "stateMachineId", stateMachineId.AsChar() );
		item.AddInt( "stateMachineRevision", stateMachineRevision );
	}

	if (experienceId.IsEmpty() == false)
	{
		item.AddString( "experienceId", experienceId.AsChar() );
		item.AddInt( "experienceRevision", experienceRevision );
	}

	item.AddString( "startDateTime", startDateTime.AsChar() );
	item.AddString( "stopDateTime", stopDateTime.AsChar() );

	return true;
}


// generate the channel information json string
bool SessionExporter::GenerateChannelJson(Json& json, Json::Item& item, const char* userId, const char* dataChunkId, const char* classifierUuid, const char* nodeUuid, Channel<double>* channel)
{
	item.AddString( "userId", userId );
	item.AddString( "dataChunkId", dataChunkId );
	item.AddString( "classifierId", classifierUuid );
	item.AddString( "nodeId", nodeUuid );
	item.AddInt( "type", 0 );
	item.AddInt( "offset", 0 );
	item.AddString( "channelName", channel->GetName() );
	item.AddDouble( "sampleRate", channel->GetSampleRate() );
	item.AddDouble( "visualMin", channel->GetMinValue() );
	item.AddDouble( "visualMax", channel->GetMaxValue() );

	return true;
}


// save channel information to .JSON file
bool SessionExporter::SaveChannelJsonToDisk(const char* filename, const char* userId, const char* dataChunkId, const char* classifierUuid, const char* nodeUuid, Channel<double>* channel)
{
	// create our json parser
	Json json;
	Json::Item rootItem = json.GetRootItem();
	GenerateChannelJson( json, rootItem, userId, dataChunkId, classifierUuid, nodeUuid, channel );

	// write the json file and return the result
	return json.WriteToFile(filename);
}


// save biodata (raw samples) to disk
bool SessionExporter::SaveSamplesToDisk(const char* filename, Channel<double>* channel)
{
	// create the file on disk
	FILE* file;
	file = fopen(filename, "wb\0");
	if (file == NULL)
	{
		LogError("Cannot save channel to '%s'. Opening file in write mode failed.", filename);
		return false;
	}

	// save the samples
	const uint32 numSamples = channel->GetNumSamples();
	fwrite( &numSamples, 1, sizeof(uint32), file );

	// iterate through and save the samples
	for (uint32 i=0; i<numSamples; ++i)
	{
		const float value = channel->GetSample(i);
		fwrite( &value, 1, sizeof(float), file );
	}

	fclose(file);
	return true;
}


// save samples to memory file
bool SessionExporter::SaveSamplesToMemoryFile(MemoryFile* outFile, Channel<double>* channel)
{
	// close the memory file, remove all data
	outFile->Close();

	// open memory file
	outFile->Open();

	// save samples
	SaveSamples( outFile, channel );

	// seek to the beginning of the file
	outFile->Seek(0);

	return true;
}


// save all samples
void SessionExporter::SaveSamples(MemoryFile* file, Channel<double>* channel)
{
	// save the number of samples
	const uint32 numSamples = channel->GetNumSamples();
	file->Write(&numSamples, sizeof(uint32));

	// iterate through and save the samples
	for (uint32 i=0; i<numSamples; ++i)
	{
		const float value = channel->GetSample(i);
		file->Write(&value, sizeof(float));
	}
}
