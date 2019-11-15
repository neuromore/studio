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

 // include required files
#include "ChannelFileReader.h"
#include "../Core/LogManager.h"
#include "Channel.h"

using namespace Core;


bool ChannelFileReader::Read(FILE* inFile, const char* filename, EFormat format, Array<Channel<double>*>& outChannels)
{
	LogInfo("Loading channels in %s format...", GetFormatName(format));

	bool success = false;

	// call the right read method
	switch (format)
	{
	case FORMAT_CSV_SIMPLE:
	case FORMAT_CSV_TIMESTAMP:
	{
		const bool useTimestamps = (format == FORMAT_CSV_TIMESTAMP ? true : false);

		success = ReadCSV(inFile, useTimestamps, outChannels);
	}
	break;
	//case FORMAT_RAW_DOUBLE: 
	//	// TODO
	//	break;
	case FORMAT_EDF_PLUS:
	{
		// read edf file
		success = ReadEDF(inFile, filename, outChannels);
	}
	break;
	default: break;
	}

	if (success == true)
	{
		// count samples and channels
		const uint32 numChannels = outChannels.Size();
		uint32 numSamples = 0;
		for (uint32 i = 0; i < numChannels; ++i)
			numSamples += outChannels[i]->GetNumSamples();

		LogInfo("Done.");
		LogDetailedInfo("Read %i samples in %i channels.", numSamples, numChannels);

		return true;
	}

	return false;
}



bool ChannelFileReader::ReadCSV(FILE* inFile, bool useTimestamps, Core::Array<Channel<double>*>& outChannels)
{
	// csv will be read line-based
	String line;	line.Reserve(4096);
	String tmpStr;	tmpStr.Reserve(128);	// for parsing fields
	char tempBuffer[4096];

	// the comma-separated fields of one line
	Array<const char*> fieldsPtr;
	Array<uint32> fieldsLen;

	//
	// first line: contains channel names, also defines number of channels
	//
	fgets( tempBuffer, 4096, inFile );
	line = tempBuffer;

	// remove trailing newline (if any)
	line.TrimRight(StringCharacter::endLine);

	// split channel names, get number of channels
	uint32 numFields = line.SplitFast(fieldsPtr, fieldsLen, StringCharacter::comma);
	
	// is 1 if first field is timestamp
	const uint32 fieldOffset = (useTimestamps ? 1 : 0);

	if (numFields <= fieldOffset)
		return false;

	uint32 numChannels = numFields - fieldOffset; 

	// no channels could mean the file has no timestamps, but format is timstamped CSV
	if (numChannels == 0)
		return false;

	// create channels
	const uint32 currNumChannels = outChannels.Size();
	for (uint32 i = currNumChannels; i < numChannels; ++i)
		outChannels.Add(new Channel<double>());

	
	// configure the channels
	for (uint32 i = 0; i < numChannels; ++i)
	{
		// copy field from line to tmpst rand trim whitespaces
		tmpStr.Copy(fieldsPtr[i+fieldOffset], fieldsLen[i+fieldOffset]);
		tmpStr.Trim();

		outChannels[i]->Reset();
		outChannels[i]->SetName(tmpStr.AsChar());
		outChannels[i]->SetSampleRate(0);		// samplerate yet unknown
		outChannels[i]->SetBufferSize(0);		// is a storage channel
	}

	//
	// 2) Second line can contain samplerates (if we find the string "Hz" or "samplerate")
	//

	// read second line in file (might be samplerate line, but also data)
	fgets( tempBuffer, 4096, inFile );
	line = tempBuffer;

	// check if second line contains samplerates 
	if (line.Contains("Hz") || line.Contains("hz") || line.Contains("Samplerate") || line.Contains("samplerate"))
	{
		double sampleRate = 0;

		// remove trailing newline (if any)
		line.TrimRight(StringCharacter::endLine);

		// split the line to get to the samplerates (first field is timestamp)
		const uint32 numLineFields = line.SplitFast(fieldsPtr, fieldsLen, StringCharacter::comma);
		const uint32 minNumChannels = Min(numLineFields - fieldOffset, numChannels);

		for (uint32 i = 0; i < minNumChannels; ++i)
		{
			// copy field from line to tmpstr and trim whitespaces
			tmpStr.Copy(fieldsPtr[i + fieldOffset], fieldsLen[i + fieldOffset]);
			tmpStr.Trim();
			tmpStr.ToLower();

			// try to parse samplerate
			const int numArgsFilled = sscanf(tmpStr.AsChar(), "%lf", &sampleRate);
			if (numArgsFilled == 1 && sampleRate >= 0.0)
				outChannels[i]->SetSampleRate(sampleRate);

		}

		// read another line
		fgets( tempBuffer, 4096, inFile );
		line = tempBuffer;
	}

	uint32 numParseErrors = 0;
	double sampleValue = 0;

	// parse all other lines: one sample per channel, in ascii floating point
	// NOTE: we read lines at the end of the loop so it can handle the optional samplerate line correctly
	while (feof(inFile) == 0)
	{
		// skip empty lines
		line.Trim();
		if (line.IsEmpty() == true)
			continue;

		// split line and get number of 
		numFields = line.SplitFast(fieldsPtr, fieldsLen, StringCharacter::comma);
		if (numFields == 0)
		{
			numParseErrors++;
		}
		else
		{

			const uint32 numExpectedChannels = numFields - fieldOffset;
			if (numExpectedChannels != outChannels.Size())
				numParseErrors++;

			// make sure that line-to-line inconsistency doesn't crash it
			const uint32 numChannelsInLine = Min(numExpectedChannels, outChannels.Size());

			// read samples and push them into the channels
			for (uint32 i = 0; i < numChannelsInLine; ++i)
			{
				// copy field from line to tmpstr and trim whitespaces
				tmpStr.Copy(fieldsPtr[i + fieldOffset], fieldsLen[i + fieldOffset]);
				tmpStr.Trim();

				// try to parse the number
				const int numArgsFilled = sscanf(tmpStr.AsChar(), "%lf", &sampleValue);
				if (numArgsFilled != 1)
				{
					numParseErrors++;
					sampleValue = 0;
				}

				Channel<double>* channel = outChannels[i]->AsType<double>();
				channel->AddSample(sampleValue);
			}
		}

		if (numParseErrors > 0)
			break;

		// read another line
		if (feof(inFile) == 0)
		{
			fgets( tempBuffer, 4096, inFile );
			line = tempBuffer;
		}
	}

	return (numParseErrors == 0);
}



bool ChannelFileReader::ReadRawDouble(FILE* file, Array<Channel<double>*>& channels)
{
	return false;
	//CORE_ASSERT(channels.IsEmpty());

	//// contains only one channel of unknown samplerate and name.. start with 1k samples to reduce grow operations
	//Channel<double>* channel = new Channel<double>(0, 1000);
}


bool ChannelFileReader::ReadEDF(FILE* file, const char* filename, Core::Array<Channel<double>*>& channels)
{
	edf_hdr_struct outputEDFHDR;
	// open edf file
	int success = edfopen_file_readonly(filename, &outputEDFHDR, EDFLIB_DO_NOT_READ_ANNOTATIONS);
	if (success == -1)
		return false;

	const int handle = outputEDFHDR.handle;

	// create channels
	//const uint32 currentNumChannels = channels.Size();
	const uint32 numChannels = outputEDFHDR.edfsignals;

	// get information from potential channels
	for (uint32 i=0;i<numChannels;++i)
	{
		// parse electrode names
		edf_param_struct electrodeSignalParam = outputEDFHDR.signalparam[i];
		String electrodeName;
		electrodeName.Copy(electrodeSignalParam.label, sizeof(electrodeSignalParam.label)/sizeof(*(electrodeSignalParam.label)));
		electrodeName.Trim();
		if (electrodeName.Contains("EEG"))
		{
			electrodeName = electrodeName.Split(StringCharacter::space)[1];
		}

		channels.Add(new Channel<double>());
		channels[i]->Reset();
		channels[i]->SetName(electrodeName);
		channels[i]->SetSampleRate(electrodeSignalParam.smp_in_datarecord);
		channels[i]->SetBufferSize(0);
	}

	// get sample values
	bool parseError = false;
	for (uint32 i=0;i<numChannels;++i)
	{
		// get last position of the indicator
		const int numSamples = edfseek(handle, i, 0LL, EDFSEEK_END);
		// reset indicator to the start position of the file
		edfrewind(handle, i);

		// reset channel
		Channel<double>* channel = channels[i]->AsType<double>();
		channel->Reset();

		if (numSamples == -1)
			parseError = true;
		
		// NOTE: this is ugly as hell due to chunked channel quick fix; we write directly in the first array, which indirectly defines the chunk size; new v2 channels will fix that
		// resize array and copy over raw values, then update channel sample counters
		Array<double>& sampleValues = channel->GetRawArray();
		sampleValues.Resize(numSamples);

		// copy samples directly into channel array
		edfread_physical_samples(handle, i, numSamples, sampleValues.GetPtr());

		channel->ForceUpdateSampleCounters();
	}

	// close edf file by handle
	edfclose_file(handle);
	return (parseError == false);
}


//
// Helpers
//

// increment pos until a char [0-9eE] is found
bool ChannelFileReader::SeekNumber(char* pos, const char* end)
{
	// this will always terminate
	while (pos <= end)
	{
		const char c = *pos;
		const bool isPartOfNumber = ((c >= '0' && c <= '9') || c == '.' || c == 'e' || c == 'E');
		if (isPartOfNumber == true)
			return true;

		pos++;
	}

	return false;
}


const char* ChannelFileReader::GetFormatName(EFormat format)
{
	switch (format)
	{
	case FORMAT_CSV_SIMPLE:			return "CSV";
	case FORMAT_CSV_TIMESTAMP:		return "CSV with timestamps";
	case FORMAT_EDF_PLUS:			return "EDF(+) / BDF(+)";
		//case FORMAT_RAW_DOUBLE:			return "Raw 64bit IEEE float";
	default:						return "";
	}
}


const char* ChannelFileReader::GetFormatExtension(EFormat format)
{
	switch (format)
	{
	case FORMAT_CSV_SIMPLE:			return "csv";
	case FORMAT_CSV_TIMESTAMP:		return "csv";
	case FORMAT_EDF_PLUS:			return "edf";
		//case FORMAT_RAW_DOUBLE:			return "nmd";
	default:						return "";
	}
}
